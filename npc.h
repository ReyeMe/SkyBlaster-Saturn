#ifndef __NPC_H__
#define __NPC_H__

#define NPC_SPAWN_X -13107200
#define NPC_DESPAWN_X 13107200
#define NPC_BASESCORE 50
#define NPC_EXP_FRM_CNT 5

/** @brief Available enemy types
 */
typedef enum NpcTypes
{
    NpcMine = 0,
    NpcDart = 1,
    NpcLightShip = 2,
    NpcLightShipSinus = 3,
} NpcTypes;

/** @brief NPC data structure
 */
typedef struct _Npc Npc;
typedef struct _Npc
{
    /* Level coordinates */
    jo_pos3D_fixed Pos;

    /* Movement velocity */
    jo_vector_fixed Velocity;

    /* Enemy type */
    NpcTypes Type;

    /* Current NPC life time */
    int LifeTime;
} Npc;

/** @brief Explosion data structure
 */
typedef struct
{
    /* current explosion location */
    jo_pos3D_fixed Pos;

    /* Current explosion life time */
    int LifeTime;

    /* Current animation frame */
    int Frame;

    /* Explosion quad */
    jo_3d_quad Mesh;
} NpcExplosion;

/** @brief Initialize NPC data
 */
void NpcInitialize();

/** @brief Spawn explosion at specified location
 *  @param pos Explosion position
 */
void NpcSpawnExplosion(const jo_pos3D_fixed *pos);

/** @brief Create NPC
 *  @param type Type of npc to create
 *  @param pos Npc position
 *  @return Created npc data
 */
Npc *NpcCreate(const NpcTypes type, const jo_pos3D_fixed *pos);

/** @brief Clear all NPCs
 */
void NpcClearAll();

/** @brief Destroy all current NPCs
 *  @return How much score player gained
 */
int NpcDestroyAll();

/** @brief Destroy all NPCs in range
 *  @param pos Center point
 *  @param range Destruction range
 *  @return How much score player gained
 */
int NpcDestroyAllInRange(const jo_pos3D_fixed *pos, const jo_fixed range);

/** @brief Update all explosions that are on screen
 */
void NpcUpdateExplosions();

/** @brief Update all available NPCs (will do tests against bullets and level bounds)
 *  @param player Player data
 *  @param playerCount Number of all players
 *  @param PlayerHitCallback What to do if player was hit
 *  @return How much score player gained
 */
int NpcUpdate(Player *player, int playerCount, void (*PlayerHitCallback)(Player * player));

/** @brief Draw all available NPCs (including explosion effects)
 */
void NpcDraw();

#endif
