#include "obstacleManager.h"

#include <cstdlib>

float randomGrassX(
    float objectWidth,
    int windowWidth,
    float roadLeft,
    float roadRight)
{
    bool leftSide = rand() % 2;

    if (leftSide)
    {
        float maxX = roadLeft - objectWidth;

        if (maxX < 0)
            maxX = 0;

        return static_cast<float>(
            rand() % (static_cast<int>(maxX) + 1));
    }
    else
    {
        float available =
            windowWidth - roadRight - objectWidth;

        if (available < 0)
            available = 0;

        return roadRight +
            static_cast<float>(
                rand() % (static_cast<int>(available) + 1));
    }
}

void ObstacleManager::spawn(
    gl2d::Texture* texture,
    glm::vec2 position,
    glm::vec2 size)
{
    Obstacle obstacle;

    obstacle.texture = texture;
    obstacle.position = position;
    obstacle.size = size;
    obstacle.active = true;

    obstacles.push_back(obstacle);
}

void ObstacleManager::update(
    float scrollSpeed,
    float deltaTime,
    int windowWidth,
    int windowHeight,
    float roadLeft,
    float roadRight,
    gl2d::Texture* treeTexture)
{
    spawnTimer += deltaTime;

    if (spawnTimer >= spawnInterval)
    {
        spawnTimer = 0.f;

        glm::vec2 size = { 120.f, 120.f };

        glm::vec2 position;
        position.x = randomGrassX(
            size.x,
            windowWidth,
            roadLeft,
            roadRight);

        position.y = -100.f;

        spawn(treeTexture, position, size);
    }

    for (auto& obstacle : obstacles)
    {
        obstacle.update(scrollSpeed, deltaTime);
    }
}

void ObstacleManager::render(gl2d::Renderer2D& renderer)
{
    for (auto& obstacle : obstacles)
    {
        obstacle.render(renderer);
    }
}