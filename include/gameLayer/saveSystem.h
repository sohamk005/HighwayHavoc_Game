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

    // ==========================================
    // V2-M2: Persistent Wallet & Banking System
    // ==========================================

    // Loads wallet balance and last banked run ID from disk.
    // Handles missing files (defaults to 0), malformed files, and pending crash recovery.
    uint32_t loadWallet();

    // Saves wallet balance and last banked run ID atomically via temporary file replacement.
    bool saveWallet(uint32_t balance, uint64_t runId);

    // Atomically and safely credits coins earned in a run exactly once.
    // Skips redundant calls for already banked runIds and runs with 0 coins.
    bool bankRun(uint64_t runId, uint32_t runCoins);

    // Checks and recovers any pending transaction from an interrupted/crashed banking event.
    void recoverPendingTransaction();

    // Validates wallet data string from disk (supports balance and optional runId).
    static bool validateWalletString(const std::string& str, uint32_t& outBalance, uint64_t& outRunId);

    // Saturating unsigned 32-bit addition (clamps at UINT32_MAX on overflow).
    static uint32_t saturatingAdd(uint32_t a, uint32_t b);

    // ==========================================
    // V2-M4: Audio & Settings Persistence
    // ==========================================

    // Loads music preference from settings.txt. Defaults to true (ON) if missing or malformed.
    bool loadMusicPreference();

    // Saves music preference to settings.txt atomically.
    bool saveMusicPreference(bool enabled);

    // Validates a settings string from disk. Returns true if recognized, outputs parsed boolean.
    static bool validateMusicSettingString(const std::string& str, bool& outEnabled);

    // Path & Cache Inspection
    std::string getSaveFilePath() const { return m_saveFilePath.string(); }
    std::string getSaveDirPath() const { return m_saveDir.string(); }
    std::string getWalletFilePath() const { return m_walletFilePath.string(); }
    std::string getWalletPendingFilePath() const { return m_walletPendingPath.string(); }
    std::string getSettingsFilePath() const { return m_settingsFilePath.string(); }

    uint32_t getCachedHighScore() const { return m_cachedHighScore; }
    void setCachedHighScore(uint32_t score) { m_cachedHighScore = score; }

    uint32_t getCachedWalletBalance() const { return m_cachedWalletBalance; }
    void setCachedWalletBalance(uint32_t balance) { m_cachedWalletBalance = balance; }

    bool getCachedMusicPreference() const { return m_cachedMusicEnabled; }
    void setCachedMusicPreference(bool enabled) { m_cachedMusicEnabled = enabled; }

    uint64_t getLastBankedRunId() const { return m_lastBankedRunId; }

    bool isInitialized() const { return m_isInitialized; }

private:
    std::filesystem::path m_saveDir;
    std::filesystem::path m_saveFilePath;
    std::filesystem::path m_tempFilePath;

    std::filesystem::path m_walletFilePath;
    std::filesystem::path m_walletTempPath;
    std::filesystem::path m_walletPendingPath;

    std::filesystem::path m_settingsFilePath;
    std::filesystem::path m_settingsTempPath;

    uint32_t m_cachedHighScore = 0;
    uint32_t m_cachedWalletBalance = 0;
    uint64_t m_lastBankedRunId = 0;
    bool m_cachedMusicEnabled = true;
    bool m_isInitialized = false;

    static std::string resolveDefaultSaveDir();
};
