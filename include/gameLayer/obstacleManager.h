#pragma once

#include <vector>
#include <gl2d/gl2d.h>

#include "obstacle.h"

struct ObstacleManager
{
    std::vector<Obstacle> obstacles;

    float spawnTimer = 0.f;
    float spawnInterval = 1.5f;

    void spawn(gl2d::Texture* texture,
        glm::vec2 position,
        glm::vec2 size);

    void update(float scrollSpeed,
        float deltaTime,
        int windowWidth,
        int windowHeight,
        float roadLeft,
        float roadRight,
        gl2d::Texture* treeTexture);

    void render(gl2d::Renderer2D& renderer);
};