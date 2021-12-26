#include <jo/jo.h>
#include "bullet.h"

// -------------------------------------
// Internal
// -------------------------------------

/** @brief Index of first bullet sprite
 */
static int FirstSpriteIndex = 0;

// -------------------------------------
// Public
// -------------------------------------

void BulletLoadAssets()
{
    FirstSpriteIndex = jo_sprite_add_tga(JO_ROOT_DIR, "PB.TGA", JO_COLOR_Black);
    jo_sprite_add_tga(JO_ROOT_DIR, "BSM.TGA", JO_COLOR_Black);
    jo_sprite_add_tga(JO_ROOT_DIR, "BBG.TGA", JO_COLOR_Black);
}

void BulletDraw(Bullet * bullet)
{
    jo_3d_push_matrix();
	{
        // ship X range back 40 (y transform)
        jo_3d_translate_matrix_fixed(bullet->Pos.x, bullet->Pos.y, 0);
        
        jo_3d_draw_sprite(FirstSpriteIndex + bullet->Type);
	}
	jo_3d_pop_matrix();
}

bool BulletUpdate(Bullet * bullet)
{
    bullet->Pos.x += bullet->Velocity.x;
    bullet->Pos.y += bullet->Velocity.y;

    // Maximal Y range (x transform) -50; minimal Y range (x transform) -10
    return (bullet->Pos.x > BULLET_DESPAWN_AREA_X) || 
        (bullet->Pos.x < BULLET_SPAWN_AREA_X) ||
        (bullet->Pos.y > BULLET_DESPAWN_AREA_Y_POS) || 
        (bullet->Pos.y < BULLET_DESPAWN_AREA_Y_NEG);
}
