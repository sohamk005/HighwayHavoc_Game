#define GLM_ENABLE_EXPERIMENTAL

#include "gameLayer.h"

#include <glad/glad.h>
#include <gl2d/gl2d.h>
#include <glm/glm.hpp>

#include "platformInput.h"
#include <platformTools.h>

gl2d::Renderer2D renderer;

struct GameData
{
    glm::vec2 playerPos = { 400.f, 300.f };
};

GameData game;

bool initGame()
{
    gl2d::init();
    renderer.create();
    return true;
}

bool gameLogic(float deltaTime)
{
    int w = platform::getFrameBufferSizeX();
    int h = platform::getFrameBufferSizeY();

    glViewport(0, 0, w, h);
    glClear(GL_COLOR_BUFFER_BIT);

    renderer.updateWindowMetrics(w, h);

    float speed = 400.f;

    if (platform::isButtonHeld(platform::Button::W))
        game.playerPos.y -= speed * deltaTime;

    if (platform::isButtonHeld(platform::Button::S))
        game.playerPos.y += speed * deltaTime;

    if (platform::isButtonHeld(platform::Button::A))
        game.playerPos.x -= speed * deltaTime;

    if (platform::isButtonHeld(platform::Button::D))
        game.playerPos.x += speed * deltaTime;

    // Keep player inside the window
    game.playerPos.x = glm::clamp(game.playerPos.x, 0.f, (float)w - 60.f);
    game.playerPos.y = glm::clamp(game.playerPos.y, 0.f, (float)h - 100.f);

    // Draw a simple placeholder
    renderer.renderRectangle(
        {
            game.playerPos.x,
            game.playerPos.y,
            60,
            100
        },
        Colors_Blue
    );

    renderer.flush();

    return true;
}

void closeGame()
{
}