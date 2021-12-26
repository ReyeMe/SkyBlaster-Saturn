#include <jo/jo.h>
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

    /* Is cloud visible */
    bool Visible;
} Cloud;

/* Current sky color */
static int CurrentSkyColor[XYZ] = { BG_SKY_BLUE_R, BG_SKY_BLUE_G, BG_SKY_BLUE_B };

/* Current sky color mode */
static BackgroundColorShift CurrentColorMode = BackgroundBlueSky;

/* RGB0 plane palette */
static jo_palette Rgb0Palette;

/* Plane movement offset */
static int PlaneMovement;

/* Index of first cloud texture */
static int CloudTextureStartIndex;

/* List of all clouds to draw at the moment */
static Cloud Clouds[BG_CLOUDS_MAX];

/* Number of updates that elapsed */
static int ElapsedUpdates;

/* Next value the cloud will spawn at */
static int NextSpawnTarget;

/** @brief Palette loader for background
 */
static jo_palette *BackgroundPaletteHandling(void)
{
    // We create a new palette for each image. It's not optimal but OK for a demo.
    jo_create_palette(&Rgb0Palette);
    return (&Rgb0Palette);
}

/** @brief Set shading colors for each scan line
 */
static void SetLineColors()
{
    // Set target level color
    int target[XYZ];

    switch (CurrentColorMode)
    {
        case BackgroundOrangeSky:
            target[X] = BG_SKY_ORANGE_R;
            target[Y] = BG_SKY_ORANGE_G;
            target[Z] = BG_SKY_ORANGE_B;
            break;

        default:
            target[X] = BG_SKY_BLUE_R;
            target[Y] = BG_SKY_BLUE_G;
            target[Z] = BG_SKY_BLUE_B;
            break;
    }

    // Transition to target color
    for (int component = 0; component < XYZ; component++)
    {
        if (target[component] > CurrentSkyColor[component]) CurrentSkyColor[component]++;
        if (target[component] < CurrentSkyColor[component]) CurrentSkyColor[component]--;
    }

    // Sets palette
    Uint16 * colorPalette = (Uint16 *)BG_COLOR_RAM_ADR;

    for(int line = 0; line < JO_TV_HEIGHT; line++)
    {
        jo_fixed multiplier = jo_sin(line / 3);

        int r = jo_fixed2int(multiplier * CurrentSkyColor[X]);
        int g = jo_fixed2int(multiplier * CurrentSkyColor[Y]);
        int b = jo_fixed2int(multiplier * CurrentSkyColor[Z]);

    	colorPalette[line+32] = JO_COLOR_RGB(r, g, b);
    }
}

/** @brief Line color table setup (to have this work, remove K_LINECOL from slKtableRB or slKtableRA in vdp2.c in jo)
 */
static void LoadLineColorTable()
{
    SetLineColors();

    // Set indexes to palette
    Uint16 * colorPalette = (Uint16 *)BG_LINE_COLOR_TABLE;

    for(int line = 0; line < JO_TV_HEIGHT; line++)
    {
    	colorPalette[line] = line + BG_COLOR_PAL_OFFSET;
    }

    // Apply line color with ration on RGB0
    slLineColDisp(RBG0ON);
    slLineColTable((void *)BG_LINE_COLOR_TABLE);
    slColorCalc(CC_RATE | CC_2ND | RBG0ON);
    slColorCalcOn(RBG0ON);
    slColRateLNCL(0x1A);
}

/** @brief Generate cloud
 *  @param cloud Cloud data
 */
