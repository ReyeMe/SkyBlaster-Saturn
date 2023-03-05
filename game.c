#include <jo/jo.h>
#include "global_defines.h"
#include "pcmsys.h"
#include "pcmstm.h"
#include "pcmcdda.h"
#include "musicHandler.h"
#include "model.h"
#include "score.h"
#include "background.h"
#include "bullet.h"
#include "bulletList.h"
#include "tools.h"
#include "explosions.h"
#include "player.h"
#include "npc.h"
#include "pickup.h"
#include "gamemode.h"
#include "gamemodeManager.h"
#include "game.h"

// -------------------------------------
// Internal
// -------------------------------------

/* Player shooting sound */
static short PlayerShootSound;

/* Bomb explosion sound */
static short PlayerBombSound;

/* Player dies */
static short PlayerDeadSound;

/* Index of life sprite */
static int LifeSpriteIndex;

/* Index of death sprite */
static int DeathSpriteIndex;

/* Index of bomb sprite */
static int BombSpriteIndex;

/* Index of life icon sprite */
static int LifeIconSpriteIndex;

/* Progress bar */
static int ProgressBar;

/* Player 3D mesh */
static SaturnMesh PlayerMesh;

/* Player 3D mesh */
static SaturnMesh Player2Mesh;

/* Player shield */
static SaturnMesh PlayerShield;

/* Explosion 3D mesh */
static SaturnMesh ExplosionMesh;

/* All present players */
static Player * Players;

/* Session score */
static Score SessionScore;

/* Inidcates whether game state is disposed */
static bool IsDisposed = true;

/* Number of points needed to revive player */
#define REQUIRED_REVIVE_POINTS (2500)

/* Score count state after players death*/
static int BeforeReviveScoreCount = 0;

/* Game mode data */
static GamemodeData Gamemode;

/** @brief Player got hit by a bullet or NPC
 */
static void PlayerHit(Player * player)
{
    if (!player->HurtProtect && player->Lives >= 0)
    {
        player->Lives--;
        ExplosionsSpawn(&player->Pos);

        int bombsToDisperse = player->Bombs / 2;
        player->Bombs -= bombsToDisperse;

        // Drop half the bombs player has collected
        for (int bomb = 0; bomb < bombsToDisperse; bomb++)
        {
            int adder = bomb % 2 == 0 ? JO_FIXED_PI_DIV_2 : 0;
            int angle = (jo_random(6)<<14);

            jo_vector2_fixed velocity = {
                -jo_fixed_sin(adder + angle) << 2,
                -jo_fixed_cos(adder + angle) << 2
            };

            PickupCreateWithVelocity(&player->Pos, &velocity, PickupTypeBomb);
        }

        // Drop players gun
        if (player->GunLevel > 0)
        {
            int angle = (jo_random(12)<<14);

            jo_vector2_fixed velocity = {
                -jo_fixed_sin(angle) << 2,
                -jo_fixed_cos(angle) << 2
            };

            PickupCreateWithVelocity(&player->Pos, &velocity, PickupTypeGun);
            player->GunLevel--;
        }

        player->StateData.IsControllable = false;
        player->StateData.SpawnTick = 1;
        player->HurtProtect = true;
        player->Pos.x = PLAYER_SPAWN_X + JO_FIXED_32;
        
        BeforeReviveScoreCount = SessionScore.Value;

        pcm_play(PlayerDeadSound, PCM_PROTECTED, 6);
    }
}

/** @brief Make that bullets will kill player
 *  @param bullet Bullet to check
 *  @param isPlayer Was bullet shot by player
 */
static void ReadBulletPositions(Bullet *bullet, bool isPlayer)
{
    if (!isPlayer)
    {
        for(int player = 0; player < Gamemode.PlayerCount; player++)
        {
            if (Players[player].Lives >= 0 && !Players[player].HurtProtect)
            {
                jo_vector_fixed fromPlayer = {{bullet->Pos.x - Players[player].Pos.x, bullet->Pos.y - Players[player].Pos.y, 0}};
                jo_fixed distance = ToolsFastVectorLength(&fromPlayer);

                if (distance < JO_FIXED_4)
                {
                    PlayerHit(&Players[player]);
                    break;
                }
            }
        }
    }
}

/** @brief Create player bullets
 */
