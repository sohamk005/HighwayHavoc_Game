#pragma once

#include <glm/vec2.hpp>
#include <gl2d/gl2d.h>

struct Player
{
    glm::vec2 position = { 400.f, 300.f };
    glm::vec2 size = { 60.f, 80.f };
    float speed = 400.f;

    gl2d::Texture* texture = nullptr;

    // Health, Damage, and Invulnerability
    static constexpr int MAX_HEALTH = 100;
    static constexpr int DAMAGE_PER_COLLISION = 25;
    static constexpr float INVULNERABILITY_DURATION = 1.5f;

    int health = MAX_HEALTH;
    float invulnerabilityTimer = 0.f;
    bool isDead = false;

    bool isInvulnerable() const
    {
        return invulnerabilityTimer > 0.f;
    }

    bool takeDamage(int amount = DAMAGE_PER_COLLISION)
    {
        if (isDead || isInvulnerable())
            return false;

        health -= amount;
        if (health <= 0)
        {
            health = 0;
            isDead = true;
        }

        invulnerabilityTimer = INVULNERABILITY_DURATION;
        return true;
    }

    void update(float deltaTime)
    {
        if (invulnerabilityTimer > 0.f)
        {
            invulnerabilityTimer -= deltaTime;
            if (invulnerabilityTimer < 0.f)
            {
                invulnerabilityTimer = 0.f;
            }
        }
    }

    void reset(float startX, float startY)
    {
        position = { startX, startY };
        health = MAX_HEALTH;
        invulnerabilityTimer = 0.f;
        isDead = false;
    }
};
