#pragma once
#include <cstdint>
#include <string>
#include "ConfigReader.h"



namespace cr
{
namespace pantilt
{
/**
 * @brief Mask for PanTilt params for encoding (serializing).
 *
 * Each flag enables serialization of the PanTiltParams field with the same
 * name (see PanTiltParams::encode(...)).
 */
struct PanTiltParamsMask
{
    /// Serialize panMotorPosition field.
    bool panMotorPosition{ true };
    /// Serialize tiltMotorPosition field.
    bool tiltMotorPosition{ true };
    /// Serialize panAngle field.
    bool panAngle{ true };
    /// Serialize tiltAngle field.
    bool tiltAngle{ true };
    /// Serialize panMotorSpeed field.
    bool panMotorSpeed{ true };
    /// Serialize tiltMotorSpeed field.
    bool tiltMotorSpeed{ true };
    /// Serialize isConnected field.
    bool isConnected{ true };
    /// Serialize isOpen field.
    bool isOpen{ true };
    /// Serialize custom1 field.
    bool custom1{ true };
    /// Serialize custom2 field.
    bool custom2{ true };
    /// Serialize custom3 field.
    bool custom3{ true };
};



/**
 * @brief Class of PanTilt library parameters.
 *
 * The ranges given in the descriptions of the fields are not checked by
 * encode(...), decode(...), the JSON reader and the copy operations: the
 * values are only transported. The implementation of the pan-tilt controller
 * must validate them before use.
 *
 * @note The class is not thread-safe: an object must not be accessed from
 * several threads at the same time without external synchronization.
 */
class PanTiltParams
{
public:

    /// Pan motor position for encoder. Range: 0 - 65535.
    int panMotorPosition{ 0 };
    /// Tilt motor position for encoder. Range: 0 - 65535.
    int tiltMotorPosition{ 0 };
    /// Pan angle. Range: -180.0 to 180.0.
    float panAngle{ 0.0f };
    /// Tilt angle. Range: -180.0 to 180.0.
    float tiltAngle{ 0.0f };
    /// Pan motor speed. Range: -100.0 to 100.0.
    float panMotorSpeed{ 0.0f };
    /// Tilt motor speed. Range: -100.0 to 100.0.
    float tiltMotorSpeed{ 0.0f };
    /// Status defining if the pan-tilt device is connected.
    bool isConnected{ false };
    /// Status defining if the pan-tilt device is initialized (open).
    bool isOpen{ false };
    /// Init string. Format depends on target controller.
    std::string initString{ "" };
    /// PanTilt custom parameter. Value depends on particular pan-tilt
    /// controller. Custom parameters used when particular pan-tilt equipment
    /// has specific unusual parameter.
    float custom1{ 0.0f };
    /// PanTilt custom parameter. Value depends on particular pan-tilt
    /// controller. Custom parameters used when particular pan-tilt equipment
    /// has specific unusual parameter.
    float custom2{ 0.0f };
    /// PanTilt custom parameter. Value depends on particular pan-tilt
    /// controller. Custom parameters used when particular pan-tilt equipment
    /// has specific unusual parameter.
    float custom3{ 0.0f };

    /// Macro from ConfigReader to make params readable/writable from JSON.
    JSON_READABLE(PanTiltParams, panMotorSpeed, tiltMotorSpeed, initString,
                  custom1, custom2, custom3)

    /**
     * @brief Default constructor. All fields have the default values given
     * in the declaration.
     */
    PanTiltParams() = default;

    /**
     * @brief Copy constructor. Copies all fields.
     * @param src Source object.
     */
    PanTiltParams(const PanTiltParams& src) = default;

    /**
     * @brief Copy assignment operator.
     *
     * Copies all fields except isConnected and isOpen: the object keeps its
     * own values of these two fields.
     *
     * @param src Source object.
     * @return Reference to this object.
     */
    PanTiltParams& operator= (const PanTiltParams& src);

    /**
     * @brief Encode (serialize) params.
     *
     * The method doesn't encode initString. The size of the encoded data
     * depends on the mask: 5 bytes (header and mask only) if all params are
     * excluded and 43 bytes if all params are encoded.
     *
     * @param data Pointer to buffer to store serialized params. Must not be
     * nullptr. The buffer must be valid for writing bufferSize bytes.
     * @param bufferSize Size of buffer, bytes. Must be >= 43 independently
     * of the mask.
     * @param size Size of encoded (serialized) data, bytes. Will be
     * <= bufferSize. Set to 0 if FALSE is returned.
     * @param mask Pointer to mask structure. Used to exclude particular
     * params from encoding (from serialization). If nullptr all params are
     * encoded.
     * @return TRUE if params encoded (serialized) or FALSE if:
     * 1. data is nullptr.
     * 2. bufferSize is less than 43.
     * If FALSE is returned the buffer is not changed.
     */
    bool encode(uint8_t* data, int bufferSize, int& size,
                                             PanTiltParamsMask* mask = nullptr);

