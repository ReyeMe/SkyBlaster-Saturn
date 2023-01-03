#include <jo/jo.h>
#include "global_defines.h"
#include "musicHandler.h"
#include "background.h"
#include "tools.h"
#include "model.h"
#include "bullet.h"
#include "bulletList.h"
#include "player.h"
#include "npc.h"
#include "gamemode.h"
#include "gamemodeStory.h"

// -------------------------------------
// Internal
// -------------------------------------

#define TEXT_SHOW_DELAY (400)
#define TEXT_FAST_DELAY_SKIP (200)

/* Story state */
static int State;

/* Stage tick count */
static int StageTickCount;

/* For how long was the text on the screen already */
static int TextAutoSkipDelay;

/* Current music to play */
static int CurrentMusic;

/** @brief Draw intro dialog
 *  @return True if dialog is being drawn
 */
static bool DrawIntroDialog()
{
    static int dialog;

    if (dialog < 4)
    {
        jo_pos2D center = { 5, 65 };
        jo_size size = { JO_TV_WIDTH - 60, 60 };

        ToolsDrawRectangle(&center, &size, 70, JO_COLOR_Black, true);
        ToolsDrawRectangle(&center, &size, 50, JO_COLOR_DarkYellow, false);

        jo_clear_screen();

        switch (dialog)
        {
            case 0:
                jo_printf_with_color(5, 20, JO_COLOR_INDEX_Green, "[Control tower]");
                jo_printf_with_color(5, 21, JO_COLOR_INDEX_White, "To all defenders! Enemy is");
                jo_printf_with_color(5, 22, JO_COLOR_INDEX_White, "closing in on the city.");
                TextAutoSkipDelay = TextAutoSkipDelay < TEXT_FAST_DELAY_SKIP ? TEXT_FAST_DELAY_SKIP : TextAutoSkipDelay;
                break;

            case 1:
                jo_printf_with_color(5, 20, JO_COLOR_INDEX_Purple, "[Pilot]");
                jo_printf_with_color(5, 21, JO_COLOR_INDEX_White, "Damn, they are fast.");
                TextAutoSkipDelay = TextAutoSkipDelay < TEXT_FAST_DELAY_SKIP ? TEXT_FAST_DELAY_SKIP : TextAutoSkipDelay;
                break;

            case 2:
                jo_printf_with_color(5, 20, JO_COLOR_INDEX_Purple, "[Pilot]");
                jo_printf_with_color(5, 21, JO_COLOR_INDEX_White, "How come they reached the city");
                jo_printf_with_color(5, 22, JO_COLOR_INDEX_White, "so quickly...");
                TextAutoSkipDelay = TextAutoSkipDelay < TEXT_FAST_DELAY_SKIP ? TEXT_FAST_DELAY_SKIP : TextAutoSkipDelay;
                break;

            case 3:
                jo_printf_with_color(5, 20, JO_COLOR_INDEX_Purple, "[Pilot]");
                jo_printf_with_color(5, 21, JO_COLOR_INDEX_White, "No time for questions...");
                jo_printf_with_color(5, 22, JO_COLOR_INDEX_White, "Here they come!");
                TextAutoSkipDelay = TextAutoSkipDelay < TEXT_FAST_DELAY_SKIP ? TEXT_FAST_DELAY_SKIP : TextAutoSkipDelay;
                break;

            default:
                break;
        }

        TextAutoSkipDelay++;

        if (TextAutoSkipDelay > TEXT_SHOW_DELAY || (jo_is_pad1_available() && jo_is_pad1_key_down(JO_KEY_A)))
        {
            jo_clear_screen();
            TextAutoSkipDelay = 0;
            dialog++;
        }

        return true;
    }
    else
    {
        dialog = 0;
    }

    return false;
}

/** @brief Draw level 2 dialog
 *  @return True if dialog is being drawn
 */
