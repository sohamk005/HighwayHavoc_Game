#pragma once

#include <gl2d/gl2d.h>
#include <glm/glm.hpp>

struct Grass
{
    float tileSize = 128.f;

    void render(
        gl2d::Renderer2D& renderer,
        gl2d::Texture& texture,
        float roadLeft,
        float roadRight,
        int windowWidth,
        int windowHeight,
        float scrollOffset);
};