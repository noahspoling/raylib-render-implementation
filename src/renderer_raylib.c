#include "renderer_raylib.h"
#include "gramarye_renderer/renderer.h"
#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    Font defaultFont;
    Camera3D camera3D;
    bool camera3DInitialized;
} RaylibRendererData;

// Convert backend-agnostic WindowFlags to raylib-specific flags
static unsigned int raylib_flags_from_window_flags(WindowFlags flags) {
    unsigned int raylibFlags = 0;
    
    if (flags & WINDOW_FLAG_VSYNC) {
        raylibFlags |= FLAG_VSYNC_HINT;
    }
    if (flags & WINDOW_FLAG_RESIZABLE) {
        raylibFlags |= FLAG_WINDOW_RESIZABLE;
    }
    if (flags & WINDOW_FLAG_BORDERLESS) {
        raylibFlags |= FLAG_BORDERLESS_WINDOWED_MODE;
    }
    if (flags & WINDOW_FLAG_MSAA_4X) {
        raylibFlags |= FLAG_MSAA_4X_HINT;
    }
    
    return raylibFlags;
}

static void raylib_init(Renderer* r, int width, int height, const char* title, WindowFlags flags) {
    // SetConfigFlags must be called BEFORE InitWindow
    SetConfigFlags(raylib_flags_from_window_flags(flags));
    InitWindow(width, height, title);
    SetTargetFPS(60);
    
    RaylibRendererData* data = (RaylibRendererData*)r->backendData;
    data->defaultFont = GetFontDefault();
    data->camera3DInitialized = false;
}

static void raylib_close(Renderer* r) {
    (void)r;
    CloseWindow();
}

static void raylib_begin_frame(Renderer* r) {
    (void)r;
    BeginDrawing();
}

static void raylib_end_frame(Renderer* r) {
    (void)r;
    EndDrawing();
}

static void raylib_execute_command(Renderer* r, const RenderCommand* cmd) {
    RaylibRendererData* data = (RaylibRendererData*)r->backendData;
    
    switch (cmd->type) {
        case RENDER_COMMAND_TYPE_NONE:
            break;
            
        case RENDER_COMMAND_TYPE_RECTANGLE: {
            DrawRectangle((int)cmd->bounds.x, (int)cmd->bounds.y, 
                         (int)cmd->bounds.width, (int)cmd->bounds.height,
                         (Color){cmd->color.r, cmd->color.g, cmd->color.b, cmd->color.a});
            break;
        }
        
        case RENDER_COMMAND_TYPE_RECTANGLE_LINES: {
            DrawRectangleLines((int)cmd->bounds.x, (int)cmd->bounds.y,
                             (int)cmd->bounds.width, (int)cmd->bounds.height,
                             (Color){cmd->color.r, cmd->color.g, cmd->color.b, cmd->color.a});
            break;
        }
        
        case RENDER_COMMAND_TYPE_TEXTURE: {
            if (cmd->data.texture.textureHandle) {
                Texture2D* tex = (Texture2D*)cmd->data.texture.textureHandle;
                Rectangle src = {
                    cmd->data.texture.srcRect.x,
                    cmd->data.texture.srcRect.y,
                    cmd->data.texture.srcRect.width,
                    cmd->data.texture.srcRect.height
                };
                Rectangle dst = {
                    cmd->bounds.x,
                    cmd->bounds.y,
                    cmd->bounds.width,
                    cmd->bounds.height
                };
                Vector2 origin = {
                    cmd->data.texture.origin.x,
                    cmd->data.texture.origin.y
                };
                DrawTexturePro(*tex, src, dst, origin, cmd->data.texture.rotation,
                             (Color){cmd->color.r, cmd->color.g, cmd->color.b, cmd->color.a});
            }
            break;
        }
        
        case RENDER_COMMAND_TYPE_TEXTURE_PRO:
            // Same as TEXTURE for now
            if (cmd->data.texture.textureHandle) {
                Texture2D* tex = (Texture2D*)cmd->data.texture.textureHandle;
                Rectangle src = {
                    cmd->data.texture.srcRect.x,
                    cmd->data.texture.srcRect.y,
                    cmd->data.texture.srcRect.width,
                    cmd->data.texture.srcRect.height
                };
                Rectangle dst = {
                    cmd->bounds.x,
                    cmd->bounds.y,
                    cmd->bounds.width,
                    cmd->bounds.height
                };
                Vector2 origin = {
                    cmd->data.texture.origin.x,
                    cmd->data.texture.origin.y
                };
                DrawTexturePro(*tex, src, dst, origin, cmd->data.texture.rotation,
                             (Color){cmd->color.r, cmd->color.g, cmd->color.b, cmd->color.a});
            }
            break;
        
        case RENDER_COMMAND_TYPE_TEXT: {
            Font* font = cmd->data.text.fontHandle ? (Font*)cmd->data.text.fontHandle : &data->defaultFont;
            if (cmd->data.text.text) {
                DrawTextEx(*font, cmd->data.text.text,
                          (Vector2){cmd->bounds.x, cmd->bounds.y},
                          cmd->data.text.fontSize,
                          cmd->data.text.spacing,
                          (Color){cmd->color.r, cmd->color.g, cmd->color.b, cmd->color.a});
            }
            break;
        }
        
        case RENDER_COMMAND_TYPE_CIRCLE: {
            DrawCircle((int)cmd->bounds.x, (int)cmd->bounds.y, cmd->bounds.width,
                      (Color){cmd->color.r, cmd->color.g, cmd->color.b, cmd->color.a});
            break;
        }
        
        case RENDER_COMMAND_TYPE_TRIANGLE: {
            Vector2 v1 = {cmd->bounds.x, cmd->bounds.y};
            Vector2 v2 = {cmd->bounds.x + cmd->bounds.width, cmd->bounds.y};
            Vector2 v3 = {cmd->bounds.x, cmd->bounds.y + cmd->bounds.height};
            DrawTriangle(v1, v2, v3,
                        (Color){cmd->color.r, cmd->color.g, cmd->color.b, cmd->color.a});
            break;
        }
        
        case RENDER_COMMAND_TYPE_BEGIN_2D:
            // Already handled by begin_frame
            break;
            
        case RENDER_COMMAND_TYPE_END_2D:
            // Already handled by end_frame
            break;
            
        case RENDER_COMMAND_TYPE_BEGIN_3D:
            if (!data->camera3DInitialized) {
                data->camera3D.position = (Vector3){0.0f, 0.0f, 10.0f};
                data->camera3D.target = (Vector3){0.0f, 0.0f, 0.0f};
                data->camera3D.up = (Vector3){0.0f, 1.0f, 0.0f};
                data->camera3D.fovy = 45.0f;
                data->camera3D.projection = CAMERA_PERSPECTIVE;
                data->camera3DInitialized = true;
            }
            BeginMode3D(data->camera3D);
            break;
            
        case RENDER_COMMAND_TYPE_END_3D:
            EndMode3D();
            break;
            
        case RENDER_COMMAND_TYPE_BEGIN_LAYER:
        case RENDER_COMMAND_TYPE_END_LAYER:
        case RENDER_COMMAND_TYPE_BEGIN_UI:
        case RENDER_COMMAND_TYPE_END_UI:
        case RENDER_COMMAND_TYPE_BEGIN_CLIP:
        case RENDER_COMMAND_TYPE_END_CLIP:
        case RENDER_COMMAND_TYPE_BEGIN_DEBUG:
        case RENDER_COMMAND_TYPE_END_DEBUG:
        case RENDER_COMMAND_TYPE_CUSTOM:
            // These are handled at a higher level or not yet implemented
            break;
            
        default:
            // Silently ignore unknown commands for now
            break;
    }
}

