#include "input.h"

#include <string.h>

#include "input_action.h"
#include "input_value_constants.h"
#include "../data_structures/se_hash_map_string.h"
#include "../data_structures/se_static_array.h"
#include "../utils/logger.h"
#include "../utils/se_string_util.h"
#include "../utils/se_assert.h"
#include "mouse.h"
#include "../asset/asset_file_loader.h"

#ifdef _MSC_VER
#pragma warning(disable : 4996) // for strcpy
#endif

void input_process_keyboard(SDL_Event event);
void input_process_mouse(SDL_Event event);
void input_load_gamepads();
void input_process_gamepad(SDL_Event event);

void input_initialize_gamepad_system(const char* controllerDBFilePath);
void input_gamepad_cleanup_flags();

//--- Input ---//
SEStringHashMap* inputActionMap = NULL;
char* inputActionNames[SE_MAX_INPUT_ACTIONS];
size_t inputActionNamesCount = 0;

SEStringHashMap* keyboardStringValuesMap = NULL;
SEStringHashMap* gamepadStringValuesMap = NULL;

typedef struct InputFlagCleaner {
    SEInputAction* inputActions[SE_MAX_INPUT_ACTIONS];
    size_t count;
} InputFlagCleaner;

InputFlagCleaner actionJustPressedClean = {0};
InputFlagCleaner actionJustReleasedClean = {0};

//--- Input Frame Event Data ---//
// Input data per frame, cleared at the end of input processing.  Will pass to any listeners who need it

#define SE_INPUT_MAX_EVENT_FRAME_DATA_PER_TYPE 8

typedef enum InputFrameEventDataType {
    InputFrameEventDataType_PRESSED = 0,
    InputFrameEventDataType_RELEASED = 1,
} InputFrameEventDataType;

typedef struct KeyboardFrameEventData {
    SDL_Scancode scancode;
    InputFrameEventDataType type;
} KeyboardFrameEventData;

typedef struct MouseFrameEventData {
    Uint8 mouseButton;
    InputFrameEventDataType type;
} MouseFrameEventData;

typedef struct GamepadFrameEventData {
    SDL_JoystickID deviceId;
    uint8_t buttonValue;
    InputFrameEventDataType type;
} GamepadFrameEventData;

typedef struct InputFrameEventData {
    KeyboardFrameEventData keyboardData[SE_INPUT_MAX_EVENT_FRAME_DATA_PER_TYPE];
    MouseFrameEventData mouseData[SE_INPUT_MAX_EVENT_FRAME_DATA_PER_TYPE];
    GamepadFrameEventData gamepadData[SE_INPUT_MAX_EVENT_FRAME_DATA_PER_TYPE];
    size_t keyboardDataCount;
    size_t mouseDataCount;
    size_t gamepadDataCount;
} InputFrameEventData;

InputFrameEventData frameEventData;

void input_frame_event_data_clear() {
    frameEventData.keyboardDataCount = 0;
    frameEventData.mouseDataCount = 0;
    frameEventData.gamepadDataCount = 0;
}

void input_frame_event_data_add_keyboard_data(SDL_Scancode scancode, InputFrameEventDataType type) {
    frameEventData.keyboardData[frameEventData.keyboardDataCount++] = (KeyboardFrameEventData) {
        .scancode = scancode, .type = type
    };
}

void input_frame_event_data_add_mouse_data(Uint8 mouseButton, InputFrameEventDataType type) {
    frameEventData.mouseData[frameEventData.mouseDataCount++] = (MouseFrameEventData) {
        .mouseButton = mouseButton, .type = type
    };
}

void input_frame_event_data_add_gamepad_data(SDL_JoystickID deviceId, uint8_t buttonValue, InputFrameEventDataType type) {
    frameEventData.gamepadData[frameEventData.gamepadDataCount++] = (GamepadFrameEventData) {
        .deviceId = deviceId, .buttonValue = buttonValue, .type = type
    };
}

bool se_input_initialize(const char* controllerDBFilePath) {
    inputActionMap = se_string_hash_map_create(32);
    // SETUP INPUT STRING VALUES
    // Keyboard
    keyboardStringValuesMap = se_string_hash_map_create(80);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_ZERO, SDL_SCANCODE_0);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_ONE, SDL_SCANCODE_1);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_TWO, SDL_SCANCODE_2);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_THREE, SDL_SCANCODE_3);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_FOUR, SDL_SCANCODE_4);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_FIVE, SDL_SCANCODE_5);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_SIX, SDL_SCANCODE_6);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_SEVEN, SDL_SCANCODE_7);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_EIGHT, SDL_SCANCODE_8);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_NINE, SDL_SCANCODE_9);

    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_A, SDL_SCANCODE_A);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_B, SDL_SCANCODE_B);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_C, SDL_SCANCODE_C);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_D, SDL_SCANCODE_D);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_E, SDL_SCANCODE_E);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_F, SDL_SCANCODE_F);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_G, SDL_SCANCODE_G);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_H, SDL_SCANCODE_H);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_I, SDL_SCANCODE_I);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_J, SDL_SCANCODE_J);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_K, SDL_SCANCODE_K);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_L, SDL_SCANCODE_L);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_M, SDL_SCANCODE_M);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_N, SDL_SCANCODE_N);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_O, SDL_SCANCODE_O);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_P, SDL_SCANCODE_P);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_Q, SDL_SCANCODE_Q);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_R, SDL_SCANCODE_R);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_S, SDL_SCANCODE_S);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_T, SDL_SCANCODE_T);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_U, SDL_SCANCODE_U);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_V, SDL_SCANCODE_V);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_W, SDL_SCANCODE_W);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_X, SDL_SCANCODE_X);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_Y, SDL_SCANCODE_Y);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_Z, SDL_SCANCODE_Z);

    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_LEFT, SDL_SCANCODE_LEFT);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_RIGHT, SDL_SCANCODE_RIGHT);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_UP, SDL_SCANCODE_UP);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_DOWN, SDL_SCANCODE_DOWN);

    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_RETURN, SDL_SCANCODE_RETURN);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_SPACE, SDL_SCANCODE_SPACE);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_ESCAPE, SDL_SCANCODE_ESCAPE);

    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_F1, SDL_SCANCODE_F1);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_F2, SDL_SCANCODE_F2);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_F3, SDL_SCANCODE_F3);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_F4, SDL_SCANCODE_F4);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_F5, SDL_SCANCODE_F5);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_F6, SDL_SCANCODE_F6);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_F7, SDL_SCANCODE_F7);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_F8, SDL_SCANCODE_F8);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_F9, SDL_SCANCODE_F9);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_F10, SDL_SCANCODE_F10);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_F11, SDL_SCANCODE_F11);
    se_string_hash_map_add_int(keyboardStringValuesMap, SE_INPUT_VALUE_F12, SDL_SCANCODE_F12);

    // Gamepad
    input_initialize_gamepad_system(controllerDBFilePath);

    return true;
}

