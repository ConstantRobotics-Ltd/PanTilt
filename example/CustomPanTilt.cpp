#include "CustomPanTilt.h"
#include "CustomPanTiltVersion.h"



cr::pantilt::CustomPanTilt::CustomPanTilt()
{

}



cr::pantilt::CustomPanTilt::~CustomPanTilt()
{
}



std::string cr::pantilt::CustomPanTilt::getVersion()
{
	return CUSTOM_PAN_TILT_VERSION;
}



bool cr::pantilt::CustomPanTilt::openPanTilt(std::string initString)
{
	// Set connection flags.
	m_params.isInitialized = true;
	m_params.isConnected = true;

	return true;
}



bool cr::pantilt::CustomPanTilt::initPanTilt(PanTiltParams& params)
{
	// Copy params.
	m_params = params;

	// Set connection flags.
	m_params.isInitialized = true;
	m_params.isConnected = true;

	return true;
}



void cr::pantilt::CustomPanTilt::closePanTilt()
{
	m_params.isConnected = false;
	m_params.isInitialized = false;
}



bool cr::pantilt::CustomPanTilt::isPanTiltOpen()
{
	return m_params.isInitialized;
}



bool cr::pantilt::CustomPanTilt::isPanTiltConnected()
{
	return m_params.isConnected;
}



bool cr::pantilt::CustomPanTilt::setParam(PanTiltParam id, float value)
{
	// Lock mutex for setting parameters.
	std::lock_guard<std::mutex>lock(m_paramsMutex);
	// Check param ID.
	switch (id)
	{
	case PanTiltParam::PAN_ANGLE:
	{
		// Check value.
		if (value < -180.0f || value > 180.0f)
		{
			return false;
		}
		m_params.panAngle = value;
		return true;
	}
	case PanTiltParam::PAN_MOTOR_POSITION:
	{
		// Check value.
		if (static_cast<int>(value) < 0 || static_cast<int>(value) > 65535)
		{
			return false;
		}
		m_params.panMotorPosition = static_cast<int>(value);
		return true;
	}
	case PanTiltParam::PAN_MOTOR_SPEED:
	{
		// Check value.
		if (value < 0.0f || value > 100.0f)
		{
			return false;
		}
		m_params.panMotorSpeed = value;
		return true;
	}
	case PanTiltParam::TILT_ANGLE:
	{
		// Check value.
		if (value < -90.0f || value > 00.0f)
		{
			return false;
		}
		m_params.tiltAngle = value;
		return true;
	}
	case PanTiltParam::TILT_MOTOR_POSITION:
	{
		// Check value.
		if (static_cast<int>(value) < 0 || static_cast<int>(value) > 65535)
		{
			return false;
		}
		m_params.tiltMotorPosition = static_cast<int>(value);
		return true;
	}
	case PanTiltParam::TILT_MOTOR_SPEED:
	{
		// Check value.
		if (value < 0.0f || value > 100.0f)
		{
			return false;
		}
		m_params.tiltMotorSpeed = value;
		return true;
	}
	case PanTiltParam::IS_CONNECTED:
	{
		// Read only.
		return false;
	}
	case PanTiltParam::IS_INITIALIZED:
	{
		// Read only.
		return false;
	}
	case PanTiltParam::CUSTOM_1:
	{
		// Custom parameter.
		return true;
	}
	case PanTiltParam::CUSTOM_2:
	{
		// Custom parameter.
		return true;
	}
	case PanTiltParam::CUSTOM_3:
	{
		// Custom parameter.
		return true;
	}
	}

	return false;
}



float cr::pantilt::CustomPanTilt::getParam(PanTiltParam id)
{
	// Lock mutex for reading parameters.
	std::lock_guard<std::mutex>lock(m_paramsMutex);
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
	case PanTiltParam::IS_INITIALIZED:
	{
		return m_params.isInitialized ? 1.0f : 0.0f;
	}
	case PanTiltParam::CUSTOM_1:
	{
		// Custom parameter.
		return m_params.custom1;
	}
	case PanTiltParam::CUSTOM_2:
	{
		// Custom parameter.
		return m_params.custom2;
	}
	case PanTiltParam::CUSTOM_3:
	{
		// Custom parameter.
		return m_params.custom3;
	}
	}

	return -1.0f;
}



void cr::pantilt::CustomPanTilt::getParams(PanTiltParams& params)
{
	// Lock mutex for assigning parameters.
	std::lock_guard<std::mutex>lock(m_paramsMutex);
	params = m_params;
}



bool cr::pantilt::CustomPanTilt::executeCommand(PanTiltCommand id,
														float arg1, float arg2)
{
	// Check command ID.
	switch (id)
	{
	case PanTiltCommand::GO_TO_HOME:
	{
		return true;
	}
	case PanTiltCommand::GO_TO_PAN_ANGLE:
	{
		return true;
	}
	case PanTiltCommand::GO_TO_PAN_POSITION:
	{
		return true;
	}
	case PanTiltCommand::GO_TO_PAN_TILT_ANGLE:
	{
		return true;
	}
	case PanTiltCommand::GO_TO_PAN_TILT_POSITION:
	{
		return true;
	}
	case PanTiltCommand::GO_TO_TILT_ANGLE:
	{
		return true;
	}
	case PanTiltCommand::GO_TO_TILT_POSITION:
	{
		return true;
	}
	case PanTiltCommand::RESTART:
	{
		return true;
	}
	case PanTiltCommand::STOP:
	{
		return true;
	}
	}

	return false;
}

 

bool cr::pantilt::CustomPanTilt::decodeAndExecuteCommand(uint8_t* data, int size)
{
	// Decode command.
	PanTiltCommand commandId = PanTiltCommand::GO_TO_PAN_POSITION;
	PanTiltParam paramId = PanTiltParam::PAN_MOTOR_POSITION;
	float value = 0.0f;
	switch (PanTilt::decodeCommand(data, size, paramId, commandId, value))
	{
		// COMMAND.
	case 0:
		// Execute command.
		return executeCommand(commandId);
		// SET_PARAM.
	case 1:
	{
		// Set param.
		return setParam(paramId, value);
	}
	default:
	{
		return false;
	}
	}

	return false;
}