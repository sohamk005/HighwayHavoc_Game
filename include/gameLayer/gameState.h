#pragma once

enum class GameState
{
    MainMenu,
    Playing,
    Paused,
    GameOver
};

inline bool isSimulationActive(GameState state)
{
    return state == GameState::Playing;
}
