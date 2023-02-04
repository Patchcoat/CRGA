/* * ===================================================================================== * *       Filename:  crga.h
 *
 *    Description: All of the functions exposed by the API 
 *
 *        Version:  1.0
 *        Created:  01/15/2023 02:48:08 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  
 *   Organization:  
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
    uint8_t *grid;
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
    CRMask mask;
    CRTile *grid;
    CREntityList entities;
    Vector2 position;
    size_t index;
    int width;
    int height;
    int map;
} CRLayer;
typedef struct {
    Texture2D texture;
    int width;
    int height;
    size_t tile_count;
} CRTilemap;

typedef struct {
    int window_width;
    int window_height;
    int fps;
    char *title;

    float tile_size;

    int default_layer_width;
    int default_layer_height;

    Color default_foreground;
    Color default_background;
    uint8_t default_visibility;

    CRLayer *world_layers;
    size_t world_layer_count;
    CRLayer *ui_layers;
    size_t ui_layer_count;

    Camera2D main_camera;

    Color background_color;

    uint8_t tilemap_flags; // 0 bit: 1 tilemap, 0 font

    Font *fonts;
    size_t font_count;

    CRTilemap *tilemaps;
    size_t tilemap_count;
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
void CRUnloadTilemaps();

// Loop
void CRLoop();
void CRWorldDraw() __attribute__((weak));
void CRUIDraw() __attribute__((weak));
void CRPreDraw() __attribute__((weak));
void CRPostDraw() __attribute__((weak));

// Font Loading
void CRLoadFont(const char *font_path);
void CRLoadFontSize(const char *font_path, int size);// malloc, realloc

// Tilemap Loading
void CRLoadTilemap(const char *tilemap_path, int tile_width, int tile_height);

// Layers
CRLayer CRInitLayer();
CRLayer CRNewLayer();
void CRInitGrid(CRLayer *layer);// malloc
void CRSetWorldLayer(int index, CRLayer layer);
void CRSetUILayer(int index, CRLayer layer);
void CRAddWorldLayer(int index, CRLayer layer);// malloc, realloc
void CRAddUILayer(int index, CRLayer layer);// malloc, realloc
void CRAppendWorldLayer(CRLayer layer);// malloc, realloc
void CRAppendUILayer(CRLayer layer);// malloc, realloc
void CRInitWorld();
void CRInitUI();

// Mask
void CRSetGridMask(uint8_t *grid, uint8_t tile, Vector2 position, int width, int height);
void CRSetLayerMask(CRLayer *layer, uint8_t tile, Vector2 position);
void CRSetWorldLayerMask(int index, uint8_t tile, Vector2 position);
void CRSetUILayerMask(int index, uint8_t tile, Vector2 position);
void CRSetWorldMask(uint8_t tile, Vector2 position);
void CRSetUIMask(uint8_t tile, Vector2 position);

// Entities
CREntity CRNewEntity(CRTile tile, Vector2 position);
void CRAddEntity(CREntity *entity);
void CRAddEntityToLayer(CRLayer *layer, CREntity *entity);

// Tiles
CRTile CRCTile(char *string);
CRTile CRITile(int index);
CRTile CRNewTileIndex(int index);
void CRSetGridTile(CRTile *grid, CRTile tile, Vector2 position, int width, int height);
void CRSetLayerTile(CRLayer *layer, CRTile tile, Vector2 position);
void CRSetLayerTileChar(CRLayer *layer, char *string, Vector2 position);
void CRSetLayerTileIndex(CRLayer *layer, int index, Vector2 position);
void CRSetWorldTile(CRTile tile, Vector2 position);
void CRSetUITile(CRTile tile, Vector2 position);
void CRSetWorldTileChar(char *character, Vector2 position);
void CRSetUITileChar(char *character, Vector2 position);
void CRSetWorldTileIndex(int index, Vector2 position);
void CRSetUITileIndex(int index, Vector2 position);
void CRSetWorldLayerTile(int index, CRTile tile, Vector2 position);
void CRSetUILayerTile(int index, CRTile tile, Vector2 position);
// Draw Tiles
void CRDrawTile(CRTile *tile, float tile_size, Vector2 position, uint8_t mask);
void CRDrawTileChar(CRTile *tile, Font *font, float tile_size, Vector2 position, uint8_t mask);
void CRDrawTileImage(CRTile *tile, CRTilemap *tilemap, float tile_size, Vector2 position, uint8_t mask);
void CRDrawLayer(CRLayer *layer);

// Camera functions
Camera2D *CRGetMainCamera();
void CRSetCameraTarget(Camera2D *camera, Vector2 target);
void CRSetCameraOffset(Camera2D *camera, Vector2 offset);
void CRShiftCameraTarget(Camera2D *camera, Vector2 target);
void CRShiftCameraOffset(Camera2D *camera, Vector2 offset);

#endif
