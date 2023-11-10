#pragma once

#include <fstream>
#include <sstream>

#include <nlohmann/json.hpp>

#include "../seika/utils/se_assert.h"


namespace JsonHelper {
// General
template<typename JsonType = nlohmann::json>
inline bool HasKey(JsonType& json, const std::string& key) {
    try {
        json.at(key);
        return true;
    } catch (nlohmann::json::type_error&) {
    } catch (nlohmann::json::out_of_range&) {
    }
    return false;
}

template<typename T, typename JsonType = nlohmann::json>
inline T Get(const JsonType& json, const std::string& key) {
    if (HasKey(json, key)) {
        return json.at(key);
    }
    SE_ASSERT_FMT(false, "Key '%s' doesn't exist in json!", key.c_str());
    return T();
}

template<typename T, typename JsonType = nlohmann::json>
inline T GetDefault(const JsonType& json, const std::string& key, T defaultValue) {
    if (HasKey(json, key)) {
        return json.at(key);
    }
    return defaultValue;
}

inline bool IsJsonValid(const std::string& jsonText) {
    return nlohmann::json::accept(jsonText);
}

// File Related
template<typename JsonType = nlohmann::json>
inline JsonType LoadFile(const std::string& filePath) {
    std::ifstream i(filePath);
    JsonType json;
    i >> json;
    return json;
}

template<typename JsonType = nlohmann::json>
inline void SaveFile(const std::string& filePath, const JsonType& outputJson, bool format = true) {
    std::ofstream myFile(filePath);
    const std::string jsonText = format ? outputJson.dump(4) : outputJson.dump();
    myFile << jsonText << "\n";
    myFile.close();
}

template<typename JsonType = nlohmann::json>
inline nlohmann::json ConvertString(const std::string& jsonString) {
    std::stringstream ss;
    ss << jsonString;
    JsonType outputJson;
    outputJson << ss;
    return outputJson;
}
} // namespace JsonHelper