    /**
     * @brief Decode (deserialize) params encoded by encode(...) method.
     *
     * The method checks the data type, the version (major and minor) and the
     * size of the data before reading any field. Params which are excluded
     * by the mask of the encoded data are set to 0 (FALSE). A boolean field
     * is FALSE if its byte is 0 and TRUE for any other value. Unused bits
     * of the mask and bytes after the last encoded field are ignored. Values
     * of the fields are not checked against any range. initString is not
     * changed.
     *
     * @param data Pointer to buffer with encoded (serialized) params. Must
     * not be nullptr. The buffer must be valid for reading dataSize bytes.
     * @param dataSize Size of encoded (serialized) params, bytes.
     * @return TRUE if params decoded or FALSE if the data is nullptr, too
     * short or has wrong data type or version. If FALSE is returned the
     * object is not changed.
     */
    bool decode(uint8_t* data, int dataSize);
};



/**
 * @brief Enum of PanTilt params.
 *
 * @note CUSTOM_3 must stay the last value: PanTilt::decodeCommand(...) uses
 * the range from PAN_MOTOR_POSITION to CUSTOM_3 to check the IDs.
 */
enum class PanTiltParam
{
    /// Pan motor position for encoder. Range: 0 - 65535.
    PAN_MOTOR_POSITION = 1,
    /// Tilt motor position for encoder. Range: 0 - 65535.
    TILT_MOTOR_POSITION,
    /// Pan angle. Range: -180.0 to 180.0.
    PAN_ANGLE,
    /// Tilt angle. Range: -180.0 to 180.0.
    TILT_ANGLE,
    /// Pan motor speed. Positive speed is clockwise,
    /// negative is counterclockwise.
    PAN_MOTOR_SPEED,
    /// Tilt motor speed. Positive speed is clockwise,
    /// negative is counterclockwise.
    TILT_MOTOR_SPEED,
    /// Status defining if the pan-tilt device is connected.
    IS_CONNECTED,
    /// Status defining if the pan-tilt device is initialized (open).
    IS_OPEN,
    /// Custom param. Value depends on implementation.
    CUSTOM_1,
    /// Custom param. Value depends on implementation.
    CUSTOM_2,
    /// Custom param. Value depends on implementation.
    CUSTOM_3
};



/**
 * @brief Enum of PanTilt library commands.
 *
 * @note MOVE_PAN_TILT must stay the last value: PanTilt::decodeCommand(...)
 * uses the range from RESTART to MOVE_PAN_TILT to check the IDs.
 */
enum class PanTiltCommand
{
    /// Restart Pan-Tilt device.
    RESTART = 1,
    /// Stop Pan-Tilt device, block all running commands and left device in
    /// current state.
    STOP,
    /// Go to given pan motor position.
    GO_TO_PAN_POSITION,
    /// Go to given tilt motor position.
    GO_TO_TILT_POSITION,
    /// Go to given pan and tilt motor position.
    GO_TO_PAN_TILT_POSITION,
    /// Go to given pan angle.
    GO_TO_PAN_ANGLE,
    /// Go to given tilt angle.
    GO_TO_TILT_ANGLE,
    /// Go to given pan and tilt angle.
    GO_TO_PAN_TILT_ANGLE,
    /// Go to home position.
    GO_TO_HOME,
    /// Move pan motor with given speed. Positive speed is clockwise,
    /// negative is counterclockwise.
    MOVE_PAN,
    /// Move tilt motor with given speed. Positive speed is clockwise,
    /// negative is counterclockwise.
    MOVE_TILT,
    /// Move pan and tilt motors with given speed. Positive speed is
    /// clockwise, negative is counterclockwise. First argument is pan speed,
    /// second is tilt speed.
    MOVE_PAN_TILT
};



/**
 * @brief Library PanTilt class with PanTilt methods.
 *
 * The class is an interface: it can be used as a base class only. The static
 * methods use no shared state and can be called from any thread.
 */
class PanTilt
{
public:

    /**
     * @brief Class destructor.
     */
    virtual ~PanTilt();

    /**
     * @brief Get the version of the PanTilt class.
     * @return A string representing the version: "Major.Minor.Patch"
     */
    static std::string getVersion();

    /**
     * @brief Open pan-tilt device.
     * @param initString Init string. Format depends on target controller.
     * @return TRUE if the pan-tilt controller is init or FALSE if not.
     */
    virtual bool openPanTilt(std::string initString) = 0;

    /**
     * @brief Init pan-tilt device with parameters structure.
     * Can be used instead of openPanTilt(...) method.
     * @param params Parameters structure, includes the init string. The
     * values are not checked by the PanTilt class: the implementation must
     * validate them.
     * @return TRUE if the pan-tilt controller init or FALSE if not.
     */
    virtual bool initPanTilt(PanTiltParams& params) = 0;

    /**
     * @brief Close pan-tilt controller connection.
     */
    virtual void closePanTilt() = 0;

    /**
     * @brief Get pan-tilt controller is open status.
     * @return TRUE if the pan-tilt controller is open or FALSE if not.
     */
    virtual bool isPanTiltOpen() = 0;

