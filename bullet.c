#include <jo/jo.h>
#include "tools.h"
#include "bullet.h"

// -------------------------------------
// Bullets
// -------------------------------------

static void BulletSimpleBehaviour(Bullet * bullet)
{
    bullet->Pos.x += bullet->Velocity.x;
    bullet->Pos.y += bullet->Velocity.y;
}

static void BulletHomingBehaviour(Bullet * bullet)
{
    if ((unsigned int)bullet->Target.x == 0xffffffff && (unsigned int)bullet->Target.y == 0xffffffff)
    {
        BulletSimpleBehaviour(bullet);
        return;
    }

    static jo_fixed max = JO_FIXED_1 >> 4;
    
    jo_vector_fixed velocity = {{
        bullet->Velocity.x,
        bullet->Velocity.y,
        0
    }};

    jo_fixed velocitiSize = ToolsFastVectorLength(&velocity);

    jo_vector_fixed oldDiff = {{
        bullet->Target.x - bullet->Pos.x,
        bullet->Target.y - bullet->Pos.y,
        0
    }};

    jo_fixed length = ToolsFastVectorLength(&oldDiff);
    
    if (length < JO_FIXED_4)
    {
        bullet->Target.x = 0xffffffff;
        bullet->Target.y = 0xffffffff;
        return;
    }

    oldDiff.x = jo_fixed_div(oldDiff.x, length);
    oldDiff.y = jo_fixed_div(oldDiff.y, length);

    bullet->Pos.x += bullet->Velocity.x;
    bullet->Pos.y += bullet->Velocity.y;

    jo_vector_fixed diff = {{
        bullet->Target.x - bullet->Pos.x,
        bullet->Target.y - bullet->Pos.y,
        0
    }};

    length = ToolsFastVectorLength(&diff);
    diff.x = jo_fixed_div(diff.x, length);
    diff.y = jo_fixed_div(diff.y, length);

    jo_fixed deltaX = oldDiff.x - diff.x;
    jo_fixed deltaY = oldDiff.y - diff.y;

    if (deltaX > max)
    {
        deltaX = max;
    }
    else if (deltaX < -max)
    {
        deltaX = -max;
    }
    
    if (deltaY > max)
    {
        deltaY = max;
    }
    else if (deltaY < -max)
    {
        deltaY = -max;
    }

    oldDiff.x += oldDiff.x + deltaX;
    oldDiff.y += oldDiff.y + deltaY;
    diff.x = jo_fixed_div(oldDiff.x, velocitiSize);
    diff.y = jo_fixed_div(oldDiff.y, velocitiSize);

    bullet->Velocity.x = oldDiff.x;
    bullet->Velocity.y = oldDiff.y;
}

// -------------------------------------
// Internal
// -------------------------------------

/** @brief Index of first bullet sprite
 */
static int FirstSpriteIndex = 0;

// -------------------------------------
// Public
// -------------------------------------

void BulletInitializeMesh(Bullet * bullet)
{
    CreateSpriteQuad(&bullet->Mesh, FirstSpriteIndex + JO_MIN(bullet->Type, 1));
}

void BulletLoadAssets()
{
    FirstSpriteIndex = jo_sprite_add_tga(JO_ROOT_DIR, "PB.TGA", JO_COLOR_Black);
    jo_sprite_add_tga(JO_ROOT_DIR, "BSM.TGA", JO_COLOR_Black);
}

void BulletDraw(Bullet *bullet)
{
    jo_3d_push_matrix();
    {
        // ship X range back 40 (y transform)
        jo_3d_translate_matrix_fixed(bullet->Pos.x, bullet->Pos.y, 0);
        jo_3d_draw(&bullet->Mesh);
    }
    jo_3d_pop_matrix();
}

bool BulletUpdate(Bullet *bullet)
{
    jo_3d_set_texture(&bullet->Mesh, FirstSpriteIndex + JO_MIN(bullet->Type, 1));

    switch (bullet->Type)
    {
        case BulletPlayerSimple:
        case BulletNpcSimple:
            BulletSimpleBehaviour(bullet);
            break;

        case BulletHoming:
            BulletHomingBehaviour(bullet);
            break;

        default:
            break;
    }

    // Maximal Y range (x transform) -50; minimal Y range (x transform) -10
    return (bullet->Pos.x > BULLET_DESPAWN_AREA_X) ||
           (bullet->Pos.x < BULLET_SPAWN_AREA_X) ||
           (bullet->Pos.y > BULLET_DESPAWN_AREA_Y_POS) ||
           (bullet->Pos.y < BULLET_DESPAWN_AREA_Y_NEG);
}
