#include "config_file_creator.h"

#include <unordered_map>

#include "../utils/helper.h"
#include "../utils/file_system_helper.h"
#include "../engine/src/core/input/input_value_constants.h"

static std::unordered_map<std::string, std::string> INPUT_TO_SCRIPT_CONSTANT_MAP = {
    { INPUT_VALUE_ZERO, "Input.Keyboard.NUM_0" },
    { INPUT_VALUE_ONE, "Input.Keyboard.NUM_1" },
    { INPUT_VALUE_TWO, "Input.Keyboard.NUM_2" },
    { INPUT_VALUE_THREE, "Input.Keyboard.NUM_3" },
    { INPUT_VALUE_FOUR, "Input.Keyboard.NUM_4" },
    { INPUT_VALUE_FIVE, "Input.Keyboard.NUM_5" },
    { INPUT_VALUE_SIX, "Input.Keyboard.NUM_6" },
    { INPUT_VALUE_SEVEN, "Input.Keyboard.NUM_7" },
    { INPUT_VALUE_EIGHT, "Input.Keyboard.NUM_8" },
    { INPUT_VALUE_NINE, "Input.Keyboard.NUM_9" },
    { INPUT_VALUE_A, "Input.Keyboard.A" },
    { INPUT_VALUE_B, "Input.Keyboard.B" },
    { INPUT_VALUE_C, "Input.Keyboard.C" },
    { INPUT_VALUE_D, "Input.Keyboard.D" },
    { INPUT_VALUE_E, "Input.Keyboard.E" },
    { INPUT_VALUE_F, "Input.Keyboard.F" },
    { INPUT_VALUE_G, "Input.Keyboard.G" },
    { INPUT_VALUE_H, "Input.Keyboard.H" },
    { INPUT_VALUE_I, "Input.Keyboard.I" },
    { INPUT_VALUE_J, "Input.Keyboard.J" },
    { INPUT_VALUE_K, "Input.Keyboard.K" },
    { INPUT_VALUE_L, "Input.Keyboard.L" },
    { INPUT_VALUE_M, "Input.Keyboard.M" },
    { INPUT_VALUE_N, "Input.Keyboard.N" },
    { INPUT_VALUE_O, "Input.Keyboard.O" },
    { INPUT_VALUE_P, "Input.Keyboard.P" },
    { INPUT_VALUE_Q, "Input.Keyboard.Q" },
    { INPUT_VALUE_R, "Input.Keyboard.R" },
    { INPUT_VALUE_S, "Input.Keyboard.S" },
    { INPUT_VALUE_T, "Input.Keyboard.T" },
    { INPUT_VALUE_U, "Input.Keyboard.U" },
    { INPUT_VALUE_V, "Input.Keyboard.V" },
    { INPUT_VALUE_W, "Input.Keyboard.W" },
    { INPUT_VALUE_X, "Input.Keyboard.X" },
    { INPUT_VALUE_Y, "Input.Keyboard.Y" },
    { INPUT_VALUE_Z, "Input.Keyboard.Z" },
    { INPUT_VALUE_LEFT, "Input.Keyboard.LEFT" },
    { INPUT_VALUE_RIGHT, "Input.Keyboard.RIGHT" },
    { INPUT_VALUE_UP, "Input.Keyboard.UP" },
    { INPUT_VALUE_DOWN, "Input.Keyboard.DOWN" },
    { INPUT_VALUE_RETURN, "Input.Keyboard.RETURN" },
    { INPUT_VALUE_SPACE, "Input.Keyboard.SPACE" },
    { INPUT_VALUE_ESCAPE, "Input.Keyboard.ESCAPE" },
    { INPUT_VALUE_F1, "Input.Keyboard.F1" },
    { INPUT_VALUE_F2, "Input.Keyboard.F2" },
    { INPUT_VALUE_F3, "Input.Keyboard.F3" },
    { INPUT_VALUE_F4, "Input.Keyboard.F4" },
    { INPUT_VALUE_F5, "Input.Keyboard.F5" },
    { INPUT_VALUE_F6, "Input.Keyboard.F6" },
    { INPUT_VALUE_F7, "Input.Keyboard.F7" },
    { INPUT_VALUE_F8, "Input.Keyboard.F8" },
    { INPUT_VALUE_F9, "Input.Keyboard.F9" },
    { INPUT_VALUE_F10, "Input.Keyboard.F10" },
    { INPUT_VALUE_F11, "Input.Keyboard.F11" },
    { INPUT_VALUE_F12, "Input.Keyboard.F12" },
    { INPUT_VALUE_JOYSTICK_BUTTON_A, "Input.Gamepad.BUTTON_A" },
    { INPUT_VALUE_JOYSTICK_BUTTON_B, "Input.Gamepad.BUTTON_B" },
    { INPUT_VALUE_JOYSTICK_BUTTON_X, "Input.Gamepad.BUTTON_X" },
    { INPUT_VALUE_JOYSTICK_BUTTON_Y, "Input.Gamepad.BUTTON_Y" },
    { INPUT_VALUE_JOYSTICK_DPAD_LEFT, "Input.Gamepad.BUTTON_DPAD_LEFT" },
    { INPUT_VALUE_JOYSTICK_DPAD_RIGHT, "Input.Gamepad.BUTTON_DPAD_RIGHT" },
    { INPUT_VALUE_JOYSTICK_DPAD_UP, "Input.Gamepad.BUTTON_DPAD_UP" },
    { INPUT_VALUE_JOYSTICK_DPAD_DOWN, "Input.Gamepad.BUTTON_DPAD_DOWN" },
    { INPUT_VALUE_JOYSTICK_START, "Input.Gamepad.BUTTON_START" },
    { INPUT_VALUE_JOYSTICK_BACK, "Input.Gamepad.BUTTON_BACK" },
    { INPUT_VALUE_JOYSTICK_LEFT_SHOULDER, "Input.Gamepad.BUTTON_LEFT_SHOULDER" },
    { INPUT_VALUE_JOYSTICK_RIGHT_SHOULDER, "Input.Gamepad.BUTTON_RIGHT_SHOULDER" },
    { INPUT_VALUE_JOYSTICK_LEFT_TRIGGER, "Input.Gamepad.AXIS_LEFT_TRIGGER" },
    { INPUT_VALUE_JOYSTICK_RIGHT_TRIGGER, "Input.Gamepad.AXIS_RIGHT_TRIGGER" },
    { INPUT_VALUE_JOYSTICK_LEFT_ANALOG, "Input.Gamepad.BUTTON_LEFT_ANALOG" },
    { INPUT_VALUE_JOYSTICK_RIGHT_ANALOG, "Input.Gamepad.BUTTON_RIGHT_ANALOG" },
    { INPUT_VALUE_JOYSTICK_LEFT_ANALOG_LEFT, "Input.Gamepad.AXIS_LEFT_ANALOG_LEFT" },
    { INPUT_VALUE_JOYSTICK_LEFT_ANALOG_RIGHT, "Input.Gamepad.AXIS_LEFT_ANALOG_RIGHT" },
    { INPUT_VALUE_JOYSTICK_LEFT_ANALOG_UP, "Input.Gamepad.AXIS_LEFT_ANALOG_UP" },
    { INPUT_VALUE_JOYSTICK_LEFT_ANALOG_DOWN, "Input.Gamepad.AXIS_LEFT_ANALOG_DOWN" },
    { INPUT_VALUE_JOYSTICK_RIGHT_ANALOG_LEFT, "Input.Gamepad.AXIS_RIGHT_ANALOG_LEFT" },
    { INPUT_VALUE_JOYSTICK_RIGHT_ANALOG_RIGHT, "Input.Gamepad.AXIS_RIGHT_ANALOG_RIGHT" },
    { INPUT_VALUE_JOYSTICK_RIGHT_ANALOG_UP, "Input.Gamepad.AXIS_RIGHT_ANALOG_UP" },
    { INPUT_VALUE_JOYSTICK_RIGHT_ANALOG_DOWN, "Input.Gamepad.AXIS_RIGHT_ANALOG_DOWN" },
};

