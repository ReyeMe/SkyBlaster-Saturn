#include <jo/jo.h>
#include "main.h"
#include "game.h"

// -------------------------------------
// Internal
// -------------------------------------

/* Player shooting sound */
static short PlayerShootSound;

/* Index of life sprite */
static int LifeSpriteIndex;

/* Index of death sprite */
static int DeathSpriteIndex;

/* Index of bomb sprite */
static int BombSpriteIndex;

/* Index of life icon sprite */
static int LifeIconSpriteIndex;

/* Player 3D mesh */
static SaturnMesh PlayerMesh;

/* Player 3D mesh */
static SaturnMesh Player2Mesh;

/* Player shield */
static SaturnMesh PlayerShield;

/* Explosion 3D mesh */
static SaturnMesh ExplosionMesh;

/* Number of players in game */
static int PlayerCount;

/* All present players */
static Player * Players;

/* Session score */
static Score SessionScore;

/* Inidcates whether game state is disposed */
static bool IsDisposed = true;

/* Current level tick */
static int CurrentLevelTick;

/* Are we playing endless mode */
static bool IsEndless;

/** @brief Player got hit by a bullet or NPC
 */
void PlayerHit(Player * player)
{
    if (!player->HurtProtect)
    {
        player->Lives--;
        NpcSpawnExplosion(&player->Pos);

        if (player->Lives >= 0)
        {
            int bombsToDisperse = player->Bombs / 2;
            player->Bombs -= bombsToDisperse;

            // Drop half the bombs player has collected
            for (int bomb = 0; bomb < bombsToDisperse; bomb++)
            {
                int adder = bomb % 2 == 0 ? JO_FIXED_PI_DIV_2 : 0;
                int angle = (jo_random(6)<<14);

                jo_vector_fixed velocity = {
                    {
                        -jo_fixed_sin(adder + angle) << 2,
                        -jo_fixed_cos(adder + angle) << 2,
                        0
                    }
                };

                PickupCreateWithVelocity(&player->Pos, &velocity, PickupTypeBomb);
            }

            // Drop players gun
            if (player->GunLevel > 0)
            {
                int angle = (jo_random(12)<<14);

                jo_vector_fixed velocity = {
                    {
                        -jo_fixed_sin(angle) << 2,
                        -jo_fixed_cos(angle) << 2,
                        0
                    }
                };

                PickupCreateWithVelocity(&player->Pos, &velocity, PickupTypeGun);
                player->GunLevel--;
            }

            player->StateData.IsControllable = false;
            player->StateData.SpawnTick = 1;
            player->HurtProtect = true;
            player->Pos.x = PLAYER_SPAWN_X + JO_FIXED_32;
        }
    }
}

/** @brief Make that bullets will kill player
 *  @param bullet Bullet to check
 *  @param isPlayer Was bullet shot by player
 */
void ReadBulletPositions(Bullet *bullet, bool isPlayer)
{
    if (!isPlayer)
    {
        for(int p = 0; p < PlayerCount; p++)
        {
            if (Players[p].Lives >= 0 && !Players[p].HurtProtect)
            {
                jo_vector_fixed fromPlayer = {{bullet->Pos.x - Players[p].Pos.x, bullet->Pos.y - Players[p].Pos.y, 0}};
                jo_fixed distance = ToolsFastVectorLength(&fromPlayer);

                if (distance < JO_FIXED_4)
                {
                    PlayerHit(&Players[p]);
                    break;
                }
            }
        }
    }
}

/** @brief Create player bullets
 */
void PlayerShoot(Player * player)
{
    pcm_play(PlayerShootSound, PCM_PROTECTED, 6);

    if (player->GunLevel == 0)
    {
        Bullet *bullet = jo_malloc_with_behaviour(sizeof(Bullet), JO_MALLOC_TRY_REUSE_SAME_BLOCK_SIZE);
        bullet->Pos.x = player->Pos.x;
        bullet->Pos.y = player->Pos.y;
        bullet->Type = 0;
        bullet->Velocity.x = -BULLET_SPEED;
        bullet->Velocity.y = 0;
        BulletInitializeMesh(bullet);
        BulletListAdd(bullet, true);
    }
    else if (player->GunLevel > 0)
    {
        jo_fixed offset = -JO_FIXED_8;

        for (int i = 0; i < 2; i++)
        {
            Bullet *bullet = jo_malloc_with_behaviour(sizeof(Bullet), JO_MALLOC_TRY_REUSE_SAME_BLOCK_SIZE);
            bullet->Pos.x = player->Pos.x;
            bullet->Pos.y = player->Pos.y + offset;
            bullet->Type = 0;
            bullet->Velocity.x = -BULLET_SPEED;
            bullet->Velocity.y = 0;
            BulletInitializeMesh(bullet);
            BulletListAdd(bullet, true);

            offset += JO_FIXED_16;
        }

        if (player->GunLevel > 1)
        {
            for (int i = -1; i < 2; i += 2)
            {
                Bullet *bullet = jo_malloc_with_behaviour(sizeof(Bullet), JO_MALLOC_TRY_REUSE_SAME_BLOCK_SIZE);
                bullet->Pos.x = player->Pos.x;
                bullet->Pos.y = player->Pos.y;
                bullet->Type = 0;
                bullet->Velocity.x = -(BULLET_SPEED / 2);
                bullet->Velocity.y = (BULLET_SPEED / 2) * i;
                BulletInitializeMesh(bullet);
                BulletListAdd(bullet, true);
            }
        }
    }
}

