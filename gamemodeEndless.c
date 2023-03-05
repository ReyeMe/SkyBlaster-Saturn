#include <jo/jo.h>
#include "global_defines.h"
#include "background.h"
#include "model.h"
#include "player.h"
#include "npc.h"
#include "musicHandler.h"
#include "gamemode.h"
#include "gamemodeEndless.h"

// -------------------------------------
// Internal
// -------------------------------------

/* Score spent on changing level */
static int Currentlevel;

/* Score spent on changing level */
static int LevelChangeSpentScore;

/* Current spawn timer */
static int SpawnTimer;

/* Next spawn delay */
static int NextSpawn;

// -------------------------------------
// Public
// -------------------------------------

void GmStartEndless(const GamemodeData * data)
{
    Currentlevel = 0;
    LevelChangeSpentScore = 0;
    SpawnTimer = 0;
    NextSpawn = 40;
}

GamemodeTickResult GmTickEndless(const GamemodeData * data)
{
    int levelPrice = (6000 * data->PlayerCount) + (1000 * data->PlayerCount);

    if (data->CurrentScore - (LevelChangeSpentScore + levelPrice) >=0)
    {
        LevelChangeSpentScore = LevelChangeSpentScore + levelPrice;
        Currentlevel++;
    }

    int stage = Currentlevel;
    int stageColor = JO_MIN(stage, (int)BackgroundBlackSky);
    int music = JO_MIN(stageColor, LVL_MUSIC_CNT - 1);
    music = JO_MAX(music, 0);
    BackgroundSetColorShift(stageColor);
    MusicSetCurrent(LVL1_MUSIC + stageColor, true);

    SpawnTimer++;

    if (NextSpawn < SpawnTimer && data->TickCount >= 350)
    {
        int spawnCount = jo_random(2 + MIN(stageColor, 3));

        for (int i = 0; i < spawnCount; i++)
        {
            jo_pos2D_fixed pos = {NPC_SPAWN_X, jo_int2fixed((jo_random(10) - 5) << 4)};
            int toSpawn = jo_random(5) - 1;
            NpcCreate(JO_MAX(toSpawn, 0), &pos);

            int expectedSpawn = jo_random(50 - stageColor);
            int limitedSpawn = 40 - (stageColor * 4);
            NextSpawn = 40 + JO_MAX(expectedSpawn, limitedSpawn);
            SpawnTimer = 0;
        }
    }

    return GmTickResultNone;
}

void GmEndEndless(const GamemodeData * data)
{
    return;
}
