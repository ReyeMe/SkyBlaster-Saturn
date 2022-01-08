#include <jo/jo.h>
#include "analogPad.h"
#include "pcmsys.h"
#include "pcmstm.h"
#include "pcmcdda.h"
#include "model.h"
#include "font3D.h"
#include "score.h"
#include "player.h"
#include "background.h"
#include "bullet.h"
#include "bulletList.h"
#include "ui.h"
#include "uiControls.h"
#include "tools.h"
#include "npc.h"
#include "pickup.h"

void MenuCreateMain(bool backButton);

/* Music stuff */
#define LWRAM (2097152)
void *loading_system_scratch_buffer = (void *)LWRAM;
int snd_adx = 0;
#define MENU_MUSIC 2
#define GAME_MUSIC 3
#define GAME_END_MUSIC 4

/* Main 3D camera */
static jo_camera camera;

/* Player 3D mesh */
static jo_3d_mesh *PlayerMesh;

/* Explosion 3D mesh */
static jo_3d_mesh *ExplosionMesh;

/* Player entity data */
static Player PlayerEntity;

/* Indicates whether player is in game */
static bool IsInGame = false;

/* Indicates whether player is in game over screen */
static bool IsInGameOver = false;

/* Indicates whether game is paused */
static bool IsPaused = false;

/* Indicates whether player can control the game */
static bool IsControllable = false;

/* Number of level player is currently at */
static int CurrentLevel = 0;

/* Current tick level title is at */
static int CurrentLevelTitleTick = 0;

/* Logo mesh collection */
static jo_3d_mesh *Logo;

/* Logo mesh size */
static int LogoSize;

/* Player shooting sound */
static short PlayerShootSound;

/* Index of life sprite */
static int LifeSpriteIndex;

/* Index of bomb sprite */
static int BombSpriteIndex;

/* Index of life icon sprite */
static int LifeIconSpriteIndex;

/* Game over score label text */
static char GameOverScoreLabel[SCORE_TEXT_LEN + 1];

/* Bomb effect */
static int BombEffectLifeTime;

static int debug = 0;
static int debug2 = 40;

/** @brief Called when new game starts
 *  @param backButton Indicates whether back button (B) was pressed
 */
void GameCreateNew(bool backButton)
{
    if (!backButton)
    {
        // Reset player data and position
        PlayerInititalize(&PlayerEntity);
        PlayerEntity.Pos.x = PLAYER_SPAWN_X + JO_FIXED_32;

        // Clear all bullets, NPCs and entities
        BulletListClear(false);
        NpcClearAll();
        PickupClearAll();

        IsInGame = true;
        IsPaused = false;
        IsInGameOver = false;
        IsControllable = false;
        CurrentLevel = 0;
        CurrentLevelTitleTick = 1;
        PlayerEntity.GunLevel = 0;
        BombEffectLifeTime = -1;

        WidgetsClearAll();

        CDDAPlaySingle(GAME_MUSIC, true);
    }
}

/** @brief Create credits menu
 *  @param backButton Indicates whether back button (B) was pressed
 */
void MenuCredits(bool backButton)
{
    if (!backButton)
    {
        IsInGame = false;
        IsPaused = false;
        WidgetsClearAll();
        WidgetsCreateLabel(0, 18, "Coding and Art");
        WidgetsCreateLabel(0, 10, "ReyeMe");
        WidgetsCreateLabel(0, -2, "Music");
        WidgetsCreateLabel(0, -10, "Random");
        WidgetsCreateLabel(0, -22, "Art");
        WidgetsCreateLabel(0, -30, "AnriFox");

        WidgetsSetCurrent(WidgetsCreateButton(44, -40, "Exit", MenuCreateMain));
    }
}

/* Draw logo process
 * @widget Generic widget
 * @message Control message
 */
void DrawLogo(WidgetsWidget *widget, WidgetMessages message)
{
    static int rotation = 0;

    if (message == WIDGET_DRAW)
    {
        jo_3d_push_matrix();
        {
            jo_3d_translate_matrix(widget->y, widget->x, -60);
            jo_3d_rotate_matrix_x(rotation++);

            for (int logoMesh = 0; logoMesh < LogoSize; logoMesh++)
            {
                jo_3d_mesh_draw(&(Logo[logoMesh]));
            }
        }
        jo_3d_pop_matrix();

        if (rotation > 359)
        {
            rotation = 0;
        }
    }
}

