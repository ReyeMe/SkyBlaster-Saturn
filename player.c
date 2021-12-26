#include <jo/jo.h>
#include "model.h"
#include "font3D.h"
#include "score.h"
#include "player.h"
#include "bullet.h"

// -------------------------------------
// Internal
// -------------------------------------

/** @brief Internal function to update movement on X axis
 *  @param player Player entity
 *  @param forwardMovement Amount the player moves forward in one frame
 */
void PlayerIntUpdateMovementThrust(Player *player, jo_fixed forwardMovement)
{
    if (player->Pos.x + forwardMovement <= PLAYER_AREA_X_POS &&
        player->Pos.x + forwardMovement >= PLAYER_AREA_X_NEG)
    {
        if (forwardMovement != 0)
        {
            // Air break
            if ((player->Velocity.x < 0 && forwardMovement > 0))
            {
                forwardMovement *= 2;
            }

            player->Velocity.x += forwardMovement;

            if (forwardMovement > 0 && player->PitchAngle < PLAYER_MAX_PITCH)
            {
                player->PitchAngle++;
            }
        }
        else
        {
            if ((player->Velocity.x > 0 && player->Velocity.x - PLAYER_VELOCITY_DRAG < 0) ||
                (player->Velocity.x < 0 && player->Velocity.x + PLAYER_VELOCITY_DRAG > 0))
            {
                player->Velocity.x = 0;
            }
            else if (player->Velocity.x > 0)
            {
                player->Velocity.x -= PLAYER_VELOCITY_DRAG;
            }
            else if (player->Velocity.x < 0)
            {
                player->Velocity.x += PLAYER_VELOCITY_DRAG;
            }

            if (player->PitchAngle < 0)
            {
                player->PitchAngle += 1;
            }
            else if (player->PitchAngle > 0)
            {
                player->PitchAngle -= 1;
            }
        }

        if (player->Velocity.x > PLAYER_VELOCITY_MAX)
        {
            player->Velocity.x = PLAYER_VELOCITY_MAX;
        }
        else if (player->Velocity.x < -PLAYER_VELOCITY_MAX)
        {
            player->Velocity.x = -PLAYER_VELOCITY_MAX;
        }

        player->Pos.x += player->Velocity.x;
    }
    else
    {
        player->Velocity.x = 0;

        if (player->Pos.x < PLAYER_AREA_X_NEG)
        {
            player->Pos.x = PLAYER_AREA_X_NEG;
        }
        else if (player->Pos.x > PLAYER_AREA_X_POS)
        {
            player->Pos.x = PLAYER_AREA_X_POS;
        }
    }
}

/** @brief Internal function to update movement on Y axis
 *  @param player Player entity
 *  @param forwardMovement Amount the player moves to the side in one frame
 */
void PlayerIntUpdateMovementSide(Player *player, jo_fixed sideMovement)
{
    if (player->Pos.y + sideMovement >= PLAYER_AREA_Y_NEG &&
        player->Pos.y + sideMovement <= PLAYER_AREA_Y_POS)
    {
        if (sideMovement != 0)
        {
            // Give movement boost when changing direction rapidly
            if ((player->Velocity.y < 0 && sideMovement > 0) ||
                (player->Velocity.y > 0 && sideMovement < 0))
            {
                sideMovement *= 2;
            }

            // Apply new velocity
            player->Velocity.y += sideMovement;
            player->TiltAngle += sideMovement < 0 ? 1 : -1;
        }
        else
        {
            if ((player->Velocity.y > 0 && player->Velocity.y - PLAYER_VELOCITY_DRAG < 0) ||
                (player->Velocity.y < 0 && player->Velocity.y + PLAYER_VELOCITY_DRAG > 0))
            {
                player->Velocity.y = 0;
            }
            else if (player->Velocity.y > 0)
            {
                player->Velocity.y -= PLAYER_VELOCITY_DRAG;
            }
            else if (player->Velocity.y < 0)
            {
                player->Velocity.y += PLAYER_VELOCITY_DRAG;
            }

            if (player->TiltAngle < 0)
            {
                player->TiltAngle += 1;
            }
            else if (player->TiltAngle > 0)
            {
                player->TiltAngle -= 1;
            }
        }

        if (player->Velocity.y > PLAYER_VELOCITY_MAX)
        {
            player->Velocity.y = PLAYER_VELOCITY_MAX;
        }
        else if (player->Velocity.y < -PLAYER_VELOCITY_MAX)
        {
            player->Velocity.y = -PLAYER_VELOCITY_MAX;
        }

        player->Pos.y += player->Velocity.y;
    }
    else
    {
        player->Velocity.y = 0;

        if (player->Pos.y < PLAYER_AREA_Y_NEG)
        {
            player->Pos.y = PLAYER_AREA_Y_NEG;
        }
        else if (player->Pos.y > PLAYER_AREA_Y_POS)
        {
            player->Pos.y = PLAYER_AREA_Y_POS;
        }
    }
}