static void PlayerShoot(Player * player)
{
    pcm_play(PlayerShootSound, PCM_PROTECTED, 6);

    if (player->GunLevel == 0)
    {
        jo_vector2_fixed velocity = { -BULLET_SPEED, 0 };
        BulletListAdd(&player->Pos, JO_NULL, &velocity, BulletPlayerSimple);
    }
    else if (player->GunLevel > 0)
    {
        jo_fixed offset = -JO_FIXED_8;

        for (int i = 0; i < 2; i++)
        {
            jo_vector2_fixed velocity = { -BULLET_SPEED, 0 };
            jo_pos2D_fixed pos = { player->Pos.x, player->Pos.y + offset };
            BulletListAdd(&pos, JO_NULL, &velocity, BulletPlayerSimple);
            offset += JO_FIXED_16;
        }

        if (player->GunLevel > 1)
        {
            for (int i = -1; i < 2; i += 2)
            {
                jo_vector2_fixed velocity = { -(BULLET_SPEED / 2), (BULLET_SPEED / 2) * i };
                BulletListAdd(&player->Pos, JO_NULL, &velocity, BulletPlayerSimple);
            }
        }
    }
}

static void DrawReviveHud(int height)
{
    int reviveCount = SessionScore.Value - BeforeReviveScoreCount;
    int percent = JO_DIV_BY_65536(jo_fixed_mult(jo_fixed_div(JO_MULT_BY_65536(reviveCount), JO_MULT_BY_65536(REQUIRED_REVIVE_POINTS)), JO_MULT_BY_65536(100)));
    
    jo_sprite_draw3D(ProgressBar + 1, 64, height, 100);

    jo_sprite_enable_clipping(false);
    jo_sprite_set_clipping_area(JO_TV_WIDTH_2 + 124 - percent, 0, JO_TV_WIDTH, JO_TV_HEIGHT, 100);
    jo_sprite_draw3D(ProgressBar, 64, height, 100);
    jo_sprite_disable_clipping();
}

/** @brief Draw first players hud
 */
static void DrawPlayer1Hud(const Player * player)
{
    if (player->Lives >= 0)
    {
        for (int live = 0; live <= player->Lives + 1; live++)
        {
            if (live == player->Lives + 1)
            {
                jo_sprite_draw3D(LifeIconSpriteIndex, 140 - (live * 14), -100, 100);
            }
            else
            {
                jo_sprite_draw3D(LifeSpriteIndex, 140 - (live * 14), -100, 100);
            }
        }

        for (int bomb = 1; bomb <= player->Bombs; bomb++)
        {
            jo_sprite_draw3D(BombSpriteIndex, 140 - (bomb * 6), -90, 100);
        }
    }
    else
    {
        DrawReviveHud(-100);
        jo_sprite_draw3D(DeathSpriteIndex, 140, -100, 100);
    }
}

/** @brief Draw second players hud
 */
static void DrawPlayer2Hud(const Player * player)
{
    if (player->Lives >= 0)
    {
        for (int live = 0; live <= player->Lives + 1; live++)
        {
            if (live == player->Lives + 1)
            {
                jo_sprite_draw3D(LifeIconSpriteIndex, 140 - (live * 14), 100, 100);
            }
            else
            {
                jo_sprite_draw3D(LifeSpriteIndex, 140 - (live * 14), 100, 100);
            }
        }

        for (int bomb = 1; bomb <= player->Bombs; bomb++)
        {
            jo_sprite_draw3D(BombSpriteIndex, 140 - (bomb * 6), 90, 100);
        }
    }
    else
    {
        DrawReviveHud(100);
        jo_sprite_draw3D(DeathSpriteIndex, 140, 100, 100);
    }
}

// -------------------------------------
// Public
// -------------------------------------