/** @brief Create main menu
 *  @param backButton Indicates whether back button (B) was pressed
 */
void MenuCreateMain(bool backButton)
{
    if (!backButton)
    {
        IsInGame = false;
        IsInGameOver = false;
        IsPaused = false;

        WidgetsClearAll();
        WidgetsCreate(0, 17, DrawLogo)->IsSelectable = false;
        WidgetsCreateLabel(0, -5, "Sky Blaster");
        WidgetsSetCurrent(WidgetsCreateButton(0, -22, "New game", GameCreateNew));
        WidgetsCreateButton(0, -30, "Credits", MenuCredits);
    }
}

/** @brief Resume game button action
 *  @param backButton Indicates whether back button (B) was pressed
 */
void MenuResumeGame(bool backButton)
{
    if (!backButton)
    {
        IsPaused = false;
        WidgetsClearAll();
        CDDASetVolume(7, 7);
    }
}

/** @brief Create player bullets
 */
void PlayerShoot()
{
    pcm_play(PlayerShootSound, PCM_PROTECTED, 6);

    if (PlayerEntity.GunLevel == 0)
    {
        Bullet *bullet = jo_malloc(sizeof(Bullet));
        bullet->Pos.x = PlayerEntity.Pos.x;
        bullet->Pos.y = PlayerEntity.Pos.y;
        bullet->Type = 0;
        bullet->Velocity.x = -BULLET_SPEED;
        bullet->Velocity.y = 0;
        BulletListAdd(bullet, true);
    }
    else if (PlayerEntity.GunLevel > 0)
    {
        jo_fixed offset = -JO_FIXED_8;

        for (int i = 0; i < 2; i++)
        {
            Bullet *bullet = jo_malloc(sizeof(Bullet));
            bullet->Pos.x = PlayerEntity.Pos.x;
            bullet->Pos.y = PlayerEntity.Pos.y + offset;
            bullet->Type = 0;
            bullet->Velocity.x = -BULLET_SPEED;
            bullet->Velocity.y = 0;
            BulletListAdd(bullet, true);

            offset += JO_FIXED_16;
        }

        if (PlayerEntity.GunLevel > 1)
        {
            for (int i = -1; i < 2; i += 2)
            {
                Bullet *bullet = jo_malloc(sizeof(Bullet));
                bullet->Pos.x = PlayerEntity.Pos.x;
                bullet->Pos.y = PlayerEntity.Pos.y;
                bullet->Type = 0;
                bullet->Velocity.x = -(BULLET_SPEED / 2);
                bullet->Velocity.y = (BULLET_SPEED / 2) * i;
                BulletListAdd(bullet, true);
            }
        }
    }
}

/** @brief Exit game and go to main menu
 *  @param backButton Indicates whether back button (B) was pressed
 */
void GameExit(bool backButton)
{
    if (!backButton)
    {
        MenuCreateMain(backButton);
        CDDASetVolume(7, 7);
        CDDAPlaySingle(MENU_MUSIC, true);
    }
}

/** @brief Called when player runs out of lives
 *  @param backButton Indicates whether back button (B) was pressed
 */
void GameCreateGameOver()
{
    IsInGame = false;
    IsInGameOver = true;
    IsPaused = false;
    WidgetsClearAll();

    WidgetsCreateLabel(0, 15, "Game over");

    sprintf(GameOverScoreLabel, "%d", PlayerEntity.Score.Value);
    WidgetsCreateLabel(0, 7, GameOverScoreLabel);

    WidgetsSetCurrent(WidgetsCreateButton(0, -10, "Try again", GameCreateNew));
    WidgetsCreateButton(0, -18, "Exit", GameExit);

    CDDAPlaySingle(GAME_END_MUSIC, false);
}

/** @brief Player gotr hit by a bullet or NPC
 */
