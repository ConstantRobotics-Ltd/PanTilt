#include "PanTilt.h"
#include "PanTiltVersion.h"
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <limits>
#include <string>
#include <utility>
#include <vector>



namespace
{
namespace pt = cr::pantilt;

/// Number of PanTiltParams fields which are encoded.
constexpr int FIELDS_NUMBER{11};
/// Sizes of the encoded fields, bytes, in the order of the encoded data
/// (the same order as the bits of the mask).
constexpr int FIELD_SIZES[FIELDS_NUMBER]{4, 4, 4, 4, 4, 4, 1, 1, 4, 4, 4};
/// Size of the encoded params header (type, version and mask), bytes.
constexpr int PARAMS_HEADER_SIZE{5};
/// Size of the encoded params if all fields are encoded, bytes.
constexpr int PARAMS_MAX_SIZE{43};
/// Size of the encoded SET_PARAM command, bytes.
constexpr int SET_PARAM_SIZE{11};
/// Size of the encoded COMMAND, bytes.
constexpr int COMMAND_SIZE{15};
/// First and last valid param IDs.
constexpr int PARAM_ID_FIRST{1};
constexpr int PARAM_ID_LAST{11};
/// First and last valid command IDs.
constexpr int COMMAND_ID_FIRST{1};
constexpr int COMMAND_ID_LAST{12};
/// Number of random objects in the round trip tests.
constexpr int ROUND_TRIP_SAMPLES{5000};
/// Number of samples of the damaged data campaign.
constexpr int DAMAGED_SAMPLES{50000};
/// Name of the file which is created by the JSON file test.
constexpr const char* JSON_FILE_NAME{"PanTiltTestParams.json"};

/// State of the test random generator. The tests use own deterministic
/// generator: results must be reproducible on all platforms.
uint32_t g_randomState{20241018U};
/// Number of files which could not be removed.
int g_removeErrors{0};



/// Get next random 32 bit number.
uint32_t randomU32()
{
    g_randomState = g_randomState * 1664525U + 1013904223U;
    const uint32_t high = g_randomState >> 16;
    g_randomState = g_randomState * 1664525U + 1013904223U;
    const uint32_t low = g_randomState >> 16;
    return (high << 16) | low;
}



/// Get next random number in [0, limit).
int randomInt(int limit)
{
    return static_cast<int>(randomU32() % static_cast<uint32_t>(limit));
}



/// Get a random float with any bit pattern (including NaN and infinity).
float randomFloat()
{
    const uint32_t bits = randomU32();
    float value{0.0f};
    std::memcpy(&value, &bits, sizeof(value));
    return value;
}



/// Get the bits of a 4 bytes value. Floats are compared by their bits.
template <class T>
uint32_t bitsOf(T value)
{
    static_assert(sizeof(T) == sizeof(uint32_t), "4 bytes value expected");
    uint32_t bits{0};
    std::memcpy(&bits, &value, sizeof(bits));
    return bits;
}



/// Compare a 4 bytes field. The number of mismatches is incremented for each
/// wrong field.
template <class T>
void checkField(const char* name, T expected, T actual, int& errors)
{
    if (bitsOf(expected) != bitsOf(actual))
    {
        std::cout << "Mismatch in field " << name << std::endl;
        ++errors;
    }
}



/// Compare a bool field.
void checkFlag(const char* name, bool expected, bool actual, int& errors)
{
    if (expected != actual)
    {
        std::cout << "Mismatch in field " << name << std::endl;
        ++errors;
    }
}



/// Compare all fields of two objects. Returns the number of mismatches.
int compareParams(const pt::PanTiltParams& expected,
                  const pt::PanTiltParams& actual)
{
    int errors{0};
    checkField("panMotorPosition", expected.panMotorPosition,
               actual.panMotorPosition, errors);
    checkField("tiltMotorPosition", expected.tiltMotorPosition,
               actual.tiltMotorPosition, errors);
    checkField("panAngle", expected.panAngle, actual.panAngle, errors);
    checkField("tiltAngle", expected.tiltAngle, actual.tiltAngle, errors);
    checkField("panMotorSpeed", expected.panMotorSpeed,
               actual.panMotorSpeed, errors);
    checkField("tiltMotorSpeed", expected.tiltMotorSpeed,
               actual.tiltMotorSpeed, errors);
    checkFlag("isConnected", expected.isConnected, actual.isConnected, errors);
    checkFlag("isOpen", expected.isOpen, actual.isOpen, errors);
    checkField("custom1", expected.custom1, actual.custom1, errors);
    checkField("custom2", expected.custom2, actual.custom2, errors);
    checkField("custom3", expected.custom3, actual.custom3, errors);
    if (expected.initString != actual.initString)
    {
        std::cout << "Mismatch in field initString" << std::endl;
        ++errors;
    }
    return errors;
}



/// Print a failed check and return false.
bool fail(const char* message)
{
    std::cout << message << std::endl;
    return false;
}



/// Get random params. All fields have random values.
pt::PanTiltParams randomParams()
{
    pt::PanTiltParams params;
    params.panMotorPosition = static_cast<int>(randomU32());
    params.tiltMotorPosition = static_cast<int>(randomU32());
    params.panAngle = randomFloat();
    params.tiltAngle = randomFloat();
    params.panMotorSpeed = randomFloat();
    params.tiltMotorSpeed = randomFloat();
    params.isConnected = randomInt(2) != 0;
    params.isOpen = randomInt(2) != 0;
    params.initString = "Init string";
    params.custom1 = randomFloat();
    params.custom2 = randomFloat();
    params.custom3 = randomFloat();
    return params;
}



/// Get params which have values that are stored exactly in a JSON file.
pt::PanTiltParams jsonParams()
{
    pt::PanTiltParams params;
    params.panMotorPosition = randomInt(65536);
    params.tiltMotorPosition = randomInt(65536);
    params.panAngle = static_cast<float>(randomInt(1441) - 720) / 4.0f;
    params.tiltAngle = static_cast<float>(randomInt(1441) - 720) / 4.0f;
    params.panMotorSpeed = static_cast<float>(randomInt(801) - 400) / 4.0f;
    params.tiltMotorSpeed = static_cast<float>(randomInt(801) - 400) / 4.0f;
    params.isConnected = randomInt(2) != 0;
    params.isOpen = randomInt(2) != 0;
    params.initString = "/dev/ttyUSB" + std::to_string(randomInt(10)) + ";9600";
    params.custom1 = static_cast<float>(randomInt(1000)) / 4.0f;
    params.custom2 = static_cast<float>(randomInt(1000)) / 4.0f;
    params.custom3 = static_cast<float>(randomInt(1000)) / 4.0f;
    return params;
}



/// Get params which differ from the default values in every field. They are
/// used to check that a rejected call does not change an object.
pt::PanTiltParams sentinelParams()
{
    pt::PanTiltParams params;
    params.panMotorPosition = 1001;
    params.tiltMotorPosition = 1002;
    params.panAngle = 1.5f;
    params.tiltAngle = 2.5f;
    params.panMotorSpeed = 3.5f;
    params.tiltMotorSpeed = 4.5f;
    params.isConnected = true;
    params.isOpen = true;
    params.initString = "Sentinel";
    params.custom1 = 7.5f;
    params.custom2 = 8.5f;
    params.custom3 = 9.5f;
    return params;
}



/// Get a mask from flags in the order of the encoded fields.
pt::PanTiltParamsMask makeMask(const bool (&flags)[FIELDS_NUMBER])
{
    pt::PanTiltParamsMask mask;
    mask.panMotorPosition = flags[0];
    mask.tiltMotorPosition = flags[1];
    mask.panAngle = flags[2];
    mask.tiltAngle = flags[3];
    mask.panMotorSpeed = flags[4];
    mask.tiltMotorSpeed = flags[5];
    mask.isConnected = flags[6];
    mask.isOpen = flags[7];
    mask.custom1 = flags[8];
    mask.custom2 = flags[9];
    mask.custom3 = flags[10];
    return mask;
}



/// Get the params which the decoder must return for the data encoded with the
/// flags: selected fields are equal to the source, the others are zero.
/// initString is not encoded, so it is taken from the given object.
pt::PanTiltParams selectParams(const pt::PanTiltParams& source,
                               const bool (&flags)[FIELDS_NUMBER],
                               const pt::PanTiltParams& target)
{
    pt::PanTiltParams result;
    result.panMotorPosition = flags[0] ? source.panMotorPosition : 0;
    result.tiltMotorPosition = flags[1] ? source.tiltMotorPosition : 0;
    result.panAngle = flags[2] ? source.panAngle : 0.0f;
    result.tiltAngle = flags[3] ? source.tiltAngle : 0.0f;
    result.panMotorSpeed = flags[4] ? source.panMotorSpeed : 0.0f;
    result.tiltMotorSpeed = flags[5] ? source.tiltMotorSpeed : 0.0f;
    result.isConnected = flags[6] ? source.isConnected : false;
    result.isOpen = flags[7] ? source.isOpen : false;
    result.custom1 = flags[8] ? source.custom1 : 0.0f;
    result.custom2 = flags[9] ? source.custom2 : 0.0f;
    result.custom3 = flags[10] ? source.custom3 : 0.0f;
    result.initString = target.initString;
    return result;
}



/// Get the size of the data encoded with the flags.
int expectedSize(const bool (&flags)[FIELDS_NUMBER])
{
    int size{PARAMS_HEADER_SIZE};
    for (int i = 0; i < FIELDS_NUMBER; ++i)
    {
        if (flags[i])
        {
            size += FIELD_SIZES[i];
        }
    }
    return size;
}



/// Get the bytes of the mask for the flags.
void expectedMaskBytes(const bool (&flags)[FIELDS_NUMBER], uint8_t (&bytes)[2])
{
    bytes[0] = 0;
    bytes[1] = 0;
    for (int i = 0; i < FIELDS_NUMBER; ++i)
    {
        if (flags[i])
        {
            bytes[i / 8] = static_cast<uint8_t>(bytes[i / 8] | (128 >> (i % 8)));
        }
    }
}



/// Get random flags.
void randomFlags(bool (&flags)[FIELDS_NUMBER])
{
    for (int i = 0; i < FIELDS_NUMBER; ++i)
    {
        flags[i] = randomInt(2) != 0;
    }
}



/// Remover of a file: the file is removed when the object is destroyed. A
/// failed removal is reported and counted.
class FileRemover
{
public:
    /// Remember the name of the file.
    explicit FileRemover(std::string name) : m_name(std::move(name))
    {
    }

