#include <jo/jo.h>
#include <jo/vdp2.h>
#include "background.h"

// -------------------------------------
// Internal
// -------------------------------------

/** @brief Cloud element data
 */
typedef struct
{
    /* Current cloud position in 3D space */
    jo_pos3D_fixed Pos;

    /* Cloud movement speed */
    jo_fixed Speed;

    /* Cloud texture number */
    int Texture;

    /* Cloud rotation */
    unsigned short Rotation;

    /* Cloud rotation speed */
    unsigned short RotationSpeed;

    /* Is cloud visible */
    bool Visible;
} Cloud;

/* Current sky color */
static int CurrentSkyColor[XYZ] = {BG_SKY_BLUE_R, BG_SKY_BLUE_G, BG_SKY_BLUE_B};

/* Current sky color mix */
static int CurrentColorMix = 32;

/* Current sky color mode */
static BackgroundColorShift CurrentColorMode = BackgroundBlueSky;

/* RGB0 plane palette */
static jo_palette Rgb0aPalette;

/* NBG1 plane palette */
static jo_palette Nbg1Palette;

/* Plane movement offset */
static jo_fixed PlaneMovement;

/* Current movement speed */
static int MovementSpeed = JO_FIXED_1;

/* Maximum movement speed */
static int MovementSpeedCap = JO_FIXED_1;

/* Index of first cloud texture */
static int CloudTextureStartIndex;

/* Offset for first cloud texture */
static int CloudTextureStartOffset;

/* List of all clouds to draw at the moment */
static Cloud Clouds[BG_CLOUDS_MAX];

/* Number of updates that elapsed */
static int ElapsedUpdates;

/* Next value the cloud will spawn at */
static int NextSpawnTarget;

/** @brief Palette loader for background
 */
static jo_palette *BackgroundPaletteAHandling(void)
{
    // We create a new palette for each image. It's not optimal but OK for a demo.
    jo_create_palette(&Rgb0aPalette);
    return (&Rgb0aPalette);
}

/** @brief Palette loader for background
 */
static jo_palette *BackgroundPaletteNbgHandling(void)
{
    jo_create_palette(&Nbg1Palette);
    return (&Nbg1Palette);
}

/** @brief Set shading colors for each scan line
 */
static void SetLineColors()
{
    // Set target level color
    int target[XYZ];
    int targetMix = BG_MIX_MAX;
    bool inTransition = false;

    switch (CurrentColorMode)
    {
        case BackgroundPurpleSky:
            target[X] = BG_SKY_PURPLE_R;
            target[Y] = BG_SKY_PURPLE_G;
            target[Z] = BG_SKY_PURPLE_B;
            targetMix = BG_SKY_PURPLE_A;
            CloudTextureStartOffset = BG_SKY_PURPLE_OFFSET;
            break;

        case BackgroundGreenSky:
            target[X] = BG_SKY_GREEN_R;
            target[Y] = BG_SKY_GREEN_G;
            target[Z] = BG_SKY_GREEN_B;
            targetMix = BG_SKY_GREEN_A;
            CloudTextureStartOffset = BG_SKY_GREEN_OFFSET;
            break;

        case BackgroundOrangeSky:
            target[X] = BG_SKY_ORANGE_R;
            target[Y] = BG_SKY_ORANGE_G;
            target[Z] = BG_SKY_ORANGE_B;
            targetMix = BG_SKY_ORANGE_A;
            CloudTextureStartOffset = BG_SKY_ORANGE_OFFSET;
            break;

        case BackgroundRedSky:
            target[X] = BG_SKY_RED_R;
            target[Y] = BG_SKY_RED_G;
            target[Z] = BG_SKY_RED_B;
            targetMix = BG_SKY_RED_A;
            CloudTextureStartOffset = BG_SKY_RED_OFFSET;
            break;

        case BackgroundBlackSky:
            target[X] = BG_SKY_BLACK_R;
            target[Y] = BG_SKY_BLACK_G;
            target[Z] = BG_SKY_BLACK_B;
            targetMix = BG_SKY_BLACK_A;
            CloudTextureStartOffset = BG_SKY_BLACK_OFFSET;
            break;

        case BackgroundCrimsonSky:
            target[X] = BG_SKY_CRIMSON_R;
            target[Y] = BG_SKY_CRIMSON_G;
            target[Z] = BG_SKY_CRIMSON_B;
            targetMix = BG_SKY_CRIMSON_A;
            CloudTextureStartOffset = BG_SKY_CRIMSON_OFFSET;
            break;

        default:
            target[X] = BG_SKY_BLUE_R;
            target[Y] = BG_SKY_BLUE_G;
            target[Z] = BG_SKY_BLUE_B;
            targetMix = BG_SKY_BLUE_A;
            CloudTextureStartOffset = BG_SKY_BLUE_OFFSET;
            break;
    }

    // Transition to target color
    for (int component = 0; component < XYZ; component++)
    {
        if (target[component] > CurrentSkyColor[component])
        {
            inTransition = true;
            CurrentSkyColor[component]++;
        }
        if (target[component] < CurrentSkyColor[component])
        {
            inTransition = true;
            CurrentSkyColor[component]--;
        }
    }

    // Transition to target mix
    if (targetMix > CurrentColorMix)
    {
        inTransition = true;
        CurrentColorMix++;
    }

    if (targetMix < CurrentColorMix)
    {
        inTransition = true;
        CurrentColorMix--;
    }

    if (inTransition)
        MovementSpeedCap = JO_FIXED_2;
    else
        MovementSpeedCap = JO_FIXED_1;

    if (MovementSpeed > MovementSpeedCap)
        MovementSpeed -= JO_DIV_BY_8(JO_FIXED_1);
    else if (MovementSpeed < MovementSpeedCap)
        MovementSpeed += JO_DIV_BY_16(JO_FIXED_1);

    // Sets palette
    Uint16 *colorPalette = (Uint16 *)Rgb0aPalette.data;

    for (int line = 0; line < 150; line++)
    {
        jo_fixed multiplier = JO_FIXED_1 - jo_sin(line / 3);

        int r = jo_fixed2int(multiplier * CurrentSkyColor[X]);
        int g = jo_fixed2int(multiplier * CurrentSkyColor[Y]);
        int b = jo_fixed2int(multiplier * CurrentSkyColor[Z]);

        colorPalette[line] = JO_COLOR_RGB(r, g, b);
    }
    
    slColRateNbg2(CurrentColorMix);
}

