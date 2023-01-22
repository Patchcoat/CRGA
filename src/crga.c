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

CRConfig *cr_config;

// Init
void CRInit() {
    CRConfig *config = (CRConfig *) malloc(sizeof(CRConfig));
    CRInitConfig(config);
    CRInitWindow();
}
void CRInitConfig(CRConfig *config) {
    config->window_width = 800;
    config->window_height = 450;
    config->fps = 60;
    config->title = "CRGA Basic Window";

    config->world_layers = 0;
    config->world_layer_count = 0;
    config->ui_layers = 0;
    config->ui_layer_count = 0;

    config->main_camera.target = (Vector2){0.0f, 0.0f};
    config->main_camera.offset = (Vector2){0.0f, 0.0f};
    config->main_camera.rotation = 0.0f;
    config->main_camera.zoom = 1.0f;

    config->tile_size = 20.0f;

    config->fonts = 0;
    config->font_count = 0;

    config->pre_draw_function = 0; // if this isn't 0 then it's called
    CRSetConfig(config);
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

// Drawing Loop
void CRSetPredrawFunction(void (*pre_draw)()) {
    cr_config->pre_draw_function = pre_draw;
}
void CRLoop() {
    while (!WindowShouldClose()) {

        if (cr_config->pre_draw_function)
            cr_config->pre_draw_function();

        BeginDrawing();

            ClearBackground(BLACK);

            BeginMode2D(cr_config->main_camera);

                CRDraw();

            EndMode2D();

        EndDrawing();
    }
}

// Font Loading
inline void CRLoadFont(const char *font_path) {
    CRLoadFontSize(font_path, 96);
}
void CRLoadFontSize(const char *font_path, int size) { // hidden malloc
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
void CRNewWorldLayer(short *grid, int width, int height, Vector2 position) {
    if (cr_config->world_layer_count == 0) {
        cr_config->world_layers = (CRLayer *) malloc(sizeof(CRLayer));
    } else {
        cr_config->world_layers = (CRLayer *) realloc(cr_config->world_layers, 
                sizeof(CRLayer) * (cr_config->world_layer_count + 1));
    }
    size_t index = cr_config->world_layer_count;
    cr_config->world_layer_count++;

    cr_config->world_layers[index].tiles = grid;
    cr_config->world_layers[index].width = width;
    cr_config->world_layers[index].height = height;
    cr_config->world_layers[index].position = position;
}
void CRNewUILayer(short *grid, int width, int height, Vector2 position) {
    if (cr_config->ui_layer_count == 0) {
        cr_config->ui_layers = (CRLayer *) malloc(sizeof(CRLayer));
    } else {
        cr_config->ui_layers = (CRLayer *) realloc(cr_config->ui_layers, 
                sizeof(CRLayer) * (cr_config->ui_layer_count + 1));
    }
    size_t index = cr_config->ui_layer_count;
    cr_config->ui_layer_count++;

    cr_config->ui_layers[index].tiles = grid;
    cr_config->ui_layers[index].width = width;
    cr_config->ui_layers[index].height = height;
    cr_config->ui_layers[index].position = position;
}

// Tile Rendering
char IndexToChar(short index) {
    char charList[] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0',' ','!','"','#','$','%','&','\'','(',')','*','+',',','-','.','/','0','1','2','3','4','5','6','7','8','9',':',';','<','=','>','?','@','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','[','\\',']','^','_','`','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','{','|','}','~','\0'};
    return charList[index];
}
void SetTiles(CRLayer *layer, short *tiles, int width, int height) {
    layer->tiles = tiles;
    layer->width = width;
    layer->height = height;
}
void CRSetWorldTiles(short *tiles, int width, int height) {
    if (cr_config->world_layer_count == 0) {
        Vector2 position = {0.0f, 0.0f};
        CRNewWorldLayer(tiles, width, height, position);
    }
    CRSetTilesOnWorldLayer(tiles, 0, width, height);
}
void CRSetUITiles(short *tiles, int width, int height) {
    if (cr_config->ui_layer_count == 0) {
        Vector2 position = {0.0f, 0.0f};
        CRNewUILayer(tiles, width, height, position);
    }
    CRSetTilesOnUILayer(tiles, 0, width, height);
}
int CRSetTilesOnWorldLayer(short *tiles, int layer, int width, int height) {
    if (layer >= cr_config->world_layer_count)
        return 1;
    SetTiles(cr_config->world_layers + layer, tiles, width, height);
    return 0;
}
int CRSetTilesOnUILayer(short *tiles, int layer, int width, int height) {
    if (layer >= cr_config->ui_layer_count)
        return 1;
    SetTiles(cr_config->ui_layers + layer, tiles, width, height);
    return 0;
}
void CRDrawLayer(CRLayer *layer) {
    float tile_size = cr_config->tile_size;
    for (int row = 0; row < layer->height; row++) {
        for (int col = 0; col < layer->width; col++) {
            short index = layer->tiles[col * layer->width + row];
            if (index == 0)
                continue;
            float x_pos = tile_size * row;
            float y_pos = tile_size * col;
            DrawRectangle(x_pos, y_pos, tile_size, tile_size, BLACK);
#if GRID_OUTLINE
            DrawRectangleLines(x_pos, y_pos, tile_size, tile_size, RED);
#endif
            char character[2] = {IndexToChar(index),'\0'};
            Vector2 position = {x_pos, y_pos};
            if (cr_config->font_count > 0) {
                position.x += tile_size/2.0f - MeasureTextEx(cr_config->fonts[0], character, 24, 0).x/2.0f;
                DrawTextEx(cr_config->fonts[0], character, position, 24, 0, WHITE);
            } else {
                position.x += tile_size/2.0f - MeasureText(character, 24)/2.0f;
                DrawText(character, position.x, position.y, 24, WHITE);
            }
        }
    }
}
void CRDraw() {
    for (int i = 0; i < cr_config->world_layer_count; i++) {
        CRDrawLayer(&cr_config->world_layers[i]);
    }
    for (int i = 0; i < cr_config->ui_layer_count; i++) {
        CRDrawLayer(&cr_config->ui_layers[i]);
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