void se_input_finalize() {}

void se_input_add_action_value(const char* actionName, const char* actionValue, int deviceId) {
    if (!se_string_hash_map_has(inputActionMap, actionName)) {
        se_string_hash_map_add(inputActionMap, actionName,
                               se_input_action_create_new_input_action(actionName, deviceId), sizeof(SEInputAction));
        inputActionNames[inputActionNamesCount++] = se_strdup(actionName);
    }
    SEInputAction* inputAction = (SEInputAction*) se_string_hash_map_get(inputActionMap, actionName);
    // TODO: Remember to properly delete char* from se_strdup
    if (se_string_hash_map_has(keyboardStringValuesMap, actionValue)) {
        inputAction->keyboardValues[inputAction->keyboardValueCount] = se_string_hash_map_get_int(
                    keyboardStringValuesMap, actionValue);
        inputAction->keyboardValueCount++;
        se_logger_debug("Added keyboard action | name: '%s', value: '%s', scancode: '%d'", actionName, actionValue,
                        inputAction->keyboardValues[inputAction->keyboardValueCount - 1]);
    } else if (se_string_hash_map_has(gamepadStringValuesMap, actionValue)) {
        inputAction->gamepadValues[inputAction->gamepadValueCount] = se_strdup(actionValue);
        inputAction->gamepadValueCount++;
        se_logger_debug("Added gamepad value '%s' with device id = %d", actionValue, deviceId);
    } else if (strcmp(actionValue, "mb_left") == 0 || strcmp(actionValue, "mb_right") == 0 || strcmp(actionValue, "mb_middle") == 0) {
        inputAction->mouseValues[inputAction->mouseValueCount] = se_strdup(actionValue);
        inputAction->mouseValueCount++;
        se_logger_debug("Added mouse action | name: '%s', value: '%s'", actionName, actionValue);
    } else {
        se_logger_error("No valid value found for action | name: '%s', value: '%s'", actionName, actionValue);
    }
}

void se_input_remove_action_value(const char* actionName, const char* actionValue) {}

void se_input_remove_action(const char* actionName) {}

void se_input_process(SDL_Event event) {
    input_frame_event_data_clear();
    input_process_keyboard(event);
    input_process_mouse(event);
    input_process_gamepad(event);
}

void se_input_clean_up_flags() {
    // Just Pressed
    for (size_t i = 0; i < actionJustPressedClean.count; i++) {
        actionJustPressedClean.inputActions[i]->isActionJustPressed = false;
        actionJustPressedClean.inputActions[i] = NULL;
    }
    actionJustPressedClean.count = 0;
    // Just Released
    for (size_t i = 0; i < actionJustReleasedClean.count; i++) {
        actionJustReleasedClean.inputActions[i]->isActionJustReleased = false;
        actionJustReleasedClean.inputActions[i] = NULL;
    }
    actionJustReleasedClean.count = 0;
    input_gamepad_cleanup_flags();
}

// Queries
bool se_input_is_action_pressed(const char* actionName) {
    if (se_string_hash_map_has(inputActionMap, actionName)) {
        SEInputAction* inputAction = (SEInputAction*) se_string_hash_map_get(inputActionMap, actionName);
        return inputAction->isActionPressed;
    }
    return false;
}

bool se_input_is_action_just_pressed(const char* actionName) {
    if (se_string_hash_map_has(inputActionMap, actionName)) {
        SEInputAction* inputAction = (SEInputAction*) se_string_hash_map_get(inputActionMap, actionName);
        return inputAction->isActionJustPressed;
    }
    return false;
}

bool se_input_is_action_just_released(const char* actionName) {
    if (se_string_hash_map_has(inputActionMap, actionName)) {
        SEInputAction* inputAction = (SEInputAction*) se_string_hash_map_get(inputActionMap, actionName);
        return inputAction->isActionJustReleased;
    }
    return false;
}

