#include "trafficManager.h"
#include <cstdlib>
#include <cmath>
#include <algorithm>

bool TrafficManager::isLaneSafeForSpawn(int lane, float spawnY) const
{
    for (const auto& car : cars)
    {
        if (car.lane == lane)
        {
            if (std::abs(car.position.y - spawnY) < MIN_SPAWN_HEADWAY)
            {
                return false;
            }
        }
    }
    return true;
}

bool TrafficManager::wouldSpawnBlockAllLanes(int candidateLane, float spawnY) const
{
    int blockedLanes = 0;
    for (int l = 0; l < Road::NUM_LANES; ++l)
    {
        if (l == candidateLane)
            continue;

        for (const auto& car : cars)
        {
            if (car.lane == l && std::abs(car.position.y - spawnY) < MIN_SPAWN_HEADWAY)
            {
                blockedLanes++;
                break;
            }
        }
    }
    // If every other lane is already occupied within headway of spawnY,
    // spawning in candidateLane would block all lanes simultaneously.
    return blockedLanes >= (Road::NUM_LANES - 1);
}

void TrafficManager::spawn(const Road& road, AssetManager& assets, float speedBoost)
{
    if (cars.size() >= MAX_TRAFFIC_CARS)
    {
        return;
    }

    constexpr float spawnY = -80.f;

    // 1. Identify safe candidate lanes using authoritative headway & fairness rules
    int safeLanes[Road::NUM_LANES];
    int numSafeLanes = 0;

    for (int l = 0; l < Road::NUM_LANES; ++l)
    {
        if (isLaneSafeForSpawn(l, spawnY) && !wouldSpawnBlockAllLanes(l, spawnY))
        {
            safeLanes[numSafeLanes++] = l;
        }
    }

    if (numSafeLanes == 0)
    {
        // All candidate lanes currently occupied near spawn point; wait for safe headway
        return;
    }

    int chosenLane = -1;

    if (numSafeLanes == 1)
    {
        chosenLane = safeLanes[0];
    }
    else
    {
        // Find the most starved safe lane
        int mostStarvedLane = safeLanes[0];
        float maxStarv = laneStarvation[safeLanes[0]];
        for (int i = 1; i < numSafeLanes; ++i)
        {
            if (laneStarvation[safeLanes[i]] > maxStarv)
            {
                maxStarv = laneStarvation[safeLanes[i]];
                mostStarvedLane = safeLanes[i];
            }
        }

        // If a safe lane has gone without traffic beyond threshold, bias selection toward it
        if (maxStarv >= STARVATION_BIAS_THRESHOLD)
        {
            // 80% bias toward the most underserved lane, 20% random among other safe candidates
            if ((rand() % 100) < 80)
            {
                chosenLane = mostStarvedLane;
            }
            else
            {
                chosenLane = safeLanes[rand() % numSafeLanes];
            }
        }
        else
        {
            // When all safe lanes are adequately served, choose randomly
            chosenLane = safeLanes[rand() % numSafeLanes];
        }
    }

    TrafficCar car;
    car.lane = chosenLane;
    car.size = { 60.f, 80.f };
    car.position.x = road.getLaneCenter(chosenLane) - car.size.x / 2.f;
    car.position.y = -car.size.y;
    car.forwardSpeed = 160.f + static_cast<float>(rand() % 80) + speedBoost; // [160, 240] + speedBoost
    car.active = true;

    // Distribute among available car textures
    int tex = rand() % 3;
    if (tex == 0)
        car.texture = &assets.redCar;
    else if (tex == 1)
        car.texture = &assets.greenCar;
    else
        car.texture = &assets.yellowCar;

    cars.push_back(car);
}

void TrafficManager::update(
    float worldScrollSpeed,
    float deltaTime,
    int windowHeight,
    const Road& road,
    AssetManager& assets,
    float speedBoost)
{
    // Gate traffic spawn until minimum cruising speed (160 px/s) to prevent negative relative speeds
    // and give comfortable onboarding on run start
    if (worldScrollSpeed >= 160.0f)
    {
        spawnTimer += deltaTime;

        if (spawnTimer >= spawnInterval)
        {
            spawnTimer = 0.f;
            spawn(road, assets, speedBoost);
        }
    }

    for (auto& car : cars)
    {
        car.update(worldScrollSpeed, deltaTime);
    }

    // Lifecycle cleanup: remove off-screen traffic cars
    constexpr float DESPAWN_MARGIN = 150.f;
    const float despawnThreshold = static_cast<float>(windowHeight) + DESPAWN_MARGIN;

    cars.erase(
        std::remove_if(
            cars.begin(),
            cars.end(),
            [despawnThreshold](const TrafficCar& car)
            {
                return car.position.y > despawnThreshold;
            }),
        cars.end());

    // Update per-lane starvation tracking:
    // Meaningful traffic occupancy is defined as an active traffic car
    // within the visible screen / roadway [0, windowHeight]
    if (worldScrollSpeed >= 160.0f)
    {
        bool laneOccupied[Road::NUM_LANES] = { false, false, false };

        for (const auto& car : cars)
        {
            if (car.lane >= 0 && car.lane < Road::NUM_LANES)
            {
                if (car.active && (car.position.y + car.size.y >= 0.f) && (car.position.y <= static_cast<float>(windowHeight)))
                {
                    laneOccupied[car.lane] = true;
                }
            }
        }

        for (int l = 0; l < Road::NUM_LANES; ++l)
        {
            if (laneOccupied[l])
            {
                laneStarvation[l] = 0.f;
            }
            else
            {
                laneStarvation[l] += deltaTime;
            }
        }
    }
}

void TrafficManager::render(gl2d::Renderer2D& renderer)
{
    for (auto& car : cars)
    {
        car.render(renderer);
    }
}

void TrafficManager::reset()
{
    cars.clear();
    spawnTimer = 0.f;
    spawnInterval = 2.0f;
    for (int l = 0; l < Road::NUM_LANES; ++l)
    {
        laneStarvation[l] = 0.f;
    }
}
