#pragma once

#include <gl2d/gl2d.h>
#include <glm/glm.hpp>

struct Road
{
    glm::vec2 position = { 0.f, 0.f };

    float width = 640.f;

    float tileWidth = 128.f;
    float tileHeight = 128.f;

    void update(int windowWidth);

    void render(gl2d::Renderer2D& renderer,
        gl2d::Texture& roadTexture,
        int windowHeight,
        float scrollOffset);

    float left() const;
    float right() const;
    float grassLeftWidth() const;
    float grassRightStart() const;

    // Authoritative 3-Lane Model
    static constexpr int NUM_LANES = 3;

    float getLaneWidth() const;
    float getLaneLeft(int laneIndex) const;
    float getLaneRight(int laneIndex) const;
    float getLaneCenter(int laneIndex) const;
    int getLaneIndexFromX(float x) const;
};