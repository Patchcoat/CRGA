/* * ===================================================================================== * *       Filename:  crga.h
 *
 *    Description: All of the functions exposed by the API 
 *
 *        Version:  1.0
 *        Created:  01/15/2023 02:48:08 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Spencer Welch
 *   Organization:  BYU-I
 *
 * =====================================================================================
 */

#ifndef CRGA_HEADER
#define CRGA_HEADER

#include <raylib.h>
#include <stdint.h>
#include <stdlib.h>

#define GRID_OUTLINE 1
#define TRANSPARENT (Color){0,0,0,0}

typedef union {
    // character representation of the tile. 4 bytes to hold unicode values.
    char c[4];
    // integer representation of the tile. Used for tile maps.
    int32_t i;
} CRTileIndex;
typedef struct {
    // The character or tilemap index to use
    CRTileIndex index;
    // how much to shift the foreground by
    Vector2 shift;
    // If text mode, this is the color of the text. If in tilemap mode, this is the tint of the tile
    Color foreground;
    // The solid color rendered behind the text character/tilemap
    Color background;
    // How opaque the tiles above this tile are.
    // 255: Default, totally opaque
    // 0: totally transparent
    uint8_t visibility;
} CRTile;
typedef struct CREntity{
    CRTile tile;
    Vector2 position;
    struct CREntity *next;
    struct CREntity *prev;
} CREntity;
typedef struct {
    CREntity *head;
    CREntity *tail;
} CREntityList;
typedef struct {
    // transparency for a given tile. 
    // 0: Default, objects are made invisible
    // 255: means totally opaque
    uint8_t *tiles;
    // flags for the behavior of the map. 
    // 0 bit: 1 mask up, 0 mask down. 
    // 1 bit: 1 don't mask current layer, 0 mask current layer.
    uint8_t flags; 
    // how many layers the mask should affect beyond the current layer. 
    // 0: mask only the current layer, or no layers if not masking the current layer.
    // -1: mask all layers in the direction defined by the flags
    int count; 
} CRMask;
typedef struct {
    CRMask *mask;
    CRTile *grid;
    CREntityList entities;
    int width;
    int height;
    Vector2 position;
    int font;
} CRLayer;

typedef struct {
    int window_width;
    int window_height;
    int fps;
    char *title;

    Color default_foreground;
    Color default_background;
    uint8_t default_visibility;

    CRLayer *world_layers;
    size_t world_layer_count;
    CRLayer *ui_layers;
    size_t ui_layer_count;

    Camera2D main_camera;

    float tile_size;

    Color background_color;

    Font *fonts;
    size_t font_count;
} CRConfig;

// Init
void CRInit();// malloc
void CRInitConfig(CRConfig *config);
void CRSetConfig(CRConfig *config);
void CRInitWindow();

// Cleanup Functions
void CRClose();
void CRUnloadLayers();
void CRUnloadFonts();

// Loop
void CRLoop();
void CRWorldDraw() __attribute__((weak));
void CRUIDraw() __attribute__((weak));
void CRPreDraw() __attribute__((weak));
void CRPostDraw() __attribute__((weak));

// Font Loading
void CRLoadFont(const char *font_path);
void CRLoadFontSize(const char *font_path, int size);// malloc, realloc

// Layers
CRLayer CRNewLayer(CRTile *grid, int width, int height, Vector2 position);
void CRSetWorldLayer(int index, CRLayer layer);
void CRSetUILayer(int index, CRLayer layer);
void CRAddWorldLayer(int index, CRLayer layer);
void CRAddUILayer(int index, CRLayer layer);
void CRAppendWorldLayer(CRLayer layer);// malloc, realloc
void CRAppendUILayer(CRLayer layer);// malloc, realloc

// Entities
CREntity CRNewEntity(CRTile tile, Vector2 position);
void CRAddEntity(CREntity *entity);
void CRAddEntityToLayer(CRLayer *layer, CREntity *entity);

// Tiles
CRTile CRCTile(char *string);
CRTile CRITile(int index);
void SetGrid(CRLayer *layer, CRTile *grid, int width, int height);
void CRSetTileOnGrid(CRTile *grid, CRTile tile, Vector2 position, int width, int height);
void CRSetTileOnLayer(CRLayer *layer, CRTile tile, Vector2 position);
void CRSetWorldTile(CRTile tile, Vector2 position);
void CRSetUITile(CRTile tile, Vector2 position);
void CRSetWorldGrid(CRTile *grid, int width, int height);
void CRSetUIGrid(CRTile *grid, int width, int height);
int CRSetTilesOnWorldLayer(CRTile *grid, int layer, int width, int height);
int CRSetTilesOnUILayer(CRTile *grid, int layer, int width, int height);
// Draw Tiles
void CRDrawTile(CRTile *tile, float tile_size, Vector2 position);
void CRDrawTileChar(CRTile *tile, float tile_size, Vector2 position);
void CRDrawLayer(CRLayer *layer);

// Camera functions
Camera2D *CRGetMainCamera();
void CRSetCameraTarget(Camera2D *camera, Vector2 target);
void CRSetCameraOffset(Camera2D *camera, Vector2 offset);
void CRShiftCameraTarget(Camera2D *camera, Vector2 target);
void CRShiftCameraOffset(Camera2D *camera, Vector2 offset);

#endif