static void GenerateCloud(Cloud * cloud)
{
    jo_fixed speed = jo_int2fixed(jo_random(jo_fixed2int(BG_CLOUD_SPEED_RANGE)));
    bool top = speed > 70000;
    cloud->Texture = jo_random(BG_CLOUD_COUNT) - 1;
    cloud->Speed = BG_CLOUD_SPEED_MIN + speed;
    cloud->Visible = true;
    cloud->Pos.x = BG_CLOUD_SPAWN_AREA_X;
    cloud->Pos.z = top ? BG_CLOUD_AREA_Z_RANGE : -BG_CLOUD_AREA_Z_RANGE;
    cloud->Pos.y = jo_int2fixed(jo_random(jo_fixed2int(BG_CLOUD_AREA_Y_RANGE)));

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
// Public
// -------------------------------------

void BackgroundInitializeRGB0()
{
    jo_enable_background_3d_plane(JO_COLOR_Black);
    jo_set_tga_palette_handling(BackgroundPaletteHandling);

    // Load RGB0 texture
    jo_img_8bits img;
    img.data = JO_NULL;
    jo_tga_8bits_loader(&img, JO_ROOT_DIR, "FLOOR.TGA", 0);
    jo_background_3d_plane_a_img(&img, Rgb0Palette.id, true, true);
    jo_free_img(&img);

    LoadLineColorTable();
}

void BackgroundInitialize()
{
    // Reset variables
    PlaneMovement = 0;
    NextSpawnTarget = 0;
    
    for (int index = 0; index < BG_CLOUDS_MAX; index++)
    {
        Clouds[index].Visible = false;
    }

    CloudTextureStartIndex = jo_sprite_add_tga(JO_ROOT_DIR, "CLOUD1.TGA", JO_COLOR_White);
    jo_sprite_add_tga(JO_ROOT_DIR, "CLOUD2.TGA", JO_COLOR_White);
    jo_sprite_add_tga(JO_ROOT_DIR, "CLOUD3.TGA", JO_COLOR_White);
}

void BackgroundDraw()
{
    jo_3d_push_matrix();
	{
        jo_3d_rotate_matrix(BG_PLACEMENT_X_ANG, BG_PLACEMENT_Y_ANG, BG_PLACEMENT_Z_ANG);
        
        // Draw floor
        jo_3d_push_matrix();
        {
            jo_3d_translate_matrix(PlaneMovement, 0, BG_PLACEMENT_DEPTH);
            jo_background_3d_plane_a_draw(true);
        }
        jo_3d_pop_matrix();

        for (int index = 0; index < BG_CLOUDS_MAX; index++)
        {
            if (Clouds[index].Visible)
            {
                jo_3d_push_matrix();
                {
                    jo_3d_translate_matrix_fixed(Clouds[index].Pos.x, Clouds[index].Pos.y, Clouds[index].Pos.z);
                    jo_3d_rotate_matrix_x(BG_CLOUD_ANGL);
                    jo_3d_draw_sprite(CloudTextureStartIndex + Clouds[index].Texture);
                }
                jo_3d_pop_matrix();
            }
        }
	}
	jo_3d_pop_matrix();
}

void BackgroundUdpate()
{
    SetLineColors();
    bool created = false;

    for (int index = 0; index < BG_CLOUDS_MAX; index++)
    {
        if (Clouds[index].Visible && Clouds[index].Pos.x > BG_CLOUD_DESPAWN_AREA_X)
        {
            Clouds[index].Visible = false;
        }
        else if (Clouds[index].Visible)
        {
            Clouds[index].Pos.x += Clouds[index].Speed;
        }

        if (NextSpawnTarget == ElapsedUpdates && !created && !Clouds[index].Visible)
        {
            created = true;
            NextSpawnTarget = BG_UPDATE_MIN + jo_random(BG_UPDATE_RANGE) - 1;
            ElapsedUpdates = BG_UPDATE_MAX;
            GenerateCloud(&Clouds[index]);
        }
    }

    PlaneMovement++;
    ElapsedUpdates++;

    if (PlaneMovement >= BG_MOVEMENT_RANGE)
        PlaneMovement = 0;

    if (ElapsedUpdates >= BG_UPDATE_MAX)
        ElapsedUpdates = 0;
}

void BackgroundSetColorShift(BackgroundColorShift color)
{
    CurrentColorMode = color;
}
