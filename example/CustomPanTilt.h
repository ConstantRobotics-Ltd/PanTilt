#pragma once
#include <string>
#include <cstdint>
#include <mutex>
#include "PanTilt.h"



namespace cr::pantilt
{
/**
 * @brief Custom pan-tilt device controller class.
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
    ~CustomPanTilt();

    /**
     * @brief Get the version of the PanTilt class.
     * @return A string representing the version: "Major.Minor.Patch"
     */
    static std::string getVersion();

    /**
     * @brief Open pan-tilt device.
     * @param initString Init string. Format depends on target controller.
     * @return TRUE if the pan-tilt controller is init or FALSE if not.
     */
    bool openPanTilt(std::string initString) override;

    /**
     * @brief Init pan-tilt device with parameters structure.
     * Can be used instead of open(...) method.
     * @param initString Init string. Format depends on target controller.
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
    bool isPanTiltOpened() override;

    /**
     * @brief Get pan-tilt controller is connected status.
     * @return TRUE if the pan-tilt controller is open or FALSE if not.
     */
    bool isPanTiltConnected() override;

    /**
     * @brief Set the value for a specific library parameter.
     * @param id The identifier of the library parameter.
     * @param value The value to set for the parameter.
     * @return TRUE if the parameter was successfully set, FALSE otherwise.
     */
    bool setParam(PanTiltParam id, float value) override;

    /**
     * @brief Get the value of a specific library parameter.
     * @param id The identifier of the library parameter.
     * @return The value of the specified parameter.
     */
    float getParam(PanTiltParam id) override;

    /**
     * @brief Get the structure containing all library parameters.
     * @param params Reference to a PanTiltParams structure.
     */
    void getParams(PanTiltParams& params) override;

    /**
     * @brief Execute a PanTilt command.
     * @param id The identifier of the library command to be executed.
     * @return TRUE if the command was executed successfully, FALSE otherwise.
     */
    bool executeCommand(PanTiltCommand id) override;

    /**
     * @brief Decode and execute command.
     * @param data Pointer to command data.
     * @param size Size of data.
     * @return 0 - command decoded, 1 - set param command decoded, -1 - error.
     */
    bool decodeAndExecuteCommand(uint8_t* data, int size);

private:

    /// Parameters structure (default params).
    PanTiltParams m_params;
    /// Mutex for parameters access.
    std::mutex m_paramsMutex;
};
}