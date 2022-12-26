#include <jo/jo.h>
#include <SEGA_SND.H>
#include "pcmsys.h"
#include "introCinematic.h"

static bool started = false;
static bool stopping = false;
static jo_event_callback onMovieEnd;
static int moviePlaybackCallback;

#define SDDRVS_TSK_SIZE			(0x6000)
#define BOOTSND_MAP_SIZE		(0x0100)

static Sint32 fileLoad(const char *name, void *addr, Sint32 bsize)
{
	Sint32 		fid, i;

	for (i = 0; i < 10; i++) {
		fid = GFS_NameToId((Sint8*)name);
		if (fid >= 0) {
			GFS_Load(fid, 0, addr, bsize);
			return 0;
		}
	}
	return -1;
}

static void sndInit(void)
{
	smpc_issue_command(SMPC_CMD_SNDOFF);
	smpc_wait_till_ready();
    
	SndIniDt 	snd_init;
    Sint32 * buff1 = jo_malloc_with_behaviour(SDDRVS_TSK_SIZE, JO_MALLOC_TRY_REUSE_BLOCK);
    Sint32 * buff2 = jo_malloc_with_behaviour(BOOTSND_MAP_SIZE, JO_MALLOC_TRY_REUSE_BLOCK);

	if (fileLoad("SDDRVS.TSK", (void *)buff1, SDDRVS_TSK_SIZE)) {
		return;
	}
	if (fileLoad("BOOTSND.MAP", (void *)buff2, BOOTSND_MAP_SIZE)) {
		return;
	}
	SND_INI_PRG_ADR(snd_init) 	= (Uint16 *)buff1;
	SND_INI_PRG_SZ(snd_init) 	= (Uint16 )SDDRVS_TSK_SIZE;
	SND_INI_ARA_ADR(snd_init) 	= (Uint16 *)buff2;
	SND_INI_ARA_SZ(snd_init) 	= (Uint16)BOOTSND_MAP_SIZE;
	SND_Init(&snd_init);
	SND_ChgMap(0);

    jo_free(buff1);
    jo_free(buff2);

	// Turn on Sound CPU again
	smpc_wait_till_ready();
	smpc_issue_command(SMPC_CMD_SNDON);
}

/** @brief Game loop
 */
static void IntroDraw()
{
    if (started)
    {
        jo_sprite_draw3D(jo_get_video_sprite(), 0, 0, 170);

		for(int input = 0; !stopping && input < JO_INPUT_MAX_DEVICE; input++)
    	{
    	    if (jo_is_input_available(input) && jo_is_input_key_down(input, JO_KEY_START))
    	    {
				stopping = true;
    	        jo_video_stop();
    	    }
    	}  
    }
}

static void IntroStopped(void)
{
    started = false;

    // Remove video playback callback
    jo_core_remove_callback(moviePlaybackCallback);
	
    // escape to our game code
    onMovieEnd();
}

void IntroCpkPlay(const jo_event_callback OnMovieEnd)
{
    onMovieEnd = OnMovieEnd;
    sndInit();

	if (jo_video_open_file("INTRO.CPK"))
    {
        started = jo_video_play(IntroStopped);
    }

    moviePlaybackCallback = jo_core_add_callback(IntroDraw);
}