static bool DrawL2Dialog()
{
    static int dialog;

    if (dialog < 2)
    {
        jo_pos2D center = { 5, 65 };
        jo_size size = { JO_TV_WIDTH - 60, 60 };

        ToolsDrawRectangle(&center, &size, 70, JO_COLOR_Black, true);
        ToolsDrawRectangle(&center, &size, 50, JO_COLOR_DarkYellow, false);

        jo_clear_screen();

        switch (dialog)
        {
            case 0:
                jo_printf_with_color(5, 20, JO_COLOR_INDEX_Green, "[Control tower]");
                jo_printf_with_color(5, 21, JO_COLOR_INDEX_White, "We have detected another enemy");
                jo_printf_with_color(5, 22, JO_COLOR_INDEX_White, "wave over the forest in");
                jo_printf_with_color(5, 23, JO_COLOR_INDEX_White, "your dire.. *static*");
                break;

            case 1:
                jo_printf_with_color(5, 20, JO_COLOR_INDEX_Purple, "[Pilot]");
                jo_printf_with_color(5, 21, JO_COLOR_INDEX_White, "Something is jamming the radio.");
                TextAutoSkipDelay = TextAutoSkipDelay < TEXT_FAST_DELAY_SKIP ? TEXT_FAST_DELAY_SKIP : TextAutoSkipDelay;
                break;

            default:
                break;
        }

        TextAutoSkipDelay++;

        if (TextAutoSkipDelay > TEXT_SHOW_DELAY || (jo_is_pad1_available() && jo_is_pad1_key_down(JO_KEY_A)))
        {
            jo_clear_screen();
            TextAutoSkipDelay = 0;
            dialog++;
        }

        return true;
    }
    else
    {
        dialog = 0;
    }

    return false;
}

/** @brief Draw level 3 dialog
 *  @return True if dialog is being drawn
 */
static bool DrawL3Dialog()
{
    static int dialog;

    if (dialog < 3)
    {
        jo_pos2D center = { 5, 65 };
        jo_size size = { JO_TV_WIDTH - 60, 60 };

        ToolsDrawRectangle(&center, &size, 70, JO_COLOR_Black, true);
        ToolsDrawRectangle(&center, &size, 50, JO_COLOR_DarkYellow, false);

        jo_clear_screen();

        switch (dialog)
        {
            case 0:
                jo_printf_with_color(5, 20, JO_COLOR_INDEX_Purple, "[Pilot]");
                jo_printf_with_color(5, 21, JO_COLOR_INDEX_White, "I still can't establish contact");
                jo_printf_with_color(5, 22, JO_COLOR_INDEX_White, "with tower.");
                TextAutoSkipDelay = TextAutoSkipDelay < TEXT_FAST_DELAY_SKIP ? TEXT_FAST_DELAY_SKIP : TextAutoSkipDelay;
                break;

            case 1:
                jo_printf_with_color(5, 20, JO_COLOR_INDEX_Purple, "[Pilot]");
                jo_printf_with_color(5, 21, JO_COLOR_INDEX_White, "The source of the interference");
                jo_printf_with_color(5, 22, JO_COLOR_INDEX_White, "looks like it is coming from");
                jo_printf_with_color(5, 23, JO_COLOR_INDEX_White, "the desert area.");
                break;

            case 2:
                jo_printf_with_color(5, 20, JO_COLOR_INDEX_Yellow, "----");
                jo_printf_with_color(5, 21, JO_COLOR_INDEX_White, "*static*");
                TextAutoSkipDelay = TextAutoSkipDelay < TEXT_FAST_DELAY_SKIP ? TEXT_FAST_DELAY_SKIP : TextAutoSkipDelay;
                break;

            default:
                break;
        }

        TextAutoSkipDelay++;

        if (TextAutoSkipDelay > TEXT_SHOW_DELAY || (jo_is_pad1_available() && jo_is_pad1_key_down(JO_KEY_A)))
        {
            jo_clear_screen();
            TextAutoSkipDelay = 0;
            dialog++;
        }

        return true;
    }
    else
    {
        dialog = 0;
    }

    return false;
}

/** @brief Draw level 4 dialog
 *  @return True if dialog is being drawn
 */