//--- Keyboard ---//
void input_process_keyboard(SDL_Event event) {
    switch (event.type) {
    case SDL_KEYDOWN: {
        const SDL_Scancode scancode = event.key.keysym.scancode;
        input_frame_event_data_add_keyboard_data(scancode, InputFrameEventDataType_PRESSED);
        break;
    }
    case SDL_KEYUP: {
        const SDL_Scancode scancode = event.key.keysym.scancode;
        input_frame_event_data_add_keyboard_data(scancode, InputFrameEventDataType_RELEASED);
        break;
    }
    default:
        break;
    }

    if (frameEventData.keyboardDataCount >= 1) {
        const KeyboardFrameEventData* eventData = &frameEventData.keyboardData[0];
        for (size_t i = 0; i < inputActionNamesCount; i++) {
            SEInputAction* inputAction = (SEInputAction*) se_string_hash_map_get(inputActionMap, inputActionNames[i]);
            for (size_t j = 0; j < inputAction->keyboardValueCount; j++) {
                SDL_Scancode scancode = (SDL_Scancode) inputAction->keyboardValues[j];
                if (eventData->scancode == scancode) {
                    if (eventData->type == InputFrameEventDataType_PRESSED && (SDL_Scancode)inputAction->lastScancodePressed != scancode) {
                        inputAction->isActionPressed = true;
                        inputAction->isActionJustPressed = true;
                        inputAction->lastScancodePressed = scancode;
                        actionJustPressedClean.inputActions[actionJustPressedClean.count++] = inputAction;
                    } else if (eventData->type == InputFrameEventDataType_RELEASED) {
                        inputAction->isActionPressed = false;
                        inputAction->isActionJustPressed = false;
                        inputAction->isActionJustReleased = true;
                        inputAction->lastScancodePressed = SDL_SCANCODE_UNKNOWN;
                        actionJustReleasedClean.inputActions[actionJustReleasedClean.count++] = inputAction;
                    }
                }
            }
        }
    }
}

//--- Mouse ---//
void input_process_mouse(SDL_Event event) {
    switch (event.type) {
    case SDL_MOUSEMOTION: {
        SEMouse* globalMouse = se_mouse_get();
        globalMouse->position.x = (float) event.motion.x;
        globalMouse->position.y = (float) event.motion.y;
        break;
    }
    case SDL_MOUSEWHEEL:
        break;
    case SDL_MOUSEBUTTONDOWN:
        input_frame_event_data_add_mouse_data(event.button.button, InputFrameEventDataType_PRESSED);
        break;
    case SDL_MOUSEBUTTONUP:
        input_frame_event_data_add_mouse_data(event.button.button, InputFrameEventDataType_RELEASED);
        break;
    default:
        break;
    }
    if (frameEventData.mouseDataCount >= 1) {
        const MouseFrameEventData* eventData = &frameEventData.mouseData[0];
        const Uint8 mouseButton = eventData->mouseButton;
        const bool mousePressed = eventData->type == InputFrameEventDataType_PRESSED;
        for (size_t i = 0; i < inputActionNamesCount; i++) {
            SEInputAction* inputAction = (SEInputAction*) se_string_hash_map_get(inputActionMap, inputActionNames[i]);
            for (size_t j = 0; j < inputAction->mouseValueCount; j++) {
                const bool isLeftMouseButton = strcmp((const char*) inputAction->mouseValues[j], "mb_left") == 0 && mouseButton == SDL_BUTTON_LEFT;
                const bool isRightMouseButton = strcmp((const char*) inputAction->mouseValues[j], "mb_right") == 0 && mouseButton == SDL_BUTTON_RIGHT;
                if (isLeftMouseButton || isRightMouseButton) {
                    // Event yes
                    if (mousePressed) {
                        inputAction->isActionPressed = true;
                        inputAction->isActionJustPressed = true;
                        actionJustPressedClean.inputActions[actionJustPressedClean.count++] = inputAction;
                    } else {
                        inputAction->isActionPressed = false;
                        inputAction->isActionJustPressed = false;
                        inputAction->isActionJustReleased = true;
                        actionJustReleasedClean.inputActions[actionJustReleasedClean.count++] = inputAction;
                    }
                    break;
                }
            }
        }
    }
}

//--- Gamepad ---//
#define CRE_MAX_GAMEPAD_INTERNAL_INPUT_ACTIONS 29
#define CRE_MAX_GAMEPAD_DEVICES 4
// TODO: Make dead zones configurable per action
#define GAMEPAD_AXIS_DEAD_ZONE 10000
#define GAMEPAD_TRIGGER_DEAD_ZONE 8000
#define INVALID_GAMEPAD_ID (-1)

typedef struct GamepadInputButtonAction {
    bool isPressed;
    bool isJustPressed;
    bool isJustReleased;
} GamepadInputButtonAction;

typedef enum GamepadInputButtonType {
    GamepadInputButtonType_START = 7,
    GamepadInputButtonType_BACK = 6,

    GamepadInputButtonType_LEFT_SHOULDER = 4,
    GamepadInputButtonType_RIGHT_SHOULDER = 5,
    GamepadInputButtonType_LEFT_ANALOG = 8,
    GamepadInputButtonType_RIGHT_ANALOG = 9,

    GamepadInputButtonType_DPAD_LEFT = 15,
    GamepadInputButtonType_DPAD_RIGHT = 16,
    GamepadInputButtonType_DPAD_UP = 17,
    GamepadInputButtonType_DPAD_DOWN = 18,
    GamepadInputButtonType_LEFT_ANALOG_LEFT = 19,
    GamepadInputButtonType_LEFT_ANALOG_RIGHT = 20,
    GamepadInputButtonType_LEFT_ANALOG_UP = 21,
    GamepadInputButtonType_LEFT_ANALOG_DOWN = 22,
    GamepadInputButtonType_RIGHT_ANALOG_LEFT = 23,
    GamepadInputButtonType_RIGHT_ANALOG_RIGHT = 24,
    GamepadInputButtonType_RIGHT_ANALOG_UP = 25,
    GamepadInputButtonType_RIGHT_ANALOG_DOWN = 26,
    GamepadInputButtonType_LEFT_TRIGGER = 27,
    GamepadInputButtonType_RIGHT_TRIGGER = 28,
} GamepadInputButtonType;

