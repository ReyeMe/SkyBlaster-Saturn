#ifndef __FONT_3D_H__
#define __FONT_3D_H__

#define FONT_NUM_START (48)
#define FONT_NUM_END (57)
#define FONT_LET_UP_START (65)
#define FONT_LET_UP_END (90)
#define FONT_LET_START (97)
#define FONT_LET_END (122)

#define FONT_WIDTH JO_FIXED_8
#define FONT_HEIGHT (393216)

/* Font modifier data */
typedef struct
{
    /* Current character location */
    jo_pos3D_fixed Pos;

    /* Current character rotation */
    jo_rot3D Rotation;

    /* Custom data */
    void *customData;
} FontModifier;

/** @brief Initialize font
 */
void FontInitialize();

/** @brief Print text in 3D font centered
 *  @param text String to print
 *  @param modifier Character rendering modifier
 *  @param customData Custom data that can be passed to the modifier
 */
void FontPrintCentered(const char *text, void (*modifier)(const int column, const int row, FontModifier *data), void *customData);

/** @brief Print text in 3D font
 *  @param text String to print
 *  @param modifier Character rendering modifier
 *  @param customData Custom data that can be passed to the modifier
 */
void FontPrint(const char *text, void (*modifier)(const int column, const int row, FontModifier *data), void *customData);

#endif
