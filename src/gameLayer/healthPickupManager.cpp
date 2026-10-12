#include "healthPickupManager.h"
#include "collision.h"
#include <cstdlib>
#include <algorithm>

void HealthPickupManager::init()
{
    pickups.clear();
    pickups.reserve(MAX_HEALTH_PICKUPS);
    spawnTimer = 0.f;
    spawnInterval = DEFAULT_SPAWN_INTERVAL;
}

void HealthPickupManager::reset()
{
    for (auto& p : pickups)
    {
        p.active = false;
    }
    spawnTimer = 0.f;
}

bool HealthPickupManager::isPositionSafeFromTraffic(const glm::vec2& pos, const glm::vec2& size,
                                                    const std::vector<TrafficCar>& trafficCars) const
{
    // Clearance buffer around candidate pickup to avoid spawning on or immediately into traffic
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

bool HealthPickupManager::trySpawnPickup(const Road& road, gl2d::Texture* texture,
                                         const std::vector<TrafficCar>& trafficCars)
{
    // 1. Check capacity
    size_t activeCount = 0;
    HealthPickup* availableSlot = nullptr;
    for (auto& p : pickups)
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

    if (activeCount >= MAX_HEALTH_PICKUPS)
    {
        return false;
    }

    if (availableSlot == nullptr)
    {
        if (pickups.size() < MAX_HEALTH_PICKUPS)
        {
            pickups.push_back(HealthPickup{});
            availableSlot = &pickups.back();
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
            availableSlot->texture = texture;
            availableSlot->active = true;
            return true;
        }
    }

    // If all candidate lanes conflict with oncoming traffic, skip this spawn attempt safely
    return false;
}

void HealthPickupManager::update(float worldScrollSpeed,
                                 float deltaTime,
                                 int windowHeight,
                                 const Road& road,
                                 gl2d::Texture* texture,
                                 const std::vector<TrafficCar>& trafficCars)
{
    // Update active pickups and despawn those past bottom screen
    for (auto& p : pickups)
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
        trySpawnPickup(road, texture, trafficCars);
    }
}

void HealthPickupManager::render(gl2d::Renderer2D& renderer)
{
    for (auto& p : pickups)
    {
        if (p.active)
        {
            p.render(renderer);
        }
    }
}
