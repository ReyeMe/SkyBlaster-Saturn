#ifndef __BULLET_H__
#define __BULLET_H__

#define BULLET_SPAWN_AREA_X (-13107200)
#define BULLET_DESPAWN_AREA_X (13107200)
#define BULLET_DESPAWN_AREA_Y_NEG (-6881280) // 120.00
#define BULLET_DESPAWN_AREA_Y_POS (6881280)  // 120.00

#define BULLET_POS_Z JO_FIXED_1
#define BULLET_SPEED (400000)

/** @brief Bullet type
 */
typedef enum BulletType
{
    BulletPlayerSimple = 0,
    BulletNpcSimple = 1,
    BulletHoming = 2
} BulletType;

/** @brief Bullet entity data
 */
typedef struct _Bullet Bullet;
typedef struct _Bullet
{
    /* Bullet position */
    jo_pos2D_fixed Pos;

    /* Bullet position */
    jo_pos2D_fixed Target;

    /* Bullet velocity */
    jo_vector2_fixed Velocity;

    /* Bullet sprite */
    BulletType Type;

    /* Mesh of the bullet */
    jo_3d_quad Mesh;
} Bullet;

/** @brief Initialize bullet mesh
 * @param bullet Bullet data
 */
void BulletInitializeMesh(Bullet * bullet);

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
bool BulletUpdate(Bullet *bullet);

#endif
