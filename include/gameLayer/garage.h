#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <filesystem>
#include <gl2d/gl2d.h>

#include "assetManager.h"
#include "saveSystem.h"

struct VehicleEntry
{
    uint32_t id = 0;
    std::string name;
    uint32_t price = 0;
    gl2d::Texture* texture = nullptr;
};

class GarageManager
{
public:
    static constexpr size_t VEHICLE_COUNT = 4;

    GarageManager() = default;
    ~GarageManager() = default;

    void init(const std::string& customSaveDir = "");

    // Setup catalogue textures from loaded AssetManager
    void bindAssets(AssetManager& assets);

    // Queries
    const std::vector<VehicleEntry>& getCatalogue() const { return m_catalogue; }
    const VehicleEntry& getVehicle(uint32_t id) const;
    const VehicleEntry& getSelectedVehicle() const;
    uint32_t getSelectedVehicleId() const { return m_selectedVehicleId; }
    bool isOwned(uint32_t id) const;

    // Selection
    bool selectVehicle(uint32_t id);

    // Purchase transaction with safe two-phase recovery
    bool buyVehicle(uint32_t id, SaveSystem& saveSystem);

    // Persistence
    bool loadGarage();
    bool saveGarage();
    void recoverPendingPurchase(SaveSystem& saveSystem);

    // Validation
    static bool validateGarageString(const std::string& str, uint32_t& outSelected, std::vector<uint32_t>& outOwned);

    // Path inspection
    std::string getGarageFilePath() const { return m_garageFilePath.string(); }

private:
    std::vector<VehicleEntry> m_catalogue;
    std::vector<uint32_t> m_ownedVehicles;
    uint32_t m_selectedVehicleId = 0;

    std::filesystem::path m_saveDir;
    std::filesystem::path m_garageFilePath;
    std::filesystem::path m_garageTempPath;
    std::filesystem::path m_purchasePendingPath;
    bool m_isInitialized = false;

    static std::string resolveDefaultSaveDir();
};
