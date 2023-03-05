#include <jo/jo.h>
#include "bullet.h"
#include "bulletList.h"
#include "tools.h"

// -------------------------------------
// Internal
// -------------------------------------

/* Bullets shot by player */
static Bullet Bullets[BULLET_COUNT];

// -------------------------------------
// Public
// -------------------------------------

Bullet * BulletListGet()
{
    return Bullets;
}

void BulletListInitialize()
{
    for (int bullet = 0; bullet < BULLET_COUNT; bullet++)
    {
        Bullets[bullet].Alive = false;
        BulletInitializeMesh(&Bullets[bullet]);
    }
}

void BulletListDraw()
{
    for (int bullet = 0; bullet < BULLET_COUNT; bullet++)
    {
        if (Bullets[bullet].Alive)
        {
            BulletDraw(&Bullets[bullet]);
        }
    }
}

void BulletListUpdate(BulletListCallback callback)
{
    for (int bullet = 0; bullet < BULLET_COUNT; bullet++)
    {
        if (Bullets[bullet].Alive)
        {
            if (BulletUpdate(&Bullets[bullet]))
            {
                Bullets[bullet].Alive = false;
            }
            else if (callback != JO_NULL)
            {
                callback(&Bullets[bullet], Bullets[bullet].Type == BulletPlayerSimple);
            }
        }
    }
}

void BulletListClear(bool onlyEnemy)
{
    for (int bullet = 0; bullet < BULLET_COUNT; bullet++)
    {
        if (!onlyEnemy && Bullets[bullet].Type == BulletPlayerSimple)
        {
            Bullets[bullet].Alive = false;
        }
    }
}

void BulletListClearEnemyBulletsInRange(const jo_pos2D_fixed *pos, const jo_fixed range)
{
    for (int bullet = 0; bullet < BULLET_COUNT; bullet++)
    {
        if (Bullets[bullet].Alive && Bullets[bullet].Type != BulletPlayerSimple)
        {
            jo_vector_fixed fromBullet = {{pos->x - Bullets[bullet].Pos.x, pos->y - Bullets[bullet].Pos.y, 0}};
            jo_fixed distance = ToolsFastVectorLength(&fromBullet);
            
            if (distance < range)
            {
                Bullets[bullet].Alive = false;
            }
        }
    }
}

void BulletListAdd(const jo_pos2D_fixed *pos, const jo_pos2D_fixed *target, const jo_vector2_fixed *velocity, BulletType type)
{
    for (int bullet = 0; bullet < BULLET_COUNT; bullet++)
    {
        if (!Bullets[bullet].Alive)
        {
            Bullets[bullet].Pos.x = pos->x;
            Bullets[bullet].Pos.y = pos->y;
            Bullets[bullet].Velocity.x = velocity->x;
            Bullets[bullet].Velocity.y = velocity->y;

            if (target != JO_NULL)
            {
                Bullets[bullet].Target.x = target->x;
                Bullets[bullet].Target.y = target->y;
            }
            else
            {
                Bullets[bullet].Target.x = pos->x;
                Bullets[bullet].Target.y = pos->y;
            }

            Bullets[bullet].Type = type;
            Bullets[bullet].Alive = true;
            return;
        }
    }
}
