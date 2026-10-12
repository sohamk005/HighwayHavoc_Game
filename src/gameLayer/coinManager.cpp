#include "coinManager.h"
#include "collision.h"
#include <cstdlib>
#include <algorithm>

void CoinManager::init()
{
    coins.clear();
    coins.reserve(MAX_COINS);
    spawnTimer = 0.f;
    spawnInterval = DEFAULT_SPAWN_INTERVAL;
}

void CoinManager::reset()
{
    for (auto& coin : coins)
    {
        coin.active = false;
    }
    spawnTimer = 0.f;
}

bool CoinManager::isPositionSafeFromTraffic(const glm::vec2& pos, const glm::vec2& size,
                                            const std::vector<TrafficCar>& trafficCars) const
{
    // Clearance buffer around candidate coin to avoid spawning on or immediately into traffic
    constexpr float bufferX = 15.f;
    constexpr float bufferY = 40.f;
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

bool CoinManager::trySpawnCoin(const Road& road, gl2d::Texture* texture,
                               const std::vector<TrafficCar>& trafficCars)
{
    // 1. Check capacity
    size_t activeCount = 0;
    Coin* availableSlot = nullptr;
    for (auto& c : coins)
    {
        if (c.active)
        {
            activeCount++;
        }
        else if (availableSlot == nullptr)
        {
            availableSlot = &c;
        }
    }

    if (activeCount >= MAX_COINS)
    {
        return false;
    }

    if (availableSlot == nullptr)
    {
        if (coins.size() < MAX_COINS)
        {
            coins.push_back(Coin{});
            availableSlot = &coins.back();
        }
        else
        {
            return false;
        }
    }

    // 2. Select lane with lateral variation
    int startLane = rand() % Road::NUM_LANES;
    const float minLegalX = road.left() + ROAD_MARGIN;
    const float maxLegalX = road.right() - COIN_SIZE - ROAD_MARGIN;

    for (int i = 0; i < Road::NUM_LANES; ++i)
    {
        int lane = (startLane + i) % Road::NUM_LANES;
        float laneCenter = road.getLaneCenter(lane);

        // Bounded lateral variation: [-MAX_LATERAL_OFFSET, +MAX_LATERAL_OFFSET]
        float offset = static_cast<float>((rand() % (static_cast<int>(2.f * MAX_LATERAL_OFFSET) + 1)) - static_cast<int>(MAX_LATERAL_OFFSET));
        float candidateX = laneCenter - COIN_SIZE * 0.5f + offset;
        candidateX = std::clamp(candidateX, minLegalX, maxLegalX);

        glm::vec2 candidatePos = { candidateX, -COIN_SIZE };
        glm::vec2 candidateSize = { COIN_SIZE, COIN_SIZE };

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

void CoinManager::update(float worldScrollSpeed,
                         float deltaTime,
                         int windowHeight,
                         const Road& road,
                         gl2d::Texture* texture,
                         const std::vector<TrafficCar>& trafficCars)
{
    // Update active coins and despawn those past bottom screen
    for (auto& coin : coins)
    {
        if (!coin.active)
            continue;

        coin.update(worldScrollSpeed, deltaTime);

        if (coin.position.y > static_cast<float>(windowHeight) + 50.f)
        {
            coin.active = false;
        }
    }

    // Spawn timer accumulation
    spawnTimer += deltaTime;
    if (spawnTimer >= spawnInterval)
    {
        spawnTimer -= spawnInterval;
        trySpawnCoin(road, texture, trafficCars);
    }
}

void CoinManager::render(gl2d::Renderer2D& renderer)
{
    for (auto& coin : coins)
    {
        if (coin.active)
        {
            coin.render(renderer);
        }
    }
}
