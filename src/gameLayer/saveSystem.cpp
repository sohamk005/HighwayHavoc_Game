#include "saveSystem.h"

#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cctype>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

std::string SaveSystem::resolveDefaultSaveDir()
{
#ifdef _WIN32
    const char* appData = std::getenv("APPDATA");
    if (appData && *appData)
    {
        return (std::filesystem::path(appData) / "HighwayHavoc").string();
    }
    const char* localAppData = std::getenv("LOCALAPPDATA");
    if (localAppData && *localAppData)
    {
        return (std::filesystem::path(localAppData) / "HighwayHavoc").string();
    }
    const char* userProfile = std::getenv("USERPROFILE");
    if (userProfile && *userProfile)
    {
        return (std::filesystem::path(userProfile) / "AppData" / "Roaming" / "HighwayHavoc").string();
    }
#endif
    return "./save";
}

void SaveSystem::init(const std::string& customSaveDir)
{
    if (!customSaveDir.empty())
    {
        m_saveDir = customSaveDir;
    }
    else
    {
        m_saveDir = resolveDefaultSaveDir();
    }

    std::error_code ec;
    std::filesystem::create_directories(m_saveDir, ec);

    m_saveFilePath = m_saveDir / "highscore.txt";
    m_tempFilePath = m_saveDir / "highscore.txt.tmp";
    m_walletFilePath = m_saveDir / "wallet.txt";
    m_walletTempPath = m_saveDir / "wallet.txt.tmp";
    m_walletPendingPath = m_saveDir / "wallet_pending.txt";
    m_isInitialized = true;
}