    FileRemover(const FileRemover&) = delete;
    FileRemover(FileRemover&&) = delete;
    FileRemover& operator= (const FileRemover&) = delete;
    FileRemover& operator= (FileRemover&&) = delete;

    /// Remove the file. Returns true if the file does not exist after the
    /// call.
    bool remove()
    {
        if (m_removed)
        {
            return true;
        }
        if (std::remove(m_name.c_str()) != 0 && errno != ENOENT)
        {
            std::cout << "Can't remove file " << m_name << std::endl;
            ++g_removeErrors;
            return false;
        }
        m_removed = true;
        return true;
    }

    /// Remove the file if the test did not do it.
    ~FileRemover()
    {
        remove();
    }

private:
    /// Name of the file.
    std::string m_name;
    /// True after the file is gone.
    bool m_removed{false};
};



/// Copy params test.
bool copyParametersTest()
{
    int errors{0};
    for (int i = 0; i < 100; ++i)
    {
        const pt::PanTiltParams source = randomParams();

        // Copy constructor copies all fields.
        const pt::PanTiltParams copied = source;
        errors += compareParams(source, copied);

        // Assignment copies all fields except isConnected and isOpen.
        pt::PanTiltParams assigned = sentinelParams();
        assigned = source;
        pt::PanTiltParams expected = source;
        expected.isConnected = true;
        expected.isOpen = true;
        errors += compareParams(expected, assigned);

        // Self assignment does not change the object.
        pt::PanTiltParams self = source;
        pt::PanTiltParams& selfRef = self;
        self = selfRef;
        errors += compareParams(source, self);
    }
    return errors == 0;
}



/// Encode / decode test with all fields.
bool encodeDecodeAllFieldsTest()
{
    int errors{0};
    for (int i = 0; i < ROUND_TRIP_SAMPLES; ++i)
    {
        const pt::PanTiltParams source = randomParams();
        pt::PanTiltParams params = source;

        uint8_t buffer[PARAMS_MAX_SIZE];
        int size{-1};
        if (!params.encode(buffer, PARAMS_MAX_SIZE, size))
        {
            return fail("Can't encode params");
        }
        if (size != PARAMS_MAX_SIZE)
        {
            return fail("Wrong size of encoded params");
        }

        // Header: data type, version and the mask with all fields selected.
        if (buffer[0] != 0x02 || buffer[1] != PAN_TILT_MAJOR_VERSION ||
            buffer[2] != PAN_TILT_MINOR_VERSION || buffer[3] != 0xFF ||
            buffer[4] != 0xE0)
        {
            return fail("Wrong header of encoded params");
        }

        // initString is not encoded.
        pt::PanTiltParams decoded;
        decoded.initString = "Not encoded";
        if (!decoded.decode(buffer, size))
        {
            return fail("Can't decode params");
        }
        pt::PanTiltParams expected = source;
        expected.initString = "Not encoded";
        errors += compareParams(expected, decoded);

        // The source object is not changed by encoding.
        errors += compareParams(source, params);
    }
    std::cout << ROUND_TRIP_SAMPLES << " objects" << std::endl;
    return errors == 0;
}



/// Encode / decode test with masks.
bool encodeDecodeMaskTest()
{
    int errors{0};

    // Masks with one field selected, with one field excluded, empty and full.
    std::vector<std::vector<bool>> masks;
    for (int i = 0; i < FIELDS_NUMBER; ++i)
    {
        std::vector<bool> one(FIELDS_NUMBER, false);
        one[static_cast<size_t>(i)] = true;
        masks.push_back(one);
        std::vector<bool> all(FIELDS_NUMBER, true);
        all[static_cast<size_t>(i)] = false;
        masks.push_back(all);
    }
    masks.push_back(std::vector<bool>(FIELDS_NUMBER, false));
    masks.push_back(std::vector<bool>(FIELDS_NUMBER, true));

    // Random masks.
    for (int i = 0; i < ROUND_TRIP_SAMPLES; ++i)
    {
        std::vector<bool> flags(FIELDS_NUMBER, false);
        for (int k = 0; k < FIELDS_NUMBER; ++k)
        {
            flags[static_cast<size_t>(k)] = randomInt(2) != 0;
        }
        masks.push_back(flags);
    }

    for (const std::vector<bool>& flagsVector : masks)
    {
        bool flags[FIELDS_NUMBER];
        for (int k = 0; k < FIELDS_NUMBER; ++k)
        {
            flags[k] = flagsVector[static_cast<size_t>(k)];
        }
        pt::PanTiltParams params = randomParams();
        pt::PanTiltParamsMask mask = makeMask(flags);

        // Encode: nothing is written after the encoded data.
        uint8_t buffer[PARAMS_MAX_SIZE + 8];
        std::memset(buffer, 0xA5, sizeof(buffer));
        int size{-1};
        if (!params.encode(buffer, PARAMS_MAX_SIZE, size, &mask))
        {
            return fail("Can't encode params");
        }
        if (size != expectedSize(flags))
        {
            return fail("Wrong size of encoded params");
        }
        for (int k = size; k < static_cast<int>(sizeof(buffer)); ++k)
        {
            if (buffer[k] != 0xA5)
            {
                return fail("Data is written after the encoded params");
            }
        }
        uint8_t maskBytes[2];
        expectedMaskBytes(flags, maskBytes);
        if (buffer[3] != maskBytes[0] || buffer[4] != maskBytes[1])
        {
            return fail("Wrong mask in encoded params");
        }

        // Decode: selected fields are restored, the others are zero.
        pt::PanTiltParams decoded = sentinelParams();
        if (!decoded.decode(buffer, size))
        {
            return fail("Can't decode params");
        }
        errors += compareParams(
                    selectParams(params, flags, sentinelParams()), decoded);
    }
    std::cout << masks.size() << " masks" << std::endl;
    return errors == 0;
}



/// Buffer size and pointer checks of the encoder.
bool encodeBufferTest()
{
    const pt::PanTiltParams source = randomParams();

    // Null pointer: rejected, size is set to 0.
    {
        pt::PanTiltParams params = source;
        int size{99};
        if (params.encode(nullptr, PARAMS_MAX_SIZE, size) || size != 0)
        {
            return fail("nullptr is not rejected");
        }
    }

    // Buffer which is too small (also with a mask which needs less): the
    // buffer is not changed, size is set to 0.
    const int badSizes[] = {std::numeric_limits<int>::min(), -1, 0, 1, 4, 5,
                            42};
    for (int bufferSize : badSizes)
    {
        pt::PanTiltParams params = source;
        pt::PanTiltParamsMask mask;
        mask.panMotorPosition = false;
        mask.tiltMotorPosition = false;
        uint8_t buffer[PARAMS_MAX_SIZE];
        std::memset(buffer, 0x5A, sizeof(buffer));
        int size{99};
        if (params.encode(buffer, bufferSize, size, &mask) || size != 0)
        {
            return fail("Too small buffer is not rejected");
        }
        for (uint8_t value : buffer)
        {
            if (value != 0x5A)
            {
                return fail("Buffer is changed by a rejected call");
            }
        }
    }

    // Exact size and larger buffers are accepted.
    const int goodSizes[] = {PARAMS_MAX_SIZE, PARAMS_MAX_SIZE + 1, 1000};
    for (int bufferSize : goodSizes)
    {
        pt::PanTiltParams params = source;
        std::vector<uint8_t> buffer(static_cast<size_t>(bufferSize), 0x5A);
        int size{-1};
        if (!params.encode(buffer.data(), bufferSize, size) ||
            size != PARAMS_MAX_SIZE)
        {
            return fail("Buffer of valid size is rejected");
        }
        for (size_t i = static_cast<size_t>(size); i < buffer.size(); ++i)
        {
            if (buffer[i] != 0x5A)
            {
                return fail("Data is written after the encoded params");
            }
        }
    }
    return true;
}



/// Invalid data checks of the decoder.
bool decodeInvalidDataTest()
{
    pt::PanTiltParams source = randomParams();
    uint8_t valid[PARAMS_MAX_SIZE];
    int validSize{0};
    if (!source.encode(valid, PARAMS_MAX_SIZE, validSize))
    {
        return fail("Can't encode params");
    }

    // A rejected call must not change the object.
    int errors{0};
    auto rejected = [&errors](const uint8_t* data, size_t size,
                              const char* what)
    {
        // Exact size heap buffer: the sanitizers see every over-read.
        std::vector<uint8_t> copy(data, data + size);
        pt::PanTiltParams target = sentinelParams();
        const bool result = target.decode(
                    copy.data(), static_cast<int>(copy.size()));
        if (result)
        {
            std::cout << "Accepted: " << what << std::endl;
            ++errors;
        }
        errors += compareParams(sentinelParams(), target);
    };

    // Null pointer and invalid sizes.
    {
        pt::PanTiltParams target = sentinelParams();
        if (target.decode(nullptr, validSize) ||
            target.decode(nullptr, 0) ||
            compareParams(sentinelParams(), target) != 0)
        {
            return fail("nullptr is not rejected");
        }
        const int sizes[] = {std::numeric_limits<int>::min(), -1, 0};
        for (int size : sizes)
        {
            if (target.decode(valid, size) ||
                compareParams(sentinelParams(), target) != 0)
            {
                return fail("Invalid size is not rejected");
            }
        }
    }

    // Every truncation: the data is too short for the mask.
    for (size_t length = 0; length < static_cast<size_t>(validSize); ++length)
    {
        rejected(valid, length, "truncated data");
    }

    // Every header byte which is wrong: data type, major and minor version.
    for (size_t pos = 0; pos < 3; ++pos)
    {
        for (int value = 0; value < 256; ++value)
        {
            if (value == valid[pos])
            {
                continue;
            }
            uint8_t damaged[PARAMS_MAX_SIZE];
            std::memcpy(damaged, valid, sizeof(damaged));
            damaged[pos] = static_cast<uint8_t>(value);
            rejected(damaged, static_cast<size_t>(validSize),
                     "wrong header byte");
        }
    }

    // Header with an empty mask: every truncation of it is rejected (the
    // mask itself is cut), the complete header is accepted.
    {
        uint8_t header[PARAMS_HEADER_SIZE]{0x02, PAN_TILT_MAJOR_VERSION,
                                           PAN_TILT_MINOR_VERSION, 0x00, 0x00};
        for (size_t length = 0; length < sizeof(header); ++length)
        {
            rejected(header, length, "truncated header");
        }
        pt::PanTiltParams target = sentinelParams();
        if (!target.decode(header, static_cast<int>(sizeof(header))))
        {
            return fail("Complete header is rejected");
        }
        pt::PanTiltParams expected;
        expected.initString = sentinelParams().initString;
        errors += compareParams(expected, target);
    }

    // Mask which needs more data than available.
    {
        uint8_t damaged[PARAMS_MAX_SIZE];
        std::memset(damaged, 0, sizeof(damaged));
        std::memcpy(damaged, valid, PARAMS_HEADER_SIZE);
        damaged[3] = 0xFF;
        damaged[4] = 0xE0;
        rejected(damaged, PARAMS_MAX_SIZE - 1, "mask needs more data");
        rejected(damaged, PARAMS_HEADER_SIZE, "mask needs more data");
    }

    // Trailing bytes are ignored.
    {
        std::vector<uint8_t> longer(valid, valid + validSize);
        longer.resize(static_cast<size_t>(validSize) + 100, 0xCC);
        pt::PanTiltParams target = sentinelParams();
        if (!target.decode(longer.data(), static_cast<int>(longer.size())))
        {
            return fail("Data with trailing bytes is rejected");
        }
        pt::PanTiltParams expected = source;
        expected.initString = sentinelParams().initString;
        errors += compareParams(expected, target);
    }

    // Any non-zero byte of a bool field is TRUE, zero is FALSE.
    {
        uint8_t bools[PARAMS_HEADER_SIZE + 2];
        bools[0] = 0x02;
        bools[1] = PAN_TILT_MAJOR_VERSION;
        bools[2] = PAN_TILT_MINOR_VERSION;
        bools[3] = 0x03;
        bools[4] = 0x00;
        for (int value = 0; value < 256; ++value)
        {
            bools[5] = static_cast<uint8_t>(value);
            bools[6] = static_cast<uint8_t>(255 - value);
            pt::PanTiltParams target = sentinelParams();
            if (!target.decode(bools, static_cast<int>(sizeof(bools))))
            {
                return fail("Data with bool bytes is rejected");
            }
            checkFlag("isConnected", value != 0, target.isConnected, errors);
            checkFlag("isOpen", value != 255, target.isOpen, errors);
        }
    }

    // Unused bits of the mask are ignored.
    {
        uint8_t data[PARAMS_HEADER_SIZE];
        data[0] = 0x02;
        data[1] = PAN_TILT_MAJOR_VERSION;
        data[2] = PAN_TILT_MINOR_VERSION;
        data[3] = 0x00;
        data[4] = 0x1F;
        pt::PanTiltParams target = sentinelParams();
        if (!target.decode(data, static_cast<int>(sizeof(data))))
        {
            return fail("Data with unused mask bits is rejected");
        }
        pt::PanTiltParams expected;
        expected.initString = sentinelParams().initString;
        errors += compareParams(expected, target);
    }
    return errors == 0;
}



/// Damaged data campaign: deterministic damaged encodings are decoded, no
/// crash is allowed, a rejected call must not change the object and the
/// result must be reproducible.
bool damagedDataTest()
{
    const uint32_t startState = g_randomState;
    uint64_t hashes[2]{0, 0};
    int accepted{0};
    int errors{0};

    for (int pass = 0; pass < 2; ++pass)
    {
        g_randomState = startState;
        uint64_t hash{14695981039346656037ULL};
        auto add = [&hash](uint32_t value)
        {
            for (int k = 0; k < 4; ++k)
            {
                hash ^= (value >> (8 * k)) & 0xFFU;
                hash *= 1099511628211ULL;
            }
        };
        accepted = 0;

        for (int i = 0; i < DAMAGED_SAMPLES; ++i)
        {
            // Valid data.
            bool flags[FIELDS_NUMBER];
            randomFlags(flags);
            pt::PanTiltParamsMask mask = makeMask(flags);
            pt::PanTiltParams source = randomParams();
            uint8_t encoded[PARAMS_MAX_SIZE];
            int size{0};
            if (!source.encode(encoded, PARAMS_MAX_SIZE, size, &mask))
            {
                return fail("Can't encode params");
            }

            // Damage: random bytes replaced, random truncation.
            std::vector<uint8_t> data(encoded, encoded + size);
            const int mode = randomInt(3);
            if (mode != 1)
            {
                const int count = 1 + randomInt(4);
                for (int k = 0; k < count; ++k)
                {
                    data[static_cast<size_t>(randomInt(size))] =
                            static_cast<uint8_t>(randomU32());
                }
            }
            if (mode != 0)
            {
                data.resize(static_cast<size_t>(randomInt(size + 1)));
            }

            // Decode.
            pt::PanTiltParams target = sentinelParams();
            const bool result = target.decode(
                        data.data(), static_cast<int>(data.size()));
            add(result ? 1U : 0U);
            if (!result)
            {
                errors += compareParams(sentinelParams(), target);
                continue;
            }
            ++accepted;

            // A decoded object must be encoded and decoded to the same
            // object with the mask of the data.
            const bool decodedFlags[FIELDS_NUMBER]{
                (data[3] & 0x80) != 0, (data[3] & 0x40) != 0,
                (data[3] & 0x20) != 0, (data[3] & 0x10) != 0,
                (data[3] & 0x08) != 0, (data[3] & 0x04) != 0,
                (data[3] & 0x02) != 0, (data[3] & 0x01) != 0,
                (data[4] & 0x80) != 0, (data[4] & 0x40) != 0,
                (data[4] & 0x20) != 0};
            pt::PanTiltParamsMask decodedMask = makeMask(decodedFlags);
            uint8_t again[PARAMS_MAX_SIZE];
            int againSize{0};
            pt::PanTiltParams second = sentinelParams();
            if (!target.encode(again, PARAMS_MAX_SIZE, againSize,
                               &decodedMask) ||
                !second.decode(again, againSize))
            {
                return fail("Decoded params can't be encoded and decoded");
            }
            errors += compareParams(target, second);
            add(bitsOf(target.panMotorPosition));
            add(bitsOf(target.panAngle));
            add(bitsOf(target.custom3));
            add(target.isOpen ? 1U : 0U);
        }
        hashes[pass] = hash;
    }

    if (hashes[0] != hashes[1])
    {
        return fail("The result of the campaign is not reproducible");
    }
    std::cout << DAMAGED_SAMPLES << " samples, " << accepted << " accepted, "
              << DAMAGED_SAMPLES - accepted << " rejected" << std::endl;
    return errors == 0;
}



/// Encode / decode test of SET_PARAM commands and COMMANDs.
bool commandsRoundTripTest()
{
    int errors{0};

    // SET_PARAM: every valid ID with random values (any bit pattern).
    for (int id = PARAM_ID_FIRST; id <= PARAM_ID_LAST; ++id)
    {
        for (int i = 0; i < 200; ++i)
        {
            const float value = randomFloat();
            uint8_t data[SET_PARAM_SIZE];
            int size{-1};
            pt::PanTilt::encodeSetParamCommand(
                        data, size, static_cast<pt::PanTiltParam>(id), value);
            if (size != SET_PARAM_SIZE || data[0] != 0x01 ||
                data[1] != PAN_TILT_MAJOR_VERSION ||
                data[2] != PAN_TILT_MINOR_VERSION)
            {
                return fail("Wrong encoded SET_PARAM command");
            }
            pt::PanTiltParam paramId{pt::PanTiltParam::CUSTOM_3};
            pt::PanTiltCommand commandId{pt::PanTiltCommand::STOP};
            float value1{5.0f};
            float value2{6.0f};
            const int result = pt::PanTilt::decodeCommand(
                        data, size, paramId, commandId, value1, value2);
            if (result != 1 || static_cast<int>(paramId) != id ||
                commandId != pt::PanTiltCommand::STOP ||
                bitsOf(value1) != bitsOf(value) ||
                bitsOf(value2) != bitsOf(0.0f))
            {
                return fail("SET_PARAM command is not decoded");
            }
        }
    }

    // COMMAND: every valid ID with random arguments.
    for (int id = COMMAND_ID_FIRST; id <= COMMAND_ID_LAST; ++id)
    {
        for (int i = 0; i < 200; ++i)
        {
            const float arg1 = randomFloat();
            const float arg2 = randomFloat();
            uint8_t data[COMMAND_SIZE];
            int size{-1};
            pt::PanTilt::encodeCommand(
                        data, size, static_cast<pt::PanTiltCommand>(id),
                        arg1, arg2);
            if (size != COMMAND_SIZE || data[0] != 0x00 ||
                data[1] != PAN_TILT_MAJOR_VERSION ||
                data[2] != PAN_TILT_MINOR_VERSION)
            {
                return fail("Wrong encoded COMMAND");
            }
            pt::PanTiltParam paramId{pt::PanTiltParam::CUSTOM_3};
            pt::PanTiltCommand commandId{pt::PanTiltCommand::STOP};
            float value1{5.0f};
            float value2{6.0f};
            const int result = pt::PanTilt::decodeCommand(
                        data, size, paramId, commandId, value1, value2);
            if (result != 0 || static_cast<int>(commandId) != id ||
                paramId != pt::PanTiltParam::CUSTOM_3 ||
                bitsOf(value1) != bitsOf(arg1) ||
                bitsOf(value2) != bitsOf(arg2))
            {
                return fail("COMMAND is not decoded");
            }
        }
    }

    // Null pointer: nothing is encoded, size is set to 0.
    {
        int size{99};
        pt::PanTilt::encodeSetParamCommand(
                    nullptr, size, pt::PanTiltParam::PAN_ANGLE, 1.0f);
        checkField("size of SET_PARAM command", 0, size, errors);
        size = 99;
        pt::PanTilt::encodeCommand(
                    nullptr, size, pt::PanTiltCommand::STOP, 1.0f, 2.0f);
        checkField("size of COMMAND", 0, size, errors);
    }
    return errors == 0;
}



/// Invalid commands: the decoder returns -1 and does not change the output
/// arguments.
bool commandsInvalidDataTest()
{
    int errors{0};
    int checks{0};

    // Decode a copy in an exact size buffer; the outputs must stay unchanged.
    auto rejected = [&errors, &checks](const uint8_t* data, size_t size,
                                       int sizeArgument, const char* what)
    {
        std::vector<uint8_t> copy(data, data + size);
        pt::PanTiltParam paramId{pt::PanTiltParam::TILT_ANGLE};
        pt::PanTiltCommand commandId{pt::PanTiltCommand::GO_TO_HOME};
        float value1{55.0f};
        float value2{66.0f};
        const int result = pt::PanTilt::decodeCommand(
                    copy.data(), sizeArgument, paramId, commandId,
                    value1, value2);
        ++checks;
        if (result != -1 || paramId != pt::PanTiltParam::TILT_ANGLE ||
            commandId != pt::PanTiltCommand::GO_TO_HOME ||
            bitsOf(value1) != bitsOf(55.0f) ||
            bitsOf(value2) != bitsOf(66.0f))
        {
            std::cout << "Not rejected or outputs changed: " << what
                      << std::endl;
            ++errors;
        }
    };

    uint8_t setParam[SET_PARAM_SIZE];
    uint8_t command[COMMAND_SIZE];
    int size{0};
    pt::PanTilt::encodeSetParamCommand(
                setParam, size, pt::PanTiltParam::PAN_ANGLE, 10.0f);
    pt::PanTilt::encodeCommand(
                command, size, pt::PanTiltCommand::GO_TO_PAN_ANGLE, 1.0f, 2.0f);

    // Wrong sizes: the size argument is changed, the buffer is large enough.
    for (int sizeArgument = -2; sizeArgument <= 40; ++sizeArgument)
    {
        std::vector<uint8_t> setParamBuffer(64, 0);
        std::memcpy(setParamBuffer.data(), setParam, sizeof(setParam));
        std::vector<uint8_t> commandBuffer(64, 0);
        std::memcpy(commandBuffer.data(), command, sizeof(command));
        if (sizeArgument != SET_PARAM_SIZE)
        {
            rejected(setParamBuffer.data(), setParamBuffer.size(),
                     sizeArgument, "SET_PARAM with wrong size");
        }
        if (sizeArgument != COMMAND_SIZE)
        {
            rejected(commandBuffer.data(), commandBuffer.size(),
                     sizeArgument, "COMMAND with wrong size");
        }
    }

    // Wrong major or minor version.
    const int validVersion[3]{0, PAN_TILT_MAJOR_VERSION, PAN_TILT_MINOR_VERSION};
    for (int pos = 1; pos <= 2; ++pos)
    {
        const int validValue = validVersion[pos];
        for (int value = 0; value < 256; ++value)
        {
            if (value == validValue)
            {
                continue;
            }
            uint8_t damagedSetParam[SET_PARAM_SIZE];
            std::memcpy(damagedSetParam, setParam, sizeof(setParam));
            damagedSetParam[pos] = static_cast<uint8_t>(value);
            rejected(damagedSetParam, sizeof(damagedSetParam), SET_PARAM_SIZE,
                     "SET_PARAM with wrong version");
            uint8_t damagedCommand[COMMAND_SIZE];
            std::memcpy(damagedCommand, command, sizeof(command));
            damagedCommand[pos] = static_cast<uint8_t>(value);
            rejected(damagedCommand, sizeof(damagedCommand), COMMAND_SIZE,
                     "COMMAND with wrong version");
        }
    }

    // Wrong data type. 0x00 (COMMAND) and 0x01 (SET_PARAM) are the only valid
    // types, each of them needs its own size.
    for (int value = 0; value < 256; ++value)
    {
        uint8_t damagedSetParam[SET_PARAM_SIZE];
        std::memcpy(damagedSetParam, setParam, sizeof(setParam));
        damagedSetParam[0] = static_cast<uint8_t>(value);
        uint8_t damagedCommand[COMMAND_SIZE];
        std::memcpy(damagedCommand, command, sizeof(command));
        damagedCommand[0] = static_cast<uint8_t>(value);
        if (value != 0x01)
        {
            rejected(damagedSetParam, sizeof(damagedSetParam), SET_PARAM_SIZE,
                     "SET_PARAM size with wrong data type");
        }
        if (value != 0x00)
        {
            rejected(damagedCommand, sizeof(damagedCommand), COMMAND_SIZE,
                     "COMMAND size with wrong data type");
        }
    }

    // IDs outside of the enum range (including the values next to the range).
    const int badParamIds[] = {std::numeric_limits<int>::min(), -1, 0,
                               PARAM_ID_LAST + 1, 256,
                               std::numeric_limits<int>::max()};
    for (int id : badParamIds)
    {
        uint8_t damagedSetParam[SET_PARAM_SIZE];
        std::memcpy(damagedSetParam, setParam, sizeof(setParam));
        std::memcpy(&damagedSetParam[3], &id, sizeof(id));
        rejected(damagedSetParam, sizeof(damagedSetParam), SET_PARAM_SIZE,
                 "SET_PARAM with unknown ID");
    }
    const int badCommandIds[] = {std::numeric_limits<int>::min(), -1, 0,
                                 COMMAND_ID_LAST + 1, 256,
                                 std::numeric_limits<int>::max()};
    for (int id : badCommandIds)
    {
        uint8_t damagedCommand[COMMAND_SIZE];
        std::memcpy(damagedCommand, command, sizeof(command));
        std::memcpy(&damagedCommand[3], &id, sizeof(id));
        rejected(damagedCommand, sizeof(damagedCommand), COMMAND_SIZE,
                 "COMMAND with unknown ID");
    }

    // Null pointer.
    {
        pt::PanTiltParam paramId{pt::PanTiltParam::TILT_ANGLE};
        pt::PanTiltCommand commandId{pt::PanTiltCommand::GO_TO_HOME};
        float value1{55.0f};
        float value2{66.0f};
        if (pt::PanTilt::decodeCommand(nullptr, COMMAND_SIZE, paramId,
                                       commandId, value1, value2) != -1 ||
            bitsOf(value1) != bitsOf(55.0f) ||
            bitsOf(value2) != bitsOf(66.0f))
        {
            return fail("nullptr is not rejected");
        }
    }
    std::cout << checks << " invalid commands" << std::endl;
    return errors == 0;
}



/// Version check: the version string and the version in the data.
bool versionTest()
{
    const std::string expected = std::to_string(PAN_TILT_MAJOR_VERSION) + "." +
                                 std::to_string(PAN_TILT_MINOR_VERSION) + "." +
                                 std::to_string(PAN_TILT_PATCH_VERSION);
    std::cout << "PanTilt version: " << pt::PanTilt::getVersion() << std::endl;
    return pt::PanTilt::getVersion() == expected &&
           expected == PAN_TILT_VERSION;
}



/// Read / write JSON through a string: round trip and invalid documents.
bool jsonStringTest()
{
    int errors{0};
    for (int i = 0; i < 200; ++i)
    {
        const pt::PanTiltParams source = jsonParams();
        cr::utils::ConfigReader writer;
        writer.set(source, "panTiltParams");
        std::string json;
        if (!writer.writeToString(json))
        {
            return fail("Can't write params to string");
        }
        cr::utils::ConfigReader reader;
        pt::PanTiltParams target;
        if (!reader.readFromString(json) ||
            !reader.get(target, "panTiltParams"))
        {
            return fail("Can't read params from string");
        }

        // Only the fields of the JSON_READABLE macro are stored.
        checkField("panMotorSpeed", source.panMotorSpeed,
                   target.panMotorSpeed, errors);
        checkField("tiltMotorSpeed", source.tiltMotorSpeed,
                   target.tiltMotorSpeed, errors);
        checkField("custom1", source.custom1, target.custom1, errors);
        checkField("custom2", source.custom2, target.custom2, errors);
        checkField("custom3", source.custom3, target.custom3, errors);
        if (source.initString != target.initString)
        {
            std::cout << "Mismatch in field initString" << std::endl;
            ++errors;
        }
    }

    // Invalid and incomplete documents are rejected and the object is not
    // changed.
    const char* const documents[] = {
        "",
        "not json",
        "{\"panTiltParams\": {\"panMotorSpeed\": 1.0",
        "{\"other\": {}}",
        "{\"panTiltParams\": {}}",
        "{\"panTiltParams\": {\"panMotorSpeed\": 9.5}}",
        "{\"panTiltParams\": {\"panMotorSpeed\": \"abc\", "
        "\"tiltMotorSpeed\": 1.0, \"initString\": \"a\", \"custom1\": 1.0, "
        "\"custom2\": 1.0, \"custom3\": 1.0}}",
        "{\"panTiltParams\": {\"panMotorSpeed\": 1.0, "
        "\"tiltMotorSpeed\": 1.0, \"initString\": 5, \"custom1\": 1.0, "
        "\"custom2\": 1.0, \"custom3\": 1.0}}",
        "{\"panTiltParams\": {\"custom1\": null}}",
        "{\"panTiltParams\": [1, 2]}",
        "{\"panTiltParams\": 5}",
        "{\"panTiltParams\": {\"custom1\": 1e999}}"};
    for (const char* document : documents)
    {
        cr::utils::ConfigReader reader;
        pt::PanTiltParams target = sentinelParams();
        const bool accepted = reader.readFromString(document) &&
                              reader.get(target, "panTiltParams");
        if (accepted)
        {
            std::cout << "Accepted document: " << document << std::endl;
            ++errors;
        }
        errors += compareParams(sentinelParams(), target);
    }

    // Deep nesting is rejected without a crash.
    {
        const std::string deep(100000, '[');
        cr::utils::ConfigReader reader;
        if (reader.readFromString(deep))
        {
            return fail("Deeply nested document is accepted");
        }
    }
    return errors == 0;
}



/// Read / write JSON through a file.
bool jsonFileTest()
{
    const pt::PanTiltParams source = jsonParams();
    FileRemover remover(JSON_FILE_NAME);
    int errors{0};

    cr::utils::ConfigReader writer;
    writer.set(source, "panTiltParams");
    if (!writer.writeToFile(JSON_FILE_NAME))
    {
        return fail("Can't write params to file");
    }

    cr::utils::ConfigReader reader;
    if (!reader.readFromFile(JSON_FILE_NAME))
    {
        return fail("Can't open config file");
    }
    pt::PanTiltParams target;
    if (!reader.get(target, "panTiltParams"))
    {
        return fail("Can't read params from file");
    }
    checkField("panMotorSpeed", source.panMotorSpeed, target.panMotorSpeed,
               errors);
    checkField("tiltMotorSpeed", source.tiltMotorSpeed,
               target.tiltMotorSpeed, errors);
    checkField("custom1", source.custom1, target.custom1, errors);
    checkField("custom2", source.custom2, target.custom2, errors);
    checkField("custom3", source.custom3, target.custom3, errors);
    if (source.initString != target.initString)
    {
        std::cout << "Mismatch in field initString" << std::endl;
        ++errors;
    }

    // A file which does not exist.
    cr::utils::ConfigReader missing;
    if (missing.readFromFile("PanTiltTestMissingFile.json"))
    {
        return fail("A missing file is read");
    }

    // The file is removed by the test.
    if (!remover.remove())
    {
        return false;
    }
    return errors == 0;
}



/// Run one test and print the result.
bool runTest(const char* title, bool (*test)())
{
    std::cout << title << ":" << std::endl;
    const bool passed = test();
    std::cout << (passed ? "OK" : "ERROR") << std::endl << std::endl;
    return passed;
}
}



int main()
{
    std::cout << "Test for PanTilt library" << std::endl << std::endl;

    bool passed{true};
    passed = runTest("Copy parameters test", copyParametersTest) && passed;
    passed = runTest("Encode / decode params test without mask",
                     encodeDecodeAllFieldsTest) && passed;
    passed = runTest("Encode / decode params test with mask",
                     encodeDecodeMaskTest) && passed;
    passed = runTest("Encode buffer test", encodeBufferTest) && passed;
    passed = runTest("Decode invalid data test",
                     decodeInvalidDataTest) && passed;
    passed = runTest("Damaged data test", damagedDataTest) && passed;
    passed = runTest("Commands encode / decode test",
                     commandsRoundTripTest) && passed;
    passed = runTest("Invalid commands test",
                     commandsInvalidDataTest) && passed;
    passed = runTest("Version test", versionTest) && passed;
    passed = runTest("Read / write JSON string test",
                     jsonStringTest) && passed;
    passed = runTest("Read / write JSON file test", jsonFileTest) && passed;

    if (g_removeErrors != 0)
    {
        passed = false;
    }
    std::cout << (passed ? "All tests passed" : "Tests failed") << std::endl;
    return passed ? 0 : 1;
}
