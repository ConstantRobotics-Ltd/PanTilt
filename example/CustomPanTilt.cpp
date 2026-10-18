#include "CustomPanTilt.h"
#include "CustomPanTiltVersion.h"
#include <mutex>
#include <string>



namespace
{
/// Minimum motor position, encoder units.
constexpr int MOTOR_POSITION_MIN{0};
/// Maximum motor position, encoder units (maximum of a 16 bit value).
constexpr int MOTOR_POSITION_MAX{65535};
/// Minimum pan angle, degrees.
constexpr float PAN_ANGLE_MIN{-180.0f};
/// Maximum pan angle, degrees.
constexpr float PAN_ANGLE_MAX{180.0f};
/// Minimum tilt angle, degrees.
constexpr float TILT_ANGLE_MIN{-90.0f};
/// Maximum tilt angle, degrees.
constexpr float TILT_ANGLE_MAX{0.0f};
/// Minimum motor speed, degrees per second.
constexpr float MOTOR_SPEED_MIN{0.0f};
/// Maximum motor speed, degrees per second.
constexpr float MOTOR_SPEED_MAX{100.0f};

/// Limit of float values which can be converted to int. 2^31 is exactly
/// representable as float, so (-LIMIT <= value < LIMIT) is the exact range
/// of int.
constexpr float INT_RANGE_LIMIT{2147483648.0f};



/// Convert float param value to int. A conversion of NaN or of a value outside
/// of the int range is undefined behavior, so such values are rejected.
bool toInt(float value, int& result)
{
    if (!(value >= -INT_RANGE_LIMIT && value < INT_RANGE_LIMIT))
    {
        return false;
    }
    result = static_cast<int>(value);
    return true;
}



/// Assign the value to the target if the value is inside [minValue, maxValue].
bool assignInRange(int& target, int value, int minValue, int maxValue)
{
    if (value < minValue || value > maxValue)
    {
        return false;
    }
    target = value;
    return true;
}



/// Assign the float value to the target if the value is inside
/// [minValue, maxValue]. NaN is rejected: the comparison is false for NaN.
bool assignFloatInRange(float& target, float value,
                        float minValue, float maxValue)
{
    if (!(value >= minValue && value <= maxValue))
    {
        return false;
    }
    target = value;
    return true;
}



/// Convert float param value to int and assign it to the target if it is
/// inside [minValue, maxValue]. The fractional part is truncated.
bool assignIntParam(int& target, float value, int minValue, int maxValue)
{
    int intValue{0};
    if (!toInt(value, intValue))
    {
        return false;
    }
    return assignInRange(target, intValue, minValue, maxValue);
}
}



cr::pantilt::CustomPanTilt::CustomPanTilt() = default;



cr::pantilt::CustomPanTilt::~CustomPanTilt() = default;



std::string cr::pantilt::CustomPanTilt::getVersion()
{
    return CUSTOM_PAN_TILT_VERSION;
}



bool cr::pantilt::CustomPanTilt::openPanTilt(std::string)
{
    // Set connection flags.
    const std::lock_guard<std::mutex> lock(m_paramsMutex);
    m_params.isOpen = true;
    m_params.isConnected = true;

    return true;
}



bool cr::pantilt::CustomPanTilt::initPanTilt(PanTiltParams& params)
{
    const std::lock_guard<std::mutex> lock(m_paramsMutex);

    // Copy params.
    m_params = params;

    // Set connection flags.
    m_params.isOpen = true;
    m_params.isConnected = true;

    return true;
}



void cr::pantilt::CustomPanTilt::closePanTilt()
{
    const std::lock_guard<std::mutex> lock(m_paramsMutex);
    m_params.isConnected = false;
    m_params.isOpen = false;
}



bool cr::pantilt::CustomPanTilt::isPanTiltOpen()
{
    const std::lock_guard<std::mutex> lock(m_paramsMutex);
    return m_params.isOpen;
}



bool cr::pantilt::CustomPanTilt::isPanTiltConnected()
{
    const std::lock_guard<std::mutex> lock(m_paramsMutex);
    return m_params.isConnected;
}