/** @brief Line color table setup (to have this work, remove K_LINECOL from slKtableRB or slKtableRA in vdp2.c in jo)
 */
static void LoadColorCalc()
{
    // set color ratio between RGB0 and NBG2
    slColorCalcOn(RBG0ON | NBG2ON);
    slColorCalc(CC_RATE | CC_EXT | NBG2ON | RBG0ON);
    
    SetLineColors();
}

/** @brief Generate cloud
 *  @param cloud Cloud data
 */
static void GenerateCloud(Cloud *cloud)
{
    jo_fixed speed = jo_int2fixed(jo_random(jo_fixed2int(BG_CLOUD_SPEED_RANGE)));
    bool top = speed > 70000;
    cloud->Texture = CloudTextureStartOffset + (jo_random(BG_CLOUD_COUNT) - 1);
    cloud->Speed = BG_CLOUD_SPEED_MIN + speed;
    cloud->Visible = true;
    cloud->Pos.x = BG_CLOUD_SPAWN_AREA_X;
    cloud->Pos.z = top ? BG_CLOUD_AREA_Z_RANGE : -BG_CLOUD_AREA_Z_RANGE;
    cloud->Pos.y = jo_int2fixed(jo_random(jo_fixed2int(BG_CLOUD_AREA_Y_RANGE)));
    cloud->Rotation = 0;
    cloud->RotationSpeed = 0;

    if (CloudTextureStartOffset == 6)
    {
        cloud->RotationSpeed = 10;
    }
    
    if (top)
    {
        cloud->Pos.y += BG_CLOUD_AREA2_Y_POS;
    }
    else
    {
        cloud->Pos.y += BG_CLOUD_AREA1_Y_POS;
    }
}

// -------------------------------------
// Handle NBG2 layer loading as I cannot get it to work properly in jo
// -------------------------------------

#define NBG2_CEL_ADR            (VDP2_VRAM_B1   + 0x02000)
#define NBG2_MAP_ADR            (VDP2_VRAM_B1   + 0x18000)

#define CELL_WIDTH              (8)
#define CELL_HEIGHT             (8)
#define CELL_SIZE               (CELL_WIDTH * CELL_HEIGHT)
#define CELL_COUNT_PER_ROW      (64)
#define MAP_LENGTH              (512 * 8)

/** @brief Copy of jo_img_to_vdp2_cells from core
 * @param img Image co convert
 * @param verticalFlip Flip vertically
 * @param cell Cell memory
 */
