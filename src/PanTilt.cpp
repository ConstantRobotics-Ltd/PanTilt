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
    initString = src.initString;

	return *this;
}



bool cr::pantilt::PanTiltParams::encode(uint8_t* data, int bufferSize,
                                        int& size, PanTiltParamsMask* mask)
{
	// Check buffer size.
	if (bufferSize < 43)
		return false;

	// Copy atributes.
	data[0] = 0x02;
	data[1] = PAN_TILT_MAJOR_VERSION;
	data[2] = PAN_TILT_MINOR_VERSION;

	// Prepare params mask.
	PanTiltParamsMask paramsMask;
	if (mask != nullptr)
		paramsMask = *mask;

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
    data[pos] = data[pos] | (paramsMask.isOpen ? (uint8_t)1 : (uint8_t)0);
    pos += 1;
	data[pos] = 0x00;
    data[pos] = data[pos] | (paramsMask.custom1 ? (uint8_t)128 : (uint8_t)0);
    data[pos] = data[pos] | (paramsMask.custom2 ? (uint8_t)64 : (uint8_t)0);
    data[pos] = data[pos] | (paramsMask.custom3 ? (uint8_t)32 : (uint8_t)0);
    pos += 1;

	// Copy params to buffer.
	if (paramsMask.panMotorPosition)
	{
		memcpy(&data[pos], &panMotorPosition, 4); pos += 4;
	}
	if (paramsMask.tiltMotorPosition)
	{
		memcpy(&data[pos], &tiltMotorPosition, 4); pos += 4;
	}
	if (paramsMask.panAngle)
	{
		memcpy(&data[pos], &panAngle, 4);
		pos += 4;
	}
	if (paramsMask.tiltAngle)
	{
		memcpy(&data[pos], &tiltAngle, 4); pos += 4;
	}
	if (paramsMask.panMotorSpeed)
	{
		memcpy(&data[pos], &panMotorSpeed, 4); pos += 4;
	}
	if (paramsMask.tiltMotorSpeed)
	{
		memcpy(&data[pos], &tiltMotorSpeed, 4); pos += 4;
	}
	if (paramsMask.isConnected)
	{
		data[pos] = isConnected == true ? 0x01 : 0x00; pos += 1;
	}
	if (paramsMask.isOpen)
	{
		data[pos] = isOpen == true ? 0x01 : 0x00; pos += 1;
	}
	if (paramsMask.custom1)
	{
		memcpy(&data[pos], &custom1, 4); pos += 4;
	}
	if (paramsMask.custom2)
	{
		memcpy(&data[pos], &custom2, 4); pos += 4;		
	}
	if (paramsMask.custom3)
	{
		memcpy(&data[pos], &custom3, 4); pos += 4;
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
		if (dataSize < pos + 4)
            return false;
		memcpy(&panMotorPosition, &data[pos], 4); pos += 4;
    }
	else
	{
		panMotorPosition = 0;
	}
	if ((data[3] & (uint8_t)64) == (uint8_t)64)
	{
		if (dataSize < pos + 4)
			return false;
		memcpy(&tiltMotorPosition, &data[pos], 4); pos += 4;
	}
	else
	{
		tiltMotorPosition = 0;
	}
	if ((data[3] & (uint8_t)32) == (uint8_t)32)
	{
		if (dataSize < pos + 4)
			return false;
		memcpy(&panAngle, &data[pos], 4); pos += 4;
	}
	else
	{
		panAngle = 0.0f;
	}
	if ((data[3] & (uint8_t)16) == (uint8_t)16)
	{
		if (dataSize < pos + 4)
			return false;
		memcpy(&tiltAngle, &data[pos], 4); pos += 4;
	}
	else
	{
		tiltAngle = 0.0f;
	}
	if ((data[3] & (uint8_t)8) == (uint8_t)8)
	{
		if (dataSize < pos + 4)
			return false;
		memcpy(&panMotorSpeed, &data[pos], 4); pos += 4;
	}
	else
	{
		panMotorSpeed = 0.0f;
	}
	if ((data[3] & (uint8_t)4) == (uint8_t)4)
	{
		if (dataSize < pos + 4)
			return false;
		memcpy(&tiltMotorSpeed, &data[pos], 4); pos += 4;
	}
	else
	{
		tiltMotorSpeed = 0.0f;
	}
	if ((data[3] & (uint8_t)2) == (uint8_t)2)
	{
		if (dataSize < pos + 1)
			return false;
		isConnected = data[pos] == 0x00 ? false : true;
		pos += 1;
	}
	else
	{
		isConnected = false;
	}
	if ((data[3] & (uint8_t)1) == (uint8_t)1)
	{
		if (dataSize < pos + 1)
			return false;
		isOpen = data[pos] == 0x00 ? false : true; pos += 1;
	}
	else
	{
		isOpen = false;
	}
	if ((data[4] & (uint8_t)128) == (uint8_t)128)
	{
		if (dataSize < pos + 4)
			return false;
		memcpy(&custom1, &data[pos], 4); pos += 4;
	}
	else
	{
		custom1 = 0.0f;
	}
	if ((data[4] & (uint8_t)64) == (uint8_t)64)
	{
		if (dataSize < pos + 4)
			return false;
		memcpy(&custom2, &data[pos], 4); pos += 4;
	}
	else
	{
		custom2 = 0.0f;
	}
	if ((data[4] & (uint8_t)32) == (uint8_t)32)
	{
		if (dataSize < pos + 4)
			return false;
		memcpy(&custom3, &data[pos], 4); pos += 4;
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
									PanTiltCommand& commandId, float& value1, float& value2)
{
	// Check size.
	if (size < 11)
		return -1;

	// Check version.
	if (data[1] != PAN_TILT_MAJOR_VERSION || data[2] != PAN_TILT_MINOR_VERSION)
		return -1;

	// Extract data.
	int id = 0;
	memcpy(&id, &data[3], 4);
	value1 = 0.0f;
    value2 = 0.0f;

	// Check command type.
	if (data[0] == 0x00)
	{
        if (size < 15)
            return -1;

		commandId = static_cast<PanTiltCommand>(id);
        memcpy(&value1, &data[7], 4);
        memcpy(&value2, &data[11], 4);
		return 0;
	}
	else if (data[0] == 0x01)
	{
		// Check size.
		if (size < 11)
		    return -1;

		paramId = static_cast<PanTiltParam>(id);
		memcpy(&value1, &data[7], 4);
		return 1;
	}

	return -1;
}