// -------------------------------------
// Public
// -------------------------------------

void PlayerInititalize(Player *player)
{
    player->Velocity.x = 0;
    player->Velocity.y = 0;
    player->Velocity.z = 0;

    player->Pos.x = PLAYER_SPAWN_X;
    player->Pos.y = PLAYER_SPAWN_Y;
    player->Pos.z = 0;

    player->TiltAngle = 0;
    player->PitchAngle = 0;
    player->GunLevel = 0;
    player->Lives = 2;
    player->Bombs = 2;
    player->HurtProtect = true;

    ScoreInitialize(&player->Score);

    for (int action = 0; action < PlayerActionCount - 1; action++)
    {
        player->ActionCooldown[action] = 0;
    }
}

void PlayerDraw(Player *player, jo_3d_mesh *mesh)
{
    static int counter = 0;
    static bool lastStatus = false;

    if ((counter > 20 && player->HurtProtect) || (!player->HurtProtect && !lastStatus))
    {
        counter = 0;

        for (int i = 0; i < PLAYER_MESH_COUNT; i++)
        {
            jo_3d_set_mesh_screen_doors(&mesh[i], lastStatus);
        }

        lastStatus = !lastStatus;
    }

    jo_3d_push_matrix();
    {
        // maximal Y range (x transform) -50; minimal Y range (x transform) -10
        // ship X range back 40 (y transform)
        jo_3d_translate_matrix_fixed(player->Pos.x, player->Pos.y, 0);

        if (player->TiltAngle != 0)
            jo_3d_rotate_matrix_x(player->TiltAngle);
        if (player->PitchAngle != 0)
            jo_3d_rotate_matrix_y(player->PitchAngle);

        for (int i = 0; i < PLAYER_MESH_COUNT; i++)
        {
            jo_3d_mesh_draw(&mesh[i]);
        }
    }
    jo_3d_pop_matrix();

    counter++;
}

PlayerActions PlayerUpdate(Player *player, int inputDeviceId)
{
    PlayerActions action = PlayerActionNone;

    // Cooldown countdown
    for (int actionIndex = 0; actionIndex < PlayerActionCount - 1; actionIndex++)
    {
        if (player->ActionCooldown[actionIndex] > 0)
        {
            player->ActionCooldown[actionIndex]--;
        }
    }

    // Handle controller input
    if (jo_is_input_available(inputDeviceId))
    {
        jo_fixed forwardMovement = 0;
        jo_fixed sideMovement = 0;

        if (jo_is_input_key_pressed(inputDeviceId, JO_KEY_LEFT))
        {
            forwardMovement += PLAYER_VELOCITY_BACK_ACCEL;
        }
        else if (jo_is_input_key_pressed(inputDeviceId, JO_KEY_RIGHT))
        {
            forwardMovement -= PLAYER_VELOCITY_ACCEL;
        }

        if (jo_is_input_key_pressed(inputDeviceId, JO_KEY_UP))
        {
            sideMovement -= PLAYER_VELOCITY_SIDE_ACCEL;
        }
        else if (jo_is_input_key_pressed(inputDeviceId, JO_KEY_DOWN))
        {
            sideMovement += PLAYER_VELOCITY_SIDE_ACCEL;
        }

        if (jo_is_input_key_pressed(inputDeviceId, JO_KEY_A) &&
            player->ActionCooldown[PlayerActionShoot - 1] <= 0)
        {
            action = PlayerActionShoot;
            player->ActionCooldown[PlayerActionShoot - 1] = PLAYER_ACTION_SHOOT;
        }
        else if (jo_is_input_key_down(inputDeviceId, JO_KEY_B) &&
                 player->Bombs > 0 &&
                 player->ActionCooldown[PlayerActionBomb - 1] <= 0)
        {
            action = PlayerActionBomb;

            // Take cost of a bomb from score and set cooldown timer
            player->Bombs--;
            player->ActionCooldown[PlayerActionBomb - 1] = PLAYER_ACTION_BOMB;
        }

        // Update movement
        PlayerIntUpdateMovementSide(player, sideMovement);
        PlayerIntUpdateMovementThrust(player, forwardMovement);

        // Clamp side movement animation
        if (player->TiltAngle >= PLAYER_MAX_TILT)
        {
            player->TiltAngle = PLAYER_MAX_TILT;
        }
        else if (player->TiltAngle <= -PLAYER_MAX_TILT)
        {
            player->TiltAngle = -PLAYER_MAX_TILT;
        }
    }

    return action;
}
