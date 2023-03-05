#ifndef __BULLET_LIST_H__
#define __BULLET_LIST_H__

/* Max number of bullets on screen */
#define BULLET_COUNT (50)

/** @brief Callback for BulletListUpdate() */
typedef void (*BulletListCallback)(Bullet *bullet, bool isPlayer);

/** @brief Get bullets
 *  @return Bullet list 
 */
Bullet * BulletListGet();

/** @brief Initialize bullet lists
 */
void BulletListInitialize();

/** @brief Draw all bullets in lists
 */
void BulletListDraw();

/** @brief Update positions of all bullets
 */
void BulletListUpdate(BulletListCallback callback);

/** @brief Clear bullets
 *  @param onlyEnemy Indicates a value whether to clear only enemy bullets
 */
void BulletListClear(bool onlyEnemy);

/** @brief Clear bullets
 *  @param pos Center point
 *  @param range Destruction range
 */
void BulletListClearEnemyBulletsInRange(const jo_pos2D_fixed *pos, const jo_fixed range);

/** @brief Add bullet to list
 *  @param pos Bullet start position
 *  @param target Target position
 *  @param velocity Initial velocity
 *  @param type Bullet type
 */
void BulletListAdd(const jo_pos2D_fixed *pos, const jo_pos2D_fixed *target, const jo_vector2_fixed *velocity, BulletType type);

#endif
