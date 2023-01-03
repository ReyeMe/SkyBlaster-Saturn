#ifndef __EXPLOSIONS_H__
#define __EXPLOSIONS_H__

#define NPC_EXP_FRM_CNT 5

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

/** @brief Initialize explosion particles
 */
void ExplosionsInitialize();

/** @brief Remove all explosion particles
 */
void ExplosionsClearAll();

/** @brief Update explosion particles
 */
void ExplosionsUpdate();

/** @brief Create explosion particle
 *  @param pos Location to create at
 */
void ExplosionsSpawn(const jo_pos3D_fixed *pos);

/** @brief Draw explosion particles
 */
void ExplosionsDraw();

#endif
