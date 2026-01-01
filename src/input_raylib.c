#include "input_raylib.h"
#include "input/input_provider.h"
#include "raylib.h"
#include <stdlib.h>
#include <string.h>

// Map InputKey enum to raylib key codes
static int raylib_key_from_input_key(InputKey key) {
    switch (key) {
        case INPUT_KEY_W: return KEY_W;
        case INPUT_KEY_A: return KEY_A;
        case INPUT_KEY_S: return KEY_S;
        case INPUT_KEY_D: return KEY_D;
        case INPUT_KEY_UP: return KEY_UP;
        case INPUT_KEY_DOWN: return KEY_DOWN;
        case INPUT_KEY_LEFT: return KEY_LEFT;
        case INPUT_KEY_RIGHT: return KEY_RIGHT;
        case INPUT_KEY_F3: return KEY_F3;
        default: return 0;
    }
}

// Map MouseButton enum to raylib mouse button codes
static int raylib_mouse_button_from_input_mouse_button(InputMouseButton button) {
    switch (button) {
        case INPUT_MOUSE_BUTTON_LEFT: return MOUSE_BUTTON_LEFT;
        case INPUT_MOUSE_BUTTON_RIGHT: return MOUSE_BUTTON_RIGHT;
        case INPUT_MOUSE_BUTTON_MIDDLE: return MOUSE_BUTTON_MIDDLE;
        default: return 0;
    }
}

static bool raylib_is_key_pressed(InputProvider* provider, InputKey key) {
    (void)provider;
    int raylibKey = raylib_key_from_input_key(key);
    return IsKeyPressed(raylibKey);
}

static bool raylib_is_mouse_button_pressed(InputProvider* provider, InputMouseButton button) {
    (void)provider;
    int raylibButton = raylib_mouse_button_from_input_mouse_button(button);
    return IsMouseButtonPressed(raylibButton);
}

static float raylib_get_mouse_wheel_move(InputProvider* provider) {
    (void)provider;
    return GetMouseWheelMove();
}

static RenderVector2 raylib_get_mouse_position(InputProvider* provider) {
    (void)provider;
    Vector2 mouse = GetMousePosition();
    return (RenderVector2){mouse.x, mouse.y};
}

static const InputProviderVTable raylib_input_vtable = {
    .is_key_pressed = raylib_is_key_pressed,
    .is_mouse_button_pressed = raylib_is_mouse_button_pressed,
    .get_mouse_wheel_move = raylib_get_mouse_wheel_move,
    .get_mouse_position = raylib_get_mouse_position,
};

InputProvider* InputProviderRaylib_create(void) {
    InputProvider* provider = (InputProvider*)malloc(sizeof(InputProvider));
    if (!provider) return NULL;
    
    provider->vtable = &raylib_input_vtable;
    provider->backendData = NULL;  // No backend-specific data needed for raylib
    
    return provider;
}

void InputProviderRaylib_destroy(InputProvider* provider) {
    if (provider) {
        free(provider);
    }
}