void PlayerHit()
{
    if (!PlayerEntity.HurtProtect)
    {
        PlayerEntity.Lives--;
        NpcSpawnExplosion(&PlayerEntity.Pos);

        if (PlayerEntity.Lives < 0)
        {
            GameCreateGameOver();
        }
        else
        {
            IsControllable = false;
            CurrentLevelTitleTick = 1;
            PlayerEntity.Pos.x = PLAYER_SPAWN_X + JO_FIXED_32;
        }
    }
}

/** @brief Make that bullets will kill player
 *  @param bullet Bullet to check
 *  @param isPlayer Was bullet shot by player
 */
void ReadBulletPositions(Bullet *bullet, bool isPlayer)
{
    if (!isPlayer && !PlayerEntity.HurtProtect)
    {
        jo_vector_fixed fromPlayer = {{bullet->Pos.x - PlayerEntity.Pos.x, bullet->Pos.y - PlayerEntity.Pos.y, 0}};
        jo_fixed distance = ToolsFastVectorLength(&fromPlayer);

        if (distance < JO_FIXED_4)
        {
            PlayerHit();
        }
    }
}

/** @brief Main gaim loop
 */
void GameLogic()
{
    // Update player and background
    BackgroundUdpate();

    if (!IsPaused && IsInGame)
    {
        // Create pause menu
        if (jo_is_pad1_key_down(JO_KEY_START))
        {
            IsPaused = true;
            WidgetsCreateLabel(0, 15, "Game paused");
            WidgetsSetCurrent(WidgetsCreateButton(0, -10, "Resume game", MenuResumeGame));
            WidgetsCreateButton(0, -18, "Exit", GameExit);
            CDDASetVolume(4, 4);
            return;
        }

        if (!IsControllable)
        {
            PlayerEntity.Pos.x -= JO_FIXED_1;
            IsControllable = PlayerEntity.Pos.x <= PLAYER_SPAWN_X;
        }
        else
        {
            // Protect player at start of game
            PlayerEntity.HurtProtect = CurrentLevelTitleTick < 350;

            // Debug random enemy generator
            debug++;
            if (debug2 < debug && CurrentLevelTitleTick >= 350)
            {
                int spawnCount = jo_random(3);

                for (int i = 0; i < spawnCount; i++)
                {
                    jo_pos3D_fixed pos = {NPC_SPAWN_X, jo_int2fixed((jo_random(10) - 5) << 4), 0};
                    NpcCreate(jo_random(4) - 1, &pos);

                    debug2 = 40 + jo_random(50);
                    debug = 0;
                }
            }

            PlayerActions action = PlayerUpdate(&PlayerEntity, 0);

            // Do player actions
            if (action == PlayerActionShoot)
            {
                // Shoot
                PlayerShoot();
            }

            if (action == PlayerActionBomb)
            {
                // start effect
                BombEffectLifeTime = 0;
            }

            // Update NPCs
            int collectedScore = NpcUpdate(&PlayerEntity, PlayerHit);

            if (collectedScore > 0)
            {
                ScoreAddValue(&PlayerEntity.Score, collectedScore);
            }

            // Update player bullets
            BulletListUpdate(ReadBulletPositions);

            // Update pickups
            PickupUpdate(&PlayerEntity);

            // Update bomb effect
            if (BombEffectLifeTime > -1)
            {
                BombEffectLifeTime++;

                // Clear all enemy bullets
                int range = (BombEffectLifeTime << 16) * 8;
                BulletListClearEnemyBulletsInRange(&PlayerEntity.Pos, range);

                // Destroy all NPCs in range and add score
                ScoreAddValue(&PlayerEntity.Score, NpcDestroyAllInRange(&PlayerEntity.Pos, range));

                if (BombEffectLifeTime > PLAYER_ACTION_BOMB)
                {
                    BombEffectLifeTime = -1;
                }
            }
        }

        NpcUpdateExplosions();
    }

    WidgetsInvokeInput();
}

/** @brief Start game fade in effect
 */
