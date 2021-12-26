#ifndef __BULLET_H__
#define __BULLET_H__

#define BULLET_SPAWN_AREA_X (-13107200)
#define BULLET_DESPAWN_AREA_X (13107200)
#define BULLET_DESPAWN_AREA_Y_NEG (-6881280) // 120.00
#define BULLET_DESPAWN_AREA_Y_POS (6881280)  // 120.00

#define BULLET_POS_Z JO_FIXED_1
#define BULLET_SPEED (400000)

/** @brief Bullet types
 */
typedef enum
{
    /* Bullet shot by player */
    BulletTypePlayer = 0,

    /* Small bullet shot by enemy */
    BulletTypeSmall = 1,

    /* Big bullet shot by enemy */
    BulletTypeBig = 2
} BulletTypes;

/** @brief Bullet entity data
 */
typedef struct
{
    /* Bullet position */
    jo_pos2D_fixed Pos;

    /* Bullet velocity */
    jo_vector2_fixed Velocity;

    /* Type of the bullet */
    BulletTypes Type;
} Bullet;

/** @brief Load textures or models
 */
void BulletLoadAssets();

/** @brief Draw single bullet
 *  @param bullet Bullet data
 */
void BulletDraw(Bullet *bullet);

/** @brief Update single bullet
 *  @param bullet Bullet data
 *  @return True if bullet is to be destroyed
 */
bool BulletUpdate(Bullet *Bullet);

#endif
