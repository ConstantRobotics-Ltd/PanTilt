![pantilt_web_logo](./static/pantilt_web_logo.png)



# **PanTilt C++ library**

**v1.1.1**



# Table of contents

- [Overview](#overview)
- [Versions](#versions)
- [Library files](#library-files)
- [PanTilt interface class description](#pantilt-interface-class-description)
  - [PanTilt interface class declaration](#pantilt-interface-class-declaration)
  - [getVersion method](#getversion-method)
  - [openPanTilt method](#openpantilt-method)
  - [initPanTilt method](#initpantilt-method)
  - [closePanTilt method](#closepantilt-method)
  - [isPanTiltOpen method](#ispantiltopen-method)
  - [isPanTiltConnected method](#ispantiltconnected-method)
  - [setParam method](#setparam-method)
  - [getParam method](#getparam-method)
  - [getParams method](#getparams-method)
  - [executeCommand method](#executecommand-method)
  - [encodeSetParamCommand method](#encodesetparamcommand-method)
  - [encodeCommand method](#encodecommand-method)
  - [decodeCommand method](#decodecommand-method) 
  - [decodeAndExecuteCommand method](#decodeandexecutecommand-method)
- [Data structures](#data-structures)
  - [PanTiltCommand enum](#pantiltcommand-enum)
  - [PanTiltParam enum](#pantiltparam-enum)
- [PanTiltParams class description](#pantiltparams-class-description)
  - [PanTiltParams class declaration](#pantiltparams-class-declaration)
  - [Serialize PanTilt params](#serialize-pantilt-params)
  - [Deserialize PanTilt params](#deserialize-pantilt-params)
  - [Read params from JSON file and write to JSON file](#read-params-from-json-file-and-write-to-json-file)
- [Build and connect to your project](#build-and-connect-to-your-project)
- [How to make custom implementation](#how-to-make-custom-implementation)



# Overview

**PanTilt** is a C++ library designed to serve as a standard interface for various pan-tilt devices. The library defines interface and data structures for pan-tilt software controllers. Also it provides methods to encode / decode commands and encode / decode parameters. **PanTilt.h** file contains list of data structures ([PanTiltCommand enum](#pantiltcommand-enum) (defines action commands IDs), [PanTiltParam enum](#pantiltparam-enum) (defines parameters IDs), [PanTiltParams](#pantiltparams-class-declaration) class) and [PanTilt](#pantilt-interface-class-declaration) class declaration. **PanTilt** interface depends on [ConfigReader](https://rapidpixel.constantrobotics.com/docs/Service/ConfigReader.html) library (provides methods to read / write JSON config files, source code included, Apache 2.0 license). It uses C++17 standard. The library is licensed under the **Apache 2.0** license.



# Versions

**Table 1** - Library versions.

| Version | Release date | What's new                                    |
| ------- | ------------ | --------------------------------------------- |
| 1.0.0   | 06.02.2024   | - First version of PanTilt interface library. |
| 1.0.1   | 23.04.2024   | - Documentation updated for website.          |
| 1.0.2   | 24.05.2024   | - Documentation updated.                      |
| 1.0.3   | 10.06.2024   | - Fix version error in decodeCommand method.  |
| 1.1.0   | 24.07.2024   | - Interface changed.<br />- CMake updated.<br />- Mistakes in documentation fixed. |
| 1.1.1   | 03.09.2024   | - Documentation updated.<br /> |



# Library files

The library supplied by source code only. The user would be given a set of files in the form of a CMake project (repository). The repository structure is shown below:

```xml
CMakeLists.txt ----------------- Main CMake file of the library.
3rdparty ----------------------- Folder with third-party libraries.
    CMakeLists.txt ------------- CMake file to include third-party libraries.
    ConfigReader --------------- Folder with ConfigReader library source code.
test --------------------------- Folder for internal tests of library.
    CMakeLists.txt ------------- CMake file for tests application.
    main.cpp ------------------- Source code file of test application.
src ---------------------------- Folder with source code of the library.
    CMakeLists.txt ------------- CMake file of the library.
    PanTilt.cpp ---------------- C++ implementation file.
    PanTilt.h ------------------ Header file which includes PanTilt class declaration.
    PanTiltVersion.h ----------- Header file which includes version of the library.
    PanTiltVersion.h.in -------- CMake service file to generate version file.
example ------------------------ Folder with source code of the custom PanTilt implementation.
    CMakeLists.txt ------------- CMake file of the library.
    CustomPanTilt.cpp ---------- C++ implementation file.
    CustomPanTilt.h ------------ Header file which includes custom PanTilt class declaration.
    CustomPanTiltVersion.h ----- Header file which includes version of the library.
    CustomPanTiltVersion.h.in -- CMake service file to generate version file.
```



# PanTilt interface class description



## PanTilt interface class declaration

**PanTilt** interface class declared in **PanTilt.h** file. Class declaration:

```cpp
namespace cr
{
namespace pantilt
{
class PanTilt
{
public:

    /// Class destructor.
    virtual ~PanTilt();

    /// Get the version of the PanTilt class.
    static std::string getVersion();

    /// Open pan-tilt device.
    virtual bool openPanTilt(std::string initString) = 0;

    /// Init pan-tilt device by set of parameters.
    virtual bool initPanTilt(PanTiltParams& params) = 0;

    /// Close pan-tilt controller connection.
    virtual void closePanTilt() = 0;

    /// Get pan-tilt controller open status.
    virtual bool isPanTiltOpen() = 0;

    /// Get pan-tilt controller is connected status.
    virtual bool isPanTiltConnected() = 0;

    /// Set the value for a specific library parameter.
    virtual bool setParam(PanTiltParam id, float value) = 0;

    /// Get the value of a specific library parameter.
    virtual float getParam(PanTiltParam id) = 0;

    /// Get the structure containing all library parameters.
    virtual void getParams(PanTiltParams& params) = 0;

    /// Execute a PanTilt command.
    virtual bool executeCommand(PanTiltCommand id,
                                float arg1 = 0.0f, float arg2 = 0.0f) = 0;

    /// Encode set param command.
    static void encodeSetParamCommand(uint8_t* data, int& size,
                                      PanTiltParam id, float value);

    /// Encode command.
    static void encodeCommand(uint8_t* data, int& size, PanTiltCommand id,
                              float arg1 = 0.0f, float arg2 = 0.0f);

    /// Decode command.
    static int decodeCommand(uint8_t* data, int size, PanTiltParam& paramId,
                    PanTiltCommand& commandId, float& value1, float& value2);

    /// Decode and execute command.
    virtual bool decodeAndExecuteCommand(uint8_t* data, int size) = 0;
};
}
}
```



## getVersion method

The **getVersion()** method returns string of current class version. Method declaration:

```cpp
static std::string getVersion();
```

Method can be used without **PanTilt** class instance:

```cpp
std::cout << "PanTilt version: " << cr::pantilt::PanTilt::getVersion();
```

Console output:

```bash
PanTilt class version: 1.1.1
```



## openPanTilt method

The **openPanTilt(...)** method opens (initializes) pan-tilt controller. This method can be used instead of [initPanTilt(...)](#initpantilt-method) method. All parameters will be initialized by default. Method declaration:

```cpp
virtual bool openPanTilt(std::string initString) = 0;
```

| Parameter  | Description                                                  |
| ---------- | ------------------------------------------------------------ |
| initString | Initialization string. Particular pan-tilt controller can have unique initialization string format. Initialization string parts have to be divided with  '**;**' symbol. Recommended pan-tilt controller initialization string for controllers which use serial port: **"/dev/ttyUSB0;9600;100"** ("/dev/ttyUSB0" - serial port name, "9600" - baudrate, "100" - serial port read timeout). |

**Returns:** TRUE if the pan-tilt initialized or FALSE if not.



## initPanTilt method

The **initPanTilt(...)** method initializes pan-tilt controller by set of parameters. This method can be used instead of [openPanTilt(...)](#openpantilt-method) method ([PanTiltParams](#pantiltparams-class-declaration) class includes **initString**) when pan-tilt controller initialization should be launched with desired parameters. Method declaration:

```cpp
virtual bool initPanTilt(PanTiltParams& params) = 0;
```

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| params    | [PanTiltParams](#pantiltparams-class-declaration) class object which includes all parameters. [PanTiltParams](#pantiltparams-class-declaration) class includes initString which used in [openPanTilt(...)](#openpantilt-method) method. |

**Returns:** TRUE if the pan-tilt controller is initialized or FALSE if not.



## closePanTilt method

The **initPanTilt(...)** method designed to close connection to pan-tilt. Method declaration:

```cpp
virtual void closePanTilt() = 0;
```



## isPanTiltOpen method

The **isPanTiltOpen(...)** method returns initialization status. This status shows if the controller was initialized but doesn't show, if controller has communication with pan-tilt equipment. For example, if pan-tilt unit has serial port and the  controller connected to serial port (opens serial port file in OS) but equipment may be not active (no power). In this case **isPanTiltOpen(...)** (returns TRUE) status just shows that the controller has opened serial port. Method declaration:

```cpp
virtual bool isPanTiltOpen() = 0;
```

**Returns:** TRUE if the pan-tilt controller is initialized or FALSE if not.



## isPanTiltConnected method

The **isPanTiltConnected(...)** is a method designed to ascertain the connection status of the pan-tilt system. This status indicates whether the pan-tilt controller is actively communicating with the pan-tilt equipment. For example, if the pan-tilt unit is physically connected to the controller via a serial port (with the port open in the operating system), but the equipment itself is inactive due to a lack of power, the method will return FALSE, signifying no data exchange. Conversely, if the pan-tilt system is successfully communicating with the equipment, the method will return TRUE. It's important to note that if the pan-tilt controller is not initialized, the connection status will always be FALSE. Method declaration:

```cpp
virtual bool isPanTiltConnected() = 0;
```

**Returns:** TRUE if the pan-tilt controller has data exchange with equipment or FALSE if not.



## setParam method

The **setParam(...)** method sets new parameters value. **PanTilt** based library should provide thread-safe **setParam(...)** method call. This means that the **setParam(...)** method can be safely called from any thread. Method declaration:

```cpp
virtual bool setParam(PanTiltParam id, float value) = 0;
```

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| id        | Parameter ID according to [PanTiltParam](#pantiltparam-enum) enum. |
| value     | Parameter value. Value depends on parameter ID.              |

**Returns:** TRUE if the parameter was set or FALSE if not.



## getParam method

The **getParam(...)** method returns parameter value. **PanTilt** based library should provide thread-safe **getParam(...)** method call. This means that the **getParam(...)** method can be safely called from any thread. Method declaration:

```cpp
virtual float getParam(PanTiltParam id) = 0;
```

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| id        | Parameter ID according to [PanTiltParam](#pantiltparam-enum) enum. |

**Returns:** parameter value or **-1** if the parameters doesn't exist.



## getParams method

The **getParams(...)** method is designed to obtain all pant-tilt parameters. **PanTilt** based library should provide thread-safe **getParams(...)** method call. This means that the **getParams(...)** method can be safely called from any thread. Method declaration:

```cpp
virtual void getParams(PanTiltParams& params) = 0;
```

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| params    | Reference to [PanTiltParams](#pantiltparams-class-description) object to store params. |



## executeCommand method

The **executeCommand(...)** method executes action command. **PanTilt** based library should provide thread-safe **executeCommand(...)** method call. This means that the **executeCommand(...)** method can be safely called from any thread. Method declaration:

```cpp
virtual bool executeCommand(PanTiltCommand id, float arg1 = 0.0f, float arg2 = 0.0f) = 0;
```

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| id        | Command ID according to [PanTiltCommand](#pantiltcommand-enum) enum. |
| arg1      | First command argument. Value depends on command ID.         |
| arg2      | Second command argument. Value depends on command ID.        |

**Returns:** TRUE if the command is executed or FALSE if not.



## encodeSetParamCommand method

The **encodeSetParamCommand(...)** static method encodes command to change any remote PanTilt parameter. To control a pan-tilt device remotely, the developer has to design his own protocol and according to it encode the command and deliver it over the communication channel. To simplify this, the **PanTilt** class contains static methods for encoding the control command. The **PanTilt** class provides two types of commands: a parameter change command (SET_PARAM) and an action command (COMMAND). **encodeSetParamCommand(...)** designed to encode SET_PARAM command. Method declaration:

```cpp
static void encodeSetParamCommand(uint8_t* data, int& size, PanTiltParam id, float value);
```

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| data      | Pointer to data buffer for encoded command. Must have size >= 11. |
| size      | Size of encoded data. Will be 11 bytes.                      |
| id        | Parameter ID according to [PanTiltParam](#pantiltparam-enum) enum. |
| value     | Parameter value. Valued depends on parameter ID.             |

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

The **encodeCommand(...)** static method encodes command for PanTilt remote control. To control a pan-tilt device remotely, the developer has to design his own protocol and according to it encode the command and deliver it over the communication channel. To simplify this, the **PanTilt** class contains static methods for encoding the control command. The **PanTilt** class provides two types of commands: a parameter change command (SET_PARAM) and an action command (COMMAND). **encodeCommand(...)** designed to encode COMMAND command (action command). Method declaration:

```cpp
static void encodeCommand(uint8_t* data, int& size, PanTiltCommand id, float arg1 = 0.0f, float arg2 = 0.0f);
```

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| data      | Pointer to data buffer for encoded command. Must have size >= 15. |
| size      | Size of encoded data. Will be 15 bytes.                      |
| id        | Command ID according to [PanTiltCommand](#pantiltcommand-enum) enum. |
| arg1      | First command argument. Value depends on command ID.         |
| arg2      | Second command argument. Value depends on command ID.        |

**encodeCommand(...)** is static and used without **PanTilt** class instance. This method used on client side (control system). Command encoding example:

```cpp
// Buffer for encoded data.
uint8_t data[7];
// Size of encoded data.
int size = 0;
// Encode command.
PanTilt::encodeCommand(data, size, PanTilt::GO_TO_PAN_ANGLE, 10, 10);
```



## decodeCommand method

The **decodeCommand(...)** static method decodes command on pan-tilt device controller side. Method declaration:

```cpp
static int decodeCommand(uint8_t* data, int size, PanTiltParam& paramId, PanTiltCommand& commandId, float& value1, float& value2);
```

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| data      | Pointer to input command.                                    |
| size      | Size of command. Must be 11 bytes for SET_PARAM and 15 bytes for COMMAND. |
| paramId   | PanTilt parameter ID according to [PanTiltParam](#pantiltparam-enum) enum. After decoding SET_PARAM command the method will return parameter ID. |
| commandId | PanTilt command ID according to [PanTiltCommand](#pantiltcommand-enum) enum. After decoding COMMAND the method will return command ID. |
| value1    | PanTilt parameter value (after decoding SET_PARAM command) or first command argument (after decoding COMMAND). |
| value1    | Second command argument (after decoding COMMAND).            |

**Returns:** **0** - in case decoding COMMAND, **1** - in case decoding SET_PARAM command or **-1** in case errors.



## decodeAndExecuteCommand method

The **decodeAndExecuteCommand(...)** method decodes and executes command on pan-tilt device controller side. The particular implementation of the PanTilt controller must provide thread-safe **decodeAndExecuteCommand(...)** method call. This means that the **decodeAndExecuteCommand(...)** method can be safely called from any thread. Method declaration:

```cpp
virtual bool decodeAndExecuteCommand(uint8_t* data, int size) = 0;
```

| Parameter | Description                                                  |
| --------- | ------------------------------------------------------------ |
| data      | Pointer to input command.                                    |
| size      | Size of command. Must be 11 bytes for SET_PARAM or 15 bytes for COMMAND. |

**Returns:** TRUE if command decoded (SET_PARAM or COMMAND) and executed (action command or set param command).



# Data structures



## PanTiltCommand enum

Enum declaration:

```cpp
namespace cr
{
namespace pantilt
{
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
    /// negative is counterclockwise. First argument is pan speed, second is tilt speed.
	MOVE_PAN_TILT
};
}
}
```

**Table 2** - Commands description.

| Command                 | Description                                                  |
| ----------------------- | ------------------------------------------------------------ |
| RESTART                 | Restart pan-tilt device.                                     |
| STOP                    | Stop movement, block all running commands and left device in current state. |
| GO_TO_PAN_POSITION      | Go to given pan motor position. Valid values from 0 to 65535 (MAX_UINT_16). |
| GO_TO_TILT_POSITION     | Go to given tilt motor position. Valid values from 0 to 65535 (MAX_UINT_16). |
| GO_TO_PAN_TILT_POSITION | Go to given pan and tilt motor position. Valid values from 0 to 65535 (MAX_UINT_16). |
| GO_TO_PAN_ANGLE         | Go to given pan angle. Valid values from -180.0° to 180.0°.  |
| GO_TO_TILT_ANGLE        | Go to given tilt angle. Valid values from -180.0° to 180.0°. |
| GO_TO_PAN_TILT_ANGLE    | Go to given pan and tilt angle. Valid values from -180.0° to 180.0°. |
| GO_TO_HOME              | Go to home position.                                         |
| MOVE_PAN                | Move pan with set velocity given as an argument.             |
| MOVE_TILT               | Move tilt with set velocity given as an argument.            |
| MOVE_PAN_TILT           | Move pan with set velocity given as first argument and tilt with second. |



## PanTiltParam enum

Enum declaration:

```cpp
namespace cr
{
namespace pantilt
{
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
    /// Pan motor speed. Positive speed is clockwise, 
    /// negative is counterclockwise.
    PAN_MOTOR_SPEED,
    /// Tilt motor speed. Positive speed is clockwise, 
    /// negative is counterclockwise.
    TILT_MOTOR_SPEED,
    /// Status defining if the pan-tilt device is connected.
    IS_CONNECTED,
    /// Status defining if the pan-tilt device is initialized (open).
    IS_OPEN,
    /// Custom param. Value depends on implementation.
    CUSTOM_1,
    /// Custom param. Value depends on implementation.
    CUSTOM_2,
    /// Custom param. Value depends on implementation.
    CUSTOM_3
};
}
}
```

**Table 3** - Params description.

| Parameter           | Access       | Description                                                  |
| ------------------- | ------------ | ------------------------------------------------------------ |
| PAN_MOTOR_POSITION  | read / write | Pan motor position for encoder. Range: 0 to 65535.           |
| TILT_MOTOR_POSITION | read / write | Tilt motor position for encoder. Range: 0 to 65535.          |
| PAN_ANGLE           | read / write | Pan angle. Range: -180.0 to 180.0.                           |
| TILT_ANGLE          | read / write | Tilt angle. Range: -180.0 - 180.0.                           |
| PAN_MOTOR_SPEED     | read / write | Pan motor speed. Positive speed is clockwise, negative is counterclockwise, degree / sec. |
| TILT_MOTOR_SPEED    | read / write | Tilt motor speed. Positive speed is clockwise, negative is counterclockwise, degree / sec. |
| IS_CONNECTED        | read only    | Connection status (read only): 1 - pan-tilt control port connected, 0 - not connected. |
| IS_OPEN             | read only    | Initialization status (read only): 1 - pan-tilt control initialized (open), 0 - not initialized (not open). |
| CUSTOM1             | read / write | Custom parameter. Depends on tan-til controller implementation. |
| CUSTOM2             | read / write | Custom parameter. Depends on tan-til controller implementation. |
| CUSTOM3             | read / write | Custom parameter. Depends on tan-til controller implementation. |



# PanTiltParams class description



## PanTiltParams class declaration

**PanTiltParams** class contains all pan-tilt parameters. Also **PanTiltParams** provides possibility to write / read params from JSON files (**JSON_READABLE** macro) and provides methods to encode and decode params. **PanTiltParams** interface class declared in **PanTilt.h** file. Class declaration:

```cpp
namespace cr
{
namespace pantilt
{
class PanTiltParams
{
public:

    /// Pan motor position for encoder. Range: 0 - 65535.
    int panMotorPosition{ 0 };
    /// Tilt motor position for encoder. Range: 0 - 65535.
    int tiltMotorPosition{ 0 };
    /// Pan angle. Range: -180.0 to 180.0.
    float panAngle{ 0.0f };
    /// Tilt angle. Range: -180.0 to 180.0.
    float tiltAngle{ 0.0f };
    /// Pan motor speed. Range: -100.0 to 100.0.
    float panMotorSpeed{ 0.0f };
    /// Tilt motor speed. Range: -100.0 to 100.0. 
    float tiltMotorSpeed{ 0.0f };
    /// Status defining if the pan-tilt device is connected.
    bool isConnected{ false };
	/// Status defining if the pan-tilt device is initialized (open).
    bool isOpen{ false };
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
    JSON_READABLE(PanTiltParams, panMotorSpeed, tiltMotorSpeed,
                  initString, custom1, custom2, custom3)

    /// operator =
    PanTiltParams& operator= (const PanTiltParams& src);

    /// Encode (serialize) params.
    bool encode(uint8_t* data, int bufferSize, int& size,
                                             PanTiltParamsMask* mask = nullptr);

    /// Decode (deserialize) params.
    bool decode(uint8_t* data, int dataSize);
};
}
}
```

**Table 4** - **PanTiltParams** class fields description is related to [PanTiltParam enum](#pantiltparam-enum) description.

| Field             | type   | Description                                            |
| ----------------- | ------ | ------------------------------------------------------ |
| panMotorPosition  | int    | Pan motor position for encoder. Range: 0 to 65535.     |
| tiltMotorPosition | int    | Tilt motor position for encoder. Range: 0 to 65535.    |
| panAngle          | float  | Pan angle. Range: -180.0 to 180.0.                     |
| tiltAngle         | float  | Tilt angle. Range: -180.0 - 180.0.                     |
| panMotorSpeed     | float  | Pan motor speed. Positive speed is clockwise, negative is counterclockwise, degree / sec. |
| tiltMotorSpeed    | float  | Tilt motor speed. Positive speed is clockwise, negative is counterclockwise, degree / sec. |
| isConnected       | bool   | Connection status (read only): 1 - pan-tilt control port connected, 0 - not connected. |
| isOpen            | bool   | Initialization status (read only): 1 - pan-tilt control initialized (open), 0 - not initialized (not open). |
| initString        | string | Initialization string. Format depends on target controller and equivalent as for [openPanTilt(...)](#openpantilt-method) method. |
| custom1           | float  | PanTilt custom parameter. Value depends on particular pan-tilt controller. Custom parameters used when particular pan-tilt equipment has specific unusual parameter. |
| custom2           | float  | PanTilt custom parameter. Value depends on particular pan-tilt controller. Custom parameters used when particular pan-tilt equipment has specific unusual parameter. |
| custom3           | float  | PanTilt custom parameter. Value depends on particular pan-tilt controller. Custom parameters used when particular pan-tilt equipment has specific unusual parameter. |

**None:** *PanTiltParams class fields listed in Table 4 **have to** reflect params set / get by methods setParam(...) and getParam(...).* 



## Serialize PanTilt params

[PanTiltParams](#pantiltparams-class-description) class provides method **encode(...)** to serialize PanTilt params. Serialization of PanTilt params is necessary in case when PanTilt params have to be sent via communication channels. Method provides options to exclude particular parameters from serialization. To do this method inserts binary mask (2 bytes) where each bit represents particular parameter and **decode(...)** method recognizes it. The method doesn't encode **initString**. Method declaration:

```cpp
bool encode(uint8_t* data, int bufferSize, int& size, PanTiltParamsMask* mask = nullptr);
```

| Parameter  | Value                                                        |
| ---------- | ------------------------------------------------------------ |
| data       | Pointer to data buffer. Buffer size must be >= 43 bytes.     |
| bufferSize | Data buffer size. Buffer size must be >= 43 bytes.           |
| size       | Size of encoded data.                                        |
| mask       | Parameters mask - pointer to **PanTiltParamsMask** structure. **PanTiltParamsMask** (declared in **PanTilt.h** file) determines flags for each field (parameter) declared in [PanTiltParams](#pantiltparams-class-description) class. If the user wants to exclude any parameters from serialization, he can put a pointer to the mask. If the user wants to exclude a particular parameter from serialization, he should set the corresponding flag in the **PanTiltParamsMask** structure. |

**Returns:** TRUE if params encoded (serialized) or FALSE if not.

**PanTiltParamsMask** structure declaration:

```cpp
struct PanTiltParamsMask
{
    bool panMotorPosition{ true };
    bool tiltMotorPosition{ true };
    bool panAngle{ true };
    bool tiltAngle{ true };
    bool panMotorSpeed{ true };
    bool tiltMotorSpeed{ true };
    bool isConnected{ true };
    bool isOpen{ true };
    bool custom1{ true };
    bool custom2{ true };
    bool custom3{ true };
};
```

Example without parameters mask:

```cpp
// Prepare parameters.
cr::pantilt::PanTiltParams params;
params.panAngle = 160.0f;

// Encode (serialize) params.
int bufferSize = 128;
uint8_t buffer[128];
int size = 0;
params.encode(buffer, bufferSize, size);
```

Example with parameters mask:

```cpp
// Prepare parameters.
cr::pantilt::PanTiltParams params;
params.panAngle = 160.0f;

// Prepare mask.
cr::pantilt::PanTiltParamsMask mask;
// Exclude tiltAngle.
mask.tiltAngle = false;

// Encode (serialize) params.
int bufferSize = 128;
uint8_t buffer[128];
int size = 0;
params1.encode(buffer, bufferSize, size, &mask);
```



## Deserialize PanTilt params

[PanTiltParams](#pantiltparams-class-description) class provides method **decode(...)** to deserialize params. Deserialization of PanTilt params is necessary in case when it is needed to receive params via communication channels. Method automatically recognizes which parameters were serialized by **encode(...)** method. Method declaration:

```cpp
bool decode(uint8_t* data, int dataSize);
```

| Parameter | Value                                          |
| --------- | ---------------------------------------------- |
| data      | Pointer to data buffer with serialized params. |
| dataSize  | Size of serialized data.                       |

**Returns:** TRUE if params decoded (deserialized) or FALSE if not.

Example:

```cpp
// Prepare parameters.
cr::pantilt::PanTiltParams params1;
params1.panAngle = 160.0f;

// Encode (serialize) params.
int bufferSize = 128;
uint8_t buffer[128];
int size = 0;
params1.encode(buffer, bufferSize, size);

// Decode (deserialize) params.
cr::pantilt::PanTiltParams params2;
params2.decode(buffer, size);
```



## Read params from JSON file and write to JSON file

**PanTilt** depends on open source [ConfigReader](https://rapidpixel.constantrobotics.com/docs/Service/ConfigReader.html) library which provides method to read params from JSON file and to write params to JSON file (**JSON_READABLE** macro defines only few parameters to read / write from JSON). Example of writing and reading params to JSON file:

```cpp
cr::utils::ConfigReader inConfig;
inConfig.set(params1, "panTiltParams");
inConfig.writeToFile("PanTiltParams.json");


cr::utils::ConfigReader outConfig;
if(!outConfig.readFromFile("PanTiltParams.json"))
{
    std::cout << "Can't open config file" << std::endl;
    return false;
}

cr::pantilt::PanTiltParams params2;
if(!outConfig.get(params2, "panTiltParams"))
{
    std::cout << "Can't read params from file" << std::endl;
    return false;
}
```

**PanTiltParams.json** will look like (JSON includes not all parameters):

```json
{
    "panTiltParams": {
        "custom1": 153.0,
        "custom2": 74.0,
        "custom3": 237.0,
        "initString": "Init string",
        "panMotorSpeed": 117.0,
        "tiltMotorSpeed": 27.0
    }
}

```



# Build and connect to your project

Typical commands to build **PanTilt**:

```bash
cd PanTilt
mkdir build
cd build
cmake ..
make
```

If you want connect **PanTilt** to your CMake project as source code you can make follow. For example, if your repository has structure:

```bash
CMakeLists.txt
src
    CMakeList.txt
    yourLib.h
    yourLib.cpp
```

Create **3rdparty** folder in your repository and copy **PanTilt** repository folder there. New structure of your repository:

```bash
CMakeLists.txt
src
    CMakeList.txt
    yourLib.h
    yourLib.cpp
3rdparty
    PanTilt
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
SET(${PARENT}_SUBMODULE_PAN_TILT                        ON  CACHE BOOL "" FORCE)
if (${PARENT}_SUBMODULE_PAN_TILT)
    SET(${PARENT}_PAN_TILT                              ON  CACHE BOOL "" FORCE)
    SET(${PARENT}_PAN_TILT_TEST                         OFF CACHE BOOL "" FORCE)
    SET(${PARENT}_PAN_TILT_EXAMPLE                      OFF CACHE BOOL "" FORCE)
endif()

################################################################################
## INCLUDING SUBDIRECTORIES
## Adding subdirectories according to the 3rd-party configuration
################################################################################
if (${PARENT}_SUBMODULE_PAN_TILT)
    add_subdirectory(PanTilt)
endif()
```

File **3rdparty/CMakeLists.txt** adds folder **PanTilt** to your project and excludes test application and example (PanTilt class test application and example of custom **PanTilt** class implementation) from compiling (by default example and test application excluded from compiling if **PanTilt** included as sub-repository). Your repository new structure will be:

```bash
CMakeLists.txt
src
    CMakeList.txt
    yourLib.h
    yourLib.cpp
3rdparty
    CMakeLists.txt
    PanTilt
```

Next you need include folder 3rdparty in main **CMakeLists.txt** file of your repository. Add string at the end of your main **CMakeLists.txt**:

```cmake
add_subdirectory(3rdparty)
```

Next you have to include **PanTilt** library in your **src/CMakeLists.txt** file:

```cmake
target_link_libraries(${PROJECT_NAME} PanTilt)
```

Done!



# How to make custom implementation

The **PanTilt** class provides only an interface, data structures, and methods for encoding and decoding commands and params. To create your own implementation of the pan-tilt controller, PanTilt repository has to be included in your project (see [Build and connect to your project](#build-and-connect-to-your-project) section). The catalogue **example** (see [Library files](#library-files) section) includes an example of the design of the custom pan-tilt controller. All the methods of the PanTilt interface class have to be included. Custom PanTilt class declaration:

```c++
namespace cr::pantilt
{
/// Custom pan-tilt device controller class.
class CustomPanTilt : public PanTilt
{
public:

    /// Class constructor.
    CustomPanTilt();

    /// Class destructor.
    ~CustomPanTilt();

    /// Get the version of the PanTilt class.
    static std::string getVersion();

    /// Open pan-tilt device.
    bool openPanTilt(std::string initString) override;

    /// Init pan-tilt device with parameters structure.
    bool initPanTilt(PanTiltParams& params) override;

    /// Close pan-tilt controller connection.
    void closePanTilt() override;

    /// Get pan-tilt controller is opened status.
    bool isPanTiltOpen() override;

    /// Get pan-tilt controller is connected status.
    bool isPanTiltConnected() override;

    /// Set the value for a specific library parameter.
    bool setParam(PanTiltParam id, float value) override;

    /// Get the value of a specific library parameter.
    float getParam(PanTiltParam id) override;

    /// Get the structure containing all library parameters.
    void getParams(PanTiltParams& params) override;

    /// Execute a PanTilt command.
    bool executeCommand(PanTiltCommand id, float arg1 = 0.0f, float arg2 = 0.0f) override;

    ///  Decode and execute command.
    bool decodeAndExecuteCommand(uint8_t* data, int size) override;
};
}
```