#include <jo/jo.h>
#include "pcmsys.h"
#include "model.h"
#include "font3D.h"
#include "score.h"
#include "explosions.h"
#include "player.h"
#include "bullet.h"
#include "bulletList.h"
#include "tools.h"
#include "npc.h"
#include "pickup.h"

// -------------------------------------
// Npc behaviours
// -------------------------------------

Bullet *NpcSimpleBehaviour(Npc *npc)
{
    npc->Pos.x += npc->Velocity.x;
    npc->Pos.y += npc->Velocity.y;
    npc->Pos.z += npc->Velocity.z;
    return JO_NULL;
}

Bullet *NpcDartBehaviour(Npc *npc)
{
    jo_fixed velocity = npc->Velocity.x;

    if (npc->LifeTime > 170 && npc->LifeTime < 230)
    {
        velocity = 0;
    }
    else if (npc->LifeTime >= 230)
    {
        velocity = npc->Velocity.x * 8;
    }

    npc->Pos.x += velocity;
    npc->Pos.y += npc->Velocity.y;
    npc->Pos.z += npc->Velocity.z;
    
    return JO_NULL;
}

Bullet *NpcGruntBehaviour(Npc *npc)
{
    NpcSimpleBehaviour(npc);
    
    if (npc->LifeTime % 60 == 0)
    {
        Bullet *bullet = jo_malloc_with_behaviour(sizeof(Bullet), JO_MALLOC_TRY_REUSE_SAME_BLOCK_SIZE);
        bullet->Pos.x = npc->Pos.x;
        bullet->Pos.y = npc->Pos.y;
        bullet->Type = BulletNpcSimple;
        bullet->Velocity.x = BULLET_SPEED >> 1;
        bullet->Velocity.y = 0;
        BulletInitializeMesh(bullet);
        return bullet;
    }
    else
    {
        return JO_NULL;
    }
}

Bullet *NpcHomingBehaviour(Npc *npc, Player *player, int playerCount)
{
    NpcDartBehaviour(npc);
    
    if (npc->LifeTime > 230 && npc->LifeTime < 240)
    {
        Player *closest = JO_NULL;
        int distance = 0;

        for (int p = 0; p < playerCount; p++)
        {
            int playerLocation = JO_ABS(player[p].Pos.x) + JO_ABS(player[p].Pos.y);
            int npcLocation = JO_ABS(npc->Pos.x) + JO_ABS(npc->Pos.y);
            int current = playerLocation - npcLocation;
            current = JO_ABS(current);
            
            if (closest == JO_NULL || distance > current)
            {
                distance = current;
                closest = &player[p];
            }
        }

        if (closest != JO_NULL)
        {
            Bullet *bullet = jo_malloc_with_behaviour(sizeof(Bullet), JO_MALLOC_TRY_REUSE_SAME_BLOCK_SIZE);
            bullet->Pos.x = npc->Pos.x;
            bullet->Pos.y = npc->Pos.y;
            bullet->Target.x = closest->Pos.x;
            bullet->Target.y = closest->Pos.y;
            bullet->Type = BulletHoming;
            bullet->Velocity.x = BULLET_SPEED >> 1;
            bullet->Velocity.y = 0;
            BulletInitializeMesh(bullet);

            npc->LifeTime = 250;
            return bullet;
        }
    }
    
    return JO_NULL;
}

Bullet *NpcSinusBehaviour(Npc *npc)
{
    npc->Pos.x += npc->Velocity.x;
    npc->Pos.y += jo_sin(npc->LifeTime) >> 1;
    npc->Pos.z += npc->Velocity.z;
    
    if (npc->LifeTime % 80 == 0)
    {
        Bullet *bullet = jo_malloc_with_behaviour(sizeof(Bullet), JO_MALLOC_TRY_REUSE_SAME_BLOCK_SIZE);
        bullet->Pos.x = npc->Pos.x;
        bullet->Pos.y = npc->Pos.y;
        bullet->Type = BulletNpcSimple;
        bullet->Velocity.x = BULLET_SPEED >> 1;
        bullet->Velocity.y = 0;
        BulletInitializeMesh(bullet);
        return bullet;
    }
    else
    {
        return JO_NULL;
    }
}

/* Currently available NPCs */
static jo_list Npcs;

/* Available NPC meshes */
static SaturnMesh NpcMeshes[3];

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
            ExplosionsSpawn(&((Npc *)data)->Pos);
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
    case NpcRocketShip:
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
    case NpcRocketShip:
        return JO_FIXED_16;

    case NpcDart:
    case NpcMine:
    default:
        return JO_FIXED_8;
    }
}

/** @brief Check NPC against bullet
 *  @param npc NPC to check
 *  @param hitLocation where NPC got hit
 *  @return True if got hit by bullet
 */
static bool CheckNpcAgainstBullets(const Npc *npc, jo_pos2D_fixed * hitLocation)
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
            hitLocation->x = bullet->Pos.x - npc->Pos.x;
            hitLocation->y = bullet->Pos.y - npc->Pos.y;
            FreeSpriteQuadData(&bullet->Mesh);
            jo_list_free_and_remove(bullets, tmp);
            return true;
        }
    }

    hitLocation->x = -JO_FIXED_360;
    hitLocation->y = -JO_FIXED_360;
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
 *  @param player Player data
 *  @param playerCount Number of all players
 */
