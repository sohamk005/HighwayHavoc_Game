#pragma once

#include <gl2d/gl2d.h>

struct AssetManager
{
    // Cars
    gl2d::Texture blueCar;
    gl2d::Texture redCar;
    gl2d::Texture greenCar;
    gl2d::Texture yellowCar;

    // Roads
    gl2d::Texture roadStraight;
    gl2d::Texture roadCurveLeft;
    gl2d::Texture roadCurveRight;

    // Environment
    gl2d::Texture treeLarge;
    gl2d::Texture treeSmall;
    gl2d::Texture barrier;
    gl2d::Texture cone;
    gl2d::Texture rock;
    // Ground
    gl2d::Texture grass;

    // UI Font
    gl2d::Font font;

    void loadAssets();
    void freeAssets();
};