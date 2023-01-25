/*
 * =====================================================================================
 *
 *       Filename:  crga.h
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
#include <stdlib.h>

#define GRID_OUTLINE 1

typedef struct {
    short *tiles;
    int width;
    int height;
    Vector2 position;
    int font;
} CRLayer;
typedef struct {
    char *tiles;
    int width;
    int height;
    Vector2 position;
    int layer;// what layer the mask is on. -1, topmost layer
    int layer_count;// how many layers the mash should affect. -1, mask all layers
    char mask_up;// bool for whether to mask higher layers, default false
} CRMask;

typedef struct {
    int window_width;
    int window_height;
    int fps;
    char *title;

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
void CRInit();
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
void CRLoadFontSize(const char *font_path, int size);

// Layers
void CRNewWorldLayer(short *grid, int width, int height, Vector2 position);
void CRNewUILayer(short *grid, int width, int height, Vector2 position);

// Tile Rendering
void CRSetWorldTiles(short *grid, int width, int height);
void CRSetUITiles(short *grid, int width, int height);
int CRSetTilesOnWorldLayer(short *grid, int layer, int width, int height);
int CRSetTilesOnUILayer(short *grid, int layer, int width, int height);
void SetTiles(CRLayer *layer, short *tiles, int width, int height);
void CRDrawLayer(CRLayer *layer);
void CRDraw();

// Camera functions
Camera2D *CRGetMainCamera();
void CRSetCameraTarget(Camera2D *camera, Vector2 target);
void CRSetCameraOffset(Camera2D *camera, Vector2 offset);
void CRShiftCameraTarget(Camera2D *camera, Vector2 target);
void CRShiftCameraOffset(Camera2D *camera, Vector2 offset);

#endif
