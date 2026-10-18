#include "PanTilt.h"
#include "PanTiltVersion.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>



namespace
{
// Encoded data stores int and float fields as raw 4 bytes values in the byte
// order of the host.
static_assert(sizeof(int) == 4, "int must be 4 bytes");
static_assert(sizeof(float) == 4, "float must be 4 bytes");
static_assert(PAN_TILT_MAJOR_VERSION >= 0 && PAN_TILT_MAJOR_VERSION <= 255,
              "major version must fit into one byte");
static_assert(PAN_TILT_MINOR_VERSION >= 0 && PAN_TILT_MINOR_VERSION <= 255,
              "minor version must fit into one byte");

/// Data type of the encoded params (first byte of the encoded data).
constexpr uint8_t PARAMS_DATA_TYPE{0x02};
/// Data type of the encoded COMMAND (first byte of the encoded command).
constexpr uint8_t COMMAND_DATA_TYPE{0x00};
/// Data type of the encoded SET_PARAM command (first byte of the command).
constexpr uint8_t SET_PARAM_DATA_TYPE{0x01};

/// Major version of the data format. The version is stored in each message.
constexpr uint8_t MAJOR_VERSION{static_cast<uint8_t>(PAN_TILT_MAJOR_VERSION)};
/// Minor version of the data format. The version is stored in each message.
constexpr uint8_t MINOR_VERSION{static_cast<uint8_t>(PAN_TILT_MINOR_VERSION)};
/// Position of the major version in the encoded data.
constexpr int MAJOR_VERSION_POS{1};
/// Position of the minor version in the encoded data.
constexpr int MINOR_VERSION_POS{2};

/// Size of an int or float field in the encoded data, bytes.
constexpr int VALUE_SIZE{4};
/// Size of a bool field in the encoded data, bytes.
constexpr int FLAG_SIZE{1};
/// Number of fields of PanTiltParams which are encoded.
constexpr int FIELDS_NUMBER{11};
/// Position of the mask of params in the encoded params.
constexpr int PARAMS_MASK_POS{3};
/// Size of the mask of params, bytes. One bit per field, the first field
/// uses the most significant bit of the first byte.
constexpr int PARAMS_MASK_SIZE{2};
/// Size of the encoded params header: data type, version and mask.
constexpr int PARAMS_HEADER_SIZE{PARAMS_MASK_POS + PARAMS_MASK_SIZE};
/// Maximum size of the encoded params (all fields encoded): 9 int / float
/// fields and 2 bool fields.
constexpr int PARAMS_MAX_SIZE{PARAMS_HEADER_SIZE + 9 * VALUE_SIZE +
                              2 * FLAG_SIZE};
static_assert(PARAMS_MAX_SIZE == 43, "wrong size of encoded params");

/// Position of the param ID or command ID in the encoded command.
constexpr int COMMAND_ID_POS{3};
/// Position of the param value in the encoded SET_PARAM command.
constexpr int SET_PARAM_VALUE_POS{7};
/// Position of the first argument in the encoded COMMAND.
constexpr int COMMAND_ARG1_POS{7};
/// Position of the second argument in the encoded COMMAND.
constexpr int COMMAND_ARG2_POS{11};
/// Size of the encoded SET_PARAM command, bytes.
constexpr int SET_PARAM_COMMAND_SIZE{11};
/// Size of the encoded COMMAND, bytes.
constexpr int COMMAND_SIZE{15};

/// First valid param ID.
constexpr int PARAM_ID_FIRST{
    static_cast<int>(cr::pantilt::PanTiltParam::PAN_MOTOR_POSITION)};
/// Last valid param ID.
constexpr int PARAM_ID_LAST{
    static_cast<int>(cr::pantilt::PanTiltParam::CUSTOM_3)};
/// First valid command ID.
constexpr int COMMAND_ID_FIRST{
    static_cast<int>(cr::pantilt::PanTiltCommand::RESTART)};
/// Last valid command ID.
constexpr int COMMAND_ID_LAST{
    static_cast<int>(cr::pantilt::PanTiltCommand::MOVE_PAN_TILT)};



/// Location of a PanTiltParams field in the encoded data.
struct FieldRef
{
    /// Pointer to the field of a PanTiltParams object.
    void* value{nullptr};
    /// Size of the field in the encoded data: VALUE_SIZE or FLAG_SIZE.
    int size{0};
};



/// Get the table of PanTiltParams fields in the order they are stored in the
/// encoded data (the same order as the bits of the mask).
std::array<FieldRef, FIELDS_NUMBER> makeFieldTable(
        cr::pantilt::PanTiltParams& p)
{
    return {{
        {&p.panMotorPosition, VALUE_SIZE},
        {&p.tiltMotorPosition, VALUE_SIZE},
        {&p.panAngle, VALUE_SIZE},
        {&p.tiltAngle, VALUE_SIZE},
        {&p.panMotorSpeed, VALUE_SIZE},
        {&p.tiltMotorSpeed, VALUE_SIZE},
        {&p.isConnected, FLAG_SIZE},
        {&p.isOpen, FLAG_SIZE},
        {&p.custom1, VALUE_SIZE},
        {&p.custom2, VALUE_SIZE},
        {&p.custom3, VALUE_SIZE}
    }};
}



/// Get the flags of the mask in the order of the table of fields.
std::array<bool, FIELDS_NUMBER> makeMaskTable(
        const cr::pantilt::PanTiltParamsMask& m)
{
    return {{
        m.panMotorPosition,
        m.tiltMotorPosition,
        m.panAngle,
        m.tiltAngle,
        m.panMotorSpeed,
        m.tiltMotorSpeed,
        m.isConnected,
        m.isOpen,
        m.custom1,
        m.custom2,
        m.custom3
    }};
}



/// Get index of the mask byte which holds the bit of the field.
constexpr int maskByteIndex(int field)
{
    return field / 8;
}



/// Get the bit of the field inside of its mask byte.
constexpr uint8_t maskBit(int field)
{
    return static_cast<uint8_t>(128 >> (field % 8));
}



/// Check that the field is selected by the mask stored in the encoded params.
bool isEncoded(const uint8_t* data, int field)
{
    return (data[PARAMS_MASK_POS + maskByteIndex(field)] & maskBit(field)) != 0;
}
}