void ImgToCells(const jo_img_8bits * const img, const bool verticalFlip, unsigned char *cell)
{
    register int                x;
    register int                y;
    register int                cell_x;
    register int                cell_y;
    register int                line_y;
    register int                i;

    for (JO_ZERO(x); x < img->width; x += CELL_WIDTH)
    {
        for (JO_ZERO(y), JO_ZERO(line_y); y < img->height; y += CELL_HEIGHT, line_y += JO_MULT_BY_8(img->width))
        {
            cell_x = x;
            cell_y = line_y;
            for (JO_ZERO(i); i < CELL_SIZE; ++i, ++cell, cell_y += img->width)
            {
                if (i != 0 && JO_MOD_POW2(i, CELL_WIDTH) == 0)
                {
                     ++cell_x;
                     cell_y = line_y;
                }
                if (!verticalFlip)
                    *cell = img->data[(img->width - cell_x - 1) + cell_y];
                else
                    *cell = img->data[cell_x + cell_y];
            }
        }
    }
}

/** @brief Copy of __jo_create_map from core
 * @param img Image co convert
 * @param map Map memory
 * @param paletteId Image palette
 * @param mapOffset Map memory offset
 */
static void ImgToMap(const jo_img_8bits * const img, unsigned short *map, const unsigned short paletteId, const int mapOffset)
{
    register int                x;
    register int                y;
    register int                x2;
    register int                y2;
    register int                i;
    unsigned short              paloff;

    paloff = JO_MULT_BY_4096(paletteId);
    y = JO_DIV_BY_8(img->height);
    x = JO_DIV_BY_8(img->width);
    JO_ZERO(y2);
    JO_ZERO(x2);

    for (JO_ZERO(i); i < MAP_LENGTH; ++i)
    {
        if (i != 0 && JO_MOD_POW2(i, CELL_COUNT_PER_ROW) == 0)
            ++x2;
        if (x2 >= x)
            JO_ZERO(x2);
        *map = (JO_MULT_BY_2(y2 + x2 * y) | paloff) + mapOffset;
            ++map;
        ++y2;
        if (y2 >= y)
            JO_ZERO(y2);
    }
}

/** @brief Load repeating image into NBG2
 * @param image Image to load
 * @param paletteId Image palette
 * @param verticalFlip Flip vertically
 */
static void LoadNbg2Image(const jo_img_8bits * image, const int paletteId, const bool verticalFlip)
{
    slPlaneNbg2(PL_SIZE_1x1);
    slCharNbg2(COL_TYPE_256, CHAR_SIZE_1x1);
    slMapNbg2((void *)NBG2_MAP_ADR, (void *)NBG2_MAP_ADR, (void *)NBG2_MAP_ADR, (void *)NBG2_MAP_ADR);
    slPageNbg2((void *)NBG2_CEL_ADR, 0, PNB_1WORD | CN_12BIT);
    ImgToCells(image, verticalFlip, (void *)NBG2_CEL_ADR);
    ImgToMap(image, (void *)NBG2_MAP_ADR, paletteId, JO_VDP2_CELL_TO_MAP_OFFSET((void *)NBG2_CEL_ADR));
}

// -------------------------------------
// Public
// -------------------------------------

void BackgroundInitializeRGB0()
{
    jo_enable_background_3d_plane(JO_COLOR_Black);

    // Load RGB0 texture
    jo_img_8bits img;
    img.data = JO_NULL;
    jo_set_tga_palette_handling(BackgroundPaletteAHandling);
    jo_tga_8bits_loader(&img, JO_ROOT_DIR, "FLOOR.TGA", 0);
    jo_vdp2_set_rbg0_plane_a_8bits_image(&img, Rgb0aPalette.id, true, true);
    jo_free_img(&img);

    img.data = JO_NULL;
    jo_set_tga_palette_handling(BackgroundPaletteNbgHandling);
    jo_tga_8bits_loader(&img, JO_ROOT_DIR, "FLOOR2.TGA", 0);
    LoadNbg2Image(&img, Nbg1Palette.id, true);
    jo_free_img(&img);

    LoadColorCalc();
}