void StartGameFadeIn()
{
    static int step = TOOLS_FADE_OFFSET;

    if (step != 0)
    {
        if (step == TOOLS_FADE_OFFSET)
        {
            jo_core_tv_on();
        }

        slBack1ColSet((void *)BACK_CRAM, CD_Black);
        slColOffsetB(-TOOLS_FADE_OFFSET, -TOOLS_FADE_OFFSET, -TOOLS_FADE_OFFSET);
        slColOffsetOn(NBG0ON | NBG1ON | NBG2ON | NBG3ON | SPRON | RBG0ON);
        slColOffsetBUse(NBG0ON | NBG1ON | NBG2ON | NBG3ON | SPRON | RBG0ON);

        if (step > 0)
        {
            step -= TOOLS_FADE_SPEED;
            slColOffsetB(-step, -step, -step);
        }
        else if (step < 0)
        {
            step = 0;
            slColOffsetB(0, 0, 0);
        }
    }
}

/** @brief Redering loop
 */
void GameDraw()
{
    StartGameFadeIn();

    // Draw 2D in-game UI
    if (IsInGame && !IsPaused)
    {
        for (int live = 0; live <= PlayerEntity.Lives + 1; live++)
        {
            if (live == PlayerEntity.Lives + 1)
            {
                jo_sprite_draw3D(LifeIconSpriteIndex, 140 - (live * 14), -100, 100);
            }
            else
            {
                jo_sprite_draw3D(LifeSpriteIndex, 140 - (live * 14), -100, 100);
            }
        }

        for (int bomb = 1; bomb <= PlayerEntity.Bombs; bomb++)
        {
            jo_sprite_draw3D(BombSpriteIndex, 140 - (bomb * 6), -90, 100);
        }
    }

    jo_3d_push_matrix();
    {
        jo_3d_rotate_matrix(0, 180, -90);

        // Redraw active widgets
        WidgetsRedraw();

        if (IsInGame && !IsPaused)
        {
            // Draw HUD
            ScoreDraw(&PlayerEntity.Score);

            if (CurrentLevelTitleTick > 0 && CurrentLevelTitleTick < 350)
            {
                if (CurrentLevelTitleTick > 150 && CurrentLevelTitleTick < 210)
                {
                    CurrentLevelTitleTick++;
                }
                else
                {
                    CurrentLevelTitleTick += 2;
                }

                /* TODO: Implement levels before un-commenting this

                // Prepare text
                char text[12];
                sprintf(text, "LEVEL %d", CurrentLevel);

                jo_3d_translate_matrix_fixed(
                    0,
                    -jo_fixed_mult(jo_sin(CurrentLevelTitleTick >> 1), jo_int2fixed(80)) + jo_int2fixed(80),
                    jo_int2fixed(-60));

                FontPrintCentered(text, JO_NULL, JO_NULL);
                */
            }
        }
    }
    jo_3d_pop_matrix();

    // Draw scene
    jo_3d_camera_look_at(&camera);

    // Draw world background
    BackgroundDraw();

    // Entities in world
    if (IsInGame && !IsPaused)
    {
        jo_3d_push_matrix();
        {
            jo_3d_rotate_matrix(BG_PLACEMENT_X_ANG, BG_PLACEMENT_Y_ANG, BG_PLACEMENT_Z_ANG);
            jo_3d_translate_matrix(0, 0, BG_PLACEMENT_DEPTH);

            // Draw player
            PlayerDraw(&PlayerEntity, PlayerMesh);

            // Draw bomb effect
            if (BombEffectLifeTime > -1)
            {
                jo_3d_push_matrix();
                {
                    int scale = BombEffectLifeTime << 16;
                    jo_3d_translate_matrix_fixed(PlayerEntity.Pos.x, PlayerEntity.Pos.y, 0);
                    jo_3d_set_scale_fixed(JO_FIXED_1 + scale, JO_FIXED_1 + scale, JO_FIXED_1);
                    jo_3d_mesh_draw(ExplosionMesh);
                }
                jo_3d_pop_matrix();
            }

            // Update NPCs and entities
            NpcDraw();
            BulletListDraw();
            PickupDraw();
        }
        jo_3d_pop_matrix();
    }
}

/** @brief Loading screen
 */
