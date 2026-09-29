#pragma once

#include <cstdint>
#include <algorithm>

struct ScoreSystem
{
    double distanceTraveled = 0.0;
    uint32_t currentScore = 0;
    uint32_t highScore = 0;

    // Continuous normalized difficulty factor [0.0f, 1.0f]
    float difficultyFactor = 0.f;

    // Tuning constants
    static constexpr float DISTANCE_PER_SCORE_POINT = 10.0f; // 1 score point per 10 pixels scrolled
    static constexpr float MAX_DIFFICULTY_SCORE = 5000.0f;  // Score at which difficulty reaches maximum (1.0)

    // Baseline and bounded difficulty scaling parameters
    static constexpr float BASE_SPAWN_INTERVAL = 2.0f;      // Baseline traffic spawn interval (seconds)
    static constexpr float MIN_SPAWN_INTERVAL = 1.0f;       // Minimum traffic spawn interval at max difficulty

    static constexpr float BASE_MAX_SCROLL_SPEED = 600.0f;  // Baseline maximum world scroll speed (px/s)
    static constexpr float SCALED_MAX_SCROLL_SPEED = 750.0f;// Maximum world scroll speed at max difficulty

    static constexpr float BASE_TRAFFIC_SPEED_MIN = 160.0f; // Baseline traffic minimum forward speed
    static constexpr float BASE_TRAFFIC_SPEED_RANGE = 80.0f;// Baseline traffic speed variance [160, 240]
    static constexpr float MAX_TRAFFIC_SPEED_BOOST = 60.0f; // Traffic speed boost at max difficulty [220, 300]

    // Updates distance, score, high score, and difficulty progression.
    // Progression is strictly halted if isDead is true.
    inline void update(float scrollSpeed, float deltaTime, bool isDead)
    {
        if (isDead)
            return;

        if (scrollSpeed > 0.f && deltaTime > 0.f)
        {
            distanceTraveled += static_cast<double>(scrollSpeed) * static_cast<double>(deltaTime);
            currentScore = static_cast<uint32_t>((distanceTraveled + 1e-4) / DISTANCE_PER_SCORE_POINT);

            if (currentScore > highScore)
            {
                highScore = currentScore;
            }

            difficultyFactor = std::clamp(static_cast<float>(currentScore) / MAX_DIFFICULTY_SCORE, 0.0f, 1.0f);
        }
    }

    // Resets run-specific metrics (score, distance, difficulty) on restart/R.
    // Retains highScore across resets during the application session.
    inline void resetRun()
    {
        distanceTraveled = 0.0;
        currentScore = 0;
        difficultyFactor = 0.f;
    }

    // Resets all session metrics including high score.
    inline void resetAll()
    {
        resetRun();
        highScore = 0;
    }

    // Returns current traffic spawn interval scaled by difficulty [2.0s -> 1.0s].
    inline float getTrafficSpawnInterval() const
    {
        return BASE_SPAWN_INTERVAL - difficultyFactor * (BASE_SPAWN_INTERVAL - MIN_SPAWN_INTERVAL);
    }

    // Returns current maximum world scroll speed scaled by difficulty [600.f -> 750.f].
    inline float getMaxScrollSpeed() const
    {
        return BASE_MAX_SCROLL_SPEED + difficultyFactor * (SCALED_MAX_SCROLL_SPEED - BASE_MAX_SCROLL_SPEED);
    }

    // Returns forward speed bonus applied to newly spawned traffic [0.f -> 60.f].
    inline float getTrafficSpeedBoost() const
    {
        return difficultyFactor * MAX_TRAFFIC_SPEED_BOOST;
    }
};
