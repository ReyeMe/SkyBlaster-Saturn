#ifndef __GAMEMODE_H__
#define __GAMEMODE_H__

/** @brief Game mode type
 */
typedef enum Gamemodes
{
    GmStory,
    GmEndless
} Gamemodes;

/** @brief End game result
 */
typedef enum GamemodeTickResult
{
    GmTickResultNone,
    GmTickResultGameOver,
    GmTickResultDemoOver,
    GmTickResultBlockControls
} GamemodeTickResult;

/** @brief Game mode data
 */
typedef struct
{
    Gamemodes Mode;
    int CurrentScore;
    int PlayerCount;
    int TickCount;
} GamemodeData;

#endif
