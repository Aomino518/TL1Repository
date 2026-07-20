#pragma once
#include <nlohmann/json.hpp>
#include "LevelData.h"

std::unique_ptr<LevelData> LoadLevelData(const std::string& fullPath);
LevelData::ObjectData ParseObject(const nlohmann::json& object);