#include <jo/jo.h>
#include "pcmsys.h"
#include "model.h"
#include "font3D.h"
#include "score.h"
#include "player.h"
#include "pickup.h"
#include "tools.h"

// -------------------------------------
// Internal
// -------------------------------------

/* List of all pickups */
static jo_list Pickups;

/* Index of first loaded pickup sprite */
static int FirstSpriteIndex;

/* Sound played when player collets pickup */
static short PickupSound;

/** @brief Collect pickup
 *  @param pickup Pickup to collect
 *  @param player Player data
 */
static int CollectPickup(const Pickup *pickup, Player *player)
{
    switch (pickup->Type)
    {
    case PickupTypeLife:

        if (player->Lives >= 9)
        {
            return 400;
        }
        else
        {
            player->Lives++;
        }

        break;

    case PickupTypeGun:

        if (player->GunLevel >= 2)
        {
            return 25;
        }
        else
        {
            player->GunLevel++;
        }

        break;

    case PickupTypeBomb:

        if (player->Bombs >= 9)
        {
            return 200;
        }
        else
        {
            player->Bombs++;
        }

        break;

    default:
        break;
    }

    pcm_play(PickupSound, PCM_PROTECTED, 6);
    return 0;
}

/** @brief Check pickup against player
 *  @param pickup Pickup to check
 *  @param player Player data
 *  @return True if collected by player
 */
static bool CheckPickupAgainstPlayer(const Pickup *pickup, const Player *player)
{
    jo_vector_fixed fromPickup = {{pickup->Pos.x - player->Pos.x, pickup->Pos.y - player->Pos.y, 0}};
    jo_fixed distance = ToolsFastVectorLength(&fromPickup);
    return distance < JO_FIXED_16;
}

// -------------------------------------
// Public
// -------------------------------------

void PickupInitialize()
{
    jo_list_init(&Pickups);
    PickupSound = load_16bit_pcm((Sint8 *)"PICKUP.PCM", 15360);
    FirstSpriteIndex = jo_sprite_add_tga(JO_ROOT_DIR, "LIFE.TGA", JO_COLOR_Transparent);
    jo_sprite_add_tga(JO_ROOT_DIR, "PUP.TGA", JO_COLOR_Transparent);
    jo_sprite_add_tga(JO_ROOT_DIR, "BMB.TGA", JO_COLOR_Transparent);
}

void PickupCreate(const jo_pos2D_fixed *pos, const PickupType type)
{
    Pickup *pickup = (Pickup *)jo_malloc(sizeof(Pickup));
    pickup->Pos.x = pos->x;
    pickup->Pos.y = pos->y;
    pickup->Velocity.x = 0;
    pickup->Velocity.y = 0;
    pickup->Type = type;

    jo_list_add_ptr(&Pickups, pickup);
}

void PickupCreateWithVelocity(const jo_pos2D_fixed *pos, jo_vector2_fixed *velocity, const PickupType type)
{
    Pickup *pickup = (Pickup *)jo_malloc(sizeof(Pickup));
    pickup->Pos.x = pos->x;
    pickup->Pos.y = pos->y;
    pickup->Velocity.x = velocity->x;
    pickup->Velocity.y = velocity->y;
    pickup->Type = type;

    jo_list_add_ptr(&Pickups, pickup);
}

void PickupClearAll()
{
    jo_list_free_and_clear(&Pickups);
}

int PickupCheckAgainstPlayer(Player *player)
{
    int score = 0;
    jo_node *tmp;

    for (tmp = Pickups.first; tmp != JO_NULL; tmp = tmp->next)
    {
        Pickup *pickup = (Pickup *)tmp->data.ptr;

        if (CheckPickupAgainstPlayer(pickup, player))
        {
            score = CollectPickup(pickup, player);
            jo_list_free_and_remove(&Pickups, tmp);
        }
    }

    return score;
}

void PickupUpdate()
{
    jo_node *tmp;

    for (tmp = Pickups.first; tmp != JO_NULL; tmp = tmp->next)
    {
        Pickup *pickup = (Pickup *)tmp->data.ptr;
        pickup->Pos.x += pickup->Velocity.x + JO_FIXED_1 + JO_FIXED_1_DIV_2;
        pickup->Pos.y += pickup->Velocity.y;

        pickup->Velocity.x = pickup->Velocity.x - (pickup->Velocity.x / 16);
        pickup->Velocity.y = pickup->Velocity.y - (pickup->Velocity.y / 16);

        if ((pickup->Pos.y < PICKUP_DESPAWN_AREA_Y_NEG))
        {
            pickup->Pos.y += PICKUP_DESPAWN_AREA_Y_NEG - pickup->Pos.y;
            pickup->Velocity.y *= -1;
        }
        else if ((pickup->Pos.y > PICKUP_DESPAWN_AREA_Y_POS))
        {
            pickup->Pos.y += PICKUP_DESPAWN_AREA_Y_POS - pickup->Pos.y;
            pickup->Velocity.y *= -1;
        }
        else if ((pickup->Pos.x > PICKUP_DESPAWN_AREA_X) ||
                 (pickup->Pos.x < PICKUP_SPAWN_AREA_X))
        {
            jo_list_free_and_remove(&Pickups, tmp);
        }
    }
}

void PickupDraw()
{
    jo_node *tmp;

    for (tmp = Pickups.first; tmp != JO_NULL; tmp = tmp->next)
    {
        Pickup *pickup = (Pickup *)tmp->data.ptr;

        jo_3d_push_matrix();
        {
            jo_3d_translate_matrix_fixed(pickup->Pos.x, pickup->Pos.y, 0);
            jo_3d_draw_sprite(FirstSpriteIndex + pickup->Type);
        }
        jo_3d_pop_matrix();
    }
}
