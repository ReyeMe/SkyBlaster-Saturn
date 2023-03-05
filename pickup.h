#ifndef __PICKUP_H__
#define __PICKUP_H__

#define PICKUP_SPAWN_AREA_X (-13107200)
#define PICKUP_DESPAWN_AREA_X (13107200)
#define PICKUP_DESPAWN_AREA_Y_NEG (-5898240) // 90.00
#define PICKUP_DESPAWN_AREA_Y_POS (5898240)  // 90.00

/** @brief Available pickup types
 */
typedef enum
{
    PickupTypeLife = 0,
    PickupTypeGun = 1,
    PickupTypeBomb = 2
} PickupType;

/** @brief Pickup data
 */
typedef struct
{
    /* Pickup location */
    jo_pos2D_fixed Pos;

    /* Pickup velocity */
    jo_vector2_fixed Velocity;

    /* Pickup type */
    PickupType Type;
} Pickup;

/** @brief Initialize pickup data
 */
void PickupInitialize();

/** @brief Create pickup
 *  @param pos Pickup location
 *  @param type Pickup type
 */
void PickupCreate(const jo_pos2D_fixed *pos, const PickupType type);

/** @brief Create pickup
 *  @param pos Pickup location
 *  @param velocity Pickup velocity
 *  @param type Pickup type
 */
void PickupCreateWithVelocity(const jo_pos2D_fixed *pos, jo_vector2_fixed *velocity, const PickupType type);

/** @brief Clear all pickups from screen
 */
void PickupClearAll();

/** @brief Check whether player has picked up something
 *  @param player Current player
 *  @return Collected score
 */
int PickupCheckAgainstPlayer(Player *player);

/** @brief Update pickups on screen
 */
void PickupUpdate();

/** @brief Draw pickups on screen
 */
void PickupDraw();

#endif