static void raylib_execute_commands(Renderer* r, const RenderCommand* commands, size_t count) {
    for (size_t i = 0; i < count; i++) {
        raylib_execute_command(r, &commands[i]);
    }
}

static void* raylib_load_texture(Renderer* r, const char* path) {
    (void)r;
    Texture2D* tex = (Texture2D*)malloc(sizeof(Texture2D));
    if (tex) {
        *tex = LoadTexture(path);
    }
    return tex;
}

static void raylib_unload_texture(Renderer* r, void* textureHandle) {
    (void)r;
    if (textureHandle) {
        Texture2D* tex = (Texture2D*)textureHandle;
        UnloadTexture(*tex);
        free(tex);
    }
}

static void* raylib_load_font(Renderer* r, const char* path, float fontSize) {
    (void)r;
    Font* font = (Font*)malloc(sizeof(Font));
    if (font) {
        *font = LoadFontEx(path, (int)fontSize, NULL, 0);
    }
    return font;
}

static void raylib_unload_font(Renderer* r, void* fontHandle) {
    (void)r;
    if (fontHandle) {
        Font* font = (Font*)fontHandle;
        UnloadFont(*font);
        free(font);
    }
}

static bool raylib_should_close(Renderer* r) {
    (void)r;
    return WindowShouldClose();
}

static float raylib_get_delta_time(Renderer* r) {
    (void)r;
    return GetFrameTime();
}

static RenderVector2 raylib_get_mouse_position(Renderer* r) {
    (void)r;
    Vector2 mouse = GetMousePosition();
    return (RenderVector2){mouse.x, mouse.y};
}

static RenderVector2 raylib_get_window_size(Renderer* r) {
    (void)r;
    int width = GetScreenWidth();
    int height = GetScreenHeight();
    return (RenderVector2){(float)width, (float)height};
}

static int raylib_get_render_width(Renderer* r) {
    (void)r;
    return GetRenderWidth();
}

static int raylib_get_render_height(Renderer* r) {
    (void)r;
    return GetRenderHeight();
}

static int raylib_get_screen_width(Renderer* r) {
    (void)r;
    return GetScreenWidth();
}

static int raylib_get_screen_height(Renderer* r) {
    (void)r;
    return GetScreenHeight();
}

// Camera transformation functions
// These cast the opaque handles to the actual game types (Camera2DEx, AspectFit)
static RenderVector2 raylib_world_to_screen(Renderer* r, CameraHandle camera, AspectFitHandle aspectFit, RenderVector2 world) {
    (void)r;
    // Cast opaque handles to actual types (these are game-specific, but we know the implementation)
    typedef struct {
        Vector2 pos;
        float zoom;
        float minZoom;
        float maxZoom;
        float speed;
        Vector2 logicalSize;
    } Camera2DEx;
    
    typedef struct {
        Rectangle dest;
        float scale;
    } AspectFit;
    
    Camera2DEx* cam = (Camera2DEx*)camera;
    AspectFit* fit = (AspectFit*)aspectFit;
    
    if (!cam || !fit) {
        return (RenderVector2){0.0f, 0.0f};
    }
    
    Vector2 logical = { (world.x - cam->pos.x) * cam->zoom, (world.y - cam->pos.y) * cam->zoom };
    Vector2 result = { fit->dest.x + logical.x * fit->scale, fit->dest.y + logical.y * fit->scale };
    return (RenderVector2){result.x, result.y};
}

static RenderVector2 raylib_screen_to_world(Renderer* r, CameraHandle camera, AspectFitHandle aspectFit, RenderVector2 screen) {
    (void)r;
    // Cast opaque handles to actual types
    typedef struct {
        Vector2 pos;
        float zoom;
        float minZoom;
        float maxZoom;
        float speed;
        Vector2 logicalSize;
    } Camera2DEx;
    
    typedef struct {
        Rectangle dest;
        float scale;
    } AspectFit;
    
    Camera2DEx* cam = (Camera2DEx*)camera;
    AspectFit* fit = (AspectFit*)aspectFit;
    
    if (!cam || !fit) {
        return (RenderVector2){0.0f, 0.0f};
    }
    
    Vector2 logical = { (screen.x - fit->dest.x) / fit->scale, (screen.y - fit->dest.y) / fit->scale };
    Vector2 result = { logical.x / cam->zoom + cam->pos.x, logical.y / cam->zoom + cam->pos.y };
    return (RenderVector2){result.x, result.y};
}

static float raylib_get_camera_zoom(Renderer* r, CameraHandle camera) {
    (void)r;
    typedef struct {
        Vector2 pos;
        float zoom;
        float minZoom;
        float maxZoom;
        float speed;
        Vector2 logicalSize;
    } Camera2DEx;
    
    Camera2DEx* cam = (Camera2DEx*)camera;
    if (!cam) return 1.0f;
    return cam->zoom;
}

static float raylib_get_aspect_fit_scale(Renderer* r, AspectFitHandle aspectFit) {
    (void)r;
    typedef struct {
        Rectangle dest;
        float scale;
    } AspectFit;
    
    AspectFit* fit = (AspectFit*)aspectFit;
    if (!fit) return 1.0f;
    return fit->scale;
}

static const RendererVTable raylib_vtable = {
    .init = raylib_init,
    .close = raylib_close,
    .begin_frame = raylib_begin_frame,
    .end_frame = raylib_end_frame,
    .execute_command = raylib_execute_command,
    .execute_commands = raylib_execute_commands,
    .load_texture = raylib_load_texture,
    .unload_texture = raylib_unload_texture,
    .load_font = raylib_load_font,
    .unload_font = raylib_unload_font,
    .should_close = raylib_should_close,
    .get_delta_time = raylib_get_delta_time,
    .get_mouse_position = raylib_get_mouse_position,
    .get_window_size = raylib_get_window_size,
    .get_render_width = raylib_get_render_width,
    .get_render_height = raylib_get_render_height,
    .get_screen_width = raylib_get_screen_width,
    .get_screen_height = raylib_get_screen_height,
    .world_to_screen = raylib_world_to_screen,
    .screen_to_world = raylib_screen_to_world,
    .get_camera_zoom = raylib_get_camera_zoom,
    .get_aspect_fit_scale = raylib_get_aspect_fit_scale,
};

Renderer* RendererRaylib_create(void) {
    Renderer* r = (Renderer*)malloc(sizeof(Renderer));
    if (!r) return NULL;
    
    r->vtable = &raylib_vtable;
    r->backendData = malloc(sizeof(RaylibRendererData));
    if (!r->backendData) {
        free(r);
        return NULL;
    }
    
    memset(r->backendData, 0, sizeof(RaylibRendererData));
    return r;
}

void RendererRaylib_destroy(Renderer* r) {
    if (r) {
        if (r->backendData) {
            free(r->backendData);
        }
        free(r);
    }
}