bool cr::pantilt::CustomPanTilt::setParam(PanTiltParam id, float value)
{
    // Lock mutex for setting parameters.
    const std::lock_guard<std::mutex> lock(m_paramsMutex);

    // Check param ID.
    switch (id)
    {
    case PanTiltParam::PAN_ANGLE:
    {
        return assignFloatInRange(
                    m_params.panAngle, value, PAN_ANGLE_MIN, PAN_ANGLE_MAX);
    }
    case PanTiltParam::PAN_MOTOR_POSITION:
    {
        return assignIntParam(m_params.panMotorPosition, value,
                              MOTOR_POSITION_MIN, MOTOR_POSITION_MAX);
    }
    case PanTiltParam::PAN_MOTOR_SPEED:
    {
        return assignFloatInRange(m_params.panMotorSpeed, value,
                                  MOTOR_SPEED_MIN, MOTOR_SPEED_MAX);
    }
    case PanTiltParam::TILT_ANGLE:
    {
        return assignFloatInRange(
                    m_params.tiltAngle, value, TILT_ANGLE_MIN, TILT_ANGLE_MAX);
    }
    case PanTiltParam::TILT_MOTOR_POSITION:
    {
        return assignIntParam(m_params.tiltMotorPosition, value,
                              MOTOR_POSITION_MIN, MOTOR_POSITION_MAX);
    }
    case PanTiltParam::TILT_MOTOR_SPEED:
    {
        return assignFloatInRange(m_params.tiltMotorSpeed, value,
                                  MOTOR_SPEED_MIN, MOTOR_SPEED_MAX);
    }
    case PanTiltParam::IS_CONNECTED:
    case PanTiltParam::IS_OPEN:
    {
        // Read only params.
        return false;
    }
    case PanTiltParam::CUSTOM_1:
    case PanTiltParam::CUSTOM_2:
    case PanTiltParam::CUSTOM_3:
    {
        // Custom parameters: accepted, the example does not store them.
        return true;
    }
    }

    // An ID outside of the enum range.
    return false;
}



float cr::pantilt::CustomPanTilt::getParam(PanTiltParam id)
{
    // Lock mutex for reading parameters.
    const std::lock_guard<std::mutex> lock(m_paramsMutex);

    // Check param ID.
    switch (id)
    {
    case PanTiltParam::PAN_ANGLE:
    {
        return m_params.panAngle;
    }
    case PanTiltParam::PAN_MOTOR_POSITION:
    {
        return static_cast<float>(m_params.panMotorPosition);
    }
    case PanTiltParam::PAN_MOTOR_SPEED:
    {
        return m_params.panMotorSpeed;
    }
    case PanTiltParam::TILT_ANGLE:
    {
        return m_params.tiltAngle;
    }
    case PanTiltParam::TILT_MOTOR_POSITION:
    {
        return static_cast<float>(m_params.tiltMotorPosition);
    }
    case PanTiltParam::TILT_MOTOR_SPEED:
    {
        return m_params.tiltMotorSpeed;
    }
    case PanTiltParam::IS_CONNECTED:
    {
        return m_params.isConnected ? 1.0f : 0.0f;
    }
    case PanTiltParam::IS_OPEN:
    {
        return m_params.isOpen ? 1.0f : 0.0f;
    }
    case PanTiltParam::CUSTOM_1:
    {
        return m_params.custom1;
    }
    case PanTiltParam::CUSTOM_2:
    {
        return m_params.custom2;
    }
    case PanTiltParam::CUSTOM_3:
    {
        return m_params.custom3;
    }
    }

    // An ID outside of the enum range.
    return -1.0f;
}



void cr::pantilt::CustomPanTilt::getParams(PanTiltParams& params)
{
    // Lock mutex for assigning parameters.
    const std::lock_guard<std::mutex> lock(m_paramsMutex);
    params = m_params;
}



bool cr::pantilt::CustomPanTilt::executeCommand(PanTiltCommand id, float, float)
{
    // Check command ID. The example accepts the commands and does nothing.
    switch (id)
    {
    case PanTiltCommand::RESTART:
    case PanTiltCommand::STOP:
    case PanTiltCommand::GO_TO_PAN_POSITION:
    case PanTiltCommand::GO_TO_TILT_POSITION:
    case PanTiltCommand::GO_TO_PAN_TILT_POSITION:
    case PanTiltCommand::GO_TO_PAN_ANGLE:
    case PanTiltCommand::GO_TO_TILT_ANGLE:
    case PanTiltCommand::GO_TO_PAN_TILT_ANGLE:
    case PanTiltCommand::GO_TO_HOME:
    {
        return true;
    }
    case PanTiltCommand::MOVE_PAN:
    case PanTiltCommand::MOVE_TILT:
    case PanTiltCommand::MOVE_PAN_TILT:
    {
        // Not supported by the example.
        return false;
    }
    }

    // An ID outside of the enum range.
    return false;
}



bool cr::pantilt::CustomPanTilt::decodeAndExecuteCommand(
        uint8_t* data, int size)
{
    // Decode command. The decoder checks the pointer, the size, the version
    // and the IDs.
    PanTiltParam paramId{PanTiltParam::PAN_MOTOR_POSITION};
    PanTiltCommand commandId{PanTiltCommand::GO_TO_PAN_POSITION};
    float value1{0.0f};
    float value2{0.0f};
    const int result = PanTilt::decodeCommand(
                data, size, paramId, commandId, value1, value2);

    // Execute the decoded command.
    if (result == 0)
    {
        return executeCommand(commandId, value1, value2);
    }
    if (result == 1)
    {
        return setParam(paramId, value1);
    }

    return false;
}