void BackgroundInitialize()
{
    // Reset variables
    PlaneMovement = 0;
    NextSpawnTarget = 0;
    CloudTextureStartOffset = 0;

    for (int index = 0; index < BG_CLOUDS_MAX; index++)
    {
        Clouds[index].Visible = false;
    }

    CloudTextureStartIndex = jo_sprite_add_tga(JO_ROOT_DIR, "CLOUD1.TGA", JO_COLOR_Purple);
    jo_sprite_add_tga(JO_ROOT_DIR, "CLOUD2.TGA", JO_COLOR_Purple);
    jo_sprite_add_tga(JO_ROOT_DIR, "CLOUD3.TGA", JO_COLOR_Purple);
    jo_sprite_add_tga(JO_ROOT_DIR, "STAR1.TGA", JO_COLOR_Purple);
    jo_sprite_add_tga(JO_ROOT_DIR, "STAR2.TGA", JO_COLOR_Purple);
    jo_sprite_add_tga(JO_ROOT_DIR, "STAR3.TGA", JO_COLOR_Purple);
    jo_sprite_add_tga(JO_ROOT_DIR, "ROCK1.TGA", JO_COLOR_Purple);
    jo_sprite_add_tga(JO_ROOT_DIR, "ROCK2.TGA", JO_COLOR_Purple);
    jo_sprite_add_tga(JO_ROOT_DIR, "ROCK3.TGA", JO_COLOR_Purple);

}

void BackgroundDraw()
{
    jo_3d_push_matrix();
    {
        jo_3d_rotate_matrix(BG_PLACEMENT_X_ANG, BG_PLACEMENT_Y_ANG, BG_PLACEMENT_Z_ANG);

        // Draw floor
        jo_3d_push_matrix();
        {
            jo_3d_translate_matrix_fixed(PlaneMovement, 360 << 16, -((-BG_PLACEMENT_DEPTH) << 16));
            jo_background_3d_plane_a_draw(true);
        }
        jo_3d_pop_matrix();

        slScrMoveNbg2(JO_FIXED_1_DIV_2, 0);

        jo_sprite_enable_gouraud_shading();

        jo_color color = JO_COLOR_RGB(CurrentSkyColor[X], CurrentSkyColor[Y], CurrentSkyColor[Z]);
        jo_set_gouraud_shading_colors(color,color,color,color);

        for (int index = 0; index < BG_CLOUDS_MAX; index++)
        {
            if (Clouds[index].Visible)
            {
                jo_3d_push_matrix();
                {
                    jo_3d_translate_matrix_fixed(Clouds[index].Pos.x, Clouds[index].Pos.y, Clouds[index].Pos.z);
                    jo_3d_rotate_matrix_z(Clouds[index].Rotation);
                    jo_3d_rotate_matrix_x(BG_CLOUD_ANGL);
                    jo_3d_draw_sprite(CloudTextureStartIndex + Clouds[index].Texture);
                }
                jo_3d_pop_matrix();

                Clouds[index].Rotation += Clouds[index].RotationSpeed;
            }
        }
        
        jo_sprite_disable_gouraud_shading();
    }
    jo_3d_pop_matrix();
}

void BackgroundUdpate()
{
    SetLineColors();
    int firstFree = -1;

    for (int index = 0; index < BG_CLOUDS_MAX; index++)
    {
        if (Clouds[index].Visible)
        {
            Clouds[index].Pos.x += Clouds[index].Speed;

            if (Clouds[index].Pos.x > BG_CLOUD_DESPAWN_AREA_X)
            {
                Clouds[index].Visible = false;
            }
        }
        else if (firstFree < 0)
        {
            firstFree = index;
        }
    }

    if (NextSpawnTarget >= ElapsedUpdates &&
        firstFree >= 0)
    {
        NextSpawnTarget = BG_UPDATE_MIN + jo_random(BG_UPDATE_RANGE) - 1;

        if ((int)CurrentColorMode >= (int)BackgroundRedSky)
        {
            NextSpawnTarget += (BG_UPDATE_MIN >> 1) * ((int)CurrentColorMode - 1);
        }

        NextSpawnTarget = JO_MIN(NextSpawnTarget, BG_UPDATE_MAX);
        ElapsedUpdates = BG_UPDATE_MAX;
        GenerateCloud(&Clouds[firstFree]);
    }

    PlaneMovement += MovementSpeed;
    ElapsedUpdates++;

    if (PlaneMovement >= BG_MOVEMENT_RANGE)
        PlaneMovement = 0;

    if (ElapsedUpdates >= BG_UPDATE_MAX)
        ElapsedUpdates = 0;
}

void BackgroundSetColorShift(const BackgroundColorShift color)
{
    CurrentColorMode = color;
}
