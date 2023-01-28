/*
 * =====================================================================================
 *
 *       Filename:  crga.c
 *
 *    Description: C file defining the primary functions exposed by the API 
 *
 *        Version:  1.0
 *        Created:  01/15/2023 03:04:06 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Spencer Welch
 *   Organization:  BYU-I
 *
 * =====================================================================================
 */
#include "crga.h"
#include <stdio.h>

CRConfig *cr_config;

// Init
void CRInit() {
    CRConfig *config = (CRConfig *) malloc(sizeof(CRConfig));
    CRInitConfig(config);
    CRSetConfig(config);
    CRInitWindow();
}
void CRInitConfig(CRConfig *config) {
    config->window_width = 800;
    config->window_height = 450;
    config->fps = 60;
    config->title = "CRGA Basic Window";

    config->default_foreground = WHITE;
    config->default_background = BLACK;
    config->default_visibility = 0;

    config->world_layers = 0;
    config->world_layer_count = 0;
    config->ui_layers = 0;
    config->ui_layer_count = 0;

    config->main_camera.target = (Vector2){0.0f, 0.0f};
    config->main_camera.offset = (Vector2){0.0f, 0.0f};
    config->main_camera.rotation = 0.0f;
    config->main_camera.zoom = 1.0f;

    config->tile_size = 20.0f;

    config->background_color = BLACK;

    config->fonts = 0;
    config->font_count = 0;
}
inline void CRSetConfig(CRConfig *config) {
    cr_config = config;
}
inline void CRInitWindow() {
    InitWindow(cr_config->window_width, cr_config->window_height, cr_config->title);
    SetTargetFPS(cr_config->fps);
}

// Cleanup Functions
void CRClose() {
    CRUnloadFonts();
    CRUnloadLayers();
    CloseWindow();
}
void CRUnloadLayers() {
    if (cr_config->world_layer_count > 0) {
        free(cr_config->world_layers);
    }
    if (cr_config->ui_layer_count > 0) {
        free(cr_config->ui_layers);
    }
}
inline void CRUnloadFonts() {
    for (int i = cr_config->font_count-1; i >= 0; i--) {
        UnloadFont(cr_config->fonts[i]);
    }
    free(cr_config->fonts);
}

// Loop
void CRLoop() {
    while (!WindowShouldClose()) {

        CRPreDraw();

        BeginDrawing();

            ClearBackground(cr_config->background_color);

            BeginMode2D(cr_config->main_camera);

                for (int i = 0; i < cr_config->world_layer_count; i++) {
                    CRDrawLayer(&cr_config->world_layers[i]);
                }
                CRWorldDraw();

            EndMode2D();

            for (int i = 0; i < cr_config->ui_layer_count; i++) {
                CRDrawLayer(&cr_config->ui_layers[i]);
            }
            CRUIDraw();

        EndDrawing();

        CRPostDraw();
    }
}
void CRWorldDraw() {}
void CRUIDraw() {}
void CRPreDraw() {}
void CRPostDraw() {}

// Font Loading
inline void CRLoadFont(const char *font_path) {
    CRLoadFontSize(font_path, 96);
}
void CRLoadFontSize(const char *font_path, int size) {
    if (cr_config->font_count == 255) {
        // there are too many fonts, exit out
        return;
    } else if (cr_config->font_count == 0) {
        cr_config->fonts = (Font *) malloc(sizeof(Font));
    } else {
        cr_config->fonts = (Font *) realloc(cr_config->fonts, sizeof(Font) * (cr_config->font_count + 1));
    }
    size_t index = cr_config->font_count;
    cr_config->font_count++;
    
    cr_config->fonts[index] = LoadFontEx(font_path, size, 0, 0);

    GenTextureMipmaps(&cr_config->fonts[index].texture);
    SetTextureFilter(cr_config->fonts[index].texture, TEXTURE_FILTER_POINT);
}

