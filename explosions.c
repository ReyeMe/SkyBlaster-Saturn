#include <jo/jo.h>
#include "pcmsys.h"
#include "tools.h"
#include "explosions.h"

// -------------------------------------
// Internal
// -------------------------------------

/* List of all current explosion sprites */
static jo_list Explosions;

/* Explosion sound */
static short ExplosionSound;

/* Index of first explosion sprite */
static int ExplosionSpriteStartIndex;

/** @brief Clear Explosion list
 */
static void ListClearInternal(jo_list *list)
{
    jo_node *tmp;

    for (tmp = list->first; tmp != JO_NULL; tmp = tmp->next)
    {
        void *data = tmp->data.ptr;

        FreeSpriteQuadData(&((NpcExplosion*)data)->Mesh);
        jo_free(data);
        jo_list_remove(list, tmp);
    }
}
// -------------------------------------
// Public
// -------------------------------------

void ExplosionsInitialize()
{
    jo_list_init(&Explosions);
    ExplosionSound = load_16bit_pcm((Sint8 *)"EXP.PCM", 15360);

    for (int sprite = 0; sprite < NPC_EXP_FRM_CNT; sprite++)
    {
        char filename[9];
        sprintf(filename, "EXP%d.TGA", sprite + 1);
        int spriteIndex = jo_sprite_add_tga(JO_ROOT_DIR, filename, JO_COLOR_RGB(255, 0, 255));

        if (sprite == 0)
        {
            ExplosionSpriteStartIndex = spriteIndex;
        }
    }
}

void ExplosionsClearAll()
{
    ListClearInternal(&Explosions);
}

void ExplosionsUpdate()
{
    jo_node *tmp;

    for (tmp = Explosions.first; tmp != JO_NULL; tmp = tmp->next)
    {
        NpcExplosion *explosion = (NpcExplosion *)tmp->data.ptr;

        if (explosion->LifeTime % 2 == 0)
        {
            explosion->Frame++;
        }

        if (explosion->Frame >= NPC_EXP_FRM_CNT)
        {
            FreeSpriteQuadData(&explosion->Mesh);
            jo_list_free_and_remove(&Explosions, tmp);
        }
        else
        {
            explosion->Pos.x += JO_FIXED_1_DIV_2;
            explosion->LifeTime++;
        }
    }
}

void ExplosionsSpawn(const jo_pos3D_fixed *pos)
{
    NpcExplosion *explosion = (NpcExplosion *)jo_malloc_with_behaviour(sizeof(NpcExplosion), JO_MALLOC_TRY_REUSE_SAME_BLOCK_SIZE);

    if (explosion != JO_NULL)
    {
        explosion->Pos.x = pos->x;
        explosion->Pos.y = pos->y;
        explosion->Pos.z = pos->z;
        explosion->LifeTime = 0;
        CreateSpriteQuad(&explosion->Mesh, ExplosionSpriteStartIndex);
    
        jo_list_add_ptr(&Explosions, explosion);
    }

    // Play sound
    pcm_play(ExplosionSound, PCM_PROTECTED, 6);
}

void ExplosionsDraw()
{
    jo_node *tmp;

    for (tmp = Explosions.first; tmp != JO_NULL; tmp = tmp->next)
    {
        NpcExplosion *explosion = (NpcExplosion *)tmp->data.ptr;

        jo_3d_push_matrix();
        {
            jo_3d_translate_matrix_fixed(explosion->Pos.x, explosion->Pos.y, explosion->Pos.z);

            int frame = JO_MAX(explosion->Frame, 0);
            frame = JO_MIN(frame, NPC_EXP_FRM_CNT);
            jo_3d_set_texture(&explosion->Mesh, ExplosionSpriteStartIndex + frame);
            jo_3d_draw(&explosion->Mesh);
        }
        jo_3d_pop_matrix();
    }
}