/** @brief Draw first players hud
 */
void DrawPlayer1Hud(const Player * player)
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
        jo_sprite_draw3D(DeathSpriteIndex, 140, -100, 100);
    }
}

/** @brief Draw second players hud
 */
void DrawPlayer2Hud(const Player * player)
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
        jo_sprite_draw3D(DeathSpriteIndex, 140, 100, 100);
    }
}

/** @brief Endless mode
 */
void EndlessModeLoop()
{
    static int spawnTimer = 0;
    static int nextSpawn = 40;

    spawnTimer++;

    if (nextSpawn < spawnTimer && CurrentLevelTick >= 350)
    {
        int spawnCount = jo_random(3);

        for (int i = 0; i < spawnCount; i++)
        {
            jo_pos3D_fixed pos = {NPC_SPAWN_X, jo_int2fixed((jo_random(10) - 5) << 4), 0};
            NpcCreate(jo_random(4) - 1, &pos);

            nextSpawn = 40 + jo_random(50);
            spawnTimer = 0;
        }
    }
}

/** @brief Story mode
 */
void StoryModeLoop()
{
    
}

// -------------------------------------
// Public
// -------------------------------------

void GameInitializeImmortal()
{
    // Game sounds, immortal since we cannot unload them atm
    PlayerShootSound = load_16bit_pcm((Sint8 *)"PEW.PCM", 15360);
    
    // Load UI sprites
    LifeSpriteIndex = jo_sprite_add_tga(JO_ROOT_DIR, "LIFECNT.TGA", JO_COLOR_RGB(255, 0, 255));
    LifeIconSpriteIndex = jo_sprite_add_tga(JO_ROOT_DIR, "LIFEICO.TGA", JO_COLOR_RGB(255, 0, 255));
    BombSpriteIndex = jo_sprite_add_tga(JO_ROOT_DIR, "BMBICO.TGA", JO_COLOR_RGB(255, 0, 255));
    DeathSpriteIndex = jo_sprite_add_tga(JO_ROOT_DIR, "DEADICO.TGA", JO_COLOR_RGB(255, 0, 255));

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
        CurrentLevelTick = 0;
        ScoreInitialize(&SessionScore);
        PlayerCount = coop ? 2 : 1;
        IsEndless = endless;
        Players = (Player*)jo_malloc_with_behaviour(sizeof(Player) * PlayerCount, JO_MALLOC_TRY_REUSE_SAME_BLOCK_SIZE);

        for (int player = 0; player < PlayerCount; player++)
        {
            PlayerInititalize(&(Players[player]));
        }

        if (coop)
        {
            Players[0].Pos.y -= JO_FIXED_16 + JO_FIXED_8;
            Players[1].Pos.y += JO_FIXED_16 + JO_FIXED_8;
        }

        IsDisposed = false;
    }
}

void GameDisposeMortal()
{
    if (!IsDisposed)
    {
        IsDisposed = true;    

        for (int player = 0; player < PlayerCount; player++)
        {
            jo_free(Players);
        }
        
        // Clear all bullets, NPCs and entities
        BulletListClear(false);
        NpcClearAll();
        PickupClearAll();
    }
}

void GameUpdateTick(CurrentState * state)
{
    if (!IsDisposed)
    {
        state->GameEnd = true;

        for (int player = 0; player < PlayerCount; player++)
        {
            if (Players[player].Lives >= 0)
            {
                state->GameEnd = false;

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

                    PlayerActions action = PlayerUpdate(&Players[player], player);

                    // Do player actions
                    if (action == PlayerActionShoot)
                    {
                        // Shoot
                        PlayerShoot(&Players[player]);
                    }

                    if (action == PlayerActionBomb)
                    {
                        // start effect
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
                PickupCheckAgainstPlayer(&Players[player]);
            }
        }

        // Debug random enemy generator
        if (IsEndless)
        {
            EndlessModeLoop();
        }
        else
        {
            StoryModeLoop();
        }

        // Update NPCs
        int collectedScore = NpcUpdate(Players, PlayerCount, PlayerHit);

        if (collectedScore > 0)
        {
            ScoreAddValue(&SessionScore, collectedScore);
        }

        // Update pickups
        PickupUpdate();

        // Update player bullets
        BulletListUpdate(ReadBulletPositions);

        NpcUpdateExplosions();

        state->Score = SessionScore.Value;
        CurrentLevelTick++;
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

        if (PlayerCount == 2)
        {
            DrawPlayer2Hud(&Players[1]);
        }

        // Entities in world
        jo_3d_push_matrix();
        {
            jo_3d_rotate_matrix(BG_PLACEMENT_X_ANG, BG_PLACEMENT_Y_ANG, BG_PLACEMENT_Z_ANG);
            jo_3d_translate_matrix(0, 0, BG_PLACEMENT_DEPTH);

            // Draw players
            for (int player = 0; player < PlayerCount; player++)
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
            BulletListDraw();
            PickupDraw();
        }
        jo_3d_pop_matrix();
    }
}