static void UpdateNpcWithBehaviour(Npc *npc, Player *player, int playerCount)
{
    Bullet *bullet = JO_NULL;

    switch (npc->Type)
    {
        case NpcLightShip:
            bullet = NpcGruntBehaviour(npc);
            break;

        case NpcMine:
            bullet = NpcSimpleBehaviour(npc);
            break;

        case NpcLightShipSinus:
            bullet = NpcSinusBehaviour(npc);
            break;

        case NpcDart:
            bullet = NpcDartBehaviour(npc);
            break;

        case NpcRocketShip:
            bullet = NpcHomingBehaviour(npc, player, playerCount);
            break;

        default:
            break;
    }

    if (bullet != JO_NULL)
    {
        BulletListAdd(bullet, false);
    }
}

// -------------------------------------
// Public
// -------------------------------------

void NpcInitialize()
{
    jo_list_init(&Npcs);

    TmfLoadMesh(&NpcMeshes[0], "BOMB.TMF", JO_ROOT_DIR);
    TmfLoadMesh(&NpcMeshes[1], "ENEMY01.TMF", JO_ROOT_DIR);
    TmfLoadMesh(&NpcMeshes[2], "ENEMY02.TMF", JO_ROOT_DIR);
}

Npc *NpcCreate(const NpcTypes type, const jo_pos3D_fixed *pos)
{
    Npc *npc = (Npc *)jo_malloc(sizeof(Npc));

    npc->Health = type + 1;
    npc->LastHit.x = -JO_FIXED_360;
    npc->LastHit.x = -JO_FIXED_360;

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
    case NpcRocketShip:
        npc->Velocity.x = JO_FIXED_1_DIV_2;
        break;

    case NpcLightShipSinus:
        npc->Velocity.x = JO_FIXED_1 + (JO_FIXED_1 >> 2);
        break;

    case NpcMine:
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
            ExplosionsSpawn(&npc->Pos);
            jo_list_free_and_remove(&Npcs, tmp);
        }
    }

    return score;
}

int NpcUpdate(Player *player, int playerCount, void (*PlayerHitCallback)(Player * player))
{
    int score = 0;
    jo_node *tmp;
    int chance = jo_random(1300);

    for (tmp = Npcs.first; tmp != JO_NULL; tmp = tmp->next)
    {
        Npc *npc = (Npc *)tmp->data.ptr;

        UpdateNpcWithBehaviour(npc, player, playerCount);
        npc->LifeTime++;
        npc->LastHitTime++;
        jo_pos2D_fixed hitLocation;

        if (CheckNpcAgainstBullets(npc, &hitLocation))
        {
            npc->Health--;

            if (npc->Health <= 0)
            {
                score += NPC_BASESCORE * (npc->Type + 1);
                ExplosionsSpawn(&npc->Pos);
                jo_list_free_and_remove(&Npcs, tmp);

                // If NPC was destroyed by player, make it possible to spawn pickup
                if (chance < 80)
                {
                    PickupCreate(&npc->Pos, chance < 35 ? (chance < 10 ? PickupTypeLife : PickupTypeBomb) : PickupTypeGun);
                }
            }
            else
            {
                npc->LastHitTime = 0;
                npc->LastHit.x = hitLocation.x;
                npc->LastHit.y = hitLocation.y;
            }
        }
        else
        {
            bool hit = false;

            for (int p = 0; p < playerCount; p++)
            {
                if (player[p].Lives >= 0 && CheckNpcAgainstPlayer(npc, &player[p]))
                {
                    hit = true;
                    PlayerHitCallback(&player[p]);
                    npc->Health -= 2;

                    if (npc->Health <= 0)
                    {
                        ExplosionsSpawn(&npc->Pos);
                        jo_list_free_and_remove(&Npcs, tmp);
                    }

                    break;
                }
            }
            
            if (!hit && npc->Pos.x > NPC_DESPAWN_X)
            {
                // Delete all NPCs that go out of screen
                jo_list_free_and_remove(&Npcs, tmp);
            }
        }
    }

    return score;
}

void NpcDraw()
{
    jo_node *tmp;
    int modelNumber;

    for (tmp = Npcs.first; tmp != JO_NULL; tmp = tmp->next)
    {
        Npc *npc = (Npc *)tmp->data.ptr;
        modelNumber = GetModelFromType(npc->Type);

        jo_3d_push_matrix();
        {
            jo_3d_translate_matrix_fixed(npc->Pos.x, npc->Pos.y, npc->Pos.z);
            TmfDraw(&NpcMeshes[modelNumber]);
        }
        jo_3d_pop_matrix();

        // Enemy was hit
        if (npc->LastHit.x != -JO_FIXED_360 && npc->LastHitTime < 25)
        {
            FIXED point[XYZ] = { npc->Pos.x + npc->LastHit.x, npc->Pos.y + npc->LastHit.y, npc->Pos.z };
            FIXED projected[XY];
            slConvert3Dto2DFX(point, projected);
  
            int scale = npc->LastHitTime;
  
            jo_pos2D screenPoint = { projected[X]>>16, projected[Y]>>16 };
  
            for (int particle = 0; particle < 4; particle++)
            {
                jo_size rectangleScale = { scale - (particle << 1), scale - (particle << 1) };
  
                if (rectangleScale.width > 0 && rectangleScale.width < 16)
                {
                    ANGLE angle = DEGtoANG((npc->LastHitTime + (particle << 2)) << 2);
                    ToolsDrawRotatedRectangle(&screenPoint, &rectangleScale, angle, 100, JO_COLOR_Yellow, false);
                }
            }
        }
    }
}

int NpcCount()
{
    return Npcs.count;
}
