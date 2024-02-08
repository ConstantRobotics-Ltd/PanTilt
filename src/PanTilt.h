#include <string>
#include <cstdint>
#include "ConfigReader.h"



namespace cr
{
namespace pantilt
{
/**
 * @brief Mask for PanTilt library params for encoding (serializing).
 */
struct PanTiltParamsMask
{
    bool panMotorPosition{ true };
    bool tiltMotorPosition{ true };
    bool panAngle{ true };
    bool tiltAngle{ true };
    bool panMotorSpeed{ true };
    bool tiltMotorSpeed{ true };
    bool isConnected{ true };
    bool isInitialized{ true };
    bool custom1{ true };
    bool custom2{ true };
    bool custom3{ true };
};

/**
 * @brief Class of PanTilt library parameters.
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
    /// Tilt angle. Range: -90.0 to 90.0.
    float tiltAngle{ 0.0f };
    /// Pan motor speed. Range: -100.0 to 100.0.
    float panMotorSpeed{ 0.0f };
    /// Tilt motor speed. Range: -100.0 to 100.0. 
    float tiltMotorSpeed{ 0.0f };
    /// Status defining if the pan-tilt device is connected.
    bool isConnected{ false };
	/// Status defining if the pan-tilt device is initialized.
    bool isInitialized{ false };
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
    JSON_READABLE(PanTiltParams, panMotorPosition, tiltMotorPosition, panAngle,
        tiltAngle, panMotorSpeed, tiltMotorSpeed, isConnected, isInitialized,
        initString, custom1, custom2, custom3)

    /// operator =
    PanTiltParams& operator= (const PanTiltParams& src);

    /**
     * @brief Encode (serialize) params.
     * @param data Pointer to buffer to store serialized params.
     * @param  bufferSize Size of buffer.
     * @param size Size of encoded (serialized) data. Will be <= bufferSize.
     * @param mask Pointer to mask structure. Used to exclude particular
     * params from encoding (from serialization).
     * @return TRUE if params encoded (serialized) or FALSE if:
     * 1. bufferSize less tha size of serialized params.
     * 2. Can be other errors depends on implementation.
     */
    bool encode(uint8_t* data, int bufferSize, int& size,
                                             PanTiltParamsMask* mask = nullptr);

    /**
     * @brief Decode (deserialize) params.
     * @param data Pointer to buffer with encoded (serialized) params.
     * @param dataSize Size of encoded (serialized) params.
     * @return TRUE if params decoded, FALSE otherwise.
     */
    bool decode(uint8_t* data, int dataSize);
};

/**
 * @brief Enum of PanTilt library params.
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
    /// Pan motor speed.  Positive speed is clockwise, 
    /// negative is counterclockwise.
    PAN_MOTOR_SPEED,
    /// Tilt motor speed.  Positive speed is clockwise, 
    /// negative is counterclockwise.
    TILT_MOTOR_SPEED,
    /// Status defining if the pan-tilt device is connected.
    IS_CONNECTED,
    /// Status defining if the pan-tilt device is initialized.
    IS_INITIALIZED
};

/**
 * @brief Enum of PanTilt library commands.
 */
enum class PanTiltCommand
{
    /// Restart Pan-Tilt device.
    RESTART = 1,
    /// Stop Pan-Tilt device, block all running commands and left device in current state.
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
	/// Move pan and tilt motors with given speed. Positive speed is clockwise,
    /// negative is counterclockwise.
	MOVE_PAN_TILT
};

/**
 * @brief Library PanTilt class with PanTilt methods.
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
     * Can be used instead of open(...) method.
     * @param initString Init string. Format depends on target controller.
     * @return TRUE if the pan-tilt controller init or FALSE if not.
     */
    virtual bool initPanTilt(PanTiltParams& params) = 0;

    /**
     * @brief Close pan-tilt controller connection.
     */
    virtual void closePanTilt() = 0;

    /**
     * @brief Get pan-tilt controller is opened status.
     * @return TRUE if the pan-tilt controller is open or FALSE if not.
     */
    virtual bool isPanTiltOpened() = 0;

    /**
     * @brief Get pan-tilt controller is connected status.
     * @return TRUE if the pan-tilt controller is open or FALSE if not.
     */
    virtual bool isPanTiltConnected() = 0;

    /**
     * @brief Set the value for a specific library parameter.
     * @param id The identifier of the library parameter.
     * @param value The value to set for the parameter.
     * @return TRUE if the parameter was successfully set, FALSE otherwise.
     */
    virtual bool setParam(PanTiltParam id, float value) = 0;

    /**
     * @brief Get the value of a specific library parameter.
     * @param id The identifier of the library parameter.
     * @return The value of the specified parameter.
     */
    virtual float getParam(PanTiltParam id) = 0;

    /**
     * @brief Get the structure containing all library parameters.
     * @param params Reference to a PanTiltParams structure.
     */
    virtual void getParams(PanTiltParams& params) = 0;

    /**
     * @brief Execute a PanTilt command.
     * @param id The identifier of the library command to be executed.
     * @return TRUE if the command was executed successfully, FALSE otherwise.
     */
    virtual bool executeCommand(PanTiltCommand id,
                                      float arg1 = 0.0f, float arg2 = 0.0f) = 0;

    /**
     * @brief Encode set param command.
     * @param data Pointer to data buffer. Must have size >= 11.
     * @param size Size of encoded data.
     * @param id PanTilt parameter id.
     * @param value PanTilt parameter value.
     */
    static void encodeSetParamCommand(uint8_t* data, int& size, PanTiltParam id,
                                                                   float value);

    /**
     * @brief Encode command.
     * @param data Pointer to data buffer. Must have size >= 7.
     * @param size Size of encoded data.
     * @param id PanTilt command ID.
     */
    static void encodeCommand(uint8_t* data, int& size, PanTiltCommand id);

    /**
     * @brief Decode command.
     * @param data Pointer to command data.
     * @param size Size of data.
     * @param paramId Output command ID.
     * @param commandId Output command ID.
     * @param value Param or command value.
     * @return 0 - command decoded, 1 - set param command decoded, -1 - error.
     */
    static int decodeCommand(uint8_t* data,
        int size,
        PanTiltParam& paramId,
        PanTiltCommand& commandId,
        float& value);

    /**
     * @brief Decode and execute command.
     * @param data Pointer to command data.
     * @param size Size of data.
     * @return TRUE if command decoded and executed or FALSE if not.
     */
    virtual bool decodeAndExecuteCommand(uint8_t* data, int size) = 0;
};
}
}