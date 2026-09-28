#include "obstacleManager.h"

#include <cstdlib>
#include <algorithm>

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

        if (maxX < 0.f)
            maxX = 0.f;

        int maxInt = static_cast<int>(maxX);
        if (maxInt > 0)
        {
            return static_cast<float>(rand() % (maxInt + 1));
        }
        return 0.f;
    }
    else
    {
        float available =
            windowWidth - roadRight - objectWidth;

        if (available < 0.f)
            available = 0.f;

        int availInt = static_cast<int>(available);
        if (availInt > 0)
        {
            return roadRight +
                static_cast<float>(rand() % (availInt + 1));
        }
        return roadRight;
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

        position.y = -size.y;

        spawn(treeTexture, position, size);
    }

    for (auto& obstacle : obstacles)
    {
        obstacle.update(scrollSpeed, deltaTime);
    }

    // Lifecycle cleanup: remove off-screen obstacles beyond viewport bottom
    constexpr float DESPAWN_MARGIN = 150.f;
    const float despawnThreshold = static_cast<float>(windowHeight) + DESPAWN_MARGIN;

    obstacles.erase(
        std::remove_if(
            obstacles.begin(),
            obstacles.end(),
            [despawnThreshold](const Obstacle& obstacle)
            {
                return obstacle.position.y > despawnThreshold;
            }),
        obstacles.end());
}

void ObstacleManager::render(gl2d::Renderer2D& renderer)
{
    for (auto& obstacle : obstacles)
    {
        obstacle.render(renderer);
    }
}

void ObstacleManager::reset()
{
    obstacles.clear();
    spawnTimer = 0.f;
}