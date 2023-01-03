#include <jo/jo.h>
#include "background.h"
#include "model.h"
#include "player.h"
#include "npc.h"
#include "gamemode.h"
#include "gamemodeEndless.h"
#include "gamemodeStory.h"
#include "gamemodeManager.h"

void GmStart(const GamemodeData * data)
{
    switch (data->Mode)
    {
        case GmStory:
            GmStartStory(data);
            break;

        default:
            GmStartEndless(data);
            break;
    }
}

GamemodeTickResult GmTick(const GamemodeData * data)
{
    switch (data->Mode)
    {
        case GmStory:
            return GmTickStory(data);

        default:
            return GmTickEndless(data);
    }
}

void GmEnd(const GamemodeData * data)
{
    switch (data->Mode)
    {
        case GmStory:
            GmEndStory(data);
            break;

        default:
            GmEndEndless(data);
            break;
    }
}
