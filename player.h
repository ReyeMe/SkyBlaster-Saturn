#ifndef __PLAYER_H__
#define __PLAYER_H__

// -------------------------------------
// Game area bounds
// -------------------------------------

#define PLAYER_SPAWN_X (9830400)
#define PLAYER_SPAWN_Y (0)
#define PLAYER_AREA_X_NEG (-9830400) // 150.00
#define PLAYER_AREA_X_POS (9830400)  // 150.00
#define PLAYER_AREA_Y_NEG (-5242880) // 80.00
#define PLAYER_AREA_Y_POS (5242880)  // 80.00

// -------------------------------------
// Player movement
// -------------------------------------

#define PLAYER_VELOCITY_MAX (100000)
#define PLAYER_VELOCITY_DRAG (3000)
#define PLAYER_VELOCITY_ACCEL (3650)
#define PLAYER_VELOCITY_BACK_ACCEL (4500)
#define PLAYER_VELOCITY_SIDE_ACCEL (2800)

// -------------------------------------
// Player action cooldown
// -------------------------------------

#define PLAYER_ACTION_SHOOT (20)
#define PLAYER_ACTION_BOMB (120)

// -------------------------------------
// Other player info
// -------------------------------------

#define PLAYER_BOMB_COST (10000)
#define PLAYER_MAX_TILT (16)
#define PLAYER_MAX_PITCH (12)

/** @brief Available player actions
 */
typedef enum
{
    /* Player did nothing */
    PlayerActionNone = 0,

    /* Player is shooting */
    PlayerActionShoot = 1,

    /* Player used the bomb */
    PlayerActionBomb = 2,

    /* Number of actions, MUST be always last in enum */
    PlayerActionCount
} PlayerActions;

typedef struct
{
    int BombEffectLifeTime;
    int SpawnTick;
    bool IsControllable;
} PlayerStateData;


/** @brief Player entity data
 */
typedef struct
{
    /* Current player position */
    jo_pos3D_fixed Pos;

    /* Current player velocity */
    jo_vector_fixed Velocity;

    /* Tilt angle when player moves to the side */
    int TiltAngle;

    /* Pitch angle when player moves back */
    int PitchAngle;

    /* What level the pew pew is at */
    int GunLevel;

    /* Player lives */
    int Lives;

    /* How many bombs player has */
    int Bombs;

    /* Cooldown for player actions (shoot, bomb, etc..) */
    int ActionCooldown[PlayerActionCount - 1];

    /* Protect player against getting hurt */
    bool HurtProtect;

    /* Player state data */
    PlayerStateData StateData;
} Player;

/** @brief Initialize player entity data
 *  @param player Player entity data
 */
void PlayerInititalize(Player *player);

/** @brief Draw player entity
 *  @param player Player entity data
 *  @param mesh Player mesh
 */
void PlayerDraw(Player *player, SaturnMesh *mesh);

/** @brief Process controller input for player and player logic
 *  @param player player entity data
 *  @param input ID of a controller to use
 *  @return Player action
 */
PlayerActions PlayerUpdate(Player *player, int input);

#endif
