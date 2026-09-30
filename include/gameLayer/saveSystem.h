#pragma once

#include <cstdint>
#include <string>
#include <filesystem>

class SaveSystem
{
public:
    SaveSystem() = default;
    ~SaveSystem() = default;

    // Initializes save directory (defaults to %APPDATA%/HighwayHavoc if customSaveDir is empty)
    void init(const std::string& customSaveDir = "");

    // Loads and validates persistent high score from disk. Returns 0 if missing or invalid.
    uint32_t loadHighScore();

    // Saves high score atomically via temporary file replacement.
    // Does NOT overwrite with a lower score.
    bool saveHighScore(uint32_t score);

    // Saves high score only if greater than currently cached persistent high score.
    bool saveHighScoreIfHigher(uint32_t score);

    // Validates a raw string read from disk.
    // Rejects empty, whitespace-only, negative, alpha, malformed, or overflow values.
    static bool validateScoreString(const std::string& str, uint32_t& outScore);

    // Path & Cache Inspection
    std::string getSaveFilePath() const { return m_saveFilePath.string(); }
    std::string getSaveDirPath() const { return m_saveDir.string(); }
    uint32_t getCachedHighScore() const { return m_cachedHighScore; }
    void setCachedHighScore(uint32_t score) { m_cachedHighScore = score; }
    bool isInitialized() const { return m_isInitialized; }

private:
    std::filesystem::path m_saveDir;
    std::filesystem::path m_saveFilePath;
    std::filesystem::path m_tempFilePath;

    uint32_t m_cachedHighScore = 0;
    bool m_isInitialized = false;

    static std::string resolveDefaultSaveDir();
};
