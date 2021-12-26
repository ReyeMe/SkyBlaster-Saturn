#include <jo/jo.h>
#include "pcmsys.h"
#include "model.h"
#include "font3D.h"
#include "score.h"
#include "player.h"
#include "bullet.h"
#include "bulletList.h"
#include "tools.h"
#include "npc.h"
#include "pickup.h"

// -------------------------------------
// Internal
// -------------------------------------

/* Explosion sound */
static short ExplosionSound;

/* Currently available NPCs */
static jo_list Npcs;

/* List of all current explosion sprites */
static jo_list Explosions;

/* Available NPC meshes */
static jo_3d_mesh *NpcMeshes[3];

/* Available NPC mesh sizes */
static int NpcMeshSizes[3];

/* Index of first explosion sprite */
static int ExplosionSpriteStartIndex;

/** @brief Clear NPC list
 *  @param list Pointer to list to clear
 *  @param spawnExplosion When true, will spawn explosion where NPC was
 *  @return total score of all destroyed NPCs
 */
static int ListClearInternal(jo_list *list, bool spawnExplosion)
{
    int collectedScore = 0;
    jo_node *tmp;

    for (tmp = list->first; tmp != JO_NULL; tmp = tmp->next)
    {
        void *data = tmp->data.ptr;

        if (spawnExplosion)
        {
            // Add score
            collectedScore += NPC_BASESCORE * (((Npc *)data)->Type + 1);

            // Create explosion particle
            NpcSpawnExplosion(&((Npc *)data)->Pos);
        }

        jo_free(data);
        jo_list_remove(list, tmp);
    }

    return collectedScore;
}

/** @brief Get model index from NPC type
 *  @param type NPC type
 */
static int GetModelFromType(NpcTypes type)
{
    switch (type)
    {
    case NpcDart:
        return 2;

    case NpcLightShipSinus:
    case NpcLightShip:
        return 1;

    case NpcMine:
    default:
        return 0;
    }
}

/** @brief Update NPC with unique behaviour
 *  @param npc NPC to update
 */
static jo_fixed GetColliderSize(const Npc *npc)
{
    switch (npc->Type)
    {
    case NpcLightShip:
    case NpcLightShipSinus:
        return JO_FIXED_16;

    case NpcDart:
    case NpcMine:
    default:
        return JO_FIXED_8;
    }
}

/** @brief Check NPC against bullet
 *  @param npc NPC to check
 *  @return True if got hit by bullet
 */
static bool CheckNpcAgainstBullets(const Npc *npc)
{
    jo_node *tmp;
    jo_fixed colliderSize = GetColliderSize(npc);
    jo_list *bullets = BulletListGet(true);

    for (tmp = bullets->first; tmp != JO_NULL; tmp = tmp->next)
    {
        Bullet *bullet = (Bullet *)tmp->data.ptr;
        jo_vector_fixed fromNpc = {{bullet->Pos.x - npc->Pos.x, bullet->Pos.y - npc->Pos.y, 0}};
        jo_fixed distance = ToolsFastVectorLength(&fromNpc);

        if (distance < colliderSize)
        {
            jo_list_free_and_remove(bullets, tmp);
            return true;
        }
    }

    return false;
}

/** @brief Check NPC against player
 *  @param npc NPC to check
 *  @param player Player data
 *  @return True if got hit by player
 */
static bool CheckNpcAgainstPlayer(const Npc *npc, const Player *player)
{
    jo_fixed colliderSize = GetColliderSize(npc);
    jo_vector_fixed fromNpc = {{player->Pos.x - npc->Pos.x, player->Pos.y - npc->Pos.y, 0}};
    jo_fixed distance = ToolsFastVectorLength(&fromNpc);
    return distance < colliderSize + JO_FIXED_2;
}

/** @brief Update NPC with unique behaviour
 *  @param npc NPC to update
 */
static void UpdateNpcWithBehaviour(Npc *npc)
{
    jo_vector_fixed velocity = {{npc->Velocity.x, npc->Velocity.y, npc->Velocity.z}};
    bool shoot = false;

    switch (npc->Type)
    {
    case NpcDart:
        if (npc->LifeTime > 170 && npc->LifeTime < 230)
        {
            velocity.x = 0;
        }
        else if (npc->LifeTime >= 230)
        {
            velocity.x *= 8;
        }

        break;

    case NpcLightShipSinus:
        velocity.y = jo_sin(npc->LifeTime) >> 1;
        shoot = true;

        if (npc->LifeTime > 359)
        {
            npc->LifeTime = 0;
        }

        break;

    case NpcLightShip:
        shoot = true;
        break;

    default:
        break;
    }

    if (shoot && npc->LifeTime % 80 == 0)
    {
        Bullet *bullet = jo_malloc(sizeof(Bullet));
        bullet->Pos.x = npc->Pos.x;
        bullet->Pos.y = npc->Pos.y;
        bullet->Type = 1;
        bullet->Velocity.x = BULLET_SPEED >> 1;
        bullet->Velocity.y = 0;
        BulletListAdd(bullet, false);
    }

    npc->Pos.x += velocity.x;
    npc->Pos.y += velocity.y;
    npc->Pos.z += velocity.z;
}

// -------------------------------------
// Public
// -------------------------------------

