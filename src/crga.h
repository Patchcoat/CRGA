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
#define MAXLAYERMASKS 16

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
    Vector2 position;
    int width;
    int height;
    // flags for the behavior of the map. 
    // 0 bit: 1 mask grid, 0 don't mask grid. 
    // 1 bit: 1 mask entities, 0 don't mask entities.
    uint8_t flags;
} CRMask;
typedef struct {
    CRTile *grid;
    CREntityList entities;
    Vector2 position;
    size_t mask_indexes[MAXLAYERMASKS];
    size_t mask_count;
    size_t tile_index;
    int width;
    int height;
    uint8_t flags;// bit 0: 0 char or 1 img | bit 1: if img, use 1 character mapping or 0 index directly
} CRLayer;
typedef struct {
    Texture2D texture;
    int width;
    int height;
    size_t tile_count;
} CRTilemap;
typedef struct CRCharIndexAssoc{
    char character[4];
    int index;
    struct CRCharIndexAssoc *next;
} CRCharIndexAssoc;

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

    CRMask *masks;
    size_t mask_count;

    Camera2D main_camera;

    Color background_color;

    CRCharIndexAssoc char_index_assoc[255];
    CRCharIndexAssoc *assocs;
    size_t assoc_count;

    Font *fonts;
    size_t font_count;

    CRTilemap *tilemaps;
    size_t tilemap_count;
} CRConfig;

// Init
void CRInit();// malloc
void CRInitConfig(CRConfig *config);
void CRSetConfig(CRConfig *config);
void CRInitCharIndexAssoc();
void CRInitWindow();

// Cleanup Functions
void CRClose();
void CRUnloadLayers();
void CRUnloadFonts();
void CRUnloadCharIndexAssoc();
void CRUnloadTilemaps();
void CRUnloadMasks();

// Loop
void CRLoop();
void CRSetWorldDraw(void (*newFunc)());
void CRSetUIDraw(void (*newFunc)());
void CRSetPreDraw(void (*newFunc)());
void CRSetPostDraw(void (*newFunc)());

// Font Loading
void CRLoadFont(const char *font_path);
void CRLoadFontSize(const char *font_path, int size);// malloc, realloc

// Tilemap Loading
void CRLoadTilemap(const char *tilemap_path, int tile_width, int tile_height);// malloc
void CRSetCharAssoc(char *character, int index);// malloc, realloc

// Configuration
void CRTileImage();
void CRTileChar();

// Layers
CRLayer CRInitLayer();
CRLayer CRNewLayer();
void CRInitGrid(CRLayer *layer);// malloc
void CRSetWorldLayer(int index, CRLayer layer);
void CRSetUILayer(int index, CRLayer layer);
void CRNewWorldLayer();
void CRNewUILayer();
void CRAddWorldLayer(int index, CRLayer layer);// malloc, realloc
void CRAddUILayer(int index, CRLayer layer);// malloc, realloc
void CRAppendWorldLayer(CRLayer layer);// malloc, realloc
void CRAppendUILayer(CRLayer layer);// malloc, realloc
void CRInitWorld();
void CRInitUI();
void CRSetLayerFlags(CRLayer *layer, int flags);
void CRSetWorldFlags(int flags);
void CRSetUIFlags(int flags);

// Mask
size_t CRNewMask(int width, int height, uint8_t flags, Vector2 position);// malloc, realloc
void CRAddMaskToLayer(size_t mask_index, CRLayer *layer);
void CRSetWorldMask(Vector2 position, uint8_t mask_value);
void CRSetUIMask(Vector2 position, uint8_t mask_value);
uint8_t CRMaskTile(CRLayer *layer, Vector2 position, uint8_t flags);

// Entities
CREntity CRNewEntity(CRTile tile, Vector2 position);
void CRAddEntity(CREntity *entity);
void CRAddEntityToLayer(CRLayer *layer, CREntity *entity);

// Tiles
CRTile CRDefaultTileConfig(int index);
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
int CRCharToIndex(char *character);
void CRDrawTile(CRTile *tile, uint8_t tilemap_flags, size_t index, float tile_size, 
        Vector2 position, uint8_t mask);
void CRDrawTileChar(CRTile *tile, Font *font, float tile_size, Vector2 position, uint8_t mask);
void CRDrawTileImage(CRTile *tile, CRTilemap *tilemap, int char_index, float tile_size, Vector2 position, uint8_t mask);
void CRDrawLayer(CRLayer *layer);

// Camera functions
Camera2D *CRGetMainCamera();
void CRSetCameraTarget(Camera2D *camera, Vector2 target);
void CRSetCameraOffset(Camera2D *camera, Vector2 offset);
void CRShiftCameraTarget(Camera2D *camera, Vector2 target);
void CRShiftCameraOffset(Camera2D *camera, Vector2 offset);

#endif
