#include <jo/jo.h>
#include "pcmsys.h"
#include "tools.h"
#include "explosions.h"

#define EXPLOSION_COUNT (50)

static int ExplosionSpriteStart;
static int ExplosionSound;
static NpcExplosion Explosions[EXPLOSION_COUNT];

// -------------------------------------
// Public
// -------------------------------------

void ExplosionsClearAll()
{
    for (int explosion = 0; explosion < EXPLOSION_COUNT; explosion++)
    {
        Explosions[explosion].LifeTime = 0xFF;
        Explosions[explosion].Frame = 0xFF;
    }
}

void ExplosionsInitialize()
{
    ExplosionSound = load_16bit_pcm((Sint8 *)"EXP.PCM", 15360);

    char filename[9];

    for (int sprite = 0; sprite < NPC_EXP_FRM_CNT; sprite++)
    {
        sprintf(filename, "EXP%d.TGA", sprite + 1);
        int spriteIndex = jo_sprite_add_tga(JO_ROOT_DIR, filename, JO_COLOR_RGB(255, 0, 255));

        if (sprite == 0) ExplosionSpriteStart = spriteIndex;
    }
    
    for (int explosion = 0; explosion < EXPLOSION_COUNT; explosion++)
    {
    	CreateSpriteQuad(&Explosions[explosion].Mesh, ExplosionSpriteStart);
    }

    ExplosionsClearAll();
}

void ExplosionsUpdate()
{
    for (int explosion = 0; explosion < EXPLOSION_COUNT; explosion++)
    {
        if (Explosions[explosion].Frame < NPC_EXP_FRM_CNT)
        {
            if (Explosions[explosion].LifeTime % 2 == 0)
            {
                Explosions[explosion].Frame++;
            }

            Explosions[explosion].LifeTime++;
            Explosions[explosion].Pos.x += JO_FIXED_1_DIV_2;
        }
    }
}

void ExplosionsSpawn(const jo_pos2D_fixed *pos)
{
    for (int explosion = 0; explosion < EXPLOSION_COUNT; explosion++)
    {
        if (Explosions[explosion].Frame >= NPC_EXP_FRM_CNT)
        {
            // Make particle alive again
            Explosions[explosion].Frame = 0;
            Explosions[explosion].LifeTime = 0;
            Explosions[explosion].Pos.x = pos->x;
            Explosions[explosion].Pos.y = pos->y;
            
            // Play sound
            pcm_play(ExplosionSound, PCM_PROTECTED, 6);
            return;
        }
    }
}

void ExplosionsDraw()
{
    for (int explosion = 0; explosion < EXPLOSION_COUNT; explosion++)
    {
        if (Explosions[explosion].Frame < NPC_EXP_FRM_CNT)
        {
            jo_3d_push_matrix();
            {
                jo_3d_translate_matrix_fixed(Explosions[explosion].Pos.x, Explosions[explosion].Pos.y, 0);
                jo_3d_set_texture(&Explosions[explosion].Mesh, ExplosionSpriteStart + Explosions[explosion].Frame);
                jo_3d_draw(&Explosions[explosion].Mesh);
            }
            jo_3d_pop_matrix();
        }
    }
}