cr::pantilt::PanTiltParams &cr::pantilt::PanTiltParams::operator= (
        const cr::pantilt::PanTiltParams& src)
{
    // Check yourself.
    if (this == &src)
    {
        return *this;
    }

    // Copy params. isConnected and isOpen are not copied.
    panMotorPosition = src.panMotorPosition;
    tiltMotorPosition = src.tiltMotorPosition;
    panAngle = src.panAngle;
    tiltAngle = src.tiltAngle;
    panMotorSpeed = src.panMotorSpeed;
    tiltMotorSpeed = src.tiltMotorSpeed;
    custom1 = src.custom1;
    custom2 = src.custom2;
    custom3 = src.custom3;
    initString = src.initString;

    return *this;
}



bool cr::pantilt::PanTiltParams::encode(uint8_t* data, int bufferSize,
                                        int& size, PanTiltParamsMask* mask)
{
    // Check pointer and buffer size. The buffer must be able to hold all
    // params independently of the mask.
    if (data == nullptr || bufferSize < PARAMS_MAX_SIZE)
    {
        size = 0;
        return false;
    }

    // Fill header.
    data[0] = PARAMS_DATA_TYPE;
    data[MAJOR_VERSION_POS] = MAJOR_VERSION;
    data[MINOR_VERSION_POS] = MINOR_VERSION;

    // Prepare params mask.
    PanTiltParamsMask paramsMask;
    if (mask != nullptr)
    {
        paramsMask = *mask;
    }
    const auto selected = makeMaskTable(paramsMask);
    const auto fields = makeFieldTable(*this);

    // Fill bit mask for params.
    std::memset(&data[PARAMS_MASK_POS], 0,
                static_cast<size_t>(PARAMS_MASK_SIZE));
    for (int i = 0; i < FIELDS_NUMBER; ++i)
    {
        if (selected[static_cast<size_t>(i)])
        {
            uint8_t& maskByte = data[PARAMS_MASK_POS + maskByteIndex(i)];
            maskByte = static_cast<uint8_t>(maskByte | maskBit(i));
        }
    }

    // Copy params to buffer.
    int pos = PARAMS_HEADER_SIZE;
    for (int i = 0; i < FIELDS_NUMBER; ++i)
    {
        if (!selected[static_cast<size_t>(i)])
        {
            continue;
        }
        const FieldRef& field = fields[static_cast<size_t>(i)];
        if (field.size == FLAG_SIZE)
        {
            data[pos] = *static_cast<const bool*>(field.value) ? 0x01 : 0x00;
        }
        else
        {
            std::memcpy(&data[pos], field.value,
                        static_cast<size_t>(VALUE_SIZE));
        }
        pos += field.size;
    }

    size = pos;

    return true;
}



