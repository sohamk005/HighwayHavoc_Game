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



AssetManager assets;
gl2d::Renderer2D renderer;
Road road;
Grass grass;
ObstacleManager obstacleManager;

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

    game.player.position.x =
        road.left() + road.width / 2.f - game.player.size.x / 2.f;

    game.player.position.y =
        (float)h - game.player.size.y - 40.f;

    game.world.scrollSpeed = 0.f;
    game.world.scrollOffset = 0.f;

    obstacleManager.reset();
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

    // Reset gameplay foundation (press R)
    if (platform::isButtonPressedOn(platform::Button::R))
    {
        resetGame();
    }

    //-----------------------------------------------------
    // Grass
    //-----------------------------------------------------

    road.update(w);

    grass.render(
        renderer,
        assets.grass,
        road.left(),
        road.right(),
        w,
        h,
        game.world.scrollOffset);

    //-----------------------------------------------------
    // Render Road
    //-----------------------------------------------------

    road.render(
        renderer,
        assets.roadStraight,
        h,
        game.world.scrollOffset);

   

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
// World Scrolling
//-----------------------------------------------------

    
     game.world.scrollSpeed +=
     game.world.acceleration * deltaTime;

     obstacleManager.update(
         game.world.scrollSpeed,
         deltaTime,
         w,
         h,
         road.left(),
         road.right(),
         &assets.treeLarge);

     if (platform::isButtonHeld(platform::Button::W))
    {
        game.world.scrollSpeed +=
        game.world.acceleration * deltaTime * 1.1;
    }

    //if (platform::isButtonHeld(platform::Button::S))
    //{
    //    game.world.scrollSpeed -=
    //       game.world.brakePower * deltaTime;
    //}

    game.world.scrollSpeed =
        std::clamp(
            game.world.scrollSpeed,
            0.f,
            game.world.maxScrollSpeed);

    game.world.scrollOffset +=
        game.world.scrollSpeed * deltaTime;



    //-----------------------------------------------------
    // Keep Player Inside Window
    //-----------------------------------------------------

    constexpr float ROAD_MARGIN = 12.f;

    game.player.position.x = glm::clamp(
        game.player.position.x,
        road.left() + ROAD_MARGIN,
        road.right() - game.player.size.x - ROAD_MARGIN);

    game.player.position.y = glm::clamp(
        game.player.position.y,
        0.f,
        (float)h - game.player.size.y);


    //Render Trees - Obstacles
    obstacleManager.render(renderer);


    //-----------------------------------------------------
    // Render Player
    //-----------------------------------------------------

    renderer.renderRectangle(
        {
            game.player.position,
            game.player.size
        },
        *game.player.texture);

    //std::cout << game.world.scrollOffset << '\n';

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