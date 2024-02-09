#include "PanTilt.h"
#include "PanTiltVersion.h"



cr::pantilt::PanTiltParams &cr::pantilt::PanTiltParams::operator= (const cr::pantilt::PanTiltParams& src)
{
	// Check yourself.
	if (this == &src)
	{
		return *this;
	}
	
	// Copy params.
	panMotorPosition = src.panMotorPosition;
	tiltMotorPosition = src.tiltMotorPosition;
	panAngle = src.panAngle;
	tiltAngle = src.tiltAngle;
	panMotorSpeed = src.panMotorSpeed;
	tiltMotorSpeed = src.tiltMotorSpeed;
	custom1 = src.custom1;
	custom2 = src.custom2;
	custom3 = src.custom3;

	return *this;
}



bool cr::pantilt::PanTiltParams::encode(uint8_t* data, int bufferSize, int& size,
														PanTiltParamsMask* mask)
{
	// Check buffer size.
	if (bufferSize < 45)
	{
		return false;
	}

	// Copy atributes.
	data[0] = 0x02;
	data[1] = PAN_TILT_MAJOR_VERSION;
	data[2] = PAN_TILT_MINOR_VERSION;

	// Prepare params mask.
	PanTiltParamsMask paramsMask;
	if (mask != nullptr)
	{
		paramsMask = *mask;
	}

	// Fill bit mask for params.
	int pos = 3;
	data[pos] = 0x00;
    data[pos] = data[pos] | (paramsMask.panMotorPosition ? (uint8_t)128 : (uint8_t)0);
    data[pos] = data[pos] | (paramsMask.tiltMotorPosition ? (uint8_t)64 : (uint8_t)0);
    data[pos] = data[pos] | (paramsMask.panAngle ? (uint8_t)32 : (uint8_t)0);
    data[pos] = data[pos] | (paramsMask.tiltAngle ? (uint8_t)16 : (uint8_t)0);
    data[pos] = data[pos] | (paramsMask.panMotorSpeed ? (uint8_t)8 : (uint8_t)0);
    data[pos] = data[pos] | (paramsMask.tiltMotorSpeed ? (uint8_t)4 : (uint8_t)0);
    data[pos] = data[pos] | (paramsMask.isConnected ? (uint8_t)2 : (uint8_t)0);
    data[pos] = data[pos] | (paramsMask.isInitialized ? (uint8_t)1 : (uint8_t)0);
    pos += 1;
	data[pos] = 0x00;
    data[pos] = data[pos] | (paramsMask.custom1 ? (uint8_t)128 : (uint8_t)0);
    data[pos] = data[pos] | (paramsMask.custom2 ? (uint8_t)64 : (uint8_t)0);
    data[pos] = data[pos] | (paramsMask.custom3 ? (uint8_t)32 : (uint8_t)0);
    pos += 1;

	// Copy params to buffer.
	if (paramsMask.panMotorPosition)
	{
		memcpy(&data[pos], &panMotorPosition, sizeof(panMotorPosition));
		pos += sizeof(panMotorPosition);
	}
	if (paramsMask.tiltMotorPosition)
	{
		memcpy(&data[pos], &tiltMotorPosition, sizeof(tiltMotorPosition));
		pos += sizeof(tiltMotorPosition);
	}
	if (paramsMask.panAngle)
	{
		memcpy(&data[pos], &panAngle, sizeof(panAngle));
		pos += sizeof(panAngle);
	}
	if (paramsMask.tiltAngle)
	{
		memcpy(&data[pos], &tiltAngle, sizeof(tiltAngle));
		pos += sizeof(tiltAngle);
	}
	if (paramsMask.panMotorSpeed)
	{
		memcpy(&data[pos], &panMotorSpeed, sizeof(panMotorSpeed));
		pos += sizeof(panMotorSpeed);
	}
	if (paramsMask.tiltMotorSpeed)
	{
		memcpy(&data[pos], &tiltMotorSpeed, sizeof(tiltMotorSpeed));
		pos += sizeof(tiltMotorSpeed);
	}
	if (paramsMask.isConnected)
	{
		data[pos] = isConnected == true ? 0x01 : 0x00;
		pos += sizeof(isConnected);
	}
	if (paramsMask.isInitialized)
	{
		data[pos] = isInitialized == true ? 0x01 : 0x00;
		pos += sizeof(isInitialized);
	}
	if (paramsMask.custom1)
	{
		memcpy(&data[pos], &custom1, sizeof(custom1));
		pos += sizeof(custom1);
	}
	if (paramsMask.custom2)
	{
		memcpy(&data[pos], &custom2, sizeof(custom2));
		pos += sizeof(custom2);
		
	}
	if (paramsMask.custom3)
	{
		memcpy(&data[pos], &custom3, sizeof(custom3));
		pos += sizeof(custom3);
	}
	
	size = pos;

	return true;
}