typedef enum GamepadInputAxisMotionType {
    GamepadInputAxisMotionType_LEFT_HORIZONTAL_AXIS = 0,
    GamepadInputAxisMotionType_LEFT_VERTICAL_AXIS = 1,
    GamepadInputAxisMotionType_RIGHT_HORIZONTAL_AXIS = 3,
    GamepadInputAxisMotionType_RIGHT_VERTICAL_AXIS = 4,

    GamepadInputAxisMotionType_LEFT_TRIGGER = 2,
    GamepadInputAxisMotionType_RIGHT_TRIGGER = 5,
} GamepadInputAxisMotionType;

typedef struct CreGamepad {
    SDL_GameController* gameController;
    SDL_Joystick* joystickController;
    SDL_JoystickID joystickId;
    // Some indices from 0 - 14 use SDL_GameControllerButton enum values
    GamepadInputButtonAction gamepadInputButtonActions[CRE_MAX_GAMEPAD_INTERNAL_INPUT_ACTIONS];
} CreGamepad;

CreGamepad activeGamePads[CRE_MAX_GAMEPAD_DEVICES];
static int activeGamepadCount = 0;
SE_STATIC_ARRAY_CREATE(int, CRE_MAX_GAMEPAD_DEVICES, activeGamepadIds);

void input_load_controller_mappings(const char* controllerDBFilePath);
void input_process_axis_motions();

void input_initialize_gamepad_system(const char* controllerDBFilePath) {
    gamepadStringValuesMap = se_string_hash_map_create(30);
    se_string_hash_map_add_int(gamepadStringValuesMap, "joystick_button_a", SDL_CONTROLLER_BUTTON_A);
    se_string_hash_map_add_int(gamepadStringValuesMap, "joystick_button_b", SDL_CONTROLLER_BUTTON_B);
    se_string_hash_map_add_int(gamepadStringValuesMap, "joystick_button_x", SDL_CONTROLLER_BUTTON_X);
    se_string_hash_map_add_int(gamepadStringValuesMap, "joystick_button_y", SDL_CONTROLLER_BUTTON_Y);
    se_string_hash_map_add_int(gamepadStringValuesMap, "joystick_dpad_left", GamepadInputButtonType_DPAD_LEFT);
    se_string_hash_map_add_int(gamepadStringValuesMap, "joystick_dpad_right", GamepadInputButtonType_DPAD_RIGHT);
    se_string_hash_map_add_int(gamepadStringValuesMap, "joystick_dpad_up", GamepadInputButtonType_DPAD_UP);
    se_string_hash_map_add_int(gamepadStringValuesMap, "joystick_dpad_down", GamepadInputButtonType_DPAD_DOWN);
    se_string_hash_map_add_int(gamepadStringValuesMap, "joystick_start", GamepadInputButtonType_START);
    se_string_hash_map_add_int(gamepadStringValuesMap, "joystick_back", GamepadInputButtonType_BACK);
    se_string_hash_map_add_int(gamepadStringValuesMap, "joystick_left_shoulder", GamepadInputButtonType_LEFT_SHOULDER);
    se_string_hash_map_add_int(gamepadStringValuesMap, "joystick_right_shoulder", GamepadInputButtonType_RIGHT_SHOULDER);
    se_string_hash_map_add_int(gamepadStringValuesMap, "joystick_left_analog", GamepadInputButtonType_LEFT_ANALOG);
    se_string_hash_map_add_int(gamepadStringValuesMap, "joystick_right_analog", GamepadInputButtonType_RIGHT_ANALOG);

    // Non Game Controller Button Action
    se_string_hash_map_add_int(gamepadStringValuesMap, "joystick_left_trigger", GamepadInputButtonType_LEFT_TRIGGER);
    se_string_hash_map_add_int(gamepadStringValuesMap, "joystick_right_trigger", GamepadInputButtonType_RIGHT_TRIGGER);
    se_string_hash_map_add_int(gamepadStringValuesMap, "joystick_left_analog_left", GamepadInputButtonType_LEFT_ANALOG_LEFT);
    se_string_hash_map_add_int(gamepadStringValuesMap, "joystick_left_analog_right", GamepadInputButtonType_LEFT_ANALOG_RIGHT);
    se_string_hash_map_add_int(gamepadStringValuesMap, "joystick_left_analog_up", GamepadInputButtonType_LEFT_ANALOG_UP);
    se_string_hash_map_add_int(gamepadStringValuesMap, "joystick_left_analog_down", GamepadInputButtonType_LEFT_ANALOG_DOWN);
    se_string_hash_map_add_int(gamepadStringValuesMap, "joystick_right_analog_left", GamepadInputButtonType_RIGHT_ANALOG_LEFT);
    se_string_hash_map_add_int(gamepadStringValuesMap, "joystick_right_analog_right", GamepadInputButtonType_RIGHT_ANALOG_RIGHT);
    se_string_hash_map_add_int(gamepadStringValuesMap, "joystick_right_analog_up", GamepadInputButtonType_RIGHT_ANALOG_UP);
    se_string_hash_map_add_int(gamepadStringValuesMap, "joystick_right_analog_down", GamepadInputButtonType_RIGHT_ANALOG_DOWN);

    // Initialize game pads
    input_load_gamepads(controllerDBFilePath);
}

