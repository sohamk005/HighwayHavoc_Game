#include "garage.h"
#include <algorithm>
#include <fstream>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#endif

std::string GarageManager::resolveDefaultSaveDir()
{
#ifdef _WIN32
    char appDataPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, appDataPath)))
    {
        return (std::filesystem::path(appDataPath) / "HighwayHavoc").string();
    }
#endif
    return "./save";
}

void GarageManager::init(const std::string& customSaveDir)
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

    m_garageFilePath = m_saveDir / "garage.txt";
    m_garageTempPath = m_saveDir / "garage.txt.tmp";
    m_purchasePendingPath = m_saveDir / "purchase_pending.txt";

    // Setup static catalogue entries
    m_catalogue.clear();
    m_catalogue.push_back({ 0, "BLUE CRUISER", 0, nullptr });
    m_catalogue.push_back({ 1, "RED FURY", 50, nullptr });
    m_catalogue.push_back({ 2, "YELLOW PHANTOM", 120, nullptr });
    m_catalogue.push_back({ 3, "SHADOW ELITE", 250, nullptr });

    m_ownedVehicles = { 0 };
    m_selectedVehicleId = 0;
    m_isInitialized = true;
}

void GarageManager::bindAssets(AssetManager& assets)
{
    if (m_catalogue.size() >= 4)
    {
        m_catalogue[0].texture = &assets.blueCar;
        m_catalogue[1].texture = &assets.redCar;
        m_catalogue[2].texture = &assets.yellowCar;
        m_catalogue[3].texture = &assets.blackCar;
    }
}

const VehicleEntry& GarageManager::getVehicle(uint32_t id) const
{
    for (const auto& v : m_catalogue)
    {
        if (v.id == id)
            return v;
    }
    return m_catalogue[0];
}

const VehicleEntry& GarageManager::getSelectedVehicle() const
{
    return getVehicle(m_selectedVehicleId);
}

bool GarageManager::isOwned(uint32_t id) const
{
    if (id == 0) return true; // Default vehicle is always owned
    return std::find(m_ownedVehicles.begin(), m_ownedVehicles.end(), id) != m_ownedVehicles.end();
}

bool GarageManager::selectVehicle(uint32_t id)
{
    if (id >= m_catalogue.size() || !isOwned(id))
    {
        return false;
    }

    m_selectedVehicleId = id;
    saveGarage();
    return true;
}

bool GarageManager::validateGarageString(const std::string& str, uint32_t& outSelected, std::vector<uint32_t>& outOwned)
{
    outSelected = 0;
    outOwned.clear();

    std::istringstream iss(str);
    uint32_t selectedId = 0;
    size_t ownedCount = 0;

    if (!(iss >> selectedId >> ownedCount))
    {
        return false;
    }

    if (ownedCount == 0 || ownedCount > VEHICLE_COUNT)
    {
        return false;
    }

    std::vector<uint32_t> parsedOwned;
    for (size_t i = 0; i < ownedCount; ++i)
    {
        uint32_t id = 0;
        if (!(iss >> id))
        {
            return false;
        }
        if (id >= VEHICLE_COUNT)
        {
            return false;
        }
        if (std::find(parsedOwned.begin(), parsedOwned.end(), id) == parsedOwned.end())
        {
            parsedOwned.push_back(id);
        }
    }

    // Vehicle 0 must always be owned
    if (std::find(parsedOwned.begin(), parsedOwned.end(), 0) == parsedOwned.end())
    {
        parsedOwned.push_back(0);
    }
    std::sort(parsedOwned.begin(), parsedOwned.end());

    // Selected ID must be owned and valid
    if (selectedId >= VEHICLE_COUNT || std::find(parsedOwned.begin(), parsedOwned.end(), selectedId) == parsedOwned.end())
    {
        selectedId = 0;
    }

    outSelected = selectedId;
    outOwned = parsedOwned;
    return true;
}

void GarageManager::recoverPendingPurchase(SaveSystem& saveSystem)
{
    std::error_code ec;
    if (!std::filesystem::exists(m_purchasePendingPath, ec))
    {
        return;
    }

    std::ifstream pIn(m_purchasePendingPath, std::ios::in);
    if (!pIn.is_open())
    {
        return;
    }

    uint32_t pendingVehicleId = 0;
    uint32_t pendingPrice = 0;
    uint32_t preBalance = 0;
    uint32_t postBalance = 0;

    if (pIn >> pendingVehicleId >> pendingPrice >> preBalance >> postBalance)
    {
        pIn.close();

        // Check if garage already reflects ownership of pendingVehicleId
        bool alreadyOwned = isOwned(pendingVehicleId);

        if (!alreadyOwned && pendingVehicleId < VEHICLE_COUNT)
        {
            // Wallet deduction was recorded in pending journal. Check current disk wallet balance.
            uint32_t curWallet = saveSystem.loadWallet();

            // If wallet was already deducted to postBalance (or less), commit the ownership!
            if (curWallet <= postBalance)
            {
                m_ownedVehicles.push_back(pendingVehicleId);
                std::sort(m_ownedVehicles.begin(), m_ownedVehicles.end());
                m_selectedVehicleId = pendingVehicleId;
                saveGarage();
            }
            // If wallet was NOT deducted (crash occurred before wallet save), do NOT deduct now, discard journal.
        }
    }
    else
    {
        pIn.close();
    }

    std::filesystem::remove(m_purchasePendingPath, ec);
}