void GameInitializeImmortal()
{
    // Game sounds, immortal since we cannot unload them atm
    PlayerShootSound = load_16bit_pcm((Sint8 *)"PEW.PCM", 15360);
    PlayerBombSound = load_16bit_pcm((Sint8 *)"BOMB.PCM", 15360);
    PlayerDeadSound = load_16bit_pcm((Sint8 *)"DEAD.PCM", 15360);
    
    // Load UI sprites
    LifeSpriteIndex = jo_sprite_add_tga(JO_ROOT_DIR, "LIFECNT.TGA", JO_COLOR_RGB(255, 0, 255));
    LifeIconSpriteIndex = jo_sprite_add_tga(JO_ROOT_DIR, "LIFEICO.TGA", JO_COLOR_RGB(255, 0, 255));
    BombSpriteIndex = jo_sprite_add_tga(JO_ROOT_DIR, "BMBICO.TGA", JO_COLOR_RGB(255, 0, 255));
    DeathSpriteIndex = jo_sprite_add_tga(JO_ROOT_DIR, "DEADICO.TGA", JO_COLOR_RGB(255, 0, 255));
    ProgressBar = jo_sprite_add_tga(JO_ROOT_DIR, "PRG.TGA", JO_COLOR_Transparent);
    jo_sprite_add_tga(JO_ROOT_DIR, "PRGBG.TGA", JO_COLOR_Transparent);

    // Load player model and bomb explosion model, we can consider these as imortal, since they are the same in every level
    TmfLoadMesh(&PlayerMesh, "PLAYER.TMF", JO_ROOT_DIR);
    TmfLoadMesh(&Player2Mesh, "PLAYER2.TMF", JO_ROOT_DIR);
    TmfLoadMesh(&ExplosionMesh, "EXP.TMF", JO_ROOT_DIR);
    TmfLoadMesh(&PlayerShield, "SHIELD.TMF", JO_ROOT_DIR);

    for (int shield = 0; shield < PlayerShield.MeshCount; shield++)
    {
        jo_3d_set_mesh_screen_doors(&PlayerShield.Meshes[shield], true);
    }
}

void GameInitializeMortal(bool coop, bool endless)
{
    if (IsDisposed)
    {
        Gamemode.CurrentScore = 0;
        Gamemode.PlayerCount = coop ? 2 : 1;
        Gamemode.TickCount = 0;

        BeforeReviveScoreCount = 0;
        ScoreInitialize(&SessionScore);
        Gamemode.Mode = endless ? GmEndless : GmStory;
        Players = (Player*)jo_malloc_with_behaviour(sizeof(Player) * Gamemode.PlayerCount, JO_MALLOC_TRY_REUSE_SAME_BLOCK_SIZE);

        for (int player = 0; player < Gamemode.PlayerCount; player++)
        {
            PlayerInititalize(&(Players[player]));
        }

        if (coop)
        {
            Players[0].Pos.y -= JO_FIXED_16 + JO_FIXED_8;
            Players[1].Pos.y += JO_FIXED_16 + JO_FIXED_8;
        }

        GmStart(&Gamemode);

        IsDisposed = false;
    }
}

void GameDisposeMortal()
{
    if (!IsDisposed)
    {
        IsDisposed = true;    

        GmEnd(&Gamemode);

        for (int player = 0; player < Gamemode.PlayerCount; player++)
        {
            jo_free(Players);
        }
        
        // Clear all bullets, NPCs and entities
        BulletListClear(false);
        NpcClearAll();
        PickupClearAll();
        ExplosionsClearAll();
    }
}