void input_load_gamepads(const char* controllerDBFilePath) {
    for (size_t i = 0; i < CRE_MAX_GAMEPAD_DEVICES; i++) {
        activeGamePads[i].gameController = NULL;
        activeGamePads[i].joystickController = NULL;
        for (size_t j = 0; j < CRE_MAX_GAMEPAD_INTERNAL_INPUT_ACTIONS; j++) {
            activeGamePads[i].gamepadInputButtonActions[j].isPressed = false;
            activeGamePads[i].gamepadInputButtonActions[j].isJustPressed = false;
            activeGamePads[i].gamepadInputButtonActions[j].isJustReleased = false;
        }
        // Set all active gamepad ids to -1
        activeGamepadIds[i] = INVALID_GAMEPAD_ID;
    }

    input_load_controller_mappings(controllerDBFilePath);
}

void input_load_controller_mappings(const char* controllerDBFilePath) {
    int result = -1;
    if (sf_asset_file_loader_get_read_mode() == SEAssetFileLoaderReadMode_ARCHIVE) {
        SEArchiveFileAsset fileAsset = sf_asset_file_loader_get_asset(controllerDBFilePath);
        SE_ASSERT_FMT(sf_asset_file_loader_is_asset_valid(&fileAsset), "Failed to load game controller db file asset from memory!  Path: '%s'", controllerDBFilePath);
        result = SDL_GameControllerAddMappingsFromRW(SDL_RWFromMem(fileAsset.buffer, (int) fileAsset.bufferSize), 1);
    } else if (sf_asset_file_loader_get_read_mode() == SEAssetFileLoaderReadMode_DISK) {
        result = SDL_GameControllerAddMappingsFromFile(controllerDBFilePath);
    }
    SE_ASSERT_FMT(result >= 0, "Couldn't load sdl controller mapping file at path '%s'!", controllerDBFilePath);
}

CreGamepad* input_find_gamepad(SDL_JoystickID id) {
    for (int i = 0; i < activeGamepadCount; i++) {
        if (id == activeGamePads[i].joystickId) {
            return &activeGamePads[i];
        }
    }
    return NULL;
}

