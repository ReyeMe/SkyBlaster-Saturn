#include <jo/jo.h>
#include "bullet.h"
#include "bulletList.h"
#include "tools.h"

// -------------------------------------
// Internal
// -------------------------------------

/* Bullets shot by player */
static jo_list PlayerBullets;

/* Bullets shot by player */
static jo_list EnemyBullets;

/** @brief Draw bullet from list
 *  @param node Bullet instance
 */
static void BulletListDrawInternal(jo_node *node)
{
    BulletDraw((Bullet*)node->data.ptr);
}

/** @brief Update bullet from list
 *  @param node Bullet instance
 *  @param callback Callback after update
 *  @param isPlayer Is parent player?
 */
static void BulletListUpdateInternal(jo_node * node, BulletListCallback callback, bool isPlayer)
{
    Bullet * bullet = (Bullet*)node->data.ptr;

    if (BulletUpdate(bullet))
    {
        // remove bullet
        jo_free(bullet);

        if (isPlayer)
        {
            jo_list_remove(&PlayerBullets, node);
        }
        else
        {
            jo_list_remove(&EnemyBullets, node);
        }
    }
    else if (callback != JO_NULL)
    {
        callback(bullet, isPlayer);
    }
}

/** @brief Clear bullet list
 *  @param list Pointer to list to clear
 */
static void BulletListClearInternal(jo_list * list)
{
    jo_node * tmp;

    for (tmp = list->first; tmp != JO_NULL; tmp = tmp->next)
    {
        Bullet * bullet = (Bullet*)tmp->data.ptr;
        jo_free(bullet);
        jo_list_remove(list, tmp);
    }
}

// -------------------------------------
// Public
// -------------------------------------

void BulletListInitialize()
{
    jo_list_init(&PlayerBullets);
    jo_list_init(&EnemyBullets);
}

void BulletListDraw()
{
    jo_list_foreach(&PlayerBullets, BulletListDrawInternal);
    jo_list_foreach(&EnemyBullets, BulletListDrawInternal);
}

jo_list * BulletListGet(bool playerList)
{
    if (playerList)
    {
        return &PlayerBullets;
    }
    else
    {
        return &EnemyBullets;
    }
}

void BulletListUpdate(BulletListCallback callback)
{
    jo_node *tmp;

    for (tmp = PlayerBullets.first; tmp != JO_NULL; tmp = tmp->next)
    {
        BulletListUpdateInternal(tmp, callback, true);
    }

    for (tmp = EnemyBullets.first; tmp != JO_NULL; tmp = tmp->next)
    {
        BulletListUpdateInternal(tmp, callback, false);
    }
}

void BulletListClear(bool onlyEnemy)
{
    if (!onlyEnemy)
    {
        BulletListClearInternal(&PlayerBullets);
    }

    BulletListClearInternal(&EnemyBullets);
}

void BulletListClearEnemyBulletsInRange(const jo_pos3D_fixed * pos, const jo_fixed range)
{
    jo_node *node;

    for (node = EnemyBullets.first; node != JO_NULL; node = node->next)
    {
        Bullet * bullet = (Bullet*)node->data.ptr;
        jo_vector_fixed fromBullet = { { pos->x - bullet->Pos.x, pos->y - bullet->Pos.y, 0 } };
        jo_fixed distance = ToolsFastVectorLength(&fromBullet);

        if (distance < range)
        {
            jo_list_free_and_remove(&EnemyBullets, node);
        }
    }
}

void BulletListAdd(Bullet * bullet, bool isPlayer)
{
    if (isPlayer)
    {
        jo_list_add_ptr(&PlayerBullets, bullet);
    }
    else
    {
        jo_list_add_ptr(&EnemyBullets, bullet);
    }
}
