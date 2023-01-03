#ifndef __WORLDBACKGROUND_H__
#define __WORLDBACKGROUND_H__

// -------------------------------------
// VDP2 stuff
// -------------------------------------

#define BG_LINE_COLOR_TABLE (VDP2_VRAM_A0 + 0x1f400)
#define BG_COLOR_RAM_ADR (VDP2_COLRAM + 0x00600)
#define BG_COLOR_PAL_OFFSET (800)

// -------------------------------------
// Placement and movement
// -------------------------------------

#define BG_MOVEMENT_RANGE (64 << 16)
#define BG_PLACEMENT_DEPTH (-24)
#define BG_PLACEMENT_X_ANG (-5)
#define BG_PLACEMENT_Y_ANG (180)
#define BG_PLACEMENT_Z_ANG (0)

// -------------------------------------
// Clouds
// -------------------------------------

#define BG_CLOUD_COUNT (3)
#define BG_CLOUD_ANGL (0)
#define BG_CLOUD_SPAWN_AREA_X (-17107200)
#define BG_CLOUD_DESPAWN_AREA_X (20107200)
#define BG_CLOUD_SPEED_MIN JO_FIXED_2
#define BG_CLOUD_SPEED_RANGE JO_FIXED_2
#define BG_CLOUD_AREA_Y_RANGE (655360)
#define BG_CLOUD_AREA2_Y_POS (-6415200)
#define BG_CLOUD_AREA1_Y_POS (6415200)
#define BG_CLOUD_AREA_Z_RANGE JO_FIXED_4
#define BG_CLOUDS_MAX (10)
#define BG_UPDATE_MAX (60)
#define BG_UPDATE_RANGE (30)
#define BG_UPDATE_MIN (25)

// -------------------------------------
// Sky Color mix
// -------------------------------------

#define BG_MIX_MAX (31)
#define BG_MIX_MIN (0)

// -------------------------------------
// Sky shades
// -------------------------------------

#define BG_SKY_BLUE_R (31)
#define BG_SKY_BLUE_G (64)
#define BG_SKY_BLUE_B (255)
#define BG_SKY_BLUE_A (31)

#define BG_SKY_GREEN_R (38)
#define BG_SKY_GREEN_G (127)
#define BG_SKY_GREEN_B (0)
#define BG_SKY_GREEN_A (31)

#define BG_SKY_ORANGE_R (255)
#define BG_SKY_ORANGE_G (125)
#define BG_SKY_ORANGE_B (0)
#define BG_SKY_ORANGE_A (31)

#define BG_SKY_RED_R (200)
#define BG_SKY_RED_G (31)
#define BG_SKY_RED_B (64)
#define BG_SKY_RED_A (26)

#define BG_SKY_PURPLE_R (131)
#define BG_SKY_PURPLE_G (0)
#define BG_SKY_PURPLE_B (200)
#define BG_SKY_PURPLE_A (24)

#define BG_SKY_BLACK_R (20)
#define BG_SKY_BLACK_G (40)
#define BG_SKY_BLACK_B (60)
#define BG_SKY_BLACK_A (24)

#define BG_SKY_CRIMSON_R (120)
#define BG_SKY_CRIMSON_G (8)
#define BG_SKY_CRIMSON_B (46)
#define BG_SKY_CRIMSON_A (18)

/** @brief Sky color shift mode
 */
typedef enum
{
    /* Blue sky shade color */
    BackgroundBlueSky,              // track 4 - water

    /* Green sky shade */
    BackgroundGreenSky,             // track 5 - forest

    /* Orange sky shade */
    BackgroundOrangeSky,            // track 6 - desert

    /* Red sky shade */
    BackgroundRedSky,               // track 7 - corrupted

    /* Purple sky shade */
    BackgroundPurpleSky,            // track 8 - portal

    /* Black sky */
    BackgroundBlackSky,             // track 9 - edge

    /* Crimson sky shade */
    BackgroundCrimsonSky            // track 9 - final
} BackgroundColorShift;

/** @brief Initialize RGB0 background
 */
void BackgroundInitializeRGB0();

/** @brief Initialize background assets
 */
void BackgroundInitialize();

/** @brief Draw background
 *  @return Current movement offset
 */
void BackgroundDraw();

/** @brief Update background movement
 *  @return Current movement offset
 */
void BackgroundUdpate();

/** @brief Set color shift applied to the level
 *  @param color Requested color shift
 */
void BackgroundSetColorShift(const BackgroundColorShift color);

#endif
