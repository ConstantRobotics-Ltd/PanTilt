#include <string>
#include <cstdint>
#include "ConfigReader.h"



namespace cr::pantilt
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
    bool panTiltMotorPosition{ true };
    bool panTiltAngle{ true };
    bool panMotorSpeed{ true };
    bool tiltMotorSpeed{ true };
    bool panTiltMotorSpeed{ true };
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
    /// Pan angle. Range: -180.0 - 180.0.
    float panAngle{ 0.0f };
    /// Tilt angle. Range: -float tiltAngle{ 0.0f };
    float tiltAngle{ 0.0f };
    /// Pan tilt motor position for encoder. Range: 0 - 65535.
    int panTiltMotorPosition{ 0 };
    /// Pan tilt angle. Range: -180.0 - 180.0.
    float panTiltAngle{ 0.0f };
    /// Pan motor speed. Range: 0.0 - 100.0.
    float panMotorSpeed{ 0.0f };
    /// Tilt motor speed. Range: 0.0 - 100.0. 
    float tiltMotorSpeed{ 0.0f };
    /// Pan tilt motor speed. Range: 0.0 - 100.0.
    float panTiltMotorSpeed{ 0.0f };

    /// Macro from ConfigReader to make params readable/writable from JSON.
    JSON_READABLE(PanTiltParams, panMotorPosition, tiltMotorPosition, panAngle,
        tiltAngle, panTiltMotorPosition, panTiltAngle, panMotorSpeed, tiltMotorSpeed,
        panTiltMotorSpeed)

    /// operator =
        PanTiltParams& operator= (const PanTiltParams& src);
    
    /**
     * @brief Copy constructor.
     */
   // PanTiltParams(const PanTiltParams& src);

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
    /// Pan angle. Range: -180.0 - 180.0.
    PAN_ANGLE,
    /// Tilt angle. Range: -90.0 - 90.0.
    TILT_ANGLE,
    /// Pan tilt motor position for encoder. Range: 0 - 65535.
    PAN_TILT_MOTOR_POSITION,
    /// Pan tilt angle. Range: -180.0 - 180.0.
    PAN_TILT_ANGLE,
    /// Pan motor speed. Range: 0.0 - 100.0.
    PAN_MOTOR_SPEED,
    /// Tilt motor speed. Range: 0.0 - 100.0.
    TILT_MOTOR_SPEED,
    /// Pan tilt motor speed. Range: 0.0 - 100.0.
    PAN_TILT_MOTOR_SPEED
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
    GO_TO_HOME
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
     * @brief Set the value for a specific library parameter.
     * @param id The identifier of the library parameter.
     * @param value The value to set for the parameter.
     * @return TRUE if the parameter was successfully set, FALSE otherwise.
     */
    bool setParam(PanTiltParam id, float value);

    /**
     * @brief Get the value of a specific library parameter.
     * @param id The identifier of the library parameter.
     * @return The value of the specified parameter.
     */
    float getParam(PanTiltParam id) const;

    /**
     * @brief Get the structure containing all library parameters.
     * @param params Reference to a PanTiltParams structure.
     */
    void getParams(PanTiltParams& params) const;

    /**
     * @brief Execute a PanTilt command.
     * @param id The identifier of the library command to be executed.
     * @return TRUE if the command was executed successfully, FALSE otherwise.
     */
    bool executeCommand(PanTiltCommand id);

    /**
     * @brief Any useful method of library.
     * @param value Output value.
     * @return TRUE if success, FALSE otherwise.
     */
    bool doSomething(int& value);

private:

    /// Library parameters structure.
    PanTiltParams m_params;
    /// Any private variable.
    int m_variable{ 0 };
};
}