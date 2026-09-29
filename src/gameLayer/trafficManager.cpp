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

void TrafficManager::spawn(const Road& road, AssetManager& assets, float speedBoost)
{
    if (cars.size() >= MAX_TRAFFIC_CARS)
    {
        return;
    }

    constexpr float spawnY = -80.f;

    // Check candidate lanes starting from a randomized index
    int startLane = rand() % Road::NUM_LANES;
    int chosenLane = -1;

    for (int i = 0; i < Road::NUM_LANES; ++i)
    {
        int lane = (startLane + i) % Road::NUM_LANES;
        if (isLaneSafeForSpawn(lane, spawnY))
        {
            chosenLane = lane;
            break;
        }
    }

    if (chosenLane == -1)
    {
        // All lanes currently occupied near spawn point; wait for safe headway
        return;
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
    spawnTimer += deltaTime;

    if (spawnTimer >= spawnInterval)
    {
        spawnTimer = 0.f;
        spawn(road, assets, speedBoost);
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
}