void input_process_gamepad(SDL_Event event) {
    switch (event.type) {
    case SDL_CONTROLLERDEVICEADDED: {
        // TODO: Trigger gamepad connected script callbacks
        SDL_GameController* newGameController = SDL_GameControllerOpen(event.cdevice.which);
        SE_ASSERT_FMT(newGameController != NULL, "Failed to load game controller with index '%d'", event.cdevice.which);
        SDL_Joystick* newJoystick = SDL_GameControllerGetJoystick(newGameController);
        SE_ASSERT_FMT(newJoystick != NULL, "Failed to load joystick with index '%d'", event.jdevice.which);
        int gamepadIndex = SDL_GameControllerGetPlayerIndex(newGameController);
        if (gamepadIndex == -1) {
            gamepadIndex = SDL_JoystickInstanceID(newJoystick);
            se_logger_warn("Unable to get player index from controller, using SDL joystick instance id '%d'",
                           gamepadIndex);
        }
        SE_ASSERT_FMT(gamepadIndex < CRE_MAX_GAMEPAD_DEVICES, "Gamepad index '%d' higher than limit.", gamepadIndex);
        activeGamePads[gamepadIndex].gameController = newGameController;
        activeGamePads[gamepadIndex].joystickController = newJoystick;
        activeGamePads[gamepadIndex].joystickId = SDL_JoystickInstanceID(newJoystick);
        SE_STATIC_ARRAY_ADD(activeGamepadIds, gamepadIndex);
        activeGamepadCount++;
        break;
    }
    case SDL_CONTROLLERDEVICEREMOVED: {
        // TODO: Trigger gamepad disconnected script callbacks
        const int gamepadIndex = event.cdevice.which;
        SDL_JoystickClose(activeGamePads[gamepadIndex].joystickController);
        for (int i = 0; i < activeGamepadCount; i++) {
            if (activeGamepadIds[i] == gamepadIndex) {
                SE_STATIC_ARRAY_REMOVE(activeGamepadIds, gamepadIndex, INVALID_GAMEPAD_ID);
                activeGamepadCount--;
                break;
            }
        }
        se_logger_error("Failed to remove gamepad with index '%d'", gamepadIndex);
        break;
    }
    case SDL_JOYBUTTONDOWN:
    case SDL_JOYBUTTONUP: {
        const SDL_JoystickID controllerId = event.jbutton.which;
        const bool isButtonPressed = event.jbutton.state == SDL_PRESSED;
        const uint8_t buttonValue = event.jbutton.button;
        SE_ASSERT(buttonValue < CRE_MAX_GAMEPAD_INTERNAL_INPUT_ACTIONS);
        CreGamepad* gamepad = input_find_gamepad(controllerId);
        if (isButtonPressed) {
            gamepad->gamepadInputButtonActions[buttonValue].isPressed = true;
            gamepad->gamepadInputButtonActions[buttonValue].isJustPressed = true;
            input_frame_event_data_add_gamepad_data(controllerId, buttonValue, InputFrameEventDataType_PRESSED);
        } else {
            gamepad->gamepadInputButtonActions[buttonValue].isPressed = false;
            gamepad->gamepadInputButtonActions[buttonValue].isJustPressed = false;
            gamepad->gamepadInputButtonActions[buttonValue].isJustReleased = true;
            input_frame_event_data_add_gamepad_data(controllerId, buttonValue, InputFrameEventDataType_RELEASED);
        }
        break;
    }
    case SDL_JOYHATMOTION: {
        const int controllerId = event.jhat.which;
        const uint8_t hatValue = event.jhat.value;
        // Process Joyhat Motion (dpad)
        if (hatValue & SDL_HAT_LEFT) {
            activeGamePads[controllerId].gamepadInputButtonActions[GamepadInputButtonType_DPAD_LEFT].isPressed = true;
            activeGamePads[controllerId].gamepadInputButtonActions[GamepadInputButtonType_DPAD_LEFT].isJustPressed = true;
            input_frame_event_data_add_gamepad_data(controllerId, GamepadInputButtonType_DPAD_LEFT, InputFrameEventDataType_PRESSED);
        } else {
            activeGamePads[controllerId].gamepadInputButtonActions[GamepadInputButtonType_DPAD_LEFT].isPressed = false;
            activeGamePads[controllerId].gamepadInputButtonActions[GamepadInputButtonType_DPAD_LEFT].isJustPressed = false;
            activeGamePads[controllerId].gamepadInputButtonActions[GamepadInputButtonType_DPAD_LEFT].isJustReleased = true;
            input_frame_event_data_add_gamepad_data(controllerId, GamepadInputButtonType_DPAD_LEFT, InputFrameEventDataType_RELEASED);
        }
        if (hatValue & SDL_HAT_RIGHT) {
            activeGamePads[controllerId].gamepadInputButtonActions[GamepadInputButtonType_DPAD_RIGHT].isPressed = true;
            activeGamePads[controllerId].gamepadInputButtonActions[GamepadInputButtonType_DPAD_RIGHT].isJustReleased = true;
            input_frame_event_data_add_gamepad_data(controllerId, GamepadInputButtonType_DPAD_RIGHT, InputFrameEventDataType_PRESSED);
        } else {
            activeGamePads[controllerId].gamepadInputButtonActions[GamepadInputButtonType_DPAD_RIGHT].isPressed = false;
            activeGamePads[controllerId].gamepadInputButtonActions[GamepadInputButtonType_DPAD_RIGHT].isJustPressed = false;
            activeGamePads[controllerId].gamepadInputButtonActions[GamepadInputButtonType_DPAD_RIGHT].isJustReleased = true;
            input_frame_event_data_add_gamepad_data(controllerId, GamepadInputButtonType_DPAD_RIGHT, InputFrameEventDataType_RELEASED);
        }
        if (hatValue & SDL_HAT_UP) {
            activeGamePads[controllerId].gamepadInputButtonActions[GamepadInputButtonType_DPAD_UP].isPressed = true;
            activeGamePads[controllerId].gamepadInputButtonActions[GamepadInputButtonType_DPAD_UP].isJustPressed = true;
            input_frame_event_data_add_gamepad_data(controllerId, GamepadInputButtonType_DPAD_UP, InputFrameEventDataType_PRESSED);
        } else {
            activeGamePads[controllerId].gamepadInputButtonActions[GamepadInputButtonType_DPAD_UP].isPressed = false;
            activeGamePads[controllerId].gamepadInputButtonActions[GamepadInputButtonType_DPAD_UP].isJustPressed = false;
            activeGamePads[controllerId].gamepadInputButtonActions[GamepadInputButtonType_DPAD_UP].isJustReleased = true;
            input_frame_event_data_add_gamepad_data(controllerId, GamepadInputButtonType_DPAD_UP, InputFrameEventDataType_RELEASED);
        }
        if (hatValue & SDL_HAT_DOWN) {
            activeGamePads[controllerId].gamepadInputButtonActions[GamepadInputButtonType_DPAD_DOWN].isPressed = true;
            activeGamePads[controllerId].gamepadInputButtonActions[GamepadInputButtonType_DPAD_DOWN].isJustPressed = true;
            input_frame_event_data_add_gamepad_data(controllerId, GamepadInputButtonType_DPAD_DOWN, InputFrameEventDataType_PRESSED);
        } else {
            activeGamePads[controllerId].gamepadInputButtonActions[GamepadInputButtonType_DPAD_DOWN].isPressed = false;
            activeGamePads[controllerId].gamepadInputButtonActions[GamepadInputButtonType_DPAD_DOWN].isJustPressed = false;
            activeGamePads[controllerId].gamepadInputButtonActions[GamepadInputButtonType_DPAD_DOWN].isJustReleased = true;
            input_frame_event_data_add_gamepad_data(controllerId, GamepadInputButtonType_DPAD_DOWN, InputFrameEventDataType_RELEASED);
        }
        break;
    }
    default:
        break;
    }

    input_process_axis_motions();

    for (size_t i = 0; i < frameEventData.gamepadDataCount; i++) {
        const GamepadFrameEventData* eventData = &frameEventData.gamepadData[i];
        for (size_t j = 0; j < inputActionNamesCount; j++) {
            SEInputAction* inputAction = (SEInputAction*) se_string_hash_map_get(inputActionMap, inputActionNames[j]);
            if (eventData->deviceId != inputAction->deviceId) {
                continue;
            }
            for (size_t k = 0; k < inputAction->gamepadValueCount; k++) {
                const char* gamepadValue = inputAction->gamepadValues[k];
                const uint8_t buttonScancode = se_string_hash_map_get_int(gamepadStringValuesMap, gamepadValue);
                if (eventData->buttonValue == buttonScancode) {
                    if (eventData->type == InputFrameEventDataType_PRESSED) {
                        inputAction->isActionPressed = true;
                        inputAction->isActionJustPressed = true;
                        actionJustPressedClean.inputActions[actionJustPressedClean.count++] = inputAction;
                    } else if (eventData->type == InputFrameEventDataType_RELEASED) {
                        inputAction->isActionPressed = false;
                        inputAction->isActionJustReleased = true;
                        actionJustReleasedClean.inputActions[actionJustReleasedClean.count++] = inputAction;
                    }
                }
            }
        }
    }
}

