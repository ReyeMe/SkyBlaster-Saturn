#include <jo/jo.h>
#include "pcmcdda.h"

void CDDASetVolume(const Uint8 left, const Uint8 right)
{
    // Stolen from: https://github.com/shicky256/garfield-master/blob/master/sound.c
    volatile Uint16 *slot_ptr;
    Uint8 limitedLeftVolume = left;
    Uint8 limitedRightVolume = right;

    //max sound volume is 7
    if (limitedLeftVolume > 7) {
        limitedLeftVolume = 7;
    }

    if (limitedRightVolume > 7) {
        limitedRightVolume = 7;
    }

    // Setup SCSP Slot 16 and Slot 17 for playing
    slot_ptr = (volatile Uint16 *)(0x25B00000 + (0x20 * 16));
    slot_ptr[0] = 0x1000;
    slot_ptr[1] = 0x0000; 
    slot_ptr[2] = 0x0000; 
    slot_ptr[3] = 0x0000; 
    slot_ptr[4] = 0x0000; 
    slot_ptr[5] = 0x0000; 
    slot_ptr[6] = 0x00FF; 
    slot_ptr[7] = 0x0000; 
    slot_ptr[8] = 0x0000; 
    slot_ptr[9] = 0x0000; 
    slot_ptr[10] = 0x0000; 
    slot_ptr[11] = 0x001F | (limitedLeftVolume << 5);
    slot_ptr[12] = 0x0000; 
    slot_ptr[13] = 0x0000; 
    slot_ptr[14] = 0x0000; 
    slot_ptr[15] = 0x0000; 

    slot_ptr = (volatile Uint16 *)(0x25B00000 + (0x20 * 17));
    slot_ptr[0] = 0x1000;
    slot_ptr[1] = 0x0000; 
    slot_ptr[2] = 0x0000; 
    slot_ptr[3] = 0x0000; 
    slot_ptr[4] = 0x0000; 
    slot_ptr[5] = 0x0000; 
    slot_ptr[6] = 0x00FF; 
    slot_ptr[7] = 0x0000; 
    slot_ptr[8] = 0x0000; 
    slot_ptr[9] = 0x0000; 
    slot_ptr[10] = 0x0000; 
    slot_ptr[11] = 0x000F | (limitedRightVolume << 5);
    slot_ptr[12] = 0x0000; 
    slot_ptr[13] = 0x0000; 
    slot_ptr[14] = 0x0000; 
    slot_ptr[15] = 0x0000;

    *((volatile Uint16 *)(0x25B00400)) = 0x020F;
}

void CDDAInitialize()
{
    CDDASetVolume(7,7);
}

void CDDAPlay(const int fromTrack, const int toTrack, const bool loop)
{
    CdcPly ply;
    CDC_PLY_STYPE(&ply) = CDC_PTYPE_TNO; //track number
	CDC_PLY_STNO(&ply)  = fromTrack;
	CDC_PLY_SIDX(&ply) = 1;
	CDC_PLY_ETYPE(&ply) = CDC_PTYPE_TNO;
	CDC_PLY_ETNO(&ply)  = toTrack;
	CDC_PLY_EIDX(&ply) = 1;

    if (loop) {
        CDC_PLY_PMODE(&ply) = CDC_PM_DFL | 0xf; //0xf = infinite repetitions
    }
    else {
        CDC_PLY_PMODE(&ply) = CDC_PM_DFL;
    }
	
    CDC_CdPlay(&ply);
}

void CDDAPlaySingle(const int track, const bool loop)
{
    CDDAPlay(track, track, loop);
}

void CDDAStop()
{
    CdcPos poswk;
    CDC_POS_PTYPE(&poswk) = CDC_PTYPE_DFL;
    CDC_CdSeek(&poswk);
}