void NpcInitialize()
{
    jo_list_init(&Npcs);
    jo_list_init(&Explosions);

    ExplosionSound = load_16bit_pcm((Sint8 *)"EXP.PCM", 15360);
    NpcMeshes[0] = ML_LoadMesh("BOMB.TMF", JO_ROOT_DIR, &(NpcMeshSizes[0]));
    NpcMeshes[1] = ML_LoadMesh("ENEMY01.TMF", JO_ROOT_DIR, &(NpcMeshSizes[1]));
    NpcMeshes[2] = ML_LoadMesh("ENEMY02.TMF", JO_ROOT_DIR, &(NpcMeshSizes[2]));

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

void NpcSpawnExplosion(const jo_pos3D_fixed *pos)
{
    NpcExplosion *explosion = (NpcExplosion *)jo_malloc(sizeof(NpcExplosion));
    explosion->Pos.x = pos->x;
    explosion->Pos.y = pos->y;
    explosion->Pos.z = pos->z;
    explosion->LifeTime = 0;

    jo_list_add_ptr(&Explosions, explosion);

    // Play sound
    pcm_play(ExplosionSound, PCM_PROTECTED, 6);
}

Npc *NpcCreate(const NpcTypes type, const jo_pos3D_fixed *pos)
{
    Npc *npc = (Npc *)jo_malloc(sizeof(Npc));
    npc->Type = type;
    npc->Pos.x = pos->x;
    npc->Pos.y = pos->y;
    npc->Pos.z = pos->z;

    npc->Velocity.x = 0;
    npc->Velocity.y = 0;
    npc->Velocity.z = 0;

    switch (type)
    {
    case NpcDart:
        npc->Velocity.x = JO_FIXED_1_DIV_2;
        break;

    case NpcMine:
    case NpcLightShipSinus:
    case NpcLightShip:
        npc->Velocity.x = JO_FIXED_1 + (JO_FIXED_1 >> 2);
        break;

    default:
        break;
    }

    jo_list_add_ptr(&Npcs, npc);
    return npc;
}

void NpcClearAll()
{
    ListClearInternal(&Npcs, false);
    ListClearInternal(&Explosions, false);
}

int NpcDestroyAll()
{
    return ListClearInternal(&Npcs, true);
}

int NpcDestroyAllInRange(const jo_pos3D_fixed *pos, const jo_fixed range)
{
    int score = 0;
    jo_node *tmp;

    for (tmp = Npcs.first; tmp != JO_NULL; tmp = tmp->next)
    {
        Npc *npc = (Npc *)tmp->data.ptr;
        jo_vector_fixed fromNpc = {{pos->x - npc->Pos.x, pos->y - npc->Pos.y, 0}};
        jo_fixed distance = ToolsFastVectorLength(&fromNpc);

        if (distance < range)
        {
            score += NPC_BASESCORE * (npc->Type + 1);
            NpcSpawnExplosion(&npc->Pos);
            jo_list_free_and_remove(&Npcs, tmp);
        }
    }

    return score;
}

void NpcUpdateExplosions()
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
            jo_list_free_and_remove(&Explosions, tmp);
        }
        else
        {
            explosion->Pos.x += JO_FIXED_1_DIV_2;
            explosion->LifeTime++;
        }
    }
}

int NpcUpdate(Player *player, void (*PlayerHitCallback)())
{
    int score = 0;
    jo_node *tmp;
    int chance = jo_random(1300);

    for (tmp = Npcs.first; tmp != JO_NULL; tmp = tmp->next)
    {
        Npc *npc = (Npc *)tmp->data.ptr;

        UpdateNpcWithBehaviour(npc);
        npc->LifeTime++;

        if (CheckNpcAgainstBullets(npc))
        {
            score += NPC_BASESCORE * (npc->Type + 1);
            NpcSpawnExplosion(&npc->Pos);
            jo_list_free_and_remove(&Npcs, tmp);

            // If NPC was destroyed by player, make it possible to spawn pickup
            if (chance < 80)
            {
                PickupCreate(&npc->Pos, chance < 35 ? (chance < 10 ? PickupTypeLife : PickupTypeBomb) : PickupTypeGun);
            }
        }
        else if (CheckNpcAgainstPlayer(npc, player))
        {
            PlayerHitCallback();
            NpcSpawnExplosion(&npc->Pos);
            jo_list_free_and_remove(&Npcs, tmp);
        }
        else if (npc->Pos.x > NPC_DESPAWN_X)
        {
            // Delete all NPCs that go out of screen
            jo_list_free_and_remove(&Npcs, tmp);
        }
    }

    return score;
}

void NpcDraw()
{
    jo_node *tmp;
    int modelNumber;
    int i;

    for (tmp = Npcs.first; tmp != JO_NULL; tmp = tmp->next)
    {
        Npc *npc = (Npc *)tmp->data.ptr;
        modelNumber = GetModelFromType(npc->Type);

        jo_3d_push_matrix();
        {
            jo_3d_translate_matrix_fixed(npc->Pos.x, npc->Pos.y, npc->Pos.z);

            for (i = 0; i < NpcMeshSizes[modelNumber]; i++)
            {
                jo_3d_mesh_draw(&(NpcMeshes[modelNumber][i]));
            }
        }
        jo_3d_pop_matrix();
    }

    for (tmp = Explosions.first; tmp != JO_NULL; tmp = tmp->next)
    {
        NpcExplosion *explosion = (NpcExplosion *)tmp->data.ptr;

        jo_3d_push_matrix();
        {
            jo_3d_translate_matrix_fixed(explosion->Pos.x, explosion->Pos.y, explosion->Pos.z);
            jo_3d_draw_sprite(ExplosionSpriteStartIndex + explosion->Frame);
        }
        jo_3d_pop_matrix();
    }
}