bool SaveSystem::validateScoreString(const std::string& str, uint32_t& outScore)
{
    // Trim leading whitespace
    size_t start = 0;
    while (start < str.size() && std::isspace(static_cast<unsigned char>(str[start])))
    {
        start++;
    }

    // Trim trailing whitespace
    size_t end = str.size();
    while (end > start && std::isspace(static_cast<unsigned char>(str[end - 1])))
    {
        end--;
    }

    if (start >= end)
    {
        // Empty or whitespace only
        return false;
    }

    std::string trimmed = str.substr(start, end - start);

    // Reject non-numeric characters (negative signs, alpha, punctuation)
    for (char c : trimmed)
    {
        if (!std::isdigit(static_cast<unsigned char>(c)))
        {
            return false;
        }
    }

    // Length check: UINT32_MAX (4294967295) is 10 digits
    if (trimmed.length() > 10)
    {
        return false;
    }

    try
    {
        unsigned long long val = std::stoull(trimmed);
        if (val > static_cast<unsigned long long>(UINT32_MAX))
        {
            return false;
        }
        outScore = static_cast<uint32_t>(val);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

uint32_t SaveSystem::loadHighScore()
{
    if (!m_isInitialized)
    {
        init();
    }

    std::error_code ec;
    if (!std::filesystem::exists(m_saveFilePath, ec))
    {
        // Normal first launch: no save file exists yet
        m_cachedHighScore = 0;
        return 0;
    }

    std::ifstream in(m_saveFilePath, std::ios::in);
    if (!in.is_open())
    {
        m_cachedHighScore = 0;
        return 0;
    }

    std::string content;
    // Read up to 256 characters (a score string needs <= 10)
    char buffer[256] = {};
    in.read(buffer, sizeof(buffer) - 1);
    content = buffer;
    in.close();

    uint32_t parsedScore = 0;
    if (validateScoreString(content, parsedScore))
    {
        m_cachedHighScore = parsedScore;
        return parsedScore;
    }

    // Corrupted / malformed save: reject safely and fallback to 0
    m_cachedHighScore = 0;
    return 0;
}

bool SaveSystem::saveHighScore(uint32_t score)
{
    if (!m_isInitialized)
    {
        init();
    }

    // A lower score must never overwrite a higher stored score
    if (score < m_cachedHighScore)
    {
        return true;
    }

    std::error_code ec;
    std::filesystem::create_directories(m_saveDir, ec);

    // 1. Write to temporary file
    {
        std::ofstream out(m_tempFilePath, std::ios::out | std::ios::trunc);
        if (!out.is_open())
        {
            return false;
        }

        out << score << "\n";
        out.flush();

        if (!out.good())
        {
            out.close();
            std::filesystem::remove(m_tempFilePath, ec);
            return false;
        }
        out.close();
    }

    // 2. Safe atomic replacement of target file
#ifdef _WIN32
    BOOL ok = MoveFileExA(m_tempFilePath.string().c_str(),
                          m_saveFilePath.string().c_str(),
                          MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
    if (!ok)
    {
        std::filesystem::remove(m_tempFilePath, ec);
        return false;
    }
#else
    std::filesystem::rename(m_tempFilePath, m_saveFilePath, ec);
    if (ec)
    {
        std::filesystem::remove(m_tempFilePath, ec);
        return false;
    }
#endif

    m_cachedHighScore = score;
    return true;
}

bool SaveSystem::saveHighScoreIfHigher(uint32_t score)
{
    if (score > m_cachedHighScore)
    {
        return saveHighScore(score);
    }
    return true;
}

// ==========================================
// V2-M2: Persistent Wallet & Banking System
// ==========================================

uint32_t SaveSystem::saturatingAdd(uint32_t a, uint32_t b)
{
    if (UINT32_MAX - a < b)
    {
        return UINT32_MAX;
    }
    return a + b;
}

bool SaveSystem::validateWalletString(const std::string& str, uint32_t& outBalance, uint64_t& outRunId)
{
    outBalance = 0;
    outRunId = 0;

    std::istringstream iss(str);
    std::string token1;
    if (!(iss >> token1))
    {
        return false; // Empty or whitespace only
    }

    // Validate token1 (balance)
    for (char c : token1)
    {
        if (!std::isdigit(static_cast<unsigned char>(c)))
            return false;
    }
    if (token1.length() > 10)
        return false;

    try
    {
        unsigned long long bVal = std::stoull(token1);
        if (bVal > static_cast<unsigned long long>(UINT32_MAX))
            return false;
        outBalance = static_cast<uint32_t>(bVal);
    }
    catch (...)
    {
        return false;
    }

    // Check optional token2 (runId)
    std::string token2;
    if (iss >> token2)
    {
        for (char c : token2)
        {
            if (!std::isdigit(static_cast<unsigned char>(c)))
                return false;
        }
        if (token2.length() > 20)
            return false;

        try
        {
            outRunId = std::stoull(token2);
        }
        catch (...)
        {
            return false;
        }
    }

    // Reject any extra trailing non-whitespace content
    std::string extra;
    if (iss >> extra)
    {
        return false;
    }

    return true;
}

void SaveSystem::recoverPendingTransaction()
{
    std::error_code ec;
    if (!std::filesystem::exists(m_walletPendingPath, ec))
    {
        return;
    }

    std::ifstream pIn(m_walletPendingPath, std::ios::in);
    if (!pIn.is_open())
    {
        return;
    }

    std::string content;
    char buffer[128] = {};
    pIn.read(buffer, sizeof(buffer) - 1);
    content = buffer;
    pIn.close();

    std::istringstream iss(content);
    uint64_t pendingRunId = 0;
    uint32_t pendingCoins = 0;
    if (iss >> pendingRunId >> pendingCoins)
    {
        uint32_t diskBalance = 0;
        uint64_t diskRunId = 0;
        if (std::filesystem::exists(m_walletFilePath, ec))
        {
            std::ifstream wIn(m_walletFilePath, std::ios::in);
            if (wIn.is_open())
            {
                std::string wContent;
                char wBuffer[256] = {};
                wIn.read(wBuffer, sizeof(wBuffer) - 1);
                wContent = wBuffer;
                wIn.close();
                validateWalletString(wContent, diskBalance, diskRunId);
            }
        }

        // If pending run was already recorded in wallet.txt, no re-credit needed
        if (diskRunId == pendingRunId && pendingRunId != 0)
        {
            m_cachedWalletBalance = diskBalance;
            m_lastBankedRunId = diskRunId;
            std::filesystem::remove(m_walletPendingPath, ec);
            return;
        }

        // Complete the uncommitted transaction
        if (pendingCoins > 0)
        {
            uint32_t newBalance = saturatingAdd(diskBalance, pendingCoins);
            saveWallet(newBalance, pendingRunId);
        }
    }

    std::filesystem::remove(m_walletPendingPath, ec);
}

uint32_t SaveSystem::loadWallet()
{
    if (!m_isInitialized)
    {
        init();
    }

    recoverPendingTransaction();

    std::error_code ec;
    if (!std::filesystem::exists(m_walletFilePath, ec))
    {
        m_cachedWalletBalance = 0;
        m_lastBankedRunId = 0;
        return 0;
    }

    std::ifstream in(m_walletFilePath, std::ios::in);
    if (!in.is_open())
    {
        m_cachedWalletBalance = 0;
        m_lastBankedRunId = 0;
        return 0;
    }

    std::string content;
    char buffer[256] = {};
    in.read(buffer, sizeof(buffer) - 1);
    content = buffer;
    in.close();

    uint32_t parsedBalance = 0;
    uint64_t parsedRunId = 0;
    if (validateWalletString(content, parsedBalance, parsedRunId))
    {
        m_cachedWalletBalance = parsedBalance;
        m_lastBankedRunId = parsedRunId;
        return parsedBalance;
    }

    m_cachedWalletBalance = 0;
    m_lastBankedRunId = 0;
    return 0;
}

bool SaveSystem::saveWallet(uint32_t balance, uint64_t runId)
{
    if (!m_isInitialized)
    {
        init();
    }

    std::error_code ec;
    std::filesystem::create_directories(m_saveDir, ec);

    // 1. Write to temporary file
    {
        std::ofstream out(m_walletTempPath, std::ios::out | std::ios::trunc);
        if (!out.is_open())
        {
            return false;
        }

        out << balance << "\n" << runId << "\n";
        out.flush();

        if (!out.good())
        {
            out.close();
            std::filesystem::remove(m_walletTempPath, ec);
            return false;
        }
        out.close();
    }

    // 2. Safe atomic replacement of target file
#ifdef _WIN32
    BOOL ok = MoveFileExA(m_walletTempPath.string().c_str(),
                          m_walletFilePath.string().c_str(),
                          MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
    if (!ok)
    {
        std::filesystem::remove(m_walletTempPath, ec);
        return false;
    }
#else
    std::filesystem::rename(m_walletTempPath, m_walletFilePath, ec);
    if (ec)
    {
        std::filesystem::remove(m_walletTempPath, ec);
        return false;
    }
#endif

    m_cachedWalletBalance = balance;
    m_lastBankedRunId = runId;
    return true;
}

bool SaveSystem::bankRun(uint64_t runId, uint32_t runCoins)
{
    if (!m_isInitialized)
    {
        init();
    }

    // Runs with 0 coins require no updates (prevent unnecessary disk I/O)
    if (runCoins == 0)
    {
        return true;
    }

    // Exactly-once: if runId is already banked, skip redundant credit
    if (runId != 0 && runId == m_lastBankedRunId)
    {
        return true;
    }

    std::error_code ec;

    // 1. Durable pending-transaction record (for crash recovery)
    {
        std::ofstream pOut(m_walletPendingPath, std::ios::out | std::ios::trunc);
        if (pOut.is_open())
        {
            pOut << runId << " " << runCoins << "\n";
            pOut.flush();
            pOut.close();
        }
    }

    // 2. Compute new balance with saturating addition
    uint32_t newBalance = saturatingAdd(m_cachedWalletBalance, runCoins);

    // 3. Commit atomically to wallet.txt
    bool saved = saveWallet(newBalance, runId);

    // 4. Remove pending record upon successful commit
    if (saved)
    {
        std::filesystem::remove(m_walletPendingPath, ec);
    }

    return saved;
}
