#pragma once

#include <fstream>
#include <sstream>

#include <nlohmann/json.hpp>

#include <seika/math/math.h>
#include <seika/assert.h>

namespace JsonHelper {
// General
template<typename JsonType = nlohmann::json>
bool HasKey(JsonType& json, const std::string& key) {
    try {
        json.at(key);
        return true;
    } catch (nlohmann::json::type_error&) {
    } catch (nlohmann::json::out_of_range&) {
    }
    return false;
}

template<typename T, typename JsonType = nlohmann::json>
T Get(const JsonType& json, const std::string& key) {
    if (HasKey(json, key)) {
        return json.at(key);
    }
    SKA_ASSERT_FMT(false, "Key '%s' doesn't exist in json!", key.c_str());
    return T();
}

template<typename T, typename JsonType = nlohmann::json>
T GetDefault(const JsonType& json, const std::string& key, T defaultValue) {
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
void SaveFile(const std::string& filePath, const JsonType& outputJson, bool format = true) {
    std::ofstream myFile(filePath);
    const std::string jsonText = format ? outputJson.dump(4) : outputJson.dump();
    myFile << jsonText << "\n";
    myFile.close();
}

template<typename JsonType = nlohmann::json>
nlohmann::json ConvertString(const std::string& jsonString) {
    std::stringstream ss;
    ss << jsonString;
    JsonType outputJson;
    outputJson << ss;
    return outputJson;
}

// Type to json helpers

inline nlohmann::ordered_json Vector2ToJson(SkaVector2 value) {
    nlohmann::ordered_json vec;
    vec["x"] = value.x;
    vec["y"] = value.y;
    return vec;
}

template<typename T = SkaSize2D>
nlohmann::ordered_json Size2DToJson(T value) {
    nlohmann::ordered_json size;
    size["w"] = value.w;
    size["h"] = value.h;
    return size;
}

inline nlohmann::ordered_json Rect2ToJson(const SkaRect2& value) {
    nlohmann::ordered_json rect;
    rect["x"] = value.x;
    rect["y"] = value.y;
    rect["w"] = value.w;
    rect["h"] = value.h;
    return rect;
}

inline nlohmann::ordered_json ColorToJson(const SkaColor& value) {
    nlohmann::ordered_json rect;
    rect["r"] = (int) (value.r * 255);
    rect["g"] = (int) (value.g * 255);
    rect["b"] = (int) (value.b * 255);
    rect["a"] = (int) (value.a * 255);
    return rect;
}

inline nlohmann::ordered_json MinMaxVector2ToJson(const SkaMinMaxVec2& value) {
    nlohmann::ordered_json minmaxVec2;
    minmaxVec2["min"] = Vector2ToJson(value.min);
    minmaxVec2["max"] = Vector2ToJson(value.max);
    return minmaxVec2;
}

} // namespace JsonHelper
