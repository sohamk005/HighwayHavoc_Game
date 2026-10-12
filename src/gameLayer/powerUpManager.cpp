#include "powerUpManager.h"
#include "collision.h"
#include <cstdlib>
#include <algorithm>

void PowerUpManager::init()
{
    powerUps.clear();
    powerUps.reserve(MAX_POWER_UPS);
    spawnTimer = 0.f;
    spawnInterval = DEFAULT_SPAWN_INTERVAL;
}

void PowerUpManager::reset()
{
    for (auto& p : powerUps)
    {
        p.active = false;
    }
    spawnTimer = 0.f;
}

bool PowerUpManager::isPositionSafeFromTraffic(const glm::vec2& pos, const glm::vec2& size,
                                               const std::vector<TrafficCar>& trafficCars) const
{
    // Clearance buffer around candidate pickup to avoid spawning directly on or immediately into oncoming traffic
    constexpr float bufferX = 16.f;
    constexpr float bufferY = 45.f;
    glm::vec2 checkPos = { pos.x - bufferX, pos.y - bufferY };
    glm::vec2 checkSize = { size.x + 2.f * bufferX, size.y + 2.f * bufferY };

    for (const auto& car : trafficCars)
    {
        if (!car.active)
            continue;

        if (collision::checkAABBOverlap(checkPos, checkSize, car.position, car.size))
        {
            return false;
        }
    }
    return true;
}

bool PowerUpManager::trySpawnPowerUp(const Road& road,
                                     const std::vector<TrafficCar>& trafficCars)
{
    // 1. Check capacity
    size_t activeCount = 0;
    PowerUp* availableSlot = nullptr;
    for (auto& p : powerUps)
    {
        if (p.active)
        {
            activeCount++;
        }
        else if (availableSlot == nullptr)
        {
            availableSlot = &p;
        }
    }

    if (activeCount >= MAX_POWER_UPS)
    {
        return false;
    }

    if (availableSlot == nullptr)
    {
        if (powerUps.size() < MAX_POWER_UPS)
        {
            powerUps.push_back(PowerUp{});
            availableSlot = &powerUps.back();
        }
        else
        {
            return false;
        }
    }

    // 2. Select lane with lateral variation
    int startLane = rand() % Road::NUM_LANES;
    const float minLegalX = road.left() + ROAD_MARGIN;
    const float maxLegalX = road.right() - PICKUP_SIZE - ROAD_MARGIN;

    for (int i = 0; i < Road::NUM_LANES; ++i)
    {
        int lane = (startLane + i) % Road::NUM_LANES;
        float laneCenter = road.getLaneCenter(lane);

        // Bounded lateral variation: [-MAX_LATERAL_OFFSET, +MAX_LATERAL_OFFSET]
        float offset = static_cast<float>((rand() % (static_cast<int>(2.f * MAX_LATERAL_OFFSET) + 1)) - static_cast<int>(MAX_LATERAL_OFFSET));
        float candidateX = laneCenter - PICKUP_SIZE * 0.5f + offset;
        candidateX = std::clamp(candidateX, minLegalX, maxLegalX);

        glm::vec2 candidatePos = { candidateX, -PICKUP_SIZE };
        glm::vec2 candidateSize = { PICKUP_SIZE, PICKUP_SIZE };

        if (isPositionSafeFromTraffic(candidatePos, candidateSize, trafficCars))
        {
            availableSlot->position = candidatePos;
            availableSlot->size = candidateSize;
            // 50% chance Shield, 50% chance DoubleCoins
            availableSlot->type = (rand() % 2 == 0) ? PowerUpType::Shield : PowerUpType::DoubleCoins;
            availableSlot->texture = nullptr;
            availableSlot->active = true;
            return true;
        }
    }

    // If all candidate lanes conflict with oncoming traffic, skip this spawn safely
    return false;
}

void PowerUpManager::update(float worldScrollSpeed,
                            float deltaTime,
                            int windowHeight,
                            const Road& road,
                            const std::vector<TrafficCar>& trafficCars)
{
    // Update active pickups and despawn those past bottom screen
    for (auto& p : powerUps)
    {
        if (!p.active)
            continue;

        p.update(worldScrollSpeed, deltaTime);

        if (p.position.y > static_cast<float>(windowHeight) + 50.f)
        {
            p.active = false;
        }
    }

    // Spawn timer accumulation
    spawnTimer += deltaTime;
    if (spawnTimer >= spawnInterval)
    {
        spawnTimer -= spawnInterval;
        trySpawnPowerUp(road, trafficCars);
    }
}

void PowerUpManager::render(gl2d::Renderer2D& renderer)
{
    for (auto& p : powerUps)
    {
        p.render(renderer);
    }
}