static bool DrawL4Dialog()
{
    static int dialog;

    if (dialog < 2)
    {
        jo_pos2D center = { 5, 65 };
        jo_size size = { JO_TV_WIDTH - 60, 60 };

        ToolsDrawRectangle(&center, &size, 70, JO_COLOR_Black, true);
        ToolsDrawRectangle(&center, &size, 50, JO_COLOR_DarkYellow, false);

        jo_clear_screen();

        switch (dialog)
        {
            case 0:
                jo_printf_with_color(5, 20, JO_COLOR_INDEX_Purple, "[Pilot]");
                jo_printf_with_color(5, 21, JO_COLOR_INDEX_White, "I am getting something on my");
                jo_printf_with_color(5, 22, JO_COLOR_INDEX_White, "radar.");
                TextAutoSkipDelay = TextAutoSkipDelay < TEXT_FAST_DELAY_SKIP ? TEXT_FAST_DELAY_SKIP : TextAutoSkipDelay;
                break;

            case 1:
                jo_printf_with_color(5, 20, JO_COLOR_INDEX_Purple, "[Pilot]");
                jo_printf_with_color(5, 21, JO_COLOR_INDEX_White, "This looks bad.");
                jo_printf_with_color(5, 22, JO_COLOR_INDEX_White, "I need to do something fast!");
                TextAutoSkipDelay = TextAutoSkipDelay < TEXT_FAST_DELAY_SKIP ? TEXT_FAST_DELAY_SKIP : TextAutoSkipDelay;
                break;

            default:
                break;
        }

        TextAutoSkipDelay++;

        if (TextAutoSkipDelay > TEXT_SHOW_DELAY || (jo_is_pad1_available() && jo_is_pad1_key_down(JO_KEY_A)))
        {
            jo_clear_screen();
            TextAutoSkipDelay = 0;
            dialog++;
        }

        return true;
    }
    else
    {
        dialog = 0;
    }

    return false;
}

/** @brief Draw end demo
 *  @return True if dialog is being drawn
 */
static bool DrawDemoDialog()
{
    static int dialog;

    if (dialog < 2)
    {
        jo_pos2D center = { 5, 65 };
        jo_size size = { JO_TV_WIDTH - 60, 60 };

        ToolsDrawRectangle(&center, &size, 70, JO_COLOR_Black, true);
        ToolsDrawRectangle(&center, &size, 50, JO_COLOR_DarkYellow, false);

        jo_clear_screen();

        switch (dialog)
        {
            case 0:
                jo_printf_with_color(5, 20, JO_COLOR_INDEX_Yellow, "[SuperReye]");
                jo_printf_with_color(5, 21, JO_COLOR_INDEX_White, "Thank you for trying");
                jo_printf_with_color(5, 22, JO_COLOR_INDEX_White, "this short demo :)");
                TextAutoSkipDelay = TextAutoSkipDelay < TEXT_FAST_DELAY_SKIP ? TEXT_FAST_DELAY_SKIP : TextAutoSkipDelay;
                break;

            case 1:
                jo_printf_with_color(5, 20, JO_COLOR_INDEX_Yellow, "[SuperReye]");
                jo_printf_with_color(5, 21, JO_COLOR_INDEX_White, "On behalf of Random and AnriFox");
                jo_printf_with_color(5, 23, JO_COLOR_INDEX_Yellow, "Thank you again,");
                jo_printf_with_color(5, 24, JO_COLOR_INDEX_Yellow, "   and good luck to new year");
                break;

            default:
                break;
        }

        TextAutoSkipDelay++;

        if (TextAutoSkipDelay > TEXT_SHOW_DELAY || (jo_is_pad1_available() && jo_is_pad1_key_down(JO_KEY_A)))
        {
            jo_clear_screen();
            TextAutoSkipDelay = 0;
            dialog++;
        }

        return true;
    }
    else
    {
        dialog = 0;
    }

    return false;
}

static void Stage1(const GamemodeData * data)
{
    static int SpawnTimer = 0;
    static int NextSpawn = 40;

    if (StageTickCount > 6800)
    {
        if (NpcCount() <= 0)
        {
            State++;
        }
    }
    else
    {
        if (NextSpawn < SpawnTimer)
        {
            int spawnCount = 1;

            for (int i = 0; i < spawnCount; i++)
            {
                jo_pos3D_fixed pos = {NPC_SPAWN_X, jo_int2fixed((jo_random(10) - 5) << 4), 0};

                int toSpawn = StageTickCount < 900 ? (int)NpcDart : (StageTickCount < 1950 ? (jo_random(2) - 1) : (jo_random(3) - 1));

                NpcCreate(JO_MAX(toSpawn, 0), &pos);
            }

            int limitedSpawn = StageTickCount < 200 ? 60 : 50;
            int expectedSpawn = 30 + jo_random(20);
            NextSpawn = 30 + JO_MAX(expectedSpawn, limitedSpawn);
            SpawnTimer = 0;
        }
    }

    SpawnTimer++;
    StageTickCount++;
}

