#ifndef __BULLET_LIST_H__
#define __BULLET_LIST_H__

/** @brief Callback for BulletListUpdate() */
typedef void (*BulletListCallback)(Bullet * bullet, bool isPlayer);

/** @brief Initialize bullet lists
 */
void BulletListInitialize();

/** @brief Draw all bullets in lists
 */
void BulletListDraw();

/** @brief Get bullet list
 *  @param playerList Indicates whether to return player bullets or enemy bullets
 *  @return Bullet list
 */
jo_list * BulletListGet(bool playerList);

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
void BulletListClearEnemyBulletsInRange(const jo_pos3D_fixed * pos, const jo_fixed range);

/** @brief Add bullet to list
 *  @param isPlayer Indicates whether bullet belongs to player entity
 */
void BulletListAdd(Bullet * bullet, bool isPlayer);

#endif
