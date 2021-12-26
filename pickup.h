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
void PickupCreate(const jo_pos3D_fixed *pos, const PickupType type);

/** @brief Clear all pickups from screen
 */
void PickupClearAll();

/** @brief Update pickups on screen
 *  @param player Current player
 */
void PickupUpdate(Player *player);

/** @brief Draw pickups on screen
 */
void PickupDraw();

#endif
