#ifndef INPUT_RAYLIB_H
#define INPUT_RAYLIB_H

#include "input/input_provider.h"

InputProvider* InputProviderRaylib_create(void);
void InputProviderRaylib_destroy(InputProvider* provider);

#endif // INPUT_RAYLIB_H

