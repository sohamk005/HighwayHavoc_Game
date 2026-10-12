#pragma once

enum class GameState
{
    MainMenu,
    Garage,
    Playing,
    Paused,
    GameOver
};

inline bool isSimulationActive(GameState state)
{
    return state == GameState::Playing;
}