// Layers
CRLayer CRNewLayer(CRTile *grid, int width, int height, Vector2 position) {
    CRLayer layer;
    layer.mask = 0;
    layer.grid = grid;
    layer.entities.head = 0;
    layer.entities.tail = 0;
    layer.width = width;
    layer.height = height;
    layer.position = position;
    return layer;
}
void CRSetWorldLayer(int index, CRLayer layer) {
    if (cr_config->world_layer_count < index)
        return;// TODO out of bounds error
    cr_config->world_layers[index] = layer;
}
void CRSetUILayer(int index, CRLayer layer) {
    if (cr_config->ui_layer_count < index)
        return;// TODO out of bounds error
    cr_config->ui_layers[index] = layer;
}
void NewLayer(CRLayer *layers, size_t count) {
    if (cr_config->world_layer_count == 0) {
        cr_config->world_layers = (CRLayer *) malloc(sizeof(CRLayer));
    } else {
        cr_config->world_layers = (CRLayer *) realloc(cr_config->world_layers, 
                sizeof(CRLayer) * (cr_config->world_layer_count + 1));
    }
}
void CRAddWorldLayer(int index, CRLayer layer) {
    if (index > cr_config->world_layer_count)
        return;// TODO out of bounds error
    if (index == cr_config->world_layer_count) {
        CRAppendWorldLayer(layer);
    }
    NewLayer(cr_config->world_layers, cr_config->world_layer_count);
    for (int i = cr_config->world_layer_count - 1; i >= index; i--) {
        cr_config->world_layers[i+1] = cr_config->world_layers[i];
    }
    cr_config->world_layer_count++;
}
void CRAddUILayer(int index, CRLayer layer) {
    if (index > cr_config->ui_layer_count)
        return;// TODO out of bounds error
    if (index == cr_config->ui_layer_count) {
        CRAppendUILayer(layer);
    }
    NewLayer(cr_config->ui_layers, cr_config->ui_layer_count);
    for (int i = cr_config->ui_layer_count - 1; i >= index; i--) {
        cr_config->ui_layers[i+1] = cr_config->ui_layers[i];
    }
    cr_config->ui_layer_count++;
}
void CRAppendWorldLayer(CRLayer layer) {
    NewLayer(cr_config->world_layers, cr_config->world_layer_count);
    cr_config->world_layers[cr_config->world_layer_count] = layer;
    cr_config->world_layer_count++;

}
void CRAppendUILayer(CRLayer layer) {
    NewLayer(cr_config->ui_layers, cr_config->ui_layer_count);
    cr_config->ui_layers[cr_config->ui_layer_count] = layer;
    cr_config->ui_layer_count++;
}

// Entities
CREntity CRNewEntity(CRTile tile, Vector2 position) {
    CREntity entity;
    entity.tile = tile;
    entity.position = position;
    entity.next = 0;
    entity.prev = 0;
    return entity;
}
void CRAddEntity(CREntity *entity) {
    if (cr_config->world_layer_count == 0)
        return;// TODO layer doesn't exist to write to
    CRAddEntityToLayer(0, entity);
}
void CRAddEntityToLayer(CRLayer *layer, CREntity *entity) {
    if (layer == 0)
        layer = &cr_config->world_layers[0];
    // Remove entity from its current position
    CREntity *next = entity->next;
    CREntity *prev = entity->prev;
    if (next != 0) {
        next->prev = prev;
        next = 0;
    }
    if (prev != 0) {
        prev->next = next;
        prev = 0;
    }
    // No entities in the layer
    if (layer->entities.head == 0) {
        layer->entities.head = entity;
        layer->entities.tail = entity;
    } else if (layer->entities.tail == entity) {
        return;
    }  else { // entity exists on layer
        layer->entities.tail->next = entity;
        layer->entities.tail = entity;
    }
}

// Tiles
CRTile DefaultTileConfig(int index) {
    CRTile tile;
    tile.index.i = index;
    tile.shift = (Vector2) {0, 0};
    tile.foreground = cr_config->default_foreground;
    tile.background = cr_config->default_background;
    tile.visibility = cr_config->default_visibility;
    return tile;
}
CRTile CRCTile(char *string) {
    CRTile tile = DefaultTileConfig(0);
    for (int i = 0; i <= 4; i++) {
        if (string[i] == 0)
            break;
        tile.index.c[i] = string[i];
    }
    return tile;
}
CRTile CRNewTileIndex(int index) {
    CRTile tile = DefaultTileConfig(index);
    return tile;
}
void SetGrid(CRLayer *layer, CRTile *grid, int width, int height) {
    layer->grid = grid;
    layer->width = width;
    layer->height = height;
}
void CRSetTileOnGrid(CRTile *grid, CRTile tile, Vector2 position, int width, int height) {
    int x = position.x;
    int y = position.y;
    if (x > width || y > height)
        return; // TODO return out of bounds error
    grid[x + y * width] = tile;
}
void CRSetTileOnLayer(CRLayer *layer, CRTile tile, Vector2 position) {
    int x = position.x;
    int y = position.y;
    int width = layer->width;
    int height = layer->height;
    if (x > width || y > height)
        return; // TODO return out of bounds error
    layer->grid[x + y * width] = tile;
}
void CRSetWorldTile(CRTile tile, Vector2 position) {
    CRSetTileOnLayer(&cr_config->world_layers[0], tile, position);
}
void CRSetUITile(CRTile tile, Vector2 position) {
    CRSetTileOnLayer(&cr_config->ui_layers[0], tile, position);
}
void CRSetTileOnWorldLayer(int layer_index, CRTile tile, Vector2 position) {
    if (cr_config->world_layer_count < layer_index)
        return; // TODO return out of bounds error
    CRSetTileOnLayer(cr_config->world_layers + layer_index, tile, position);
}
void CRSetTileOnUILayer(int layer_index, CRTile tile, Vector2 position) {
    if (cr_config->ui_layer_count < layer_index)
        return; // TODO return out of bounds error
    CRSetTileOnLayer(cr_config->world_layers + layer_index, tile, position);
}
void CRSetWorldGrid(CRTile *grid, int width, int height) {
    if (cr_config->world_layer_count == 0) {
        Vector2 position = {0.0f, 0.0f};
        CRLayer layer = CRNewLayer(grid, width, height, position);
        CRAppendWorldLayer(layer);
    }
    CRSetTilesOnWorldLayer(grid, 0, width, height);
}
void CRSetUIGrid(CRTile *grid, int width, int height) {
    if (cr_config->ui_layer_count == 0) {
        Vector2 position = {0.0f, 0.0f};
        CRLayer layer = CRNewLayer(grid, width, height, position);
        CRAppendUILayer(layer);
    }
    CRSetTilesOnUILayer(grid, 0, width, height);
}
int CRSetTilesOnWorldLayer(CRTile *grid, int layer, int width, int height) {
    if (layer >= cr_config->world_layer_count)
        return 1;
    SetGrid(cr_config->world_layers + layer, grid, width, height);
    return 0;
}
int CRSetTilesOnUILayer(CRTile *grid, int layer, int width, int height) {
    if (layer >= cr_config->ui_layer_count)
        return 1;
    SetGrid(cr_config->ui_layers + layer, grid, width, height);
    return 0;
}

