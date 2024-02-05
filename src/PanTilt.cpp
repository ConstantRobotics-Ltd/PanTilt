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
    pos += 1;

	// Copy params to buffer.
	if (paramsMask.panAngle)
	{
		memcpy(&data[pos], &panAngle, sizeof(float)); pos += sizeof(float);
	}
	if (paramsMask.panMotorPosition)
	{
		memcpy(&data[pos], &panMotorPosition, sizeof(int)); pos += sizeof(int);
	}
	if (paramsMask.panMotorSpeed)
	{
		memcpy(&data[pos], &panMotorSpeed, sizeof(float)); pos += sizeof(float);
	}
	if (paramsMask.panTiltAngle)
	{
		memcpy(&data[pos], &panTiltAngle, sizeof(float)); pos += sizeof(float);
	}
	if (paramsMask.panTiltMotorPosition)
	{
		memcpy(&data[pos], &panTiltMotorPosition, sizeof(int)); pos += sizeof(int);
	}
	if (paramsMask.panTiltMotorSpeed)
	{
		memcpy(&data[pos], &panTiltMotorSpeed, sizeof(float)); pos += sizeof(float);
	}
	if (paramsMask.tiltAngle)
	{
		memcpy(&data[pos], &tiltAngle, sizeof(float)); pos += sizeof(float);
	}
	if (paramsMask.tiltMotorPosition)
	{
		memcpy(&data[pos], &tiltMotorPosition, sizeof(int)); pos += sizeof(int);
	}
	if (paramsMask.tiltMotorSpeed)
	{
		memcpy(&data[pos], &tiltMotorSpeed, sizeof(float)); pos += sizeof(float);
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

	return true;
}



cr::pantilt::PanTilt::~PanTilt()
{

}



std::string cr::pantilt::PanTilt::getVersion()
{
	return PAN_TILT_VERSION;
}