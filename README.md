![logo](_static/template_web_logo.png)



# **Template  C++ library**

**v1.0.0**



# Table of contents

- [Overview](#Overview)
- [Versions](#Versions)
- [Library files](#Library-files)
- [TemplateLibrary interface class description](#TemplateLibrary-interface-class-description)
  - [Class declaration](#Class-declaration)
  - [getVersion method](#getVersion-method)
  - [setParam method](#setParam-method)
  - [getParam method](#getParam-method)
  - [getParams method](#getParams-method)
  - [executeCommand method](#executeCommand-method)
  - [doSomething method](#doSomething-method)
- [Data structures](#Data-structures)
  - [TemplateLibraryCommand enum](#TemplateLibraryCommand-enum)
  - [TemplateLibraryParam enum](#TemplateLibraryParam-enum)
- [TemplateLibraryParams class description](#TemplateLibraryParams-class-description)
  - [Class declaration](#Class-declaration)
  - [Serialize template params](#Serialize-template-params)
  - [Deserialize template params](#Deserialize-template-params)
  - [Read params from JSON file and write to JSON file](#Read-params-from-JSON-file-and-write-to-JSON-file)
- [Build and connect to your project](#Build-and-connect-to-your-project)
- [Example](#Example)



# Overview

**PanTilt** is a C++ library designed to serve as a standard interface for various pan-tilt devices. The library defines data structures and rules to showcase an example structure for similar repositories. The library provides methods to encode/decode commands and encode/decode parameters. **PanTilt.h** file contains list of data structures (**[PanTiltCommand enum](#PanTiltCommand-enum)**, **[PanTiltParam enum](#PanTiltParam-enum)** and **PanTiltParams class**) and **PanTilt** class declaration. **PanTilt** interface depends on **[ConfigReader](https://github.com/ConstantRobotics-Ltd/ConfigReader)** library to provide methods to read/write JSON config files.

# Versions

**Table 1** - Library versions.

| Version | Release date | What's new                          |
| ------- | ------------ | ----------------------------------- |
| 1.0.0   | 06.02.2024   | - First version of PanTilt library. |



# Library files

The **PanTilt** is a CMake project. Library files:

```xml
CMakeLists.txt ------------------- Main CMake file of the library.
3rdparty ------------------------- Folder with third-party libraries.
    CMakeLists.txt --------------- CMake file which includes third-party libraries.
    ConfigReader ----------------- Source code of the ConfigReader library.
test ----------------------------- Folder for internal tests of library.
    CMakeLists.txt --------------- CMake file for tests application.
    main.cpp --------------------- Source code file tests application.
src ------------------------------ Folder with source code of the library.
    CMakeLists.txt --------------- CMake file of the library.
    PanTilt.cpp ------------------ Source code file of the library.
    PanTilt.h -------------------- Header file which includes PanTilt class declaration.
    PanTiltVersion.h ------------- Header file which includes version of the library.
    PanTiltVersion.h.in ---------- CMake service file to generate version file.
```



# PanTilt interface class description



## Class declaration

**PanTilt** interface class declared in **PanTilt.h** file. Class declaration:

```cpp
class PanTilt
{
public:

   
    // Class virtual destructor.
    virtual ~PanTilt();

    // Get the version of the PanTilt class.
    static std::string getVersion();

	// Set the value for a specific library parameter.
    virtual bool setParam(PanTiltParam id, float value) = 0;

	// Get the value of a specific library parameter.
    virtual float getParam(PanTiltParam id) const = 0;

	// Get the structure containing all library parameters.
    virtual void getParams(PanTiltParams& params) const = 0;

	// Execute a PanTilt command.
    virtual bool executeCommand(PanTiltCommand id) = 0;

	// Encode set param command.
    static void encodeSetParamCommand(
        uint8_t* data, int& size, PanTiltParam id, float value);

	// Encode command.
    static void encodeCommand(
        uint8_t* data, int& size, PanTiltCommand id);

	// Decode command.
    static int decodeCommand(uint8_t* data,
        int size,
        PanTiltParam& paramId,
        PanTiltCommand& commandId,
        float& value);

	// Decode and execute command.
    virtual bool decodeAndExecuteCommand(uint8_t* data, int size) = 0;
};
```



## getVersion method

**getVersion()** method returns string of current class version. Method declaration:

```cpp
static std::string getVersion();
```

Method can be used without **PanTilt** class instance:

```cpp
std::cout << "PanTilt version: " << cr::pantilt::PanTilt::getVersion();
```

Console output:

```bash
PanTilt class version: 1.0.0
```



## setParam method

**setParam(...)** method sets new parameters value. **PanTilt** based library should provide thread-safe **setParam(...)** method call. This means that the **setParam(...)** method can be safely called from any thread. Method declaration:

```cpp
bool setParam(PanTiltParam id, float value);
```

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| id        | Parameter ID according to [PanTiltParam](#PanTiltParam-enum) enum. |
| value     | Parameter value. Value depends on parameter ID.              |

**Returns:** TRUE if the parameter was set or FALSE if not.



## getParam method

**getParam(...)** method returns parameter value. **PanTilt** based library should provide thread-safe **getParam(...)** method call. This means that the **getParam(...)** method can be safely called from any thread. Method declaration:

```cpp
float getParam(PanTiltParam id);
```

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| id        | Parameter ID according to [PanTiltParam](#PanTiltParam-enum) enum. |

**Returns:** parameter value or **-1** if the parameters doesn't exist.



## getParams method

**getParams(...)** method is designed to obtain params structure. **PanTilt** based library should provide thread-safe **getParams(...)** method call. This means that the **getParams(...)** method can be safely called from any thread. Method declaration:

```cpp
void getParams(PanTiltParams& params);
```

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| params    | Reference to [PanTiltParams](#PanTiltParams-class-description) object to store params. |



## executeCommand method

**executeCommand(...)** method executes library command. **PanTilt** based library should provide thread-safe **executeCommand(...)** method call. This means that the **executeCommand(...)** method can be safely called from any thread. Method declaration:

```cpp
bool executeCommand(PanTiltCommand id);
```

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| id        | Command  ID according to [PanTiltCommand](#PanTiltCommand-enum) enum. |

**Returns:** TRUE if the command executed or FALSE if not.



## encodeSetParamCommand method

**encodeSetParamCommand(...)** static method encodes command to change any PanTilt parameter value. To control a pan-tilt device remotely, the developer has to design his own protocol and according to it encode the command and deliver it over the communication channel. To simplify this, the **PanTilt** class contains static methods for encoding the control command. The **PanTilt** class provides two types of commands: a parameter change command (SET_PARAM) and an action command (COMMAND). **encodeSetParamCommand(...)** designed to encode SET_PARAM command. Method declaration:

```cpp
static void encodeSetParamCommand(uint8_t* data, int& size, PanTiltParam id, float value);
```

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| data      | Pointer to data buffer for encoded command. Must have size >= 11. |
| size      | Size of encoded data. Will be 11 bytes.                      |
| id        | Parameter ID according to [**PanTilt enum**](#PanTilt-enum). |
| value     | Parameter value.                                             |

**SET_PARAM** command format:

| Byte | Value | Description                                        |
| ---- | ----- | -------------------------------------------------- |
| 0    | 0x01  | SET_PARAM command header value.                    |
| 1    | Major | Major version of PanTilt class.                    |
| 2    | Minor | Minor version of PanTilt class.                    |
| 3    | id    | Parameter ID **int32_t** in Little-endian format.  |
| 4    | id    | Parameter ID **int32_t** in Little-endian format.  |
| 5    | id    | Parameter ID **int32_t** in Little-endian format.  |
| 6    | id    | Parameter ID **int32_t** in Little-endian format.  |
| 7    | value | Parameter value **float** in Little-endian format. |
| 8    | value | Parameter value **float** in Little-endian format. |
| 9    | value | Parameter value **float** in Little-endian format. |
| 10   | value | Parameter value **float** in Little-endian format. |

**encodeSetParamCommand(...)** is static and used without **PanTilt** class instance. This method used on client side (control system). Command encoding example:

```cpp
// Buffer for encoded data.
uint8_t data[11];
// Size of encoded data.
int size = 0;
// Random parameter value.
float outValue = static_cast<float>(rand() % 20);
// Encode command.
PanTilt::encodeSetParamCommand(data, size, PanTiltParam::PAN_ANGLE, outValue);
```



## encodeCommand method

**encodeCommand(...)** static method encodes command for PanTilt remote control. To control a pan-tilt device remotely, the developer has to design his own protocol and according to it encode the command and deliver it over the communication channel. To simplify this, the **PanTilt ** class contains static methods for encoding the control command. The **PanTilt ** class provides two types of commands: a parameter change command (SET_PARAM) and an action command (COMMAND). **encodeCommand(...)** designed to encode COMMAND command (action command). Method declaration:

```cpp
static void encodeCommand(uint8_t* data, int& size, PanTilt Command id);
```

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| data      | Pointer to data buffer for encoded command. Must have size >= 7. |
| size      | Size of encoded data. Will be 7 bytes.                       |
| id        | Command ID according to [**PanTiltCommand enum**](#PanTiltCommand-enum). |

**COMMAND** format:

| Byte | Value | Description                                     |
| ---- | ----- | ----------------------------------------------- |
| 0    | 0x00  | COMMAND header value.                           |
| 1    | Major | Major version of Camera class.                  |
| 2    | Minor | Minor version of Camera class.                  |
| 3    | id    | Command ID **int32_t** in Little-endian format. |
| 4    | id    | Command ID **int32_t** in Little-endian format. |
| 5    | id    | Command ID **int32_t** in Little-endian format. |
| 6    | id    | Command ID **int32_t** in Little-endian format. |

**encodeCommand(...)** is static and used without **Camera** class instance. This method used on client side (control system). Command encoding example:

```cpp
// Buffer for encoded data.
uint8_t data[7];
// Size of encoded data.
int size = 0;
// Encode command.
Camera::encodeCommand(data, size, CameraCommand::NUC);
```



## decodeCommand method

**decodeCommand(...)** static method decodes command on camera controller side. Method declaration:

```cpp
static int decodeCommand(uint8_t* data, int size, CameraParam& paramId, CameraCommand& commandId, float& value);
```

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| data      | Pointer to input command.                                    |
| size      | Size of command. Must be 11 bytes for SET_PARAM and 7 bytes for COMMAND. |
| paramId   | Camera parameter ID according to [**CameraParam enum**](#CameraParam-enum). After decoding SET_PARAM command the method will return parameter ID. |
| commandId | Camera command ID according to [**CameraCommand enum**](#CameraCommand-enum). After decoding COMMAND the method will return command ID. |
| value     | Camera parameter value (after decoding SET_PARAM command).   |

**Returns:** **0** - in case decoding COMMAND, **1** - in case decoding SET_PARAM command or **-1** in case errors.



## decodeAndExecuteCommand method

**decodeAndExecuteCommand(...)** method decodes and executes command on camera controller side. The particular implementation of the camera controller must provide thread-safe **decodeAndExecuteCommand(...)** method call. This means that the **decodeAndExecuteCommand(...)** method can be safely called from any thread. Method declaration:

```cpp
virtual bool decodeAndExecuteCommand(uint8_t* data, int size) = 0;
```

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| data      | Pointer to input command.                                    |
| size      | Size of command. Must be 11 bytes for SET_PARAM or 7 bytes for COMMAND. |

**Returns:** TRUE if command decoded (SET_PARAM or COMMAND) and executed (action command or set param command).

# Data structures



## TemplateLibraryCommand enum

Enum declaration:

```cpp
enum class TemplateLibraryCommand
{
    /// First command.
    FIRST_COMMAND = 1,
    /// Second command.
    SECOND_COMMAND,
    /// Third command.
    THIRD_COMMAND
};
```

**Table 2** - Commands description.

| Command        | Description                   |
| -------------- | ----------------------------- |
| FIRST_COMMAND  | First command.                |
| SECOND_COMMAND | Second command.               |
| THIRD_COMMAND  | Third command.                |



## TemplateLibraryParam enum

Enum declaration:

```cpp
enum class TemplateLibraryParam
{
    /// First param. Here describe nuances and param value vaid range.
    FIRST_PARAM = 1,
    /// Second param. Here describe nuances and param value vaid range.
    SECOND_PARAM,
    /// Third param. Here describe nuances and param value vaid range.
    THIRD_PARAM
};
```

**Table 3** - Params description.

| Parameter    | Access       | Description                                                  |
| ------------ | ------------ | ------------------------------------------------------------ |
| FIRST_PARAM  | read / write | First param. Valid values **0** or **1**:<br />**0** - set to set firstParam of TempleLibraryParams class to **false**.<br />**1** - set to set firstParam of TempleLibraryParams class to **true**. |
| SECOND_PARAM | read / write | Second param. Valid values from **-100** to **100**.         |
| THIRD_PARAM  | read / write | Third param. Valid values from **-100** to **100**.          |



# TemplateLibraryParams class description



## Class declaration

**TemplateLibraryParams** class is used to provide example params structure. Also **TemplateLibraryParams** provides possibility to write/read params from JSON files (**JSON_READABLE** macro) and provides methods to encode and decode params. **TemplateLibraryParams** interface class declared in **TemplateLibrary.h** file. Class declaration:

```cpp
class TemplateLibraryParams
{
public:

    /// First param. Here describe what status does this flag define.
    bool firstParam{ false };
    /// Second param. Here describe nuances and param value vaid range.
    int secondParam{ 0 };
    /// Third param. Here describe nuances and param value vaid range.
    float thirdParam{ 0.0f };

    /// Macro from ConfigReader to make params readable/writable from JSON.
    JSON_READABLE(TemplateLibraryParams, firstParam, secondParam, thirdParam)

    /// operator =
    TemplateLibraryParams& operator= (const TemplateLibraryParams& src);

    /// Encode (serialize) params.
    bool encode(uint8_t* data, int bufferSize, int& size,
                TemplateLibraryParamsMask* mask = nullptr);

    /// Decode (deserialize) params.
    bool decode(uint8_t* data, int dataSize);
};
```

**Table 4** - TemplateLibraryParams class fields description is related to [TemplateLibraryParam enum](#TemplateLibraryParam-enum) description.

| Field       | type  | Description            |
| ----------- | ----- | ---------------------- |
| firstParam  | bool  | First template param.  |
| secondParam | int   | Second template param. |
| thirdParam  | float | Third template param.  |

**None:** *TemplateParams class fields listed in Table 4 **must** reflect params set/get by methods setParam(...) and getParam(...).* 



## Serialize template params

[TemplateLibraryParams](#TemplateLibraryParams-class-description) class provides method **encode(...)** to serialize template params. Serialization of template params necessary in case when you have to send template params via communication channels. Method provides options to exclude particular parameters from serialization. To do this method inserts binary mask (1 byte) where each bit represents particular parameter and **decode(...)** method recognizes it. Method declaration:

```cpp
bool encode(uint8_t* data, int bufferSize, int& size, TemplateLibraryParamsMask* mask = nullptr);
```

| Parameter  | Value                                                        |
| ---------- | ------------------------------------------------------------ |
| data       | Pointer to data buffer. Buffer size must be >= 237 bytes.    |
| bufferSize | Data buffer size. Buffer size must be >= 237 bytes.          |
| size       | Size of encoded data.                                        |
| mask       | Parameters mask - pointer to **TemplateLibraryParamsMask** structure. **TemplateLibraryParamsMask** (declared in TemplateLibrary.h file) determines flags for each field (parameter) declared in [TemplateLibraryParams class](#TemplateLibraryParams-class-description). If the user wants to exclude any parameters from serialization, he can put a pointer to the mask. If the user wants to exclude a particular parameter from serialization, he should set the corresponding flag in the **TemplateLibraryParamsMask** structure. |

**Returns:** TRUE if params encoded (serialized) or FALSE if not.

**TemplateLibraryParamsMask** structure declaration:

```cpp
struct TemplateLibraryParamsMask
{
    bool firstParam{ true };
    bool secondParam{ true };
    bool thirdParam{ true };
};
```

Example without parameters mask:

```cpp
// Prepare random params.
cr::templ::TemplateLibraryParams params1;
params1.firstParam = true;
params1.secondParam = rand() % 255;
params1.thirdParam = static_cast<float>(rand() % 255);

// Encode (serialize) params.
int bufferSize = 128;
uint8_t buffer[128];
int size = 0;
params1.encode(buffer, bufferSize, size);
```

Example with parameters mask:

```cpp
// Prepare random params.
cr::templ::TemplateLibraryParams params1;
params1.firstParam = true;
params1.secondParam = rand() % 255;
params1.thirdParam = static_cast<float>(rand() % 255);

// Prepare mask.
cr::templ::TemplateLibraryParamsMask mask;
mask.firstParam = false;
mask.secondParam = true; // Include only one param fr encoding.
mask.thirdParam = false;

// Encode (serialize) params.
int bufferSize = 128;
uint8_t buffer[128];
int size = 0;
params1.encode(buffer, bufferSize, size, &mask);
```



## Deserialize template params

[TemplateLibraryParams](#TemplateLibraryParams-class-description) class provides method **decode(...)** to deserialize params. Deserialization of template params necessary in case when you need to receive params via communication channels. Method automatically recognizes which parameters were serialized by **encode(...)** method. Method declaration:

```cpp
bool decode(uint8_t* data, int dataSize);
```

| Parameter | Value                                          |
| --------- | ---------------------------------------------- |
| data      | Pointer to data buffer with serialized params. |
| dataSize  | Size of command data.                          |

**Returns:** TRUE if params decoded (deserialized) or FALSE if not.

Example:

```cpp
// Prepare random params.
cr::templ::TemplateLibraryParams params1;
params1.firstParam = true;
params1.secondParam = rand() % 255;
params1.thirdParam = static_cast<float>(rand() % 255);

// Encode (serialize) params.
int bufferSize = 128;
uint8_t buffer[128];
int size = 0;
params1.encode(buffer, bufferSize, size);

// Decode (deserialize) params.
cr::templ::TemplateLibraryParams params2;
params2.decode(buffer, size);
```



## Read params from JSON file and write to JSON file

**TemplateLibrary** depends on open source [**ConfigReader**](https://github.com/ConstantRobotics-Ltd/ConfigReader) library which provides method to read params from JSON file and to write params to JSON file. Example of writing and reading params to JSON file:

```cpp
// Write params to file.
cr::utils::ConfigReader inConfig;
cr::templ::TemplateLibraryParams in;
inConfig.set(in, "templateParams");
inConfig.writeToFile("TestTemplateParams.json");

// Read params from file.
cr::utils::ConfigReader outConfig;
if(!outConfig.readFromFile("TestTemplateParams.json"))
{
    cout << "Can't open config file" << endl;
    return false;
}
```

**TestTemplateParams.json** will look like:

```json
{
    "templateParams":
    {
        "firstParam": true,
        "secondParam": 1,
        "thirdParam": 0.5f
    }
}
```



# Build and connect to your project

Typical commands to build **TemplateLibrary**:

```bash
git clone https://github.com/ConstantRobotics-Ltd/TemplateLibrary.git
cd TemplateLibrary
git submodule update --init --recursive
mkdir build
cd build
cmake ..
make
```

If you want connect **TemplateLibrary** to your CMake project as source code you can make follow. For example, if your repository has structure:

```bash
CMakeLists.txt
src
    CMakeList.txt
    yourLib.h
    yourLib.cpp
```

You can add repository **TemplateLibrary** as submodule by commands:

```bash
cd <your respository folder>
git submodule add https://github.com/ConstantRobotics-Ltd/TemplateLibrary.git 3rdparty/TemplateLibrary
git submodule update --init --recursive
```

In you repository folder will be created folder **3rdparty/TemplateLibrary** which contains files of **TemplateLibrary** repository with subrepository **ConfigReader** and **ConfigReader**. New structure of your repository:

```bash
CMakeLists.txt
src
    CMakeList.txt
    yourLib.h
    yourLib.cpp
3rdparty
    TemplateLibrary
```

Create CMakeLists.txt file in **3rdparty** folder. CMakeLists.txt should contain:

```cmake
cmake_minimum_required(VERSION 3.13)

################################################################################
## 3RD-PARTY
## dependencies for the project
################################################################################
project(3rdparty LANGUAGES CXX)

################################################################################
## SETTINGS
## basic 3rd-party settings before use
################################################################################
# To inherit the top-level architecture when the project is used as a submodule.
SET(PARENT ${PARENT}_YOUR_PROJECT_3RDPARTY)
# Disable self-overwriting of parameters inside included subdirectories.
SET(${PARENT}_SUBMODULE_CACHE_OVERWRITE OFF CACHE BOOL "" FORCE)

################################################################################
## CONFIGURATION
## 3rd-party submodules configuration
################################################################################
SET(${PARENT}_SUBMODULE_TEMPLATE_LIBRARY                ON  CACHE BOOL "" FORCE)
if (${PARENT}_SUBMODULE_TEMPLATE_LIBRARY)
    SET(${PARENT}_TEMPLATE_LIBRARY                      ON  CACHE BOOL "" FORCE)
    SET(${PARENT}_TEMPLATE_LIBRARY_TEST                 OFF CACHE BOOL "" FORCE)
    SET(${PARENT}_TEMPLATE_LIBRARY_EXAMPLE              OFF CACHE BOOL "" FORCE)
    SET(${PARENT}_TEMPLATE_LIBRARY_DEMO                 OFF CACHE BOOL "" FORCE)
endif()

################################################################################
## INCLUDING SUBDIRECTORIES
## Adding subdirectories according to the 3rd-party configuration
################################################################################
if (${PARENT}_SUBMODULE_TEMPLATE_LIBRARY)
    add_subdirectory(TemplateLibrary)
endif()
```

File **3rdparty/CMakeLists.txt** adds folder **TemplateLibrary** to your project and excludes test application and example (TemplateLibrary class test applications and example of custom TemplateLibrary class implementation) from compiling. Your repository new structure will be:

```bash
CMakeLists.txt
src
    CMakeList.txt
    yourLib.h
    yourLib.cpp
3rdparty
    CMakeLists.txt
    TemplateLibrary
```

Next you need include folder 3rdparty in main **CMakeLists.txt** file of your repository. Add string at the end of your main **CMakeLists.txt**:

```cmake
add_subdirectory(3rdparty)
```

Next you have to include **TemplateLibrary** library in your **src/CMakeLists.txt** file:

```cmake
target_link_libraries(${PROJECT_NAME} TemplateLibrary)
```

Done!



# Example

Simple example shows how to use library.

```cpp
#include <iostream>
#include "TemplateLibrary.h"

int main(void)
{
	// Create library object.
	cr::templ::TemplateLibrary lib;

	// Set params.
	lib.setParam(cr::templ::TemplateLibraryParam::FIRST_PARAM, 1.0f);
	lib.setParam(cr::templ::TemplateLibraryParam::SECOND_PARAM, 2.0f);
	lib.setParam(cr::templ::TemplateLibraryParam::THIRD_PARAM, 2.0f);

	// Display params.
	std::cout << lib.getParam(cr::templ::TemplateLibraryParam::FIRST_PARAM) << std::endl;
	std::cout << lib.getParam(cr::templ::TemplateLibraryParam::SECOND_PARAM) << std::endl;
	std::cout << lib.getParam(cr::templ::TemplateLibraryParam::THIRD_PARAM) << std::endl;

	// Execute commands.
	lib.executeCommand(cr::templ::TemplateLibraryCommand::FIRST_COMMAND);
	lib.executeCommand(cr::templ::TemplateLibraryCommand::SECOND_COMMAND);
	lib.executeCommand(cr::templ::TemplateLibraryCommand::THIRD_COMMAND);

	// Get params.
	cr::templ::TemplateLibraryParams params;
	lib.getParams(params);

	// Display params.
	std::cout << std::string(params.firstParam ? "true" : "false") << std::endl;
	std::cout << params.secondParam << std::endl;
	std::cout << params.thirdParam << std::endl;

	// Do something.
	std::cout << "Do something:" << std::endl;
	for (int i = 0; i < 10; ++i)
	{
		int value = 0;
		lib.doSomething(value);
		std::cout << "Value: " << value << std::endl;
	}

	return 0;
}
```