bool GarageManager::loadGarage()
{
    if (!m_isInitialized)
    {
        init();
    }

    std::error_code ec;
    if (!std::filesystem::exists(m_garageFilePath, ec))
    {
        m_ownedVehicles = { 0 };
        m_selectedVehicleId = 0;
        return true;
    }

    std::ifstream in(m_garageFilePath, std::ios::in);
    if (!in.is_open())
    {
        m_ownedVehicles = { 0 };
        m_selectedVehicleId = 0;
        return true;
    }

    std::string content;
    char buffer[256] = {};
    in.read(buffer, sizeof(buffer) - 1);
    content = buffer;
    in.close();

    uint32_t parsedSelected = 0;
    std::vector<uint32_t> parsedOwned;
    if (validateGarageString(content, parsedSelected, parsedOwned))
    {
        m_selectedVehicleId = parsedSelected;
        m_ownedVehicles = parsedOwned;
        return true;
    }

    // Corrupted file: fallback safely to vehicle 0 owned and selected
    m_ownedVehicles = { 0 };
    m_selectedVehicleId = 0;
    return true;
}

bool GarageManager::saveGarage()
{
    if (!m_isInitialized)
    {
        init();
    }

    std::error_code ec;
    std::filesystem::create_directories(m_saveDir, ec);

    // 1. Write to temporary file
    {
        std::ofstream out(m_garageTempPath, std::ios::out | std::ios::trunc);
        if (!out.is_open())
        {
            return false;
        }

        out << m_selectedVehicleId << "\n";
        out << m_ownedVehicles.size() << "\n";
        for (size_t i = 0; i < m_ownedVehicles.size(); ++i)
        {
            out << m_ownedVehicles[i] << (i + 1 < m_ownedVehicles.size() ? " " : "\n");
        }
        out.flush();

        if (!out.good())
        {
            out.close();
            std::filesystem::remove(m_garageTempPath, ec);
            return false;
        }
        out.close();
    }

    // 2. Safe atomic replacement of target file
#ifdef _WIN32
    BOOL ok = MoveFileExA(m_garageTempPath.string().c_str(),
                          m_garageFilePath.string().c_str(),
                          MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
    if (!ok)
    {
        std::filesystem::remove(m_garageTempPath, ec);
        return false;
    }
#else
    std::filesystem::rename(m_garageTempPath, m_garageFilePath, ec);
    if (ec)
    {
        std::filesystem::remove(m_garageTempPath, ec);
        return false;
    }
#endif

    return true;
}

bool GarageManager::buyVehicle(uint32_t id, SaveSystem& saveSystem)
{
    if (!m_isInitialized)
    {
        init();
    }

    // 1. Validation
    if (id >= m_catalogue.size() || isOwned(id))
    {
        return false; // Invalid or already owned
    }

    uint32_t price = m_catalogue[id].price;
    uint32_t curWallet = saveSystem.getCachedWalletBalance();
    if (curWallet < price)
    {
        return false; // Insufficient balance
    }

    uint32_t newWallet = curWallet - price;
    std::error_code ec;

    // 2. Write durable purchase journal before initiating two-file update
    {
        std::ofstream pOut(m_purchasePendingPath, std::ios::out | std::ios::trunc);
        if (pOut.is_open())
        {
            pOut << id << " " << price << " " << curWallet << " " << newWallet << "\n";
            pOut.flush();
            pOut.close();
        }
    }

    // 3. Atomically update wallet first
    bool walletSaved = saveSystem.saveWallet(newWallet, saveSystem.getLastBankedRunId());
    if (!walletSaved)
    {
        // Wallet save failed; abort transaction cleanly
        std::filesystem::remove(m_purchasePendingPath, ec);
        return false;
    }

    // 4. Update in-memory garage state
    m_ownedVehicles.push_back(id);
    std::sort(m_ownedVehicles.begin(), m_ownedVehicles.end());
    m_selectedVehicleId = id;

    // 5. Atomically update garage file
    bool garageSaved = saveGarage();
    if (!garageSaved)
    {
        // Retry garage save once
        garageSaved = saveGarage();
    }

    // 6. Purge pending journal upon success
    if (garageSaved)
    {
        std::filesystem::remove(m_purchasePendingPath, ec);
    }

    return true;
}