    /**
     * @brief Get pan-tilt controller is connected status.
     * @return TRUE if the pan-tilt controller is connected or FALSE if not.
     */
    virtual bool isPanTiltConnected() = 0;

    /**
     * @brief Set the value for a specific library parameter.
     * @note The implementation must be thread-safe.
     * @param id The identifier of the library parameter.
     * @param value The value to set for the parameter. The implementation
     * must validate it (including NaN and infinity).
     * @return TRUE if the parameter was successfully set, FALSE otherwise.
     */
    virtual bool setParam(PanTiltParam id, float value) = 0;

    /**
     * @brief Get the value of a specific library parameter.
     * @note The implementation must be thread-safe.
     * @param id The identifier of the library parameter.
     * @return The value of the specified parameter or -1 if the parameter
     * doesn't exist.
     */
    virtual float getParam(PanTiltParam id) = 0;

    /**
     * @brief Get the structure containing all library parameters.
     * @note The implementation must be thread-safe.
     * @param params Reference to a PanTiltParams structure.
     */
    virtual void getParams(PanTiltParams& params) = 0;

    /**
     * @brief Execute a PanTilt command.
     * @note The implementation must be thread-safe.
     * @param id The identifier of the library command to be executed.
     * @param arg1 The argument value used by the command. The implementation
     * must validate it (including NaN and infinity).
     * @param arg2 The argument value used by the command. The implementation
     * must validate it (including NaN and infinity).
     * @return TRUE if the command was executed successfully, FALSE otherwise.
     */
    virtual bool executeCommand(PanTiltCommand id,
                                float arg1 = 0.0f, float arg2 = 0.0f) = 0;

    /**
     * @brief Encode set param command.
     *
     * The method doesn't check the id and the value: they are encoded as
     * given.
     *
     * @param data Pointer to data buffer. Must not be nullptr. The buffer
     * must be valid for writing 11 bytes.
     * @param size Size of encoded command. Will be 11 bytes. Set to 0 if
     * data is nullptr (nothing is encoded).
     * @param id PanTilt parameter id.
     * @param value PanTilt parameter value.
     */
    static void encodeSetParamCommand(uint8_t* data, int& size,
                                     PanTiltParam id, float value);

    /**
     * @brief Encode command.
     *
     * The method doesn't check the id and the arguments: they are encoded as
     * given.
     *
     * @param data Pointer to data buffer. Must not be nullptr. The buffer
     * must be valid for writing 15 bytes.
     * @param size Size of encoded command. Will be 15 bytes. Set to 0 if
     * data is nullptr (nothing is encoded).
     * @param id PanTilt command ID.
     * @param arg1 The argument value used by the command.
     * @param arg2 The argument value used by the command.
     */
    static void encodeCommand(uint8_t* data, int& size,
                       PanTiltCommand id, float arg1 = 0.0f, float arg2 = 0.0f);

    /**
     * @brief Decode command.
     *
     * The method checks the data type, the version (major and minor), the
     * exact size of the command (11 bytes for SET_PARAM, 15 bytes for
     * COMMAND) and the range of the ID. The values of the arguments are not
     * checked.
     *
     * @param data Pointer to command data. Must not be nullptr. The buffer
     * must be valid for reading size bytes.
     * @param size Size of data, bytes. Must be 11 for SET_PARAM command and
     * 15 for COMMAND.
     * @param paramId Output param ID. Changed only if 1 is returned.
     * @param commandId Output command ID. Changed only if 0 is returned.
     * @param value1 Parameter value (SET_PARAM command) or first command
     * argument (COMMAND). Changed only if 0 or 1 is returned.
     * @param value2 Second command argument (COMMAND) or 0 (SET_PARAM
     * command). Changed only if 0 or 1 is returned.
     * @return 0 - command decoded, 1 - set param command decoded, -1 - error
     * (data is nullptr, wrong size, version, data type or ID).
     */
    static int decodeCommand(uint8_t* data,
        int size,
        PanTiltParam& paramId,
        PanTiltCommand& commandId,
        float& value1, float& value2);

    /**
     * @brief Decode and execute command.
     * @note The implementation must be thread-safe.
     * @param data Pointer to command data. Must not be nullptr.
     * @param size Size of data, bytes. Must be 11 for SET_PARAM command and
     * 15 for COMMAND.
     * @return TRUE if command decoded and executed or FALSE if not.
     */
    virtual bool decodeAndExecuteCommand(uint8_t* data, int size) = 0;

protected:

    /**
     * @brief Default constructor. The class can be used as a base only.
     */
    PanTilt() = default;

    /**
     * @brief Copy constructor.
     * @param src Source object.
     */
    PanTilt(const PanTilt& src) = default;

    /**
     * @brief Move constructor.
     * @param src Source object.
     */
    PanTilt(PanTilt&& src) = default;

    /**
     * @brief Copy assignment operator.
     * @param src Source object.
     * @return Reference to this object.
     */
    PanTilt& operator= (const PanTilt& src) = default;

    /**
     * @brief Move assignment operator.
     * @param src Source object.
     * @return Reference to this object.
     */
    PanTilt& operator= (PanTilt&& src) = default;
};
}
}
