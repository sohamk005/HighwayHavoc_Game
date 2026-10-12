#define GLM_ENABLE_EXPERIMENTAL

#include "gameLayer.h"

#include <glad/glad.h>
#include <gl2d/gl2d.h>
#include <glm/glm.hpp>

#include "platformInput.h"
#include <platformTools.h>

#include "assetManager.h"

#include "road.h"

#include "grass.h"

#include "obstacleManager.h"
#include "trafficManager.h"
#include "coinManager.h"
#include "player.h"
#include "collision.h"
#include "scoreSystem.h"
#include "gameState.h"
#include "ui/uiTheme.h"
#include "ui/hud.h"
#include "ui/menu.h"
#include "audioManager.h"
#include "saveSystem.h"

AssetManager assets;
AudioManager audioManager;
SaveSystem saveSystem;
gl2d::Renderer2D renderer;
Road road;
Grass grass;
ObstacleManager obstacleManager;
TrafficManager trafficManager;
CoinManager coinManager;

//=========================================================
// Game World --
//=========================================================

struct GameWorld
{
    float scrollSpeed = 0.f;
    float maxScrollSpeed = 600.f;

    float acceleration = 100.f;
    float brakePower = 900.f;

    float scrollOffset = 0.f;
};

//=========================================================
// Game Data
//=========================================================

struct GameData
{
    Player player{};
    GameWorld world;
    ScoreSystem score;
    uint32_t coinCount = 0;
    uint64_t currentRunId = 1;
    GameState state = GameState::MainMenu;
};

GameData game;

//=========================================================
// Reset Gameplay Foundation
//=========================================================

void resetGame()
{
    // V2-M2: Bank unbanked coins from the completed/abandoned run exactly once
    saveSystem.bankRun(game.currentRunId, game.coinCount);
    game.coinCount = 0;
    game.currentRunId++;

    int w = platform::getFrameBufferSizeX();
    int h = platform::getFrameBufferSizeY();

    road.update(w);

    float startX = road.getLaneCenter(1) - game.player.size.x / 2.f;
    float startY = (float)h - game.player.size.y - 40.f;

    game.player.reset(startX, startY);

    game.world.scrollSpeed = 0.f;
    game.world.scrollOffset = 0.f;

    // Reset current run score & difficulty; session high score is preserved!
    game.score.resetRun();
    game.world.maxScrollSpeed = game.score.getMaxScrollSpeed();

    obstacleManager.reset();
    trafficManager.reset();
    coinManager.reset();

    ilog("Run Reset. High Score:", game.score.highScore, "| Total Wallet:", saveSystem.getCachedWalletBalance());
}

void triggerGameOver()
{
    if (game.state == GameState::GameOver)
        return;

    game.state = GameState::GameOver;
    game.world.scrollSpeed = 0.f;
    audioManager.stopMusic();
    audioManager.playDeathSound();
    saveSystem.saveHighScoreIfHigher(game.score.highScore);
    saveSystem.bankRun(game.currentRunId, game.coinCount);
    ilog("GAME OVER! Final Score:", game.score.currentScore,
         "| High Score:", game.score.highScore,
         "| Run Coins:", game.coinCount,
         "| Banked Wallet:", saveSystem.getCachedWalletBalance());
}

//=========================================================
// Initialization
//=========================================================

bool initGame()
{
    gl2d::init();

    renderer.create();

    assets.loadAssets();
    audioManager.init();
    audioManager.loadAssets();

    coinManager.init();

    saveSystem.init();
    uint32_t loadedHighScore = saveSystem.loadHighScore();
    game.score.highScore = loadedHighScore;
    saveSystem.setCachedHighScore(loadedHighScore);

    uint32_t loadedWallet = saveSystem.loadWallet();
    game.currentRunId = saveSystem.getLastBankedRunId() + 1;

    game.player.texture = &assets.blueCar;

    resetGame();
    game.state = GameState::MainMenu;
    audioManager.playMenuMusic();

    ilog("Highway Havoc Initialized. State: MainMenu (Press ENTER or SPACE to start) | High Score:", loadedHighScore, "| Wallet:", loadedWallet);

    return true;
}

//=========================================================
// Main Game Loop
//=========================================================