void GameUpdateTick(CurrentState * state)
{
    if (!IsDisposed)
    {
        Gamemode.CurrentScore = SessionScore.Value;
        state->GameEnd = ResultGameOver;
        
        GamemodeTickResult result = GmTick(&Gamemode);

        if (result == GmTickResultGameOver || result == GmTickResultDemoOver)
        {
            if (result == GmTickResultDemoOver)
            {
                state->GameEnd = ResultDemoOver;
            }

            state->Score = SessionScore.Value;
            return;
        }

        for (int player = 0; player < Gamemode.PlayerCount; player++)
        {
            if (Players[player].Lives >= 0)
            {
                state->GameEnd = ResultNone;

                // Check whether player has diead and revive animation played
                if (!Players[player].StateData.IsControllable)
                {
                    Players[player].Pos.x -= JO_FIXED_1;
                    Players[player].StateData.IsControllable = Players[player].Pos.x <= PLAYER_SPAWN_X;
                }
                else
                {
                    // Protect player after death
                    if (Players[player].HurtProtect)
                    {
                        Players[player].StateData.SpawnTick++;

                        if (Players[player].StateData.SpawnTick > 150)
                        {
                            Players[player].HurtProtect = false;
                            Players[player].StateData.SpawnTick = 0;
                        }
                    }

                    PlayerActions action = PlayerUpdate(&Players[player], result != GmTickResultBlockControls ? player : -1);

                    // Do player actions
                    if (action == PlayerActionShoot)
                    {
                        // Shoot
                        PlayerShoot(&Players[player]);
                    }

                    if (action == PlayerActionBomb)
                    {
                        // start effect
                        pcm_play(PlayerBombSound, PCM_PROTECTED, 6);
                        Players[player].StateData.BombEffectLifeTime = 0;
                    }
                }

                // Update bomb effect
                if (Players[player].StateData.BombEffectLifeTime > -1)
                {
                    Players[player].StateData.BombEffectLifeTime++;

                    // Clear all enemy bullets
                    int range = (Players[player].StateData.BombEffectLifeTime << 16) * 8;
                    BulletListClearEnemyBulletsInRange(&Players[player].Pos, range);

                    // Destroy all NPCs in range and add score
                    ScoreAddValue(&SessionScore, NpcDestroyAllInRange(&Players[player].Pos, range));

                    if (Players[player].StateData.BombEffectLifeTime > PLAYER_ACTION_BOMB)
                    {
                        Players[player].StateData.BombEffectLifeTime = -1;
                    }
                }
                
                // Update pickups
                ScoreAddValue(&SessionScore, PickupCheckAgainstPlayer(&Players[player]));
            }
            else if (Players[player].Lives < 0 && (SessionScore.Value - BeforeReviveScoreCount) >= REQUIRED_REVIVE_POINTS)
            {
                Players[player].Lives = 1;
                Players[player].Bombs = 1;
                Players[player].StateData.IsControllable = false;
                Players[player].StateData.SpawnTick = 1;
                Players[player].HurtProtect = true;
                Players[player].Pos.x = PLAYER_SPAWN_X + JO_FIXED_32;
                BeforeReviveScoreCount = SessionScore.Value;
            }
        }

        if (state->GameEnd == ResultNone)
        {
            // Update NPCs
            int collectedScore = NpcUpdate(Players, Gamemode.PlayerCount, PlayerHit);

            if (collectedScore > 0)
            {
                ScoreAddValue(&SessionScore, collectedScore);
            }

            // Update pickups
            PickupUpdate();

            // Update player bullets
            BulletListUpdate(ReadBulletPositions);

            ExplosionsUpdate();

            Gamemode.TickCount++;
        }

        state->Score = SessionScore.Value;
    }
}

void GameScoreDraw()
{
    ScoreDraw(&SessionScore, 60);
}

void GameDraw()
{
    static int shieldRot = 0;
    shieldRot = shieldRot > 360 ? 0 : (shieldRot + 60);

    if (!IsDisposed)
    {
        DrawPlayer1Hud(&Players[0]);

        if (Gamemode.PlayerCount == 2)
        {
            DrawPlayer2Hud(&Players[1]);
        }

        // Entities in world
        jo_3d_push_matrix();
        {
            jo_3d_rotate_matrix(BG_PLACEMENT_X_ANG, BG_PLACEMENT_Y_ANG, BG_PLACEMENT_Z_ANG);
            jo_3d_translate_matrix(0, 0, BG_PLACEMENT_DEPTH);

            // Draw players
            for (int player = 0; player < Gamemode.PlayerCount; player++)
            {
                if (Players[player].Lives >= 0)
                {
                    if (Players[player].HurtProtect)
                    {
                        for (int shield = 0; shield < PlayerShield.MeshCount; shield++)
                        {
                            jo_3d_push_matrix();
                            {
                                jo_3d_translate_matrix_fixed(Players[player].Pos.x, Players[player].Pos.y, 0);
                                jo_3d_rotate_matrix_z(shield == 0 ? shieldRot : -shieldRot);
                                jo_3d_mesh_draw(&PlayerShield.Meshes[shield]);
                            }
                            jo_3d_pop_matrix();
                        }
                    }

                    PlayerDraw(&Players[player], player == 0 ? &PlayerMesh : &Player2Mesh);
                
                    // Draw bomb effect
                    if (Players[player].StateData.BombEffectLifeTime > -1)
                    {
                        jo_3d_push_matrix();
                        {
                            int scale = Players[player].StateData.BombEffectLifeTime << 16;
                            jo_3d_translate_matrix_fixed(Players[player].Pos.x, Players[player].Pos.y, 0);
                            jo_3d_set_scale_fixed(JO_FIXED_1 + scale, JO_FIXED_1 + scale, JO_FIXED_1);
                            TmfDraw(&ExplosionMesh);
                        }
                        jo_3d_pop_matrix();
                    }
                }
            }

            // Update NPCs and entities
            NpcDraw();
            ExplosionsDraw();
            BulletListDraw();
            PickupDraw();
        }
        jo_3d_pop_matrix();
    }
}