// TODO: Get 'strength' of axis motions for more fine tuned controls from the user's perspective
void input_process_axis_motions() {
    for (int i = 0; i < activeGamepadCount; i++) {
        const int deviceId = activeGamepadIds[i];
        CreGamepad* gamepad = &activeGamePads[deviceId];

        // LEFT AXIS
        // Horizontal
        const int16_t leftHorizontalValue = SDL_JoystickGetAxis(gamepad->joystickController, GamepadInputAxisMotionType_LEFT_HORIZONTAL_AXIS);
        if (leftHorizontalValue < -GAMEPAD_AXIS_DEAD_ZONE) {
            if (!gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_LEFT].isPressed) {
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_LEFT].isPressed = true;
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_LEFT].isJustPressed = true;
                input_frame_event_data_add_gamepad_data(deviceId, GamepadInputButtonType_LEFT_ANALOG_LEFT, InputFrameEventDataType_PRESSED);
            }
        } else if (leftHorizontalValue > GAMEPAD_AXIS_DEAD_ZONE) {
            if (!gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_RIGHT].isPressed) {
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_RIGHT].isPressed = true;
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_RIGHT].isJustPressed = true;
                input_frame_event_data_add_gamepad_data(deviceId, GamepadInputButtonType_LEFT_ANALOG_RIGHT, InputFrameEventDataType_PRESSED);
            }
        } else {
            if (gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_LEFT].isPressed) {
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_LEFT].isPressed = false;
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_LEFT].isJustReleased = true;
                input_frame_event_data_add_gamepad_data(deviceId, GamepadInputButtonType_LEFT_ANALOG_LEFT, InputFrameEventDataType_RELEASED);
            }
            if (gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_RIGHT].isPressed) {
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_RIGHT].isPressed = false;
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_RIGHT].isJustReleased = true;
                input_frame_event_data_add_gamepad_data(deviceId, GamepadInputButtonType_LEFT_ANALOG_RIGHT, InputFrameEventDataType_RELEASED);
            }
        }
        // Vertical
        const int16_t leftVerticalValue = SDL_JoystickGetAxis(gamepad->joystickController, GamepadInputAxisMotionType_LEFT_VERTICAL_AXIS);
        if (leftVerticalValue < -GAMEPAD_AXIS_DEAD_ZONE) {
            if (!gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_UP].isPressed) {
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_UP].isPressed = true;
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_UP].isJustPressed = true;
                input_frame_event_data_add_gamepad_data(deviceId, GamepadInputButtonType_LEFT_ANALOG_UP, InputFrameEventDataType_PRESSED);
            }
        } else if (leftVerticalValue > GAMEPAD_AXIS_DEAD_ZONE) {
            if (!gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_DOWN].isPressed) {
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_DOWN].isPressed = true;
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_DOWN].isJustPressed = true;
                input_frame_event_data_add_gamepad_data(deviceId, GamepadInputButtonType_LEFT_ANALOG_DOWN, InputFrameEventDataType_PRESSED);
            }
        } else {
            if (gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_UP].isPressed) {
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_UP].isPressed = false;
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_UP].isJustReleased = true;
                input_frame_event_data_add_gamepad_data(deviceId, GamepadInputButtonType_LEFT_ANALOG_UP, InputFrameEventDataType_RELEASED);
            }
            if (gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_DOWN].isPressed) {
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_DOWN].isPressed = false;
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_DOWN].isJustReleased = true;
                input_frame_event_data_add_gamepad_data(deviceId, GamepadInputButtonType_LEFT_ANALOG_DOWN, InputFrameEventDataType_RELEASED);
            }
        }

        // RIGHT AXIS
        // Horizontal
        const int16_t rightHorizontalValue = SDL_JoystickGetAxis(gamepad->joystickController, GamepadInputAxisMotionType_RIGHT_HORIZONTAL_AXIS);
        if (rightHorizontalValue < -GAMEPAD_AXIS_DEAD_ZONE) {
            if (!gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_LEFT].isPressed) {
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_LEFT].isPressed = true;
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_LEFT].isJustPressed = true;
                input_frame_event_data_add_gamepad_data(deviceId, GamepadInputButtonType_RIGHT_ANALOG_LEFT, InputFrameEventDataType_PRESSED);
            }
        } else if (rightHorizontalValue > GAMEPAD_AXIS_DEAD_ZONE) {
            if (!gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_RIGHT].isPressed) {
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_RIGHT].isPressed = true;
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_RIGHT].isJustPressed = true;
                input_frame_event_data_add_gamepad_data(deviceId, GamepadInputButtonType_RIGHT_ANALOG_RIGHT, InputFrameEventDataType_PRESSED);
            }
        } else {
            if (gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_LEFT].isPressed) {
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_LEFT].isPressed = false;
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_LEFT].isJustReleased = true;
                input_frame_event_data_add_gamepad_data(deviceId, GamepadInputButtonType_RIGHT_ANALOG_LEFT, InputFrameEventDataType_RELEASED);
            }
            if (gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_RIGHT].isPressed) {
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_RIGHT].isPressed = false;
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_RIGHT].isJustReleased = true;
                input_frame_event_data_add_gamepad_data(deviceId, GamepadInputButtonType_RIGHT_ANALOG_RIGHT, InputFrameEventDataType_RELEASED);
            }
        }
        // Vertical
        const int16_t rightVerticalValue = SDL_JoystickGetAxis(gamepad->joystickController, GamepadInputAxisMotionType_RIGHT_VERTICAL_AXIS);
        if (rightVerticalValue < -GAMEPAD_AXIS_DEAD_ZONE) {
            if (!gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_UP].isPressed) {
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_UP].isPressed = true;
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_UP].isJustPressed = true;
                input_frame_event_data_add_gamepad_data(deviceId, GamepadInputButtonType_RIGHT_ANALOG_UP, InputFrameEventDataType_PRESSED);
            }
        } else if (rightVerticalValue > GAMEPAD_AXIS_DEAD_ZONE) {
            if (!gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_DOWN].isPressed) {
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_DOWN].isPressed = true;
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_DOWN].isJustPressed = true;
                input_frame_event_data_add_gamepad_data(deviceId, GamepadInputButtonType_RIGHT_ANALOG_DOWN, InputFrameEventDataType_PRESSED);
            }
        } else {
            if (gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_UP].isPressed) {
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_UP].isPressed = false;
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_UP].isJustReleased = true;
                input_frame_event_data_add_gamepad_data(deviceId, GamepadInputButtonType_RIGHT_ANALOG_UP, InputFrameEventDataType_RELEASED);
            }
            if (gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_DOWN].isPressed) {
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_DOWN].isPressed = false;
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_DOWN].isJustReleased = true;
                input_frame_event_data_add_gamepad_data(deviceId, GamepadInputButtonType_RIGHT_ANALOG_DOWN, InputFrameEventDataType_RELEASED);
            }
        }

        // TRIGGERS
        // Left Trigger
        const int16_t leftTriggerValue = SDL_JoystickGetAxis(gamepad->joystickController, GamepadInputAxisMotionType_LEFT_TRIGGER);
        if (leftTriggerValue < -GAMEPAD_TRIGGER_DEAD_ZONE) {
            if (gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_TRIGGER].isPressed) {
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_TRIGGER].isPressed = false;
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_TRIGGER].isJustReleased = true;
                input_frame_event_data_add_gamepad_data(deviceId, GamepadInputButtonType_LEFT_TRIGGER, InputFrameEventDataType_RELEASED);
            }
        } else if (leftTriggerValue > GAMEPAD_AXIS_DEAD_ZONE) {
            if (!gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_TRIGGER].isPressed) {
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_TRIGGER].isPressed = true;
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_TRIGGER].isJustPressed = true;
                input_frame_event_data_add_gamepad_data(deviceId, GamepadInputButtonType_LEFT_TRIGGER, InputFrameEventDataType_PRESSED);
            }
        }
        // Right Trigger
        const int16_t rightTriggerValue = SDL_JoystickGetAxis(gamepad->joystickController, GamepadInputAxisMotionType_RIGHT_TRIGGER);
        if (rightTriggerValue < -GAMEPAD_TRIGGER_DEAD_ZONE) {
            if (gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_TRIGGER].isPressed) {
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_TRIGGER].isPressed = false;
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_TRIGGER].isJustReleased = true;
                input_frame_event_data_add_gamepad_data(deviceId, GamepadInputButtonType_RIGHT_TRIGGER, InputFrameEventDataType_RELEASED);
            }
        } else if (rightTriggerValue > GAMEPAD_AXIS_DEAD_ZONE) {
            if (!gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_TRIGGER].isPressed) {
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_TRIGGER].isPressed = true;
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_TRIGGER].isJustPressed = true;
                input_frame_event_data_add_gamepad_data(deviceId, GamepadInputButtonType_RIGHT_TRIGGER, InputFrameEventDataType_PRESSED);
            }
        }
    }
}