// Draw Tiles
Vector2 ShiftPosition(Vector2 position, Vector2 shift) {
    position.x += shift.x;
    position.y += shift.y;

    return position;
}
Vector2 CenterText(Vector2 position, float tile_size, char *string_out) {
    Vector2 size;
    size.x = MeasureText(string_out, 24);
    position.x += tile_size/2.0f - size.x/2.0f;
    return position;
}
Vector2 CenterTextEx(Vector2 position, Font font, float tile_size, char *string_out) {
    Vector2 size = MeasureTextEx(font, string_out, 24, 0);
    position.x += tile_size/2.0f - size.x/2.0f;
    position.y += tile_size/2.0f - size.y/2.0f;
    return position;
}
void CRDrawTile(CRTile *tile, float tile_size, Vector2 position) {
    CRDrawTileChar(tile, tile_size, position);
}
void CRDrawTileChar(CRTile *tile, float tile_size, Vector2 position) {
    if (tile->index.i == 0)
        return;
    char string_out[5];
    for (int i = 0; i <= 4; i++)
        string_out[i] = tile->index.c[i];
    float x_pos = position.x;
    float y_pos = position.y;
    DrawRectangle(x_pos, y_pos, tile_size, tile_size, tile->background);
#if GRID_OUTLINE
    DrawRectangleLines(x_pos, y_pos, tile_size, tile_size, RED);
#endif
    Vector2 shift = tile->shift;
    if (cr_config->font_count > 0) {
        position = CenterTextEx(position, cr_config->fonts[0], tile_size, string_out);
        position = ShiftPosition(position, shift);
        DrawTextEx(cr_config->fonts[0], string_out, position, 24, 0, WHITE);
    } else {
        position = CenterText(position, tile_size, string_out);
        position = ShiftPosition(position, shift);
        DrawText(string_out, position.x, position.y, 24, WHITE);
    }
}
#if 0 // TODO
void CRDrawTileMap(CRTile *tile, float tile_size, float x_pos, float y_pos) {
    if (tile->index.i == 0)
        return;
    int32_t index = tile->index.i;
    DrawRectangle(x_pos, y_pos, tile_size, tile_size, tile->background);
#if GRID_OUTLINE
    DrawRectangleLines(x_pos, y_pos, tile_size, tile_size, RED);
#endif
    Vector2 position = {x_pos, y_pos};
}
#endif
void CRDrawLayer(CRLayer *layer) {
    float tile_size = cr_config->tile_size;
    for (int row = 0; row < layer->height; row++) {
        for (int col = 0; col < layer->width; col++) {
            CRTile *tile = &layer->grid[col * layer->width + row];
            CRDrawTile(tile, tile_size, (Vector2) {tile_size * row, tile_size * col});
        }
    }
    if (layer->entities.head == 0)
        return;
    CREntity *itr = layer->entities.head;
    while (itr != 0) {
        CRTile *tile = &itr->tile;
        Vector2 position = layer->entities.head->position;
        position.x *= tile_size;
        position.y *= tile_size;
        CRDrawTile(tile, tile_size, position);
        itr = itr->next;
    }
}

// Camera functions
Camera2D *CRGetMainCamera() {
    return &cr_config->main_camera;
}
void CRSetCameraTarget(Camera2D *camera, Vector2 target) {
    Vector2 target_out = (Vector2) {target.x * cr_config->tile_size, target.y * cr_config->tile_size};
    camera->target = target_out;
}
void CRSetCameraOffset(Camera2D *camera, Vector2 offset) {
    Vector2 offset_out = (Vector2) {offset.x * cr_config->tile_size, offset.y * cr_config->tile_size};
    camera->offset = offset_out;
}
void CRShiftCameraTarget(Camera2D *camera, Vector2 target) {
    Vector2 target_out = (Vector2) {camera->target.x + target.x * cr_config->tile_size, 
        camera->target.y + target.y * cr_config->tile_size};
    camera->target = target_out;
}
void CRShiftCameraOffset(Camera2D *camera, Vector2 offset) {
    Vector2 offset_out = (Vector2) {camera->offset.x + offset.x * cr_config->tile_size, 
        camera->offset.y + offset.y * cr_config->tile_size};
    camera->offset = offset_out;
}
