#ifndef __ANALOGPAD_H__
#define __ANALOGPAD_H__

#define ANALOGPAD_TYPE (22)

/** @brief Analog pad data
 */
typedef struct
{
    /** @brief X axis, range -128 (up) to 128 (down)
     */
    char AxisX;

    /** @brief Y axis, range -128 (left) to 128 (right)
     */
    char AxisY;

    /** @brief Left trigger axis, range 0 to 255 (pressed)
     */
    unsigned char TriggerL;

    /** @brief Right trigger axis, range 0 to 255 (pressed)
     */
    unsigned char TriggerR;

    /** @brief Indicates whether analog pad is available
     */
    bool IsAvailable;
} AnalogPad;

/** @brief Check whether analog pad is available
 *  @param port Gamepad port
 */
bool AnalogPadIsAvailable(const unsigned int port);

/** @brief Get fetched analog pad data
 *  @param port Gamepad port
 *  @param data Analog pad data
 */
void AnalogPadGetData(const unsigned int port, AnalogPad *data);

#endif