void input_gamepad_cleanup_flags() {
    // Gamepad action clean up references
    for (int i = 0; i < activeGamepadCount; i++) {
        const int deviceId = activeGamepadIds[i];
        for (size_t j = 0; j < CRE_MAX_GAMEPAD_INTERNAL_INPUT_ACTIONS; j++) {
            activeGamePads[deviceId].gamepadInputButtonActions[j].isJustPressed = false;
            activeGamePads[deviceId].gamepadInputButtonActions[j].isJustReleased = false;
        }
    }
}

void se_input_gamepad_start_vibration(int device, float weakMagnitude, float strongMagnitude, float durationSeconds) {
    for (int i = 0; i < activeGamepadCount; i++) {
        if (i == device) {
            weakMagnitude = se_math_clamp_float(weakMagnitude, 0.0f, 1.0f);
            strongMagnitude = se_math_clamp_float(strongMagnitude, 0.0f, 1.0f);
            const Uint16 weakMag = (Uint16)(weakMagnitude * 65535.0f + 0.5f);
            const Uint16 strongMag = (Uint16)(strongMagnitude * 65535.0f + 0.5f);
            const Uint32 durationMilliseconds = (Uint32)(durationSeconds * 1000.0f);
            const int deviceId = activeGamepadIds[i];
            SDL_JoystickRumble(activeGamePads[deviceId].joystickController, weakMag, strongMag, durationMilliseconds);
            break;
        }
    }
}

void se_input_gamepad_stop_vibration(int device) {
    for (int i = 0; i < activeGamepadCount; i++) {
        if (i == device) {
            const int deviceId = activeGamepadIds[i];
            SDL_JoystickRumble(activeGamePads[deviceId].joystickController, 0, 0, 0);
            break;
        }
    }
}
