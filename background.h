#ifndef __WORLDBACKGROUND_H__
#define __WORLDBACKGROUND_H__

// -------------------------------------
// VDP2 stuff
// -------------------------------------

#define	BG_LINE_COLOR_TABLE (VDP2_VRAM_A0 + 0x1f400)
#define	BG_COLOR_RAM_ADR (VDP2_COLRAM + 0x00600)
#define	BG_COLOR_PAL_OFFSET (800)

// -------------------------------------
// Placement and movement
// -------------------------------------

#define BG_MOVEMENT_RANGE (64)
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
#define BG_CLOUDS_MAX (20)
#define BG_UPDATE_MAX (45)
#define BG_UPDATE_RANGE (30)
#define BG_UPDATE_MIN (15)

// -------------------------------------
// Sky shades
// -------------------------------------

#define BG_SKY_BLUE_R (31)
#define BG_SKY_BLUE_G (64)
#define BG_SKY_BLUE_B (255)

#define BG_SKY_ORANGE_R (255)
#define BG_SKY_ORANGE_G (125)
#define BG_SKY_ORANGE_B (0)

/** @brief Sky color shift mode
 */
typedef enum
{
    /* Blue sky shade color */
    BackgroundBlueSky,

    /* Orange sky shade */
    BackgroundOrangeSky
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
void BackgroundSetColorShift(BackgroundColorShift color);

#endif
