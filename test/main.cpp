#include <iostream>
#include "PanTilt.h"



/**
 * @brief Copy params test.
 */
bool copyParametersTest();

/**
 * @brief Encode/decode params test without mask.
 */
bool encodeDecodeTestWithoutMask();

/**
 * @brief Encode/decode params test with mask.
 */
bool encodeDecodeTestWithMask();



int main(void)
{
	std::cout << "Test for PanTilt library" << std::endl << std::endl;

	std::cout << "Copy parameters test:" << std::endl;
	if (copyParametersTest())
	{
		std::cout << "OK" << std::endl;
	}
	else
	{
		std::cout << "ERROR" << std::endl;
	}
	std::cout << std::endl;

	std::cout << "Encode/decode params test without mask:" << std::endl;
	if (encodeDecodeTestWithoutMask())
	{
		std::cout << "OK" << std::endl;
	}
	else
	{
		std::cout << "ERROR" << std::endl;
	}
	std::cout << std::endl;

	std::cout << "Encode/decode params test with mask:" << std::endl;
	if (encodeDecodeTestWithMask())
	{
		std::cout << "OK" << std::endl;
	}
	else
	{
		std::cout << "ERROR" << std::endl;
	}
	std::cout << std::endl;

	return 1;
}



bool copyParametersTest()
{
	// Prepare random params.
	cr::pantilt::PanTiltParams params1;
	params1.firstParam = true;
	params1.secondParam = rand() % 255;
	params1.thirdParam = static_cast<float>(rand() % 255);

	// Copy params.
	cr::pantilt::PanTiltParams params2 = params1;

	// Compare params.
	bool result = true;
	if (params1.firstParam != params2.firstParam)
	{
		std::cout << "[" << __LINE__ << "] " << "firstParam not equal" << std::endl;
		result = false;
	}
	if (params1.secondParam != params2.secondParam)
	{
		std::cout << "[" << __LINE__ << "] " << "secondParam not equal" << std::endl;
		result = false;
	}
	if (params1.thirdParam != params2.thirdParam)
	{
		std::cout << "[" << __LINE__ << "] " << "thirdParam not equal" << std::endl;
		result = false;
	}

	return result;
}



bool encodeDecodeTestWithoutMask()
{
	// Prepare random params.
	cr::pantilt::PanTiltParams params1;
	params1.firstParam = true;
	params1.secondParam = rand() % 255;
	params1.thirdParam = static_cast<float>(rand() % 255);

	// Encode (serialize) params.
	int bufferSize = 128;
	uint8_t buffer[128];
	int size = 0;
	if (!params1.encode(buffer, bufferSize, size))
	{
		std::cout << "[" << __LINE__ << "] " << "Can't encode params" << std::endl;
		return false;
	}

	// Decode (deserialize) params.
	cr::pantilt::PanTiltParams params2;
	if (!params2.decode(buffer, size))
	{
		std::cout << "[" << __LINE__ << "] " << "Can't decode params" << std::endl;
		return false;
	}

	// Compare params.
	bool result = true;
	if (params1.firstParam != params2.firstParam)
	{
		std::cout << "[" << __LINE__ << "] " << "firstParam not equal" << std::endl;
		result = false;
	}
	if (params1.secondParam != params2.secondParam)
	{
		std::cout << "[" << __LINE__ << "] " << "secondParam not equal" << std::endl;
		result = false;
	}
	if (params1.thirdParam != params2.thirdParam)
	{
		std::cout << "[" << __LINE__ << "] " << "thirdParam not equal" << std::endl;
		result = false;
	}

	return result;
}



bool encodeDecodeTestWithMask()
{
	// Prepare random params.
	cr::pantilt::PanTiltParams params1;
	params1.firstParam = true;
	params1.secondParam = rand() % 255;
	params1.thirdParam = static_cast<float>(rand() % 255);

	// Prepare mask.
	cr::pantilt::PanTiltParamsMask mask;
	mask.firstParam = false;
	mask.secondParam = true; // Include only one param fr encoding.
	mask.thirdParam = false;

	// Encode (serialize) params.
	int bufferSize = 128;
	uint8_t buffer[128];
	int size = 0;
	if (!params1.encode(buffer, bufferSize, size, &mask))
	{
		std::cout << "[" << __LINE__ << "] " << "Can't encode params" << std::endl;
		return false;
	}

	// Decode (deserialize) params.
	cr::pantilt::PanTiltParams params2;
	if (!params2.decode(buffer, size))
	{
		std::cout << "[" << __LINE__ << "] " << "Can't decode params" << std::endl;
		return false;
	}

	// Compare params.
	bool result = true;
	if (params2.firstParam != false)
	{
		std::cout << "[" << __LINE__ << "] " << "firstParam not default" << std::endl;
		result = false;
	}
	if (params2.secondParam != params1.secondParam)
	{
		std::cout << "[" << __LINE__ << "] " << "secondParam not equal" << std::endl;
		result = false;
	}
	if (params2.thirdParam != 0.0f)
	{
		std::cout << "[" << __LINE__ << "] " << "thirdParam not default" << std::endl;
		result = false;
	}

	return result;
}