#include "main.h"

void MenuCreateGame(bool backButton);
void MenuCreateMain(bool backButton);
void GameCreateGameOver();

/* Music stuff */
#define LWRAM (2097152)
void *loading_system_scratch_buffer = (void *)LWRAM;
int snd_adx = 0;
#define MENU_MUSIC 2
#define GAME_MUSIC 3
#define GAME_END_MUSIC 4

/* Main 3D camera */
static jo_camera camera;

/* Indicates whether game assets have loaded */
static bool GameLoaded = false;

/* Logo mesh collection */
static SaturnMesh Logo;

/* Logo mesh size */
static int LogoSize;

/* Game over score label text */
static char GameOverScoreLabel[SCORE_TEXT_LEN + 1];

/* Game end score */
static int CurrentGameEndScore = 0;

/* Do game draw */
static bool DoGameDraw = false;

/* Is coop mode */
static bool IsCoop = false;

/* Is endless mode */
static bool IsEndless = false;

/** @brief Resume game button action
 *  @param backButton Indicates whether back button (B) was pressed
 */
void MenuResumePauseGame(bool backButton)
{
    if (!backButton)
    {
        for (jo_node *node = WidgetsGetAll()->first; node != JO_NULL; node = node->next)
        {
            ((WidgetsWidget*)(node->data.ptr))->IsVisible = !((WidgetsWidget*)(node->data.ptr))->IsVisible;
        }

        if (!WidgetsById(0)->IsVisible)
        {
            // Game was not paused
            WidgetsSetCurrent(WidgetsById(2));
            CDDASetVolume(4, 4);
        }
        else
        {
            // Game was paused
            WidgetsSetCurrent(WidgetsById(0));
            CDDASetVolume(7, 7);
        }
    }
}

/** @brief Check if any controller got start button pressed
 * @return Was any start button pressed
 */
bool AnyStartButtonPressed()
{
    for(int input = 0; input < JO_INPUT_MAX_DEVICE; input++)
    {
        if (jo_is_input_available(input) && jo_is_input_key_down(input, JO_KEY_START))
        {
            return true;
        }
    }

    return false;
}

/** @brief Game logic handler
 * @param widget Game screen widget
 * @param message Game logic message
 */
void GameHandler(WidgetsWidget *widget, WidgetMessages message)
{
    switch (message)
    {
        case WIDGET_INIT:
            GameInitializeMortal(IsCoop, IsEndless);
            break;

        case WIDGET_FREE:
            GameDisposeMortal();
            break;

        case WIDGET_INPUT:
        
            // Create pause menu
            if (AnyStartButtonPressed())
            {
                MenuResumePauseGame(false);
                return;
            }

            CurrentState state;
            GameUpdateTick(&state);
            CurrentGameEndScore = state.Score;

            if (state.GameEnd)
            {
                GameCreateGameOver();
            }

            break;

        case WIDGET_DRAW:
            GameScoreDraw();
            DoGameDraw = true;
            break;

        default:
            break;
    }
}

/** @brief Exit game and go to main menu
 *  @param backButton Indicates whether back button (B) was pressed
 */
void GameExit(bool backButton)
{
    if (!backButton)
    {
        WidgetsClearAll();
        MenuCreateMain(backButton);
        CDDASetVolume(7, 7);
        CDDAPlaySingle(MENU_MUSIC, true);
    }
}

/** @brief Called when new game starts
 *  @param backButton Indicates whether back button (B) was pressed
 */
void GameCreateNew(bool backButton)
{
    if (!backButton)
    {
        WidgetsClearAll();
        WidgetsSetCurrent(WidgetsCreate(0, 0, GameHandler));

        WidgetsCreateLabel(0, 15, "Game paused")->IsVisible = false;
        WidgetsCreateButton(0, -10, "Resume game", MenuResumePauseGame)->IsVisible = false;
        WidgetsCreateButton(0, -18, "Exit", GameExit)->IsVisible = false;

        CDDAPlaySingle(GAME_MUSIC, true);
    }
    else
    {
        MenuCreateGame(false);
    }
}

/** @brief Create credits menu
 *  @param backButton Indicates whether back button (B) was pressed
 */
void MenuCredits(bool backButton)
{
    if (!backButton)
    {
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
            TmfDraw(&Logo);
        }
        jo_3d_pop_matrix();

        if (rotation > 359)
        {
            rotation = 0;
        }
    }
}

/** @brief Create coop game
 *  @param backButton Indicates whether back button (B) was pressed
 */
void MenuCreateCoop(bool backButton)
{
    IsCoop = true;
    GameCreateNew(backButton);
}

/** @brief Create game menu
 *  @param backButton Indicates whether back button (B) was pressed
 */
void MenuCreatePlayerMode(bool backButton)
{
    if (!backButton)
    {
        IsCoop = false;

        WidgetsClearAll();
        WidgetsCreate(0, 17, DrawLogo)->IsSelectable = false;
        WidgetsCreateLabel(0, -5, IsEndless ? "Endless play" : "Story mode");
        WidgetsSetCurrent(WidgetsCreateButton(0, -22, "Single", GameCreateNew));
        WidgetsCreateButton(0, -30, "Cooperative", MenuCreateCoop);
    }
    else
    {
        MenuCreateMain(false);
    }
}

/** @brief Create coop game
 *  @param backButton Indicates whether back button (B) was pressed
 */
void MenuCreateEndless(bool backButton)
{
    IsEndless = true;
    MenuCreatePlayerMode(backButton);
}

/** @brief Create game menu
 *  @param backButton Indicates whether back button (B) was pressed
 */
void MenuCreateGame(bool backButton)
{
    if (!backButton)
    {
        IsEndless = false;

        WidgetsClearAll();
        WidgetsCreate(0, 17, DrawLogo)->IsSelectable = false;
        WidgetsCreateLabel(0, -5, "New game");
        
        WidgetsCreateButton(0, -22, "Story", MenuCreatePlayerMode)->IsSelectable = false;
        WidgetsSetCurrent(WidgetsCreateButton(0, -30, "Endless", MenuCreateEndless));
    }
}

/** @brief Create main menu
 *  @param backButton Indicates whether back button (B) was pressed
 */
void MenuCreateMain(bool backButton)
{
    if (!backButton)
    {
        WidgetsClearAll();
        WidgetsCreate(0, 17, DrawLogo)->IsSelectable = false;
        WidgetsCreateLabel(0, -5, "Sky Blaster");
        WidgetsSetCurrent(WidgetsCreateButton(0, -22, "New game", MenuCreateGame));
        WidgetsCreateButton(0, -30, "Credits", MenuCredits);
    }
}

/** @brief Called when player runs out of lives
 *  @param backButton Indicates whether back button (B) was pressed
 */
void GameCreateGameOver()
{
    WidgetsClearAll();

    WidgetsCreateLabel(0, 15, "Game over");

    sprintf(GameOverScoreLabel, "%d", CurrentGameEndScore);
    WidgetsCreateLabel(0, 7, GameOverScoreLabel);

    WidgetsSetCurrent(WidgetsCreateButton(0, -10, "Try again", GameCreateNew));
    WidgetsCreateButton(0, -18, "Exit", GameExit);

    CDDAPlaySingle(GAME_END_MUSIC, false);
}

/** @brief Main gaim loop
 */
void MainLogic()
{
    // Update player and background
    BackgroundUdpate();
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
void MainDraw()
{
    StartGameFadeIn();

    jo_3d_push_matrix();
    {
        jo_3d_rotate_matrix(0, 180, -90);

        // Redraw active widgets
        WidgetsRedraw();
    }
    jo_3d_pop_matrix();

    // Draw scene
    jo_3d_camera_look_at(&camera);

    // Draw world background
    BackgroundDraw();

    if (DoGameDraw)
    {
        GameDraw();
        DoGameDraw = false;
    }
}

/** @brief Loading screen
 */
void LoadingScreen()
{
    static int rotation = 0;
    static jo_fixed scale = JO_FIXED_1;

    jo_printf(15, 20, "Loading...");

    jo_3d_push_matrix();
    {
        jo_3d_rotate_matrix(0, 180, -90);
        jo_3d_translate_matrix(0, 0, -60);
        jo_3d_set_scale_fixed(scale, scale, scale);
        jo_3d_rotate_matrix_x(rotation);
        TmfDraw(&Logo);
    }
    jo_3d_pop_matrix();

    if (GameLoaded)
    {
        rotation += 17;
        scale -= 1700;
    }

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
    TmfLoadMesh(&Logo, "LOGO.TMF", JO_ROOT_DIR);
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

    // Load and initialize game
    GameInitializeImmortal();

    // All base assets are loaded now
    GameLoaded = true;

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
    MainLogic();
    MainDraw();

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
