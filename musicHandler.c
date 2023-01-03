#include <jo/jo.h>
#include "global_defines.h"
#include "pcmcdda.h"
#include "musicHandler.h"

/* Current music track */
static int CurrentMusic = 0;

/* Game music volume */
static int CurrentMusicVolume = 90;

void MusicSetVolumeDirect(const int volume)
{
    int transformed = JO_DIV_BY_65536(jo_fixed_mult(JO_MULT_BY_65536(volume), 842605));
    CurrentMusicVolume = JO_MIN(90, transformed);
    CurrentMusicVolume = JO_MAX(0, CurrentMusicVolume);
    CDDASetVolume(volume, volume);
}

void MusicSetCurrentDirect(const int musicToSet, bool repeat)
{
    CurrentMusic = musicToSet;
    CDDAPlaySingle(musicToSet, repeat);
}

void MusicSetCurrent(const int musicToSet, bool repeat)
{
    int limitedMusicIndex = musicToSet;

    if (CurrentMusic != limitedMusicIndex && CurrentMusicVolume > 0)
    {
        CurrentMusicVolume--;
    }
    else if (CurrentMusic == limitedMusicIndex && CurrentMusicVolume <= 90)
    {
        CurrentMusicVolume++;
    }

    if (CurrentMusicVolume <= 90)
    {
        int transformed = JO_DIV_BY_65536(jo_fixed_mult(jo_sin(CurrentMusicVolume), JO_MULT_BY_65536(7)));
        transformed = JO_ABS(transformed) + 1;
        CDDASetVolume(transformed, transformed);
    }

    if (CurrentMusicVolume == 0)
    {
        CurrentMusic = limitedMusicIndex;
        CDDAPlaySingle(CurrentMusic, repeat);
    }
}
