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
#include "player.h"
#include "collision.h"
#include "scoreSystem.h"
#include "gameState.h"
#include "ui/uiTheme.h"
#include "ui/hud.h"
#include "ui/menu.h"

AssetManager assets;
gl2d::Renderer2D renderer;
Road road;
Grass grass;
ObstacleManager obstacleManager;
TrafficManager trafficManager;

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
    GameState state = GameState::MainMenu;
};

GameData game;

//=========================================================
// Reset Gameplay Foundation
//=========================================================

void resetGame()
{
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

    ilog("Run Reset. High Score:", game.score.highScore);
}

//=========================================================
// Initialization
//=========================================================

bool initGame()
{
    gl2d::init();

    renderer.create();

    assets.loadAssets();

    game.player.texture = &assets.blueCar;

    resetGame();
    game.state = GameState::MainMenu;

    ilog("Highway Havoc Initialized. State: MainMenu (Press ENTER or SPACE to start)");

    return true;
}

//=========================================================
// Main Game Loop
//=========================================================

bool gameLogic(float deltaTime)
{
    int w = platform::getFrameBufferSizeX();
    int h = platform::getFrameBufferSizeY();

    glViewport(0, 0, w, h);
    glClear(GL_COLOR_BUFFER_BIT);

    renderer.updateWindowMetrics(w, h);

    // State machine input handling & transitions
    switch (game.state)
    {
    case GameState::MainMenu:
    {
        if (platform::isButtonPressedOn(platform::Button::Enter) ||
            platform::isButtonPressedOn(platform::Button::Space))
        {
            resetGame();
            game.state = GameState::Playing;
            ilog("State Transition: MainMenu -> Playing (Game Started)");
        }
        break;
    }
    case GameState::Playing:
    {
        if (platform::isButtonPressedOn(platform::Button::P))
        {
            game.state = GameState::Paused;
            ilog("State Transition: Playing -> Paused");
        }
        else if (platform::isButtonPressedOn(platform::Button::R))
        {
            resetGame();
            game.state = GameState::Playing;
        }
        break;
    }
    case GameState::Paused:
    {
        if (platform::isButtonPressedOn(platform::Button::P))
        {
            game.state = GameState::Playing;
            ilog("State Transition: Paused -> Playing (Resumed)");
        }
        break;
    }
    case GameState::GameOver:
    {
        if (platform::isButtonPressedOn(platform::Button::R))
        {
            resetGame();
            game.state = GameState::Playing;
            ilog("State Transition: GameOver -> Playing (Restarted)");
        }
        else if (platform::isButtonPressedOn(platform::Button::Escape))
        {
            resetGame();
            game.state = GameState::MainMenu;
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

        // 6. Collision Detection & Response
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

                if (tookDamage && game.player.isDead)
                {
                    game.state = GameState::GameOver;
                    game.world.scrollSpeed = 0.f;
                    ilog("GAME OVER! Final Score:", game.score.currentScore, "| High Score:", game.score.highScore);
                    break;
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

                    if (tookDamage && game.player.isDead)
                    {
                        game.state = GameState::GameOver;
                        game.world.scrollSpeed = 0.f;
                        ilog("GAME OVER! Final Score:", game.score.currentScore, "| High Score:", game.score.highScore);
                        break;
                    }
                }
            }
        }

        if (game.player.isDead && game.state == GameState::Playing)
        {
            game.state = GameState::GameOver;
            game.world.scrollSpeed = 0.f;
            ilog("GAME OVER! Final Score:", game.score.currentScore, "| High Score:", game.score.highScore);
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

    // Traffic Cars
    trafficManager.render(renderer);

    // Trees - Obstacles
    obstacleManager.render(renderer);

    // Player with Collision Feedback
    gl2d::Color4f playerColor = { 1.f, 1.f, 1.f, 1.f };
    if (game.player.isDead)
    {
        // Darkened / red tint when health reaches zero
        playerColor = { 0.6f, 0.2f, 0.2f, 0.8f };
    }
    else if (game.player.isInvulnerable())
    {
        // Flashing visibility/tint during invulnerability
        if (std::fmod(game.player.invulnerabilityTimer, 0.2f) < 0.1f)
        {
            playerColor = { 1.f, 0.3f, 0.3f, 0.6f };
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
                           static_cast<float>(w), static_cast<float>(h));
        break;
    }
    case GameState::Playing:
    {
        UI::renderPlayingHUD(renderer, assets.font,
                             game.score.currentScore, game.score.highScore,
                             game.player.health, Player::MAX_HEALTH,
                             static_cast<float>(w), static_cast<float>(h));
        break;
    }
    case GameState::Paused:
    {
        UI::renderPlayingHUD(renderer, assets.font,
                             game.score.currentScore, game.score.highScore,
                             game.player.health, Player::MAX_HEALTH,
                             static_cast<float>(w), static_cast<float>(h));
        UI::renderPauseOverlay(renderer, assets.font,
                               game.score.currentScore, game.score.highScore,
                               static_cast<float>(w), static_cast<float>(h));
        break;
    }
    case GameState::GameOver:
    {
        UI::renderPlayingHUD(renderer, assets.font,
                             game.score.currentScore, game.score.highScore,
                             game.player.health, Player::MAX_HEALTH,
                             static_cast<float>(w), static_cast<float>(h));
        UI::renderGameOverScreen(renderer, assets.font,
                                 game.score.currentScore, game.score.highScore,
                                 static_cast<float>(w), static_cast<float>(h));
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
    assets.freeAssets();
}