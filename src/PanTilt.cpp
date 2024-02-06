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
	panTiltMotorPosition = src.panTiltMotorPosition;
	panTiltAngle = src.panTiltAngle;
	panMotorSpeed = src.panMotorSpeed;
	tiltMotorSpeed = src.tiltMotorSpeed;
	panTiltMotorSpeed = src.panTiltMotorSpeed;

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
    data[pos] = data[pos] | (paramsMask.panAngle ? (uint8_t)128 : (uint8_t)0);
    data[pos] = data[pos] | (paramsMask.panMotorPosition ? (uint8_t)64 : (uint8_t)0);
    data[pos] = data[pos] | (paramsMask.panMotorSpeed ? (uint8_t)32 : (uint8_t)0);
    data[pos] = data[pos] | (paramsMask.panTiltAngle ? (uint8_t)16 : (uint8_t)0);
    data[pos] = data[pos] | (paramsMask.panTiltMotorPosition ? (uint8_t)8 : (uint8_t)0);
    data[pos] = data[pos] | (paramsMask.panTiltMotorSpeed ? (uint8_t)4 : (uint8_t)0);
    data[pos] = data[pos] | (paramsMask.tiltAngle ? (uint8_t)2 : (uint8_t)0);
    data[pos] = data[pos] | (paramsMask.tiltMotorPosition ? (uint8_t)1 : (uint8_t)0);
    pos += 1;
	data[pos] = 0x00;
    data[pos] = data[pos] | (paramsMask.tiltMotorSpeed ? (uint8_t)128 : (uint8_t)0);
    data[pos] = data[pos] | (paramsMask.isConnected ? (uint8_t)64 : (uint8_t)0);
    data[pos] = data[pos] | (paramsMask.isOpened ? (uint8_t)32 : (uint8_t)0);
    pos += 1;

	// Copy params to buffer.
	if (paramsMask.panAngle)
	{
		memcpy(&data[pos], &panAngle, sizeof(panAngle));
		pos += sizeof(panAngle);
	}
	if (paramsMask.panMotorPosition)
	{
		memcpy(&data[pos], &panMotorPosition, sizeof(panMotorPosition));
		pos += sizeof(panMotorPosition);
	}
	if (paramsMask.panMotorSpeed)
	{
		memcpy(&data[pos], &panMotorSpeed, sizeof(panMotorSpeed));
		pos += sizeof(panMotorSpeed);
	}
	if (paramsMask.panTiltAngle)
	{
		memcpy(&data[pos], &panTiltAngle, sizeof(panTiltAngle));
		pos += sizeof(panTiltAngle);
	}
	if (paramsMask.panTiltMotorPosition)
	{
		memcpy(&data[pos], &panTiltMotorPosition, sizeof(panTiltMotorPosition));
		pos += sizeof(panTiltMotorPosition);
	}
	if (paramsMask.panTiltMotorSpeed)
	{
		memcpy(&data[pos], &panTiltMotorSpeed, sizeof(panTiltMotorSpeed));
		pos += sizeof(panTiltMotorSpeed);
	}
	if (paramsMask.tiltAngle)
	{
		memcpy(&data[pos], &tiltAngle, sizeof(tiltAngle));
		pos += sizeof(tiltAngle);
	}
	if (paramsMask.tiltMotorPosition)
	{
		memcpy(&data[pos], &tiltMotorPosition, sizeof(tiltMotorPosition));
		pos += sizeof(tiltMotorPosition);
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
	if (paramsMask.isOpened)
	{
		data[pos] = isOpened == true ? 0x01 : 0x00;
		pos += sizeof(isOpened);
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
		if (dataSize < pos + sizeof(panAngle))
        {
            return false;
		}
		memcpy(&panAngle, &data[pos], 4);
		pos += sizeof(panAngle);
    }
	else
	{
		panAngle = 0.0f;
	}
	if ((data[3] & (uint8_t)64) == (uint8_t)64)
	{
		if (dataSize < pos + sizeof(panMotorPosition))
		{
			return false;
		}
		memcpy(&panMotorPosition, &data[pos], sizeof(panMotorPosition));
		pos += sizeof(panMotorPosition);
	}
	else
	{
		panMotorPosition = 0;
	}
	if ((data[3] & (uint8_t)32) == (uint8_t)32)
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
	if ((data[3] & (uint8_t)16) == (uint8_t)16)
	{
		if (dataSize < pos + sizeof(panTiltAngle))
		{
			return false;
		}
		memcpy(&panTiltAngle, &data[pos], sizeof(panTiltAngle));
		pos += sizeof(panTiltAngle);
	}
	else
	{
		panTiltAngle = 0.0f;
	}
	if ((data[3] & (uint8_t)8) == (uint8_t)8)
	{
		if (dataSize < pos + sizeof(panTiltMotorPosition))
		{
			return false;
		}
		memcpy(&panTiltMotorPosition, &data[pos], sizeof(panTiltMotorSpeed));
		pos += sizeof(panTiltMotorPosition);
	}
	else
	{
		panTiltMotorPosition = 0;
	}
	if ((data[3] & (uint8_t)4) == (uint8_t)4)
	{
		if (dataSize < pos + sizeof(panTiltMotorSpeed))
		{
			return false;
		}
		memcpy(&panTiltMotorSpeed, &data[pos], sizeof(panTiltMotorSpeed));
		pos += sizeof(panTiltMotorSpeed);
	}
	else
	{
		panTiltMotorSpeed = 0.0f;
	}
	if ((data[3] & (uint8_t)2) == (uint8_t)2)
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
	if ((data[3] & (uint8_t)1) == (uint8_t)1)
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
	if ((data[4] & (uint8_t)128) == (uint8_t)128)
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
	if ((data[4] & (uint8_t)64) == (uint8_t)64)
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
		isConnected = 0.0f;
	}
	if ((data[4] & (uint8_t)32) == (uint8_t)32)
	{
		if (dataSize < pos + sizeof(isOpened))
		{
			return false;
		}
		isOpened = data[pos] == 0x00 ? false : true;
		pos += sizeof(isOpened);
	}
	else
	{
		isOpened = 0.0f;
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



void cr::pantilt::PanTilt::encodeCommand(uint8_t* data, int& size, PanTiltCommand id)
{
	// Fill header.
	data[0] = 0x00;
	data[1] = PAN_TILT_MAJOR_VERSION;
	data[2] = PAN_TILT_MINOR_VERSION;

	// Fill data.
	int commandId = (int)id;
	memcpy(&data[3], &commandId, 4);
	size = 7;
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