namespace {
std::string GetInputValueConstant(const std::string& inputValue) {
    if (INPUT_TO_SCRIPT_CONSTANT_MAP.count(inputValue) > 0) {
        return INPUT_TO_SCRIPT_CONSTANT_MAP[inputValue];
    }
    return "\"" + inputValue + "\"";
}
} // namespace

void ConfigFileCreator::GenerateConfigFile(const char *filePath, ProjectProperties* properties) {
    std::string fileContents = "from crescent_api import *\n";
    // Configure Game
    fileContents += "\n";
    fileContents += "configure_game(\n";
    fileContents += "   game_title=\"" + properties->gameTitle + "\",\n";
    fileContents += "   window_width=" + std::to_string(properties->windowWidth) + ",\n";
    fileContents += "   window_height=" + std::to_string(properties->windowHeight) + ",\n";
    fileContents += "   resolution_width=" + std::to_string(properties->resolutionWidth) + ",\n";
    fileContents += "   resolution_height=" + std::to_string(properties->resolutionHeight) + ",\n";
    fileContents += "   target_fps=" + std::to_string(properties->targetFPS) + ",\n";
    fileContents += "   initial_node_path=\"" + properties->initialNodePath + "\",\n";
    fileContents += "   colliders_visible=" + Helper::BoolToStringCaptital(properties->areCollidersVisible) + ",\n";
    fileContents += ")\n";
    fileContents += "\n";

    // Configure Assets
    fileContents += "configure_assets(\n";
    // Audio Sources
    fileContents += "   audio_sources=[\n";
    for (const auto& audioSource : properties->assets.audioSources) {
        fileContents += "       AudioSource(file_path=\"" + std::string(audioSource.file_path) + "\"),\n";
    }
    fileContents += "   ],\n";
    // Textures
    fileContents += "   textures=[\n";
    for (const auto& texture : properties->assets.textures) {
        fileContents += "       Texture(\n";
        fileContents += "           file_path=\"" + std::string(texture.file_path) + "\",\n";
        fileContents += "           wrap_s=\"" + std::string(texture.wrap_s) + "\",\n";
        fileContents += "           wrap_t=\"" + std::string(texture.wrap_t) + "\",\n";
        fileContents += "           filter_min=\"" + std::string(texture.filter_min) + "\",\n";
        fileContents += "           filter_mag=\"" + std::string(texture.filter_mag) + "\",\n";
        fileContents += "       ),\n";
    }
    fileContents += "   ],\n";
    // Fonts
    fileContents += "   fonts=[\n";
    for (const auto& font : properties->assets.fonts) {
        fileContents += "       Font(\n";
        fileContents += "           file_path=\"" + std::string(font.file_path) + "\",\n";
        fileContents += "           uid=\"" + std::string(font.uid) + "\",\n";
        fileContents += "           size=" + std::to_string(font.size) + ",\n";
        fileContents += "       ),\n";
    }
    fileContents += "   ],\n";
    // Closing parentheses for configure assets
    fileContents += ")\n";
    fileContents += "\n";

    // Inputs
    fileContents += "configure_inputs(\n";
    fileContents += "   input_actions=[\n";
    for (const auto& inputAction : properties->inputs.actions) {
        std::string valuesArrayText;
        for (size_t i = 0; i < inputAction.values.size(); i++) {
            const std::string& value = inputAction.values[i];
            const std::string endText = i != inputAction.values.size() - 1 ? ", " : "";
            valuesArrayText += "" + GetInputValueConstant(value) + endText;
        }
        fileContents += "       InputAction(name=\"" + std::string(inputAction.name) + "\", values=[" + valuesArrayText + "]),\n";
    }
    fileContents += "   ],\n";
    fileContents += ")\n";

    FileSystemHelper::WriteFile(filePath, fileContents);
}
