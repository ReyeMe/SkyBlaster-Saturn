#ifndef __GAME_H__
#define __GAME_H__

/** @brief Current game state
 */
typedef struct
{
    int Score;
    bool GameEnd;
} CurrentState;


/** @brief Loads and initializes assets that are present through the whole game
 */
void GameInitializeImmortal();

/** @brief Loads and initializes assets that are present only in levels
 * @param coop Number of players present in game
 * @param endless Game mode
 */
void GameInitializeMortal(bool coop, bool endless);

/** @brief Dispose of game assets
 */
void GameDisposeMortal();

/** @brief Game update tick
 */
void GameUpdateTick(CurrentState * state);

/** @brief Draw score to global hud
 */
void GameScoreDraw();

/** @brief Draw game screen
 */
void GameDraw();

#endif
