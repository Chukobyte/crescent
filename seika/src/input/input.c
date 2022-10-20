#include "input.h"

#include <string.h>

#include "input_action.h"
#include "input_value_constants.h"
#include "../data_structures/rbe_hash_map_string.h"
#include "../data_structures/rbe_static_array.h"
#include "../utils/logger.h"
#include "../utils/rbe_string_util.h"
#include "../utils/rbe_assert.h"
#include "../engine_context.h"

#ifdef _MSC_VER
#pragma warning(disable : 4996) // for strcpy
#endif

void input_process_keyboard();
void input_process_mouse(SDL_Event event);
void input_load_gamepads();
void input_process_gamepad(SDL_Event event);

void input_initialize_gamepad_system();
void input_gamepad_cleanup_flags();

//--- Input ---//
RBEStringHashMap* inputActionMap = NULL;
const char* inputActionNames[MAX_INPUT_ACTIONS];
size_t inputActionNamesCount = 0;

RBEStringHashMap* keyboardStringValuesMap = NULL;
RBEStringHashMap* gamepadStringValuesMap = NULL;

typedef struct InputFlagCleaner {
    InputAction* inputActions[MAX_INPUT_ACTIONS];
    size_t count;
} InputFlagCleaner;

InputFlagCleaner actionJustPressedClean = {0};
InputFlagCleaner actionJustReleasedClean = {0};

bool cre_input_initialize() {
    inputActionMap = rbe_string_hash_map_create(32);
    // SETUP INPUT STRING VALUES
    // Keyboard
    keyboardStringValuesMap = rbe_string_hash_map_create(80);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_ZERO, SDL_SCANCODE_0);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_ONE, SDL_SCANCODE_1);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_TWO, SDL_SCANCODE_2);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_THREE, SDL_SCANCODE_3);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_FOUR, SDL_SCANCODE_4);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_FIVE, SDL_SCANCODE_5);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_SIX, SDL_SCANCODE_6);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_SEVEN, SDL_SCANCODE_7);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_EIGHT, SDL_SCANCODE_8);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_NINE, SDL_SCANCODE_9);

    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_A, SDL_SCANCODE_A);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_B, SDL_SCANCODE_B);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_C, SDL_SCANCODE_C);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_D, SDL_SCANCODE_D);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_E, SDL_SCANCODE_E);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_F, SDL_SCANCODE_F);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_G, SDL_SCANCODE_G);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_H, SDL_SCANCODE_H);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_I, SDL_SCANCODE_I);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_J, SDL_SCANCODE_J);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_K, SDL_SCANCODE_K);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_L, SDL_SCANCODE_L);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_M, SDL_SCANCODE_M);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_N, SDL_SCANCODE_N);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_O, SDL_SCANCODE_O);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_P, SDL_SCANCODE_P);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_Q, SDL_SCANCODE_Q);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_R, SDL_SCANCODE_R);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_S, SDL_SCANCODE_S);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_T, SDL_SCANCODE_T);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_U, SDL_SCANCODE_U);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_V, SDL_SCANCODE_V);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_W, SDL_SCANCODE_W);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_X, SDL_SCANCODE_X);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_Y, SDL_SCANCODE_Y);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_Z, SDL_SCANCODE_Z);

    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_LEFT, SDL_SCANCODE_LEFT);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_RIGHT, SDL_SCANCODE_RIGHT);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_UP, SDL_SCANCODE_UP);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_DOWN, SDL_SCANCODE_DOWN);

    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_RETURN, SDL_SCANCODE_RETURN);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_SPACE, SDL_SCANCODE_SPACE);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_ESCAPE, SDL_SCANCODE_ESCAPE);

    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_F1, SDL_SCANCODE_F1);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_F2, SDL_SCANCODE_F2);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_F3, SDL_SCANCODE_F3);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_F4, SDL_SCANCODE_F4);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_F5, SDL_SCANCODE_F5);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_F6, SDL_SCANCODE_F6);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_F7, SDL_SCANCODE_F7);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_F8, SDL_SCANCODE_F8);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_F9, SDL_SCANCODE_F9);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_F10, SDL_SCANCODE_F10);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_F11, SDL_SCANCODE_F11);
    rbe_string_hash_map_add_int(keyboardStringValuesMap, INPUT_VALUE_F12, SDL_SCANCODE_F12);

    // Gamepad
    input_initialize_gamepad_system();

    return true;
}