bool cr::pantilt::PanTiltParams::decode(uint8_t* data, int dataSize)
{
    // Check pointer and data size (header must be complete).
    if (data == nullptr || dataSize < PARAMS_HEADER_SIZE)
    {
        return false;
    }

    // Check data type and version.
    if (data[0] != PARAMS_DATA_TYPE ||
        data[MAJOR_VERSION_POS] != MAJOR_VERSION ||
        data[MINOR_VERSION_POS] != MINOR_VERSION)
    {
        return false;
    }

    // Check that all fields selected by the mask are inside the data. This
    // object is changed only if the whole data is valid.
    const auto fields = makeFieldTable(*this);
    int requiredSize = PARAMS_HEADER_SIZE;
    for (int i = 0; i < FIELDS_NUMBER; ++i)
    {
        if (isEncoded(data, i))
        {
            requiredSize += fields[static_cast<size_t>(i)].size;
        }
    }
    if (dataSize < requiredSize)
    {
        return false;
    }

    // Decode params. Fields not selected by the mask are set to zero.
    int pos = PARAMS_HEADER_SIZE;
    for (int i = 0; i < FIELDS_NUMBER; ++i)
    {
        const FieldRef& field = fields[static_cast<size_t>(i)];
        if (!isEncoded(data, i))
        {
            std::memset(field.value, 0, static_cast<size_t>(field.size));
            continue;
        }
        if (field.size == FLAG_SIZE)
        {
            // Any non-zero byte is TRUE: a bool object must not receive an
            // arbitrary byte.
            *static_cast<bool*>(field.value) = data[pos] != 0x00;
        }
        else
        {
            std::memcpy(field.value, &data[pos],
                        static_cast<size_t>(VALUE_SIZE));
        }
        pos += field.size;
    }

    return true;
}



cr::pantilt::PanTilt::~PanTilt() = default;



std::string cr::pantilt::PanTilt::getVersion()
{
    return PAN_TILT_VERSION;
}



void cr::pantilt::PanTilt::encodeSetParamCommand(
        uint8_t* data, int& size, PanTiltParam id, float value)
{
    // Nothing can be encoded without a buffer.
    if (data == nullptr)
    {
        size = 0;
        return;
    }

    // Fill header.
    data[0] = SET_PARAM_DATA_TYPE;
    data[MAJOR_VERSION_POS] = MAJOR_VERSION;
    data[MINOR_VERSION_POS] = MINOR_VERSION;

    // Fill data.
    const int paramId = static_cast<int>(id);
    std::memcpy(&data[COMMAND_ID_POS], &paramId,
                static_cast<size_t>(VALUE_SIZE));
    std::memcpy(&data[SET_PARAM_VALUE_POS], &value,
                static_cast<size_t>(VALUE_SIZE));
    size = SET_PARAM_COMMAND_SIZE;
}



void cr::pantilt::PanTilt::encodeCommand(
        uint8_t* data, int& size, PanTiltCommand id, float arg1, float arg2)
{
    // Nothing can be encoded without a buffer.
    if (data == nullptr)
    {
        size = 0;
        return;
    }

    // Fill header.
    data[0] = COMMAND_DATA_TYPE;
    data[MAJOR_VERSION_POS] = MAJOR_VERSION;
    data[MINOR_VERSION_POS] = MINOR_VERSION;

    // Fill data.
    const int commandId = static_cast<int>(id);
    std::memcpy(&data[COMMAND_ID_POS], &commandId,
                static_cast<size_t>(VALUE_SIZE));
    std::memcpy(&data[COMMAND_ARG1_POS], &arg1,
                static_cast<size_t>(VALUE_SIZE));
    std::memcpy(&data[COMMAND_ARG2_POS], &arg2,
                static_cast<size_t>(VALUE_SIZE));
    size = COMMAND_SIZE;
}



int cr::pantilt::PanTilt::decodeCommand(
        uint8_t* data, int size, PanTiltParam& paramId,
        PanTiltCommand& commandId, float& value1, float& value2)
{
    // Check pointer and size: SET_PARAM is the shortest command.
    if (data == nullptr || size < SET_PARAM_COMMAND_SIZE)
    {
        return -1;
    }

    // Check version.
    if (data[MAJOR_VERSION_POS] != MAJOR_VERSION ||
        data[MINOR_VERSION_POS] != MINOR_VERSION)
    {
        return -1;
    }

    // Extract ID.
    int id = 0;
    std::memcpy(&id, &data[COMMAND_ID_POS], static_cast<size_t>(VALUE_SIZE));

    // Check command type. The size is exact for each type and an ID outside
    // of the enum range is an error: such an ID must not reach the
    // implementation. Output arguments are changed only for a valid command.
    if (data[0] == COMMAND_DATA_TYPE)
    {
        if (size != COMMAND_SIZE ||
            id < COMMAND_ID_FIRST || id > COMMAND_ID_LAST)
        {
            return -1;
        }
        commandId = static_cast<PanTiltCommand>(id);
        std::memcpy(&value1, &data[COMMAND_ARG1_POS],
                    static_cast<size_t>(VALUE_SIZE));
        std::memcpy(&value2, &data[COMMAND_ARG2_POS],
                    static_cast<size_t>(VALUE_SIZE));
        return 0;
    }
    if (data[0] == SET_PARAM_DATA_TYPE)
    {
        if (size != SET_PARAM_COMMAND_SIZE ||
            id < PARAM_ID_FIRST || id > PARAM_ID_LAST)
        {
            return -1;
        }
        paramId = static_cast<PanTiltParam>(id);
        std::memcpy(&value1, &data[SET_PARAM_VALUE_POS],
                    static_cast<size_t>(VALUE_SIZE));
        value2 = 0.0f;
        return 1;
    }

    return -1;
}
