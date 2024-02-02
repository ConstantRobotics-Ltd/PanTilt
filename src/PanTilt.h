#include <string>
#include <cstdint>
#include "ConfigReader.h"



namespace cr::pantilt
{
/**
 * @brief Mask for PanTilt library params for encoding (serializing).
 */
struct PanTiltParamsMask
{
    bool firstParam{ true };
    bool secondParam{ true };
    bool thirdParam{ true };
};

/**
 * @brief Class of PanTilt library parameters.
 */
class PanTiltParams
{
public:

    /// First param. Here describe what status does this flag define.
    bool firstParam{ false };
    /// Second param. Here describe nuances and param value valid range.
    int secondParam{ 0 };
    /// Third param. Here describe nuances and param value valid range.
    float thirdParam{ 0.0f };

    /// Macro from ConfigReader to make params readable/writable from JSON.
    JSON_READABLE(PanTiltParams, firstParam, secondParam, thirdParam)

    /// operator =
        PanTiltParams& operator= (const PanTiltParams& src);

    /**
     * @brief Encode (serialize) params.
     * @param data Pointer to buffer to store serialized params.
     * @param  bufferSize Size of buffer.
     * @param size Size of encoded (serialized) data. Will be <= bufferSize.
     * @param mask Pointer to mask structure. Used to exclude particular
     * params from encoding (from serialization).
     * @return TRUE if params encoded (serialized) or FALSE if:
     * 1. bufferSize less tha size of serialized params.
     * 2. Can be other errors depends on implementation.
     */
    bool encode(uint8_t* data, int bufferSize, int& size,
                                             PanTiltParamsMask* mask = nullptr);

    /**
     * @brief Decode (deserialize) params.
     * @param data Pointer to buffer with encoded (serialized) params.
     * @param dataSize Size of encoded (serialized) params.
     * @return TRUE if params decoded, FALSE otherwise.
     */
    bool decode(uint8_t* data, int dataSize);
};

/**
 * @brief Enum of PanTilt library params.
 */
enum class PanTiltParam
{
    /// First param. Here describe nuances and param value valid range.
    FIRST_PARAM = 1,
    /// Second param. Here describe nuances and param value valid range.
    SECOND_PARAM,
    /// Third param. Here describe nuances and param value valid range.
    THIRD_PARAM
};

/**
 * @brief Enum of PanTilt library commands.
 */
enum class PanTiltCommand
{
    /// First command.
    FIRST_COMMAND = 1,
    /// Second command.
    SECOND_COMMAND,
    /// Third command.
    THIRD_COMMAND
};

/**
 * @brief Library PanTilt class with PanTilt methods.
 */
class PanTilt
{
public:

    /**
     * @brief Class constructor.
     */
    PanTilt();

    /**
     * @brief Class destructor.
     */
    ~PanTilt();

    /**
     * @brief Get the version of the PanTilt class.
     * @return A string representing the version: "Major.Minor.Patch"
     */
    static std::string getVersion();

    /**
     * @brief Set the value for a specific library parameter.
     * @param id The identifier of the library parameter.
     * @param value The value to set for the parameter.
     * @return TRUE if the parameter was successfully set, FALSE otherwise.
     */
    bool setParam(PanTiltParam id, float value);

    /**
     * @brief Get the value of a specific library parameter.
     * @param id The identifier of the library parameter.
     * @return The value of the specified parameter.
     */
    float getParam(PanTiltParam id) const;

    /**
     * @brief Get the structure containing all library parameters.
     * @param params Reference to a PanTiltParams structure.
     */
    void getParams(PanTiltParams& params) const;

    /**
     * @brief Execute a PanTilt command.
     * @param id The identifier of the library command to be executed.
     * @return TRUE if the command was executed successfully, FALSE otherwise.
     */
    bool executeCommand(PanTiltCommand id);

    /**
     * @brief Any useful method of library.
     * @param value Output value.
     * @return TRUE if success, FALSE otherwise.
     */
    bool doSomething(int& value);

private:

    /// Library parameters structure.
    PanTiltParams m_params;
    /// Any private variable.
    int m_variable{ 0 };
};
}