#define GLM_ENABLE_EXPERIMENTAL

#include "gameLayer.h"

#include <glad/glad.h>
#include <gl2d/gl2d.h>
#include <glm/glm.hpp>

#include "platformInput.h"
#include <platformTools.h>

#include "assetManager.h"

AssetManager assets;
gl2d::Renderer2D renderer;

//=========================================================
// Player
//=========================================================

struct Player
{
    glm::vec2 position = { 400.f, 300.f };

    glm::vec2 size = { 60.f, 80.f };

    float speed = 400.f;

    gl2d::Texture* texture = nullptr;
};

//=========================================================
// Game Data
//=========================================================

struct GameData
{
    Player player{};
};

GameData game;

//=========================================================
// Initialization
//=========================================================

bool initGame()
{
    gl2d::init();

    renderer.create();

    assets.loadAssets();

    game.player.texture = &assets.blueCar;

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

    //-----------------------------------------------------
    // Player Movement
    //-----------------------------------------------------

    if (platform::isButtonHeld(platform::Button::W))
        game.player.position.y -= game.player.speed * deltaTime;

    if (platform::isButtonHeld(platform::Button::S))
        game.player.position.y += game.player.speed * deltaTime;

    if (platform::isButtonHeld(platform::Button::A))
        game.player.position.x -= game.player.speed * deltaTime;

    if (platform::isButtonHeld(platform::Button::D))
        game.player.position.x += game.player.speed * deltaTime;

    //-----------------------------------------------------
    // Keep Player Inside Window
    //-----------------------------------------------------

    game.player.position.x = glm::clamp(
        game.player.position.x,
        0.f,
        (float)w - game.player.size.x);

    game.player.position.y = glm::clamp(
        game.player.position.y,
        0.f,
        (float)h - game.player.size.y);

    //-----------------------------------------------------
    // Render Player
    //-----------------------------------------------------

    renderer.renderRectangle(
        {
            game.player.position,
            game.player.size
        },
        *game.player.texture);

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