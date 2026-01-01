#ifndef RENDERER_RAYLIB_H
#define RENDERER_RAYLIB_H

#include "gramarye_renderer/renderer.h"

Renderer* RendererRaylib_create(void);
void RendererRaylib_destroy(Renderer* r);

#endif