void cre_input_finalize() {}

void cre_input_add_action_value(const char* actionName, const char* actionValue, int deviceId) {
    if (!rbe_string_hash_map_has(inputActionMap, actionName)) {
        rbe_string_hash_map_add(inputActionMap, actionName, rbe_input_action_create_new_input_action(actionName, deviceId), sizeof(InputAction));
        inputActionNames[inputActionNamesCount] = actionName;
        inputActionNamesCount++;
    }
    InputAction* inputAction = (InputAction*) rbe_string_hash_map_get(inputActionMap, actionName);
    // TODO: Remember to properly delete char* from rbe_strdup
    if (rbe_string_hash_map_has(keyboardStringValuesMap, actionValue)) {
        inputAction->keyboardValues[inputAction->keyboardValueCount] = rbe_string_hash_map_get_int(keyboardStringValuesMap, actionValue);
        inputAction->keyboardValueCount++;
        rbe_logger_debug("Added keyboard action | name: '%s', value: '%s', scancode: '%d'", actionName, actionValue, inputAction->keyboardValues[inputAction->keyboardValueCount - 1]);
    } else if (rbe_string_hash_map_has(gamepadStringValuesMap, actionValue)) {
        inputAction->gamepadValues[inputAction->gamepadValueCount] = rbe_strdup(actionValue);
        inputAction->gamepadValueCount++;
        rbe_logger_debug("Added gamepad value '%s' with device id = %d", actionValue, deviceId);
    } else if (strcmp(actionValue, "mb_left") == 0 || strcmp(actionValue, "mb_right") == 0 || strcmp(actionValue, "mb_middle") == 0) {
        strcpy(&inputAction->mouseValues[inputAction->mouseValueCount], actionValue);
        rbe_logger_debug("Added mouse action | name: '%s', value: '%s'", actionName, inputAction->mouseValues[inputAction->mouseValueCount]);
        inputAction->mouseValueCount++;
    } else {
        rbe_logger_error("No valid value found for action | name: '%s', value: '%s'", actionName, actionValue);
    }
}

void cre_input_remove_action_value(const char* actionName, const char* actionValue) {}

void cre_input_remove_action(const char* actionName) {}

void cre_input_process(SDL_Event event) {
    input_process_keyboard();
    input_process_mouse(event);
    input_process_gamepad(event);
}