bool cr::pantilt::PanTiltParams::decode(uint8_t* data, int dataSize)
{
	// Check data size.
	if (dataSize < 5)
	{
		return false;
	}

	// Check atributes.
	if (data[0] != 0x02 ||
		data[1] != PAN_TILT_MAJOR_VERSION ||
		data[2] != PAN_TILT_MINOR_VERSION)
	{
		return false;
	}

	// Decode params.
	int pos = 5;
    if ((data[3] & (uint8_t)128) == (uint8_t)128)
    {
		if (dataSize < pos + sizeof(panMotorPosition))
        {
            return false;
		}
		memcpy(&panMotorPosition, &data[pos], 4);
		pos += sizeof(panMotorPosition);
    }
	else
	{
		panMotorPosition = 0;
	}
	if ((data[3] & (uint8_t)64) == (uint8_t)64)
	{
		if (dataSize < pos + sizeof(tiltMotorPosition))
		{
			return false;
		}
		memcpy(&tiltMotorPosition, &data[pos], sizeof(tiltMotorPosition));
		pos += sizeof(tiltMotorPosition);
	}
	else
	{
		tiltMotorPosition = 0;
	}
	if ((data[3] & (uint8_t)32) == (uint8_t)32)
	{
		if (dataSize < pos + sizeof(panAngle))
		{
			return false;
		}
		memcpy(&panAngle, &data[pos], sizeof(panAngle));
		pos += sizeof(panAngle);
	}
	else
	{
		panAngle = 0.0f;
	}
	if ((data[3] & (uint8_t)16) == (uint8_t)16)
	{
		if (dataSize < pos + sizeof(tiltAngle))
		{
			return false;
		}
		memcpy(&tiltAngle, &data[pos], sizeof(tiltAngle));
		pos += sizeof(tiltAngle);
	}
	else
	{
		tiltAngle = 0.0f;
	}
	if ((data[3] & (uint8_t)8) == (uint8_t)8)
	{
		if (dataSize < pos + sizeof(panMotorSpeed))
		{
			return false;
		}
		memcpy(&panMotorSpeed, &data[pos], sizeof(panMotorSpeed));
		pos += sizeof(panMotorSpeed);
	}
	else
	{
		panMotorSpeed = 0.0f;
	}
	if ((data[3] & (uint8_t)4) == (uint8_t)4)
	{
		if (dataSize < pos + sizeof(tiltMotorSpeed))
		{
			return false;
		}
		memcpy(&tiltMotorSpeed, &data[pos], sizeof(tiltMotorSpeed));
		pos += sizeof(tiltMotorSpeed);
	}
	else
	{
		tiltMotorSpeed = 0.0f;
	}
	if ((data[3] & (uint8_t)2) == (uint8_t)2)
	{
		if (dataSize < pos + sizeof(isConnected))
		{
			return false;
		}
		isConnected = data[pos] == 0x00 ? false : true;
		pos += sizeof(isConnected);
	}
	else
	{
		isConnected = false;
	}
	if ((data[3] & (uint8_t)1) == (uint8_t)1)
	{
		if (dataSize < pos + sizeof(isInitialized))
		{
			return false;
		}
		isInitialized = data[pos] == 0x00 ? false : true;
		pos += sizeof(isInitialized);
	}
	else
	{
		isInitialized = false;
	}
	if ((data[4] & (uint8_t)128) == (uint8_t)128)
	{
		if (dataSize < pos + sizeof(custom1))
		{
			return false;
		}
		memcpy(&custom1, &data[pos], sizeof(custom1));
		pos += sizeof(custom1);
	}
	else
	{
		custom1 = 0.0f;
	}
	if ((data[4] & (uint8_t)64) == (uint8_t)64)
	{
		if (dataSize < pos + sizeof(custom2))
		{
			return false;
		}
		memcpy(&custom2, &data[pos], sizeof(custom2));
		pos += sizeof(custom2);
	}
	else
	{
		custom2 = 0.0f;
	}
	if ((data[4] & (uint8_t)32) == (uint8_t)32)
	{
		if (dataSize < pos + sizeof(custom3))
		{
			return false;
		}
		memcpy(&custom3, &data[pos], sizeof(custom3));
		pos += sizeof(custom3);
	}
	else
	{
		custom3 = 0.0f;
	}

	return true;
}



cr::pantilt::PanTilt::~PanTilt()
{

}



std::string cr::pantilt::PanTilt::getVersion()
{
	return PAN_TILT_VERSION;
}



void cr::pantilt::PanTilt::encodeSetParamCommand(
	uint8_t* data, int& size, PanTiltParam id, float value)
{
	// Fill header.
	data[0] = 0x01;
	data[1] = PAN_TILT_MAJOR_VERSION;
	data[2] = PAN_TILT_MINOR_VERSION;

	// Fill data.
	int paramId = (int)id;
	memcpy(&data[3], &paramId, 4);
	memcpy(&data[7], &value, 4);
	size = 11;
}



void cr::pantilt::PanTilt::encodeCommand(uint8_t* data, int& size, 
									PanTiltCommand id, float arg1, float arg2)
{
	// Fill header.
	data[0] = 0x00;
	data[1] = PAN_TILT_MAJOR_VERSION;
	data[2] = PAN_TILT_MINOR_VERSION;

	// Fill data.
	int commandId = (int)id;
	memcpy(&data[3], &commandId, 4);
	memcpy(&data[7], &arg1, 4);
	memcpy(&data[11], &arg2, 4);
	size = 15;
}



int cr::pantilt::PanTilt::decodeCommand(uint8_t* data, int size, PanTiltParam& paramId,
									PanTiltCommand& commandId, float& value)
{
	// Check size.
	if (size < 7)
	{
		return -1;
	}

	// Check version.
	if (data[1] != PAN_TILT_MAJOR_VERSION || data[2] != PAN_TILT_MAJOR_VERSION)
	{
		return -1;
	}

	// Extract data.
	int id = 0;
	memcpy(&id, &data[3], 4);
	value = 0.0f;

	// Check command type.
	if (data[0] == 0x00)
	{
		commandId = static_cast<PanTiltCommand>(id);
		return 0;
	}
	else if (data[0] == 0x01)
	{
		// Check size.
		if (size != 11)
		{
			return false;
		}

		paramId = static_cast<PanTiltParam>(id);
		memcpy(&value, &data[7], 4);
		return 1;
	}

	return -1;
}