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
    static constexpr int HEAL_PER_PICKUP = 25;
    static constexpr float INVULNERABILITY_DURATION = 1.5f;
    static constexpr float SHIELD_DURATION = 12.0f;
    static constexpr float SHIELD_BREAK_INVULNERABILITY = 0.75f;
    static constexpr float MULTIPLIER_DURATION = 10.0f;

    int health = MAX_HEALTH;
    float invulnerabilityTimer = 0.f;
    bool isDead = false;

    // Power-Up States
    bool hasShield = false;
    float shieldTimer = 0.f;

    float multiplierTimer = 0.f;

    bool isInvulnerable() const
    {
        return invulnerabilityTimer > 0.f;
    }

    bool isMultiplierActive() const
    {
        return multiplierTimer > 0.f;
    }

    void activateShield()
    {
        hasShield = true;
        shieldTimer = SHIELD_DURATION;
    }

    void breakShield()
    {
        hasShield = false;
        shieldTimer = 0.f;
        // Grant temporary protection without shortening any longer existing invulnerability period
        if (invulnerabilityTimer < SHIELD_BREAK_INVULNERABILITY)
        {
            invulnerabilityTimer = SHIELD_BREAK_INVULNERABILITY;
        }
    }

    void activateMultiplier()
    {
        multiplierTimer = MULTIPLIER_DURATION;
    }

    bool takeDamage(int amount = DAMAGE_PER_COLLISION)
    {
        if (isDead || isInvulnerable())
            return false;

        // If shielded, absorb the hit completely without deducting health
        if (hasShield)
        {
            breakShield();
            return false;
        }

        health -= amount;
        if (health <= 0)
        {
            health = 0;
            isDead = true;
        }

        invulnerabilityTimer = INVULNERABILITY_DURATION;
        return true;
    }

    bool heal(int amount = HEAL_PER_PICKUP)
    {
        if (isDead || health >= MAX_HEALTH)
            return false;

        health += amount;
        if (health > MAX_HEALTH)
        {
            health = MAX_HEALTH;
        }

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

        if (hasShield)
        {
            shieldTimer -= deltaTime;
            if (shieldTimer <= 0.f)
            {
                hasShield = false;
                shieldTimer = 0.f;
            }
        }

        if (multiplierTimer > 0.f)
        {
            multiplierTimer -= deltaTime;
            if (multiplierTimer < 0.f)
            {
                multiplierTimer = 0.f;
            }
        }
    }

    void reset(float startX, float startY)
    {
        position = { startX, startY };
        health = MAX_HEALTH;
        invulnerabilityTimer = 0.f;
        isDead = false;
        hasShield = false;
        shieldTimer = 0.f;
        multiplierTimer = 0.f;
    }
};

