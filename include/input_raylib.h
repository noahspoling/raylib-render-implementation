#ifndef INPUT_RAYLIB_H
#define INPUT_RAYLIB_H

#include "gramarye_renderer/input_provider.h"

InputProvider* InputProviderRaylib_create(void);
void InputProviderRaylib_destroy(InputProvider* provider);

#endif // INPUT_RAYLIB_H

