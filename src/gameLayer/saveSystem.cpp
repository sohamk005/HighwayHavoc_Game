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