static void Stage2(const GamemodeData * data)
{
    static int SpawnTimer = 0;
    static int NextSpawn = 40;

    if (StageTickCount > 6800)
    {
        if (NpcCount() <= 0)
        {
            State++;
        }
    }
    else
    {
        if (NextSpawn < SpawnTimer)
        {
            int spawnCount = jo_random(2) - 1;
            spawnCount = JO_MAX(spawnCount, 1);

            for (int i = 0; i < spawnCount; i++)
            {
                jo_pos3D_fixed pos = {NPC_SPAWN_X, jo_int2fixed((jo_random(10) - 5) << 4), 0};

                int toSpawn = StageTickCount < 1550 ? (jo_random(2) - 1) : (jo_random(4) - 1);

                NpcCreate(JO_MAX(toSpawn, 0), &pos);
            }

            int limitedSpawn = StageTickCount < 200 ? 50 : (StageTickCount < 1200 ? 40 : 30);
            int expectedSpawn = 20 + jo_random(20);
            NextSpawn = 30 + JO_MAX(expectedSpawn, limitedSpawn);
            SpawnTimer = 0;
        }
    }

    SpawnTimer++;
    StageTickCount++;
}

static void Stage4(const GamemodeData * data)
{
    static int SpawnTimer = 0;
    static int NextSpawn = 40;

    if (StageTickCount > 8500)
    {
        if (NpcCount() <= 0)
        {
            State++;
        }
    }
    else
    {
        if (NextSpawn < SpawnTimer)
        {
            int spawnCount = jo_random(3) - 1;
            spawnCount = JO_MAX(spawnCount, 1);

            if (StageTickCount < 1500)
            {
                spawnCount = JO_MIN(spawnCount, 2);
            }

            for (int i = 0; i < spawnCount; i++)
            {
                jo_pos3D_fixed pos = {NPC_SPAWN_X, jo_int2fixed((jo_random(10) - 5) << 4), 0};

                int toSpawn = jo_random(5) - 1;

                NpcCreate(JO_MAX(toSpawn, 0), &pos);
            }

            int expectedSpawn = 20 + jo_random(25);
            NextSpawn = 20 + expectedSpawn;
            SpawnTimer = 0;
        }
    }

    SpawnTimer++;
    StageTickCount++;
}

// -------------------------------------
// Public
// -------------------------------------

void GmStartStory(const GamemodeData * data)
{
    TextAutoSkipDelay = 0;
    StageTickCount = 0;
    State = 0;
    CurrentMusic = LVL1_MUSIC;
    BackgroundSetColorShift(BackgroundBlueSky);
}

GamemodeTickResult GmTickStory(const GamemodeData * data)
{
    MusicSetCurrent(CurrentMusic, true);

    switch (State)
    {
        case 0:
            BulletListClear(true);

            if (!DrawIntroDialog())
            {
                State++;
                StageTickCount = 0;
            }
            return GmTickResultBlockControls;

        case 1:
            Stage1(data);
            break;

        case 2:
            BulletListClear(true);

            if (!DrawL2Dialog())
            {
                BackgroundSetColorShift(BackgroundGreenSky);
                CurrentMusic = LVL2_MUSIC;
                StageTickCount = 0;
                State++;
            }
            return GmTickResultBlockControls;

        case 3:
            Stage2(data);
            break;

        case 4:
            BulletListClear(true);

            if (!DrawL3Dialog())
            {
                BackgroundSetColorShift(BackgroundOrangeSky);
                
                CurrentMusic = LVL3_MUSIC;
                StageTickCount = 0;
                State++;
            }
            return GmTickResultBlockControls;

        case 5:
            Stage4(data);
            break;

        case 6:
            BulletListClear(true);

            if (!DrawL4Dialog())
            {
                BackgroundSetColorShift(BackgroundRedSky);
                CurrentMusic = LVL4_MUSIC;
                StageTickCount = 0;
                State++;
            }
            return GmTickResultBlockControls;

        case 7:
            Stage4(data);
            break;

        case 8:
            BulletListClear(true);

            if (!DrawDemoDialog())
            {
                return GmTickResultDemoOver;
            }
            return GmTickResultBlockControls;

        default:
            State = 0;
            break;
    }
    
    return GmTickResultNone;
}

void GmEndStory(const GamemodeData * data)
{

}