bool gameLogic(float deltaTime)
{
    // Sanity clamp delta time [0.0f, 0.1f] against clock shifts, NaN, or frame lag spikes
    if (std::isnan(deltaTime) || deltaTime < 0.0f)
    {
        deltaTime = 0.0f;
    }
    else if (deltaTime > 0.1f)
    {
        deltaTime = 0.1f;
    }

    int w = platform::getFrameBufferSizeX();
    int h = platform::getFrameBufferSizeY();

    glViewport(0, 0, w, h);
    glClear(GL_COLOR_BUFFER_BIT);

    renderer.updateWindowMetrics(w, h);

    // Audio stream buffer update (must be called every frame for active music streaming)
    audioManager.update();

    // State machine input handling & transitions
    switch (game.state)
    {
    case GameState::MainMenu:
    {
        if (platform::isButtonPressedOn(platform::Button::Enter) ||
            platform::isButtonPressedOn(platform::Button::Space))
        {
            audioManager.playBeepSound();
            resetGame();
            game.state = GameState::Playing;
            audioManager.playGameplayMusic(true);
            ilog("State Transition: MainMenu -> Playing (Game Started)");
        }
        break;
    }
    case GameState::Playing:
    {
        if (platform::isButtonPressedOn(platform::Button::P))
        {
            audioManager.playBeepSound();
            game.state = GameState::Paused;
            audioManager.pauseGameplayMusic();
            ilog("State Transition: Playing -> Paused");
        }
        else if (platform::isButtonPressedOn(platform::Button::R))
        {
            audioManager.playBeepSound();
            saveSystem.saveHighScoreIfHigher(game.score.highScore);
            resetGame();
            game.state = GameState::Playing;
            audioManager.playGameplayMusic(true);
        }
        break;
    }
    case GameState::Paused:
    {
        if (platform::isButtonPressedOn(platform::Button::P))
        {
            audioManager.playBeepSound();
            game.state = GameState::Playing;
            audioManager.resumeGameplayMusic();
            ilog("State Transition: Paused -> Playing (Resumed)");
        }
        else if (platform::isButtonPressedOn(platform::Button::R))
        {
            audioManager.playBeepSound();
            saveSystem.saveHighScoreIfHigher(game.score.highScore);
            resetGame();
            game.state = GameState::Playing;
            audioManager.playGameplayMusic(true);
            ilog("State Transition: Paused -> Playing (Restarted)");
        }
        else if (platform::isButtonPressedOn(platform::Button::Escape))
        {
            audioManager.playBeepSound();
            saveSystem.saveHighScoreIfHigher(game.score.highScore);
            resetGame();
            game.state = GameState::MainMenu;
            audioManager.playMenuMusic(true);
            ilog("State Transition: Paused -> MainMenu (Title Screen)");
        }
        break;
    }
    case GameState::GameOver:
    {
        if (platform::isButtonPressedOn(platform::Button::R))
        {
            audioManager.playBeepSound();
            saveSystem.saveHighScoreIfHigher(game.score.highScore);
            resetGame();
            game.state = GameState::Playing;
            audioManager.playGameplayMusic(true);
            ilog("State Transition: GameOver -> Playing (Restarted)");
        }
        else if (platform::isButtonPressedOn(platform::Button::Escape))
        {
            audioManager.playBeepSound();
            saveSystem.saveHighScoreIfHigher(game.score.highScore);
            resetGame();
            game.state = GameState::MainMenu;
            audioManager.playMenuMusic(true);
            ilog("State Transition: GameOver -> MainMenu (Title Screen)");
        }
        break;
    }
    }

    road.update(w);

    constexpr float ROAD_MARGIN = 12.f;

    // Active simulation: executes ONLY when in Playing state
    if (game.state == GameState::Playing)
    {
        // 1. Update player invulnerability cooldown
        game.player.update(deltaTime);

        // 2. Player Movement
        if (platform::isButtonHeld(platform::Button::W))
            game.player.position.y -= game.player.speed * deltaTime;

        if (platform::isButtonHeld(platform::Button::S))
            game.player.position.y += game.player.speed * deltaTime;

        if (platform::isButtonHeld(platform::Button::A))
            game.player.position.x -= game.player.speed * deltaTime;

        if (platform::isButtonHeld(platform::Button::D))
            game.player.position.x += game.player.speed * deltaTime;

        // Keep Player Inside Road & Window Bounds
        game.player.position.x = glm::clamp(
            game.player.position.x,
            road.left() + ROAD_MARGIN,
            road.right() - game.player.size.x - ROAD_MARGIN);

        game.player.position.y = glm::clamp(
            game.player.position.y,
            0.f,
            (float)h - game.player.size.y);

        // 3. Score & Difficulty Progression
        game.score.update(game.world.scrollSpeed, deltaTime, game.player.isDead);
        game.world.maxScrollSpeed = game.score.getMaxScrollSpeed();
        trafficManager.spawnInterval = game.score.getTrafficSpawnInterval();

        // 4. World Scrolling
        game.world.scrollSpeed +=
            game.world.acceleration * deltaTime;

        if (platform::isButtonHeld(platform::Button::W))
        {
            game.world.scrollSpeed +=
                game.world.acceleration * deltaTime * 1.1f;
        }

        game.world.scrollSpeed =
            std::clamp(
                game.world.scrollSpeed,
                0.f,
                game.world.maxScrollSpeed);

        game.world.scrollOffset +=
            game.world.scrollSpeed * deltaTime;

        // 5. Update obstacles & traffic
        obstacleManager.update(
            game.world.scrollSpeed,
            deltaTime,
            w,
            h,
            road.left(),
            road.right(),
            &assets.treeLarge);

        trafficManager.update(
            game.world.scrollSpeed,
            deltaTime,
            h,
            road,
            assets,
            game.score.getTrafficSpeedBoost());

        coinManager.update(
            game.world.scrollSpeed,
            deltaTime,
            h,
            road,
            &assets.coin,
            trafficManager.cars);

        // 6. Collision Detection & Response
        // Player vs. Collectible Coins
        for (auto& coin : coinManager.coins)
        {
            if (!coin.active)
                continue;

            if (collision::checkAABBOverlap(game.player.position, game.player.size, coin.position, coin.size))
            {
                coin.active = false;
                game.coinCount++;
                audioManager.playBeepSound();
            }
        }

        // Player vs. Traffic Cars
        for (const auto& car : trafficManager.cars)
        {
            if (!car.active)
                continue;

            if (collision::checkAABBOverlap(game.player.position, game.player.size, car.position, car.size))
            {
                bool tookDamage = game.player.takeDamage(Player::DAMAGE_PER_COLLISION);
                collision::resolveAABBCollision(game.player.position, game.player.size, car.position, car.size);

                game.player.position.x = glm::clamp(
                    game.player.position.x,
                    road.left() + ROAD_MARGIN,
                    road.right() - game.player.size.x - ROAD_MARGIN);
                game.player.position.y = glm::clamp(
                    game.player.position.y,
                    0.f,
                    (float)h - game.player.size.y);

                if (collision::checkAABBOverlap(game.player.position, game.player.size, car.position, car.size))
                {
                    game.player.position.y = car.position.y + car.size.y + 1.f;
                    game.player.position.y = glm::clamp(game.player.position.y, 0.f, (float)h - game.player.size.y);
                }

                if (tookDamage)
                {
                    if (game.player.isDead)
                    {
                        triggerGameOver();
                        break;
                    }
                    else
                    {
                        audioManager.playCollisionSound();
                    }
                }
            }
        }

        // Player vs. Roadside Obstacles (only if still Playing)
        if (game.state == GameState::Playing)
        {
            for (const auto& obs : obstacleManager.obstacles)
            {
                if (!obs.active)
                    continue;

                if (collision::checkAABBOverlap(game.player.position, game.player.size, obs.position, obs.size))
                {
                    bool tookDamage = game.player.takeDamage(Player::DAMAGE_PER_COLLISION);
                    collision::resolveAABBCollision(game.player.position, game.player.size, obs.position, obs.size);

                    game.player.position.x = glm::clamp(
                        game.player.position.x,
                        road.left() + ROAD_MARGIN,
                        road.right() - game.player.size.x - ROAD_MARGIN);
                    game.player.position.y = glm::clamp(
                        game.player.position.y,
                        0.f,
                        (float)h - game.player.size.y);

                    if (tookDamage)
                    {
                        if (game.player.isDead)
                        {
                            triggerGameOver();
                            break;
                        }
                        else
                        {
                            audioManager.playCollisionSound();
                        }
                    }
                }
            }
        }

        if (game.player.isDead && game.state == GameState::Playing)
        {
            triggerGameOver();
        }
    }
    else
    {
        // Non-playing states (MainMenu, Paused, GameOver): simulation is completely frozen
        game.world.scrollSpeed = 0.f;
    }

    //-----------------------------------------------------
    // Rendering (runs in all states to display scene)
    //-----------------------------------------------------

    // Grass
    grass.render(
        renderer,
        assets.grass,
        road.left(),
        road.right(),
        w,
        h,
        game.world.scrollOffset);

    // Road
    road.render(
        renderer,
        assets.roadStraight,
        h,
        game.world.scrollOffset);

    // Collectible Coins
    coinManager.render(renderer);

    // Traffic Cars
    trafficManager.render(renderer);

    // Trees - Obstacles
    obstacleManager.render(renderer);

    // Ground shadow under player car
    renderer.renderRectangle(
        {
            game.player.position.x + 3.f,
            game.player.position.y + 4.f,
            game.player.size.x,
            game.player.size.y
        },
        gl2d::Color4f{ 0.05f, 0.06f, 0.08f, 0.35f });

    // Player with Collision Feedback
    gl2d::Color4f playerColor = { 1.f, 1.f, 1.f, 1.f };
    if (game.player.isDead)
    {
        // Darkened charred wreck tint when dead with crash skid mark
        renderer.renderRectangle(
            {
                game.player.position.x - 4.f,
                game.player.position.y + game.player.size.y - 12.f,
                game.player.size.x + 8.f,
                14.f
            },
            gl2d::Color4f{ 0.10f, 0.04f, 0.04f, 0.60f });
        playerColor = { 0.45f, 0.15f, 0.15f, 0.90f };
    }
    else if (game.player.isInvulnerable())
    {
        // Initial 0.15s high-intensity impact flash
        if (game.player.invulnerabilityTimer > (Player::INVULNERABILITY_DURATION - 0.15f))
        {
            playerColor = { 1.0f, 0.90f, 0.90f, 0.95f };
        }
        // Rhythmic flashing visibility/tint during remainder of recovery cooldown
        else if (std::fmod(game.player.invulnerabilityTimer, 0.16f) < 0.08f)
        {
            playerColor = { 1.0f, 0.35f, 0.35f, 0.50f };
        }
    }

    renderer.renderRectangle(
        {
            game.player.position,
            game.player.size
        },
        *game.player.texture,
        playerColor);

    // Player-Facing UI presentation by GameState
    switch (game.state)
    {
    case GameState::MainMenu:
    {
        UI::renderMainMenu(renderer, assets.font, game.score.highScore,
                           static_cast<float>(w), static_cast<float>(h),
                           saveSystem.getCachedWalletBalance());
        break;
    }
    case GameState::Playing:
    {
        UI::renderPlayingHUD(renderer, assets.font,
                             game.score.currentScore, game.score.highScore,
                             game.player.health, Player::MAX_HEALTH,
                             game.coinCount,
                             static_cast<float>(w), static_cast<float>(h));
        break;
    }
    case GameState::Paused:
    {
        UI::renderPlayingHUD(renderer, assets.font,
                             game.score.currentScore, game.score.highScore,
                             game.player.health, Player::MAX_HEALTH,
                             game.coinCount,
                             static_cast<float>(w), static_cast<float>(h));
        UI::renderPauseOverlay(renderer, assets.font,
                               game.score.currentScore, game.score.highScore,
                               static_cast<float>(w), static_cast<float>(h),
                               game.coinCount);
        break;
    }
    case GameState::GameOver:
    {
        UI::renderPlayingHUD(renderer, assets.font,
                             game.score.currentScore, game.score.highScore,
                             game.player.health, Player::MAX_HEALTH,
                             game.coinCount,
                             static_cast<float>(w), static_cast<float>(h));
        UI::renderGameOverScreen(renderer, assets.font,
                                 game.score.currentScore, game.score.highScore,
                                 static_cast<float>(w), static_cast<float>(h),
                                 game.coinCount,
                                 saveSystem.getCachedWalletBalance());
        break;
    }
    }

    renderer.flush();

    return true;
}

//=========================================================
// Cleanup
//=========================================================

void closeGame()
{
    static bool closed = false;
    if (closed)
        return;
    closed = true;

    saveSystem.saveHighScoreIfHigher(game.score.highScore);
    saveSystem.bankRun(game.currentRunId, game.coinCount);
    audioManager.cleanup();
    assets.freeAssets();
}