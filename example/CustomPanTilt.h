#pragma once
#include <cstdint>
#include <mutex>
#include <string>
#include "PanTilt.h"



namespace rapidpixel
{
/**
 * @brief Custom pan-tilt device controller class.
 *
 * The class implements all methods of the PanTilt interface. The methods
 * are thread-safe: all access to the parameters is protected by a mutex.
 */
class CustomPanTilt : public PanTilt
{
public:

    /**
     * @brief Class constructor.
     */
    CustomPanTilt();

    /**
     * @brief Class destructor.
     */
    ~CustomPanTilt() override;

    /**
     * @brief Copy constructor. The class owns a mutex: copying is deleted.
     */
    CustomPanTilt(const CustomPanTilt&) = delete;

    /**
     * @brief Move constructor. The class owns a mutex: moving is deleted.
     */
    CustomPanTilt(CustomPanTilt&&) = delete;

    /**
     * @brief Copy assignment operator. Copying is deleted.
     * @return Reference to this object.
     */
    CustomPanTilt& operator= (const CustomPanTilt&) = delete;

    /**
     * @brief Move assignment operator. Moving is deleted.
     * @return Reference to this object.
     */
    CustomPanTilt& operator= (CustomPanTilt&&) = delete;

    /**
     * @brief Get the version of the CustomPanTilt class.
     * @return A string representing the version: "Major.Minor.Patch"
     */
    static std::string getVersion();

    /**
     * @brief Open pan-tilt device.
     * @param initString Init string. It is not used by the example.
     * @return TRUE if the pan-tilt controller is init or FALSE if not.
     */
    bool openPanTilt(std::string initString) override;

    /**
     * @brief Init pan-tilt device with parameters structure.
     * Can be used instead of openPanTilt(...) method.
     * @param params Parameters structure.
     * @return TRUE if the pan-tilt controller init or FALSE if not.
     */
    bool initPanTilt(PanTiltParams& params) override;

    /**
     * @brief Close pan-tilt controller connection.
     */
    void closePanTilt() override;

    /**
     * @brief Get pan-tilt controller is opened status.
     * @return TRUE if the pan-tilt controller is open or FALSE if not.
     */
    bool isPanTiltOpen() override;

    /**
     * @brief Get pan-tilt controller is connected status.
     * @return TRUE if the pan-tilt controller is connected or FALSE if not.
     */
    bool isPanTiltConnected() override;

    /**
     * @brief Set the value for a specific library parameter.
     *
     * The value is checked against the range of the parameter. NaN and
     * infinity are rejected. IS_CONNECTED and IS_OPEN are read only.
     * CUSTOM_1, CUSTOM_2 and CUSTOM_3 are accepted but not stored.
     *
     * @param id The identifier of the library parameter.
     * @param value The value to set for the parameter.
     * @return TRUE if the parameter was successfully set, FALSE otherwise.
     */
    bool setParam(PanTiltParam id, float value) override;

    /**
     * @brief Get the value of a specific library parameter.
     * @param id The identifier of the library parameter.
     * @return The value of the specified parameter or -1 if the parameter
     * doesn't exist.
     */
    float getParam(PanTiltParam id) override;

    /**
     * @brief Get the structure containing all library parameters.
     * @param params Reference to a PanTiltParams structure.
     */
    void getParams(PanTiltParams& params) override;

    /**
     * @brief Execute a PanTilt command.
     *
     * The example accepts the commands RESTART, STOP and all GO_TO_*
     * commands (it does nothing). MOVE_PAN, MOVE_TILT and MOVE_PAN_TILT are
     * not supported.
     *
     * @param id The identifier of the library command to be executed.
     * @param arg1 The argument value used by the command. Not used.
     * @param arg2 The argument value used by the command. Not used.
     * @return TRUE if the command was executed successfully, FALSE otherwise.
     */
    bool executeCommand(PanTiltCommand id,
                        float arg1 = 0.0f, float arg2 = 0.0f) override;

    /**
     * @brief Decode and execute command.
     * @param data Pointer to command data. Must not be nullptr.
     * @param size Size of data, bytes. Must be 11 for SET_PARAM command and
     * 15 for COMMAND.
     * @return TRUE if command decoded and executed or FALSE if not.
     */
    bool decodeAndExecuteCommand(uint8_t* data, int size) override;

private:

    /// Parameters structure (default params). Protected by m_paramsMutex.
    PanTiltParams m_params;
    /// Mutex for parameters access.
    std::mutex m_paramsMutex;
};
}
