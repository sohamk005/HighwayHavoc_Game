#include "assetManager.h"

void AssetManager::loadAssets()
{
    // ===== Cars =====
    blueCar.loadFromFile(
        RESOURCES_PATH "textures/cars/car_blue_1.png", true);

    redCar.loadFromFile(
        RESOURCES_PATH "textures/cars/car_red_1.png", true);

    greenCar.loadFromFile(
        RESOURCES_PATH "textures/cars/car_green_1.png", true);

    yellowCar.loadFromFile(
        RESOURCES_PATH "textures/cars/car_yellow_1.png", true);


    // ===== Roads =====
    roadStraight.loadFromFile(
        RESOURCES_PATH "textures/roads/road_asphalt01.png", true);

    roadCurveLeft.loadFromFile(
        RESOURCES_PATH "textures/roads/road_asphalt22.png", true);

    roadCurveRight.loadFromFile(
        RESOURCES_PATH "textures/roads/road_asphalt24.png", true);


    // ===== Environment =====
    treeLarge.loadFromFile(
        RESOURCES_PATH "textures/environment/tree_large.png", true);

    treeSmall.loadFromFile(
        RESOURCES_PATH "textures/environment/tree_small.png", true);

    barrier.loadFromFile(
        RESOURCES_PATH "textures/environment/barrier_red.png", true);

    cone.loadFromFile(
        RESOURCES_PATH "textures/environment/cone_straight.png", true);

    rock.loadFromFile(
        RESOURCES_PATH "textures/environment/rock_large.png", true);
}

void AssetManager::freeAssets()
{
    // ===== Cars =====
    blueCar.cleanup();
    redCar.cleanup();
    greenCar.cleanup();
    yellowCar.cleanup();

    // ===== Roads =====
    roadStraight.cleanup();
    roadCurveLeft.cleanup();
    roadCurveRight.cleanup();

    // ===== Environment =====
    treeLarge.cleanup();
    treeSmall.cleanup();
    barrier.cleanup();
    cone.cleanup();
    rock.cleanup();
}