void cre_input_clean_up_flags() {
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
bool cre_input_is_action_pressed(const char* actionName) {
    if (rbe_string_hash_map_has(inputActionMap, actionName)) {
        InputAction* inputAction = (InputAction*) rbe_string_hash_map_get(inputActionMap, actionName);
        return inputAction->isActionPressed;
    }
    return false;
}

bool cre_input_is_action_just_pressed(const char* actionName) {
    if (rbe_string_hash_map_has(inputActionMap, actionName)) {
        InputAction* inputAction = (InputAction*) rbe_string_hash_map_get(inputActionMap, actionName);
        return inputAction->isActionJustPressed;
    }
    return false;
}

bool cre_input_is_action_just_released(const char* actionName) {
    if (rbe_string_hash_map_has(inputActionMap, actionName)) {
        InputAction* inputAction = (InputAction*) rbe_string_hash_map_get(inputActionMap, actionName);
        return inputAction->isActionJustReleased;
    }
    return false;
}

// TODO: Centralize looping through input actions names to prevent multiple loops
//--- Keyboard ---//
void input_process_keyboard() {
    const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
    for (size_t i = 0; i < inputActionNamesCount; i++) {
        InputAction* inputAction = (InputAction*) rbe_string_hash_map_get(inputActionMap, inputActionNames[i]);
        for (size_t j = 0; j < inputAction->keyboardValueCount; j++) {
            SDL_Scancode scancode = (SDL_Scancode) inputAction->keyboardValues[j];
            bool isKeyboardValuePressed = keyboardState[scancode];
            if (isKeyboardValuePressed && !inputAction->isActionPressed) {
//                rbe_logger_debug("isKeyboardValuePressed = %d, scancode = %d", isKeyboardValuePressed, scancode);
                inputAction->isActionPressed = true;
                inputAction->isActionJustPressed = true;
                inputAction->lastScancodePressed = scancode;
                actionJustPressedClean.inputActions[actionJustPressedClean.count++] = inputAction;
                break;
            } else if (!isKeyboardValuePressed && !inputAction->isActionJustReleased && inputAction->isActionPressed && (int) scancode == inputAction->lastScancodePressed) {
                inputAction->isActionPressed = false;
                inputAction->isActionJustPressed = false;
                inputAction->isActionJustReleased = true;
                actionJustReleasedClean.inputActions[actionJustReleasedClean.count++] = inputAction;
            }
        }
    }
}

//--- Mouse ---//
void input_process_mouse(SDL_Event event) {
    bool isMouseEvent = false;
    bool mousePressed = false;
    Uint8 mouseButton = 0;
    switch (event.type) {
    case SDL_MOUSEMOTION:
//            inputEvent.type = InputEventType::MOUSE;
//            inputEvent.mouseMotion = Vector2(event.motion.x, event.motion.y);
        break;
    case SDL_MOUSEWHEEL:
//            inputEvent.type = InputEventType::MOUSE;
        break;
    case SDL_MOUSEBUTTONDOWN:
        isMouseEvent = true;
        mousePressed = true;
        mouseButton = event.button.button;
        break;
    case SDL_MOUSEBUTTONUP:
        isMouseEvent = true;
        mousePressed = false;
        mouseButton = event.button.button;
        break;
    default:
        break;
    }
    if (isMouseEvent) {
        for (size_t i = 0; i < inputActionNamesCount; i++) {
            InputAction* inputAction = (InputAction*) rbe_string_hash_map_get(inputActionMap, inputActionNames[i]);
            for (size_t j = 0; j < inputAction->mouseValueCount; j++) {
                bool isLeftMouseButton = strcmp(&inputAction->mouseValues[j], "mb_left") == 0 && mouseButton == SDL_BUTTON_LEFT;
                bool isRightMouseButton = strcmp(&inputAction->mouseValues[j], "mb_right") == 0 && mouseButton == SDL_BUTTON_RIGHT;
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
RBE_STATIC_ARRAY_CREATE(int, CRE_MAX_GAMEPAD_DEVICES, activeGamepadIds);

bool input_process_axis_motions();

void input_initialize_gamepad_system() {
    gamepadStringValuesMap = rbe_string_hash_map_create(30);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_button_a", SDL_CONTROLLER_BUTTON_A);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_button_b", SDL_CONTROLLER_BUTTON_B);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_button_x", SDL_CONTROLLER_BUTTON_X);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_button_y", SDL_CONTROLLER_BUTTON_Y);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_dpad_left", GamepadInputButtonType_DPAD_LEFT);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_dpad_right", GamepadInputButtonType_DPAD_RIGHT);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_dpad_up", GamepadInputButtonType_DPAD_UP);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_dpad_down", GamepadInputButtonType_DPAD_DOWN);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_start", GamepadInputButtonType_START);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_back", GamepadInputButtonType_BACK);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_left_shoulder", GamepadInputButtonType_LEFT_SHOULDER);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_right_shoulder", GamepadInputButtonType_RIGHT_SHOULDER);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_left_analog", GamepadInputButtonType_LEFT_ANALOG);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_right_analog", GamepadInputButtonType_RIGHT_ANALOG);

    // Non Game Controller Button Action
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_left_trigger", GamepadInputButtonType_LEFT_TRIGGER);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_right_trigger", GamepadInputButtonType_RIGHT_TRIGGER);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_left_analog_left", GamepadInputButtonType_LEFT_ANALOG_LEFT);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_left_analog_right", GamepadInputButtonType_LEFT_ANALOG_RIGHT);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_left_analog_up", GamepadInputButtonType_LEFT_ANALOG_UP);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_left_analog_down", GamepadInputButtonType_LEFT_ANALOG_DOWN);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_right_analog_left", GamepadInputButtonType_RIGHT_ANALOG_LEFT);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_right_analog_right", GamepadInputButtonType_RIGHT_ANALOG_RIGHT);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_right_analog_up", GamepadInputButtonType_RIGHT_ANALOG_UP);
    rbe_string_hash_map_add_int(gamepadStringValuesMap, "joystick_right_analog_down", GamepadInputButtonType_RIGHT_ANALOG_DOWN);

    // Initialize game pads
    input_load_gamepads();
}

// TODO: Make better (e.g. loading more than one controller and checking for connects/disconnects)...
void input_load_gamepads() {
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

    RBEEngineContext* engineContext = rbe_engine_context_get();
    char controllerMappingFilePath[256];
    strcpy(controllerMappingFilePath, engineContext->internalAssetsDir);
    strcat(controllerMappingFilePath, "/assets/resources/game_controller_db.txt");
    const int result = SDL_GameControllerAddMappingsFromFile(controllerMappingFilePath);
    RBE_ASSERT_FMT(result >= 0, "Couldn't load sdl controller mapping file at path '%s'!", controllerMappingFilePath);
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
    bool buttonInputUpdated = false;
    switch (event.type) {
    case SDL_CONTROLLERDEVICEADDED: {
        // TODO: Trigger gamepad connected script callbacks
        SDL_GameController* newGameController = SDL_GameControllerOpen(event.cdevice.which);
        RBE_ASSERT_FMT(newGameController != NULL, "Failed to load game controller with index '%d'", event.cdevice.which);
        SDL_Joystick* newJoystick = SDL_GameControllerGetJoystick(newGameController);
        RBE_ASSERT_FMT(newJoystick != NULL, "Failed to load joystick with index '%d'", event.jdevice.which);
        int gamepadIndex = SDL_GameControllerGetPlayerIndex(newGameController);
        if (gamepadIndex == -1) {
            gamepadIndex = SDL_JoystickInstanceID(newJoystick);
            rbe_logger_warn("Unable to get player index from controller, using SDL joystick instance id '%d'", gamepadIndex);
        }
        RBE_ASSERT_FMT(gamepadIndex < CRE_MAX_GAMEPAD_DEVICES, "Gamepad index '%d' higher than limit.", gamepadIndex);
        activeGamePads[gamepadIndex].gameController = newGameController;
        activeGamePads[gamepadIndex].joystickController = newJoystick;
        activeGamePads[gamepadIndex].joystickId = SDL_JoystickInstanceID(newJoystick);
        RBE_STATIC_ARRAY_ADD(activeGamepadIds, gamepadIndex);
        activeGamepadCount++;
        break;
    }
    case SDL_CONTROLLERDEVICEREMOVED: {
        // TODO: Trigger gamepad disconnected script callbacks
        const int gamepadIndex = event.cdevice.which;
        SDL_JoystickClose(activeGamePads[gamepadIndex].joystickController);
        for (int i = 0; i < activeGamepadCount; i++) {
            if (activeGamepadIds[i] == gamepadIndex) {
                RBE_STATIC_ARRAY_REMOVE(activeGamepadIds, gamepadIndex, INVALID_GAMEPAD_ID);
                activeGamepadCount--;
                break;
            }
        }
        rbe_logger_error("Failed to remove gamepad with index '%d'", gamepadIndex);
        break;
    }
    case SDL_JOYBUTTONDOWN:
    case SDL_JOYBUTTONUP: {
        buttonInputUpdated = true;
        const SDL_JoystickID controllerId = event.jbutton.which;
        const bool isButtonPressed = event.jbutton.state == SDL_PRESSED;
        const uint8_t buttonValue = event.jbutton.button;
        RBE_ASSERT(buttonValue < CRE_MAX_GAMEPAD_INTERNAL_INPUT_ACTIONS);
        CreGamepad* gamepad = input_find_gamepad(controllerId);
        if (isButtonPressed) {
            gamepad->gamepadInputButtonActions[buttonValue].isPressed = true;
            gamepad->gamepadInputButtonActions[buttonValue].isJustPressed = true;
        } else {
            gamepad->gamepadInputButtonActions[buttonValue].isPressed = false;
            gamepad->gamepadInputButtonActions[buttonValue].isJustPressed = false;
            gamepad->gamepadInputButtonActions[buttonValue].isJustReleased = true;
        }
        break;
    }
    case SDL_JOYHATMOTION: {
        buttonInputUpdated = true;
        const int controllerId = event.jhat.which;
        const uint8_t hatValue = event.jhat.value;
        // Process Joyhat Motion (dpad)
        if (hatValue & SDL_HAT_LEFT) {
            activeGamePads[controllerId].gamepadInputButtonActions[GamepadInputButtonType_DPAD_LEFT].isPressed = true;
            activeGamePads[controllerId].gamepadInputButtonActions[GamepadInputButtonType_DPAD_LEFT].isJustPressed = true;
        } else {
            activeGamePads[controllerId].gamepadInputButtonActions[GamepadInputButtonType_DPAD_LEFT].isPressed = false;
            activeGamePads[controllerId].gamepadInputButtonActions[GamepadInputButtonType_DPAD_LEFT].isJustPressed = false;
            activeGamePads[controllerId].gamepadInputButtonActions[GamepadInputButtonType_DPAD_LEFT].isJustReleased = true;
        }
        if (hatValue & SDL_HAT_RIGHT) {
            activeGamePads[controllerId].gamepadInputButtonActions[GamepadInputButtonType_DPAD_RIGHT].isPressed = true;
            activeGamePads[controllerId].gamepadInputButtonActions[GamepadInputButtonType_DPAD_RIGHT].isJustReleased = true;
        } else {
            activeGamePads[controllerId].gamepadInputButtonActions[GamepadInputButtonType_DPAD_RIGHT].isPressed = false;
            activeGamePads[controllerId].gamepadInputButtonActions[GamepadInputButtonType_DPAD_RIGHT].isJustPressed = false;
            activeGamePads[controllerId].gamepadInputButtonActions[GamepadInputButtonType_DPAD_RIGHT].isJustReleased = true;
        }
        if (hatValue & SDL_HAT_UP) {
            activeGamePads[controllerId].gamepadInputButtonActions[GamepadInputButtonType_DPAD_UP].isPressed = true;
            activeGamePads[controllerId].gamepadInputButtonActions[GamepadInputButtonType_DPAD_UP].isJustPressed = true;
        } else {
            activeGamePads[controllerId].gamepadInputButtonActions[GamepadInputButtonType_DPAD_UP].isPressed = false;
            activeGamePads[controllerId].gamepadInputButtonActions[GamepadInputButtonType_DPAD_UP].isJustPressed = false;
            activeGamePads[controllerId].gamepadInputButtonActions[GamepadInputButtonType_DPAD_UP].isJustReleased = true;
        }
        if (hatValue & SDL_HAT_DOWN) {
            activeGamePads[controllerId].gamepadInputButtonActions[GamepadInputButtonType_DPAD_DOWN].isPressed = true;
            activeGamePads[controllerId].gamepadInputButtonActions[GamepadInputButtonType_DPAD_DOWN].isJustPressed = true;
        } else {
            activeGamePads[controllerId].gamepadInputButtonActions[GamepadInputButtonType_DPAD_DOWN].isPressed = false;
            activeGamePads[controllerId].gamepadInputButtonActions[GamepadInputButtonType_DPAD_DOWN].isJustPressed = false;
            activeGamePads[controllerId].gamepadInputButtonActions[GamepadInputButtonType_DPAD_DOWN].isJustReleased = true;
        }
        break;
    }
    default:
        break;
    }

    // Process Axis Motion
    if (input_process_axis_motions()) {
        buttonInputUpdated = true;
    }


    // Update actions
    if (buttonInputUpdated) {
        for (size_t i = 0; i < inputActionNamesCount; i++) {
            InputAction* inputAction = (InputAction*) rbe_string_hash_map_get(inputActionMap, inputActionNames[i]);
            for (size_t j = 0; j < inputAction->gamepadValueCount; j++) {
                const char* gamepadValue = inputAction->gamepadValues[j];
                const uint8_t buttonScancode = rbe_string_hash_map_get_int(gamepadStringValuesMap, gamepadValue);
                inputAction->isActionPressed = activeGamePads[inputAction->deviceId].gamepadInputButtonActions[buttonScancode].isPressed;
                inputAction->isActionJustPressed = activeGamePads[inputAction->deviceId].gamepadInputButtonActions[buttonScancode].isJustPressed;
                inputAction->isActionJustReleased = activeGamePads[inputAction->deviceId].gamepadInputButtonActions[buttonScancode].isJustReleased;
                // Set flags
                if (inputAction->isActionJustPressed) {
                    actionJustPressedClean.inputActions[actionJustPressedClean.count++] = inputAction;
                }
                if (inputAction->isActionJustReleased) {
                    actionJustReleasedClean.inputActions[actionJustReleasedClean.count++] = inputAction;
                }
            }
        }
    }
}

// TODO: Get 'strength' of axis motions for more fine tuned controls from the user's perspective
bool input_process_axis_motions() {
    bool inputUpdated = false;

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
                inputUpdated = true;
            }
        } else if (leftHorizontalValue > GAMEPAD_AXIS_DEAD_ZONE) {
            if (!gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_RIGHT].isPressed) {
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_RIGHT].isPressed = true;
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_RIGHT].isJustPressed = true;
                inputUpdated = true;
            }
        } else {
            if (gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_LEFT].isPressed) {
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_LEFT].isPressed = false;
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_LEFT].isJustReleased = true;
                inputUpdated = true;
            }
            if (gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_RIGHT].isPressed) {
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_RIGHT].isPressed = false;
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_RIGHT].isJustReleased = true;
                inputUpdated = true;
            }
        }
        // Vertical
        const int16_t leftVerticalValue = SDL_JoystickGetAxis(gamepad->joystickController, GamepadInputAxisMotionType_LEFT_VERTICAL_AXIS);
        if (leftVerticalValue < -GAMEPAD_AXIS_DEAD_ZONE) {
            if (!gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_UP].isPressed) {
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_UP].isPressed = true;
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_UP].isJustPressed = true;
                inputUpdated = true;
            }
        } else if (leftVerticalValue > GAMEPAD_AXIS_DEAD_ZONE) {
            if (!gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_DOWN].isPressed) {
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_DOWN].isPressed = true;
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_DOWN].isJustPressed = true;
                inputUpdated = true;
            }
        } else {
            if (gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_UP].isPressed) {
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_UP].isPressed = false;
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_UP].isJustReleased = true;
                inputUpdated = true;
            }
            if (gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_DOWN].isPressed) {
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_DOWN].isPressed = false;
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_ANALOG_DOWN].isJustReleased = true;
                inputUpdated = true;
            }
        }

        // RIGHT AXIS
        // Horizontal
        const int16_t rightHorizontalValue = SDL_JoystickGetAxis(gamepad->joystickController, GamepadInputAxisMotionType_RIGHT_HORIZONTAL_AXIS);
        if (rightHorizontalValue < -GAMEPAD_AXIS_DEAD_ZONE) {
            if (!gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_LEFT].isPressed) {
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_LEFT].isPressed = true;
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_LEFT].isJustPressed = true;
                inputUpdated = true;
            }
        } else if (rightHorizontalValue > GAMEPAD_AXIS_DEAD_ZONE) {
            if (!gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_RIGHT].isPressed) {
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_RIGHT].isPressed = true;
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_RIGHT].isJustPressed = true;
                inputUpdated = true;
            }
        } else {
            if (gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_LEFT].isPressed) {
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_LEFT].isPressed = false;
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_LEFT].isJustReleased = true;
                inputUpdated = true;
            }
            if (gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_RIGHT].isPressed) {
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_RIGHT].isPressed = false;
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_RIGHT].isJustReleased = true;
                inputUpdated = true;
            }
        }
        // Vertical
        const int16_t rightVerticalValue = SDL_JoystickGetAxis(gamepad->joystickController, GamepadInputAxisMotionType_RIGHT_VERTICAL_AXIS);
        if (rightVerticalValue < -GAMEPAD_AXIS_DEAD_ZONE) {
            if (!gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_UP].isPressed) {
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_UP].isPressed = true;
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_UP].isJustPressed = true;
                inputUpdated = true;
            }
        } else if (rightVerticalValue > GAMEPAD_AXIS_DEAD_ZONE) {
            if (!gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_DOWN].isPressed) {
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_DOWN].isPressed = true;
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_DOWN].isJustPressed = true;
                inputUpdated = true;
            }
        } else {
            if (gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_UP].isPressed) {
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_UP].isPressed = false;
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_UP].isJustReleased = true;
                inputUpdated = true;
            }
            if (gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_DOWN].isPressed) {
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_DOWN].isPressed = false;
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_ANALOG_DOWN].isJustReleased = true;
                inputUpdated = true;
            }
        }

        // TRIGGERS
        // Left Trigger
        const int16_t leftTriggerValue = SDL_JoystickGetAxis(gamepad->joystickController, GamepadInputAxisMotionType_LEFT_TRIGGER);
        if (leftTriggerValue < -GAMEPAD_TRIGGER_DEAD_ZONE) {
            if (gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_TRIGGER].isPressed) {
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_TRIGGER].isPressed = false;
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_TRIGGER].isJustReleased = true;
                inputUpdated = true;
            }
        } else if (leftTriggerValue > GAMEPAD_AXIS_DEAD_ZONE) {
            if (!gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_TRIGGER].isPressed) {
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_TRIGGER].isPressed = true;
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_LEFT_TRIGGER].isJustPressed = true;
                inputUpdated = true;
            }
        }
        // Right Trigger
        const int16_t rightTriggerValue = SDL_JoystickGetAxis(gamepad->joystickController, GamepadInputAxisMotionType_RIGHT_TRIGGER);
        if (rightTriggerValue < -GAMEPAD_TRIGGER_DEAD_ZONE) {
            if (gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_TRIGGER].isPressed) {
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_TRIGGER].isPressed = false;
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_TRIGGER].isJustReleased = true;
                inputUpdated = true;
            }
        } else if (rightTriggerValue > GAMEPAD_AXIS_DEAD_ZONE) {
            if (!gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_TRIGGER].isPressed) {
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_TRIGGER].isPressed = true;
                gamepad->gamepadInputButtonActions[GamepadInputButtonType_RIGHT_TRIGGER].isJustPressed = true;
                inputUpdated = true;
            }
        }
    }

    return inputUpdated;
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