void LoadingScreen()
{
    jo_printf(15, 20, "Loading...");

    jo_3d_push_matrix();
    {
        jo_3d_rotate_matrix(0, 180, -90);
        jo_3d_translate_matrix(0, 0, -60);

        for (int logoMesh = 0; logoMesh < LogoSize; logoMesh++)
        {
            jo_3d_mesh_draw(&(Logo[logoMesh]));
        }
    }
    jo_3d_pop_matrix();

    // Draw PoneSound logo
    jo_printf(5, 27, "Audio powered by");
    jo_printf(5, 28, "PoneSound");
    jo_sprite_draw3D2(0, 0, 200, 90);
}

/** @brief Initialize game data
 */
void GameInitialize()
{
    jo_core_tv_off();

    // Load PoneSound logo
    jo_sprite_add_tga(JO_ROOT_DIR, "PONE.TGA", JO_COLOR_Transparent);

    // Load logo model
    Logo = ML_LoadMesh("LOGO.TMF", JO_ROOT_DIR, &LogoSize);
    LoadingScreen();

    // Fade in loading screen
    ToolsFadeIn(SPRON | NBG0ON, LoadingScreen);

    // Load background assets
    BackgroundInitialize();

    // Load bullets
    BulletLoadAssets();

    // Load font
    FontInitialize();

    // Load resources for controls
    WidgetsControlsInitialize();

    // Load NPCs and entities
    NpcInitialize();
    PickupInitialize();
    BulletListInitialize();

    // Load UI sprites
    LifeSpriteIndex = jo_sprite_add_tga(JO_ROOT_DIR, "LIFECNT.TGA", JO_COLOR_RGB(255, 0, 255));
    LifeIconSpriteIndex = jo_sprite_add_tga(JO_ROOT_DIR, "LIFEICO.TGA", JO_COLOR_RGB(255, 0, 255));
    BombSpriteIndex = jo_sprite_add_tga(JO_ROOT_DIR, "BMBICO.TGA", JO_COLOR_RGB(255, 0, 255));

    // Game sounds
    PlayerShootSound = load_16bit_pcm((Sint8 *)"PEW.PCM", 15360);

    // Load player and explosion model
    int temp = 0;
    PlayerMesh = ML_LoadMesh("PLAYER.TMF", JO_ROOT_DIR, &temp);
    ExplosionMesh = ML_LoadMesh("EXP.TMF", JO_ROOT_DIR, &temp);

    // Show all other layers and turn off TV
    ToolsFadeOut(SPRON | NBG0ON, LoadingScreen);

    // Load ever present RGB0 background
    BackgroundInitializeRGB0();
    jo_clear_screen();
}

/** @brief game logic gets called here
 */
void Logic()
{
    static bool startup = true;
    slUnitMatrix(0);
    GameLogic();
    GameDraw();

    if (jo_is_pad1_key_pressed(JO_KEY_A) &&
        jo_is_pad1_key_pressed(JO_KEY_B) &&
        jo_is_pad1_key_pressed(JO_KEY_C) &&
        jo_is_pad1_key_pressed(JO_KEY_START))
    {
        jo_goto_boot_menu();
    }

    if (startup)
    {
        CDDAPlaySingle(MENU_MUSIC, true);
        startup = false;
    }
}

/** @brief Called every vertical blank
 */
void VBlank()
{
    jo_get_inputs_vblank();
    sdrv_stm_vblank_rq();
}

/** @brief Application entry point
 */
void jo_main(void)
{
    // Prepare scene
    jo_core_init(JO_COLOR_Black);

    // Load sound driver
    load_drv(ADX_MASTER_2304);
    snd_adx = add_adx_front_buffer(23040);
    add_adx_back_buffer((void *)LWRAM);
    CDDAInitialize();

    *(volatile unsigned char *)0x060FFCD8 = 0x1F;
    jo_3d_camera_init(&camera);
    jo_random_seed = jo_time_get_frc();

    /* Initialize game stuff */
    WidgetsInitialize();
    GameInitialize();

    /* Make main menu */
    MenuCreateMain(false);

    // Start game
    slIntFunction(VBlank);
    pcm_stream_init(30720, PCM_TYPE_8BIT);
    pcm_stream_host(Logic);
}
