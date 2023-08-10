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
 *         Author:  
 *   Organization:  
 *
 * =====================================================================================
 */
#include "crga.h"
#include "crgahelper.h"
#include <stdio.h>
#include <string.h>

CRConfig *cr_config;
void (*CRWorldDraw)();
void (*CRUIDraw)();
void (*CRPreDraw)();
void (*CRPostDraw)();

// Init
void CRInit() {
    CRConfig *config = (CRConfig *) malloc(sizeof(CRConfig));
    CRInitConfig(config);
    CRSetConfig(config);
    CRInitCharIndexAssoc();
    CRInitWorld();
    CRInitUI();
    CRInitWindow();
}
void CRInitConfig(CRConfig *config) {
    config->window_width = 800;
    config->window_height = 800;
    config->fps = 60;
    config->title = "CRGA Basic Window";

    config->tile_size = 20.0f;

    config->default_layer_width = config->window_width / config->tile_size;
    config->default_layer_height = config->window_height / config->tile_size;

    config->default_foreground = WHITE;
    config->default_background = BLACK;
    config->default_visibility = 0;

    config->world_layers = 0;
    config->world_layer_count = 0;
    config->ui_layers = 0;
    config->ui_layer_count = 0;

    config->shaders = 0;
    config->shader_count = 0;

    config->main_camera.target = (Vector2){0.0f, 0.0f};
    config->main_camera.offset = (Vector2){0.0f, 0.0f};
    config->main_camera.rotation = 0.0f;
    config->main_camera.zoom = 1.0f;

    config->background_color = BLACK;

    config->fonts = 0;
    config->font_count = 0;

    config->tilemaps = 0;
    config->tilemap_count = 0;
}
inline void CRSetConfig(CRConfig *config) {
    cr_config = config;
}
inline void CRInitCharIndexAssoc() {
    for (int i = 0; i < 1024; i++) {
        int index = i;
        // set the character string to the ASCII value followed by three null terminators
        cr_config->char_index_assoc[i].character[0] = (uint32_t) 0;
        cr_config->char_index_assoc[i].character[0] = (char) i;
        // Shift all ASCII control codes to the end
        index = (i > 0 && i < 32) ? i + 96 : index;
        // shift all (non extended ascii) characters to the front
        index = (i >= 32 && i <= 127) ? i - 31 : index;
        // extended ascii character number matches it's index
        cr_config->char_index_assoc[i].index = index;
    }
}
inline void CRInitWindow() {
    InitWindow(cr_config->window_width, cr_config->window_height, cr_config->title);
    SetTargetFPS(cr_config->fps);
}
inline void CRInitWindowFlags(unsigned int flags) {
    InitWindow(cr_config->window_width, cr_config->window_height, cr_config->title);
    SetWindowState(flags);
    SetTargetFPS(cr_config->fps);
}
inline void CRInitWindowFullscreen() { 
    CRInitWindow();
    CRToggleFullscreen();
}
inline void CRInitWindowed() {
    unsigned int flags = FLAG_WINDOW_RESIZABLE;
    CRInitWindowFlags(flags);
}
inline void CRInitBorderlessWindowed() {
    unsigned int flags = FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_TOPMOST;
    CRInitWindowFlags(flags);
}

// Cleanup Functions
void CRClose() {
    CRUnloadFonts();
    CRUnloadTilemaps();
    CRUnloadLayers();
    CRUnloadMasks();
    CRUnloadShaders();
    CloseWindow();
}
void CRUnloadLayers() {
    size_t count = cr_config->world_layer_count;
    if (count > 0) {
        for (int i = 0; i < count; i++) {
            free(cr_config->world_layers[i].grid);
        }
        free(cr_config->world_layers);
    }
    count = cr_config->ui_layer_count;
    if (count > 0) {
        for (int i = 0; i < count; i++) {
            free(cr_config->ui_layers[i].grid);
        }
        free(cr_config->ui_layers);
    }
}
inline void CRUnloadFonts() {
    for (int i = cr_config->font_count-1; i >= 0; i--) {
        UnloadFont(cr_config->fonts[i]);
    }
    free(cr_config->fonts);
}
inline void CRUnloadTilemaps() {
    free(cr_config->tilemaps);
}
void CRUnloadMasks() {
    if (cr_config->mask_count == 0)
        return;
    for (int i = 0; i < cr_config->mask_count; i++)
        free(cr_config->masks[i].grid);
    free(cr_config->masks);
}
void CRUnloadShaders() {
    if (cr_config->shader_count == 0)
        return;
    for (int i = 0; i < cr_config->shader_count; i++) {
        UnloadShader(cr_config->shaders[i]);
    }
    free(cr_config->shaders);
}

// Window
void CRToggleFullscreen() {
    if (IsWindowFullscreen()) {
        int monitor = GetCurrentMonitor();
        SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
    } else {
        SetWindowSize(cr_config->window_width, cr_config->window_height);
    }
    ToggleFullscreen();
}
void CRMaximize() {
    MaximizeWindow();
}
void CRMinimize() {
    MinimizeWindow();
}
void CRSetWindowed() {
    unsigned int flags = FLAG_WINDOW_RESIZABLE;
    SetWindowState(flags);
}
void CRSetBorderlessWindowed() {
    unsigned int flags = FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_TOPMOST;
    SetWindowState(flags);
}

// Loop
void CRLoop() {
    while (!WindowShouldClose())
        {

        if (CRPreDraw != 0)
            (*CRPreDraw)();

        BeginDrawing();

            ClearBackground(cr_config->background_color);

            BeginMode2D(cr_config->main_camera);

                for (int i = 0; i < cr_config->world_layer_count; i++) {
                    CRDrawLayer(&cr_config->world_layers[i]);
                }
                if (CRWorldDraw != 0)
                    (*CRWorldDraw)();
            EndMode2D();

            for (int i = 0; i < cr_config->ui_layer_count; i++) {
                CRDrawLayer(&cr_config->ui_layers[i]);
            }
            if (CRUIDraw != 0)
                (*CRUIDraw)();
        EndDrawing();

        if (CRPostDraw != 0)
            (*CRPostDraw)();
    }
}
void CRSetWorldDraw(void (*new_func)()) {
    CRWorldDraw = new_func;
}
void CRSetUIDraw(void (*new_func)()) {
    CRUIDraw = new_func;
}
void CRSetPreDraw(void (*new_func)()) {
    CRPreDraw = new_func;
}
void CRSetPostDraw(void (*new_func)()) {
    CRPostDraw = new_func;
}

// Font Loading
inline void CRLoadFont(const char *font_path) {
    CRLoadFontSize(font_path, 96);
}
void CRLoadFontSize(const char *font_path, int size) {
    if (cr_config->font_count == 255) {
        printf("ERROR: Could not load font. Reached limit of 255.\n");
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

// Tilemap Loading
void CRLoadTilemap(const char *tilemap_path, int tile_width, int tile_height) {
    if (cr_config->tilemap_count == 255) {
        printf("ERROR: Could not load tilemap. Reached limit of 255.\n");
        return;
    } else if (cr_config->tilemap_count == 0) {
        cr_config->tilemaps = (CRTilemap *) malloc(sizeof(CRTilemap));
    } else {
        cr_config->tilemaps = (CRTilemap *) realloc(cr_config->tilemaps, sizeof(CRTilemap) * (cr_config->tilemap_count + 1));
    }
    size_t index = cr_config->tilemap_count;
    cr_config->tilemap_count++;

    cr_config->tilemaps[index].texture = LoadTexture(tilemap_path);
    Texture2D tilemap_texture = cr_config->tilemaps[index].texture;
    
    int count_h = tilemap_texture.width / tile_width;
    int count_v = tilemap_texture.height / tile_height;
    int count = count_h * count_v;
    cr_config->tilemaps[index].width = tile_width;
    cr_config->tilemaps[index].height = tile_height;
    cr_config->tilemaps[index].tile_count = count;
}
void CRSetCharAssoc(char* character, int index) {
    CRCharIndexAssoc new_assoc;
    // Convert string to index
    int assoc_index = UTF8CharToInt(character);
    memcpy(&new_assoc.character, character, 4);
    new_assoc.index = index;
    // insert new_assoc
    cr_config->char_index_assoc[assoc_index] = new_assoc;
}

// Layers
CRLayer CRNewLayer() {
    CRLayer layer;
    layer.grid = 0;
    layer.entities.head = 0;
    layer.entities.tail = 0;
    layer.tile_index = 0;
    layer.width = cr_config->default_layer_width;
    layer.height = cr_config->default_layer_height;
    layer.position = (Vector2) {0, 0};
    layer.flags = 0;
    layer.mask_count = 0;
    return layer;
}
void CRInitGrid(CRLayer *layer) {
    int size = layer->width * layer->height;
    if (layer->grid != 0)
        free(layer->grid);
    layer->grid = malloc(sizeof(CRTile) * size);
    CRTile zero = {0};
    zero.index.i = 0;
    for (int i = 0; i < size; i++)
        layer->grid[i] = zero;
}
CRLayer CRInitLayer() {
    CRLayer layer = CRNewLayer();
    CRInitGrid(&layer);
    return layer;
}
void CRSetWorldLayer(int index, CRLayer layer) {
    if (cr_config->world_layer_count < index) {
        printf("ERROR: Index %d for world layer out of bounds\n", index);
        return;
    }
    cr_config->world_layers[index] = layer;
}
void CRSetUILayer(int index, CRLayer layer) {
    if (cr_config->ui_layer_count < index) {
        printf("ERROR: Index %d for UI layer out of bounds\n", index);
        return;
    }
    cr_config->ui_layers[index] = layer;
}
void CRNewWorldLayer() {
    if (cr_config->world_layer_count == 0) {
        cr_config->world_layers = (CRLayer *) malloc(sizeof(CRLayer));
    } else {
        cr_config->world_layers = (CRLayer *) realloc(cr_config->world_layers, 
                sizeof(CRLayer) * (cr_config->world_layer_count + 1));
    }
}
void CRNewUILayer() {
    if (cr_config->ui_layer_count == 0) {
        cr_config->ui_layers = (CRLayer *) malloc(sizeof(CRLayer));
    } else {
        cr_config->ui_layers = (CRLayer *) realloc(cr_config->ui_layers, 
                sizeof(CRLayer) * (cr_config->ui_layer_count + 1));
    }
}
void CRAddWorldLayer(int index, CRLayer layer) {
    if (index > cr_config->world_layer_count) {
        printf("ERROR: Index %d for world layer out of bounds\n", index);
        return;
    }
    if (index == cr_config->world_layer_count) {
        CRAppendWorldLayer(layer);
        return;
    }
    CRNewWorldLayer();
    for (int i = cr_config->world_layer_count - 1; i >= index; i--) {
        cr_config->world_layers[i+1] = cr_config->world_layers[i];
    }
    cr_config->world_layer_count++;
    cr_config->world_layers[index] = layer;
}
void CRAddUILayer(int index, CRLayer layer) {
    if (index > cr_config->ui_layer_count) {
        printf("ERROR: Index %d for world layer out of bounds\n", index);
        return;
    }
    if (index == cr_config->ui_layer_count) {
        CRAppendUILayer(layer);
        return;
    }
    CRNewUILayer();
    for (int i = cr_config->ui_layer_count - 1; i >= index; i--) {
        cr_config->ui_layers[i+1] = cr_config->ui_layers[i];
    }
    cr_config->ui_layer_count++;
    cr_config->ui_layers[index] = layer;
}
void CRAppendWorldLayer(CRLayer layer) {
    CRNewWorldLayer();
    cr_config->world_layers[cr_config->world_layer_count] = layer;
    cr_config->world_layer_count++;
}
void CRAppendUILayer(CRLayer layer) {
    CRNewUILayer();
    cr_config->ui_layers[cr_config->ui_layer_count] = layer;
    cr_config->ui_layer_count++;
}
void CRInitWorld() {
    if (cr_config->world_layer_count != 0)
        return;
    CRLayer layer = CRNewLayer();
    CRInitGrid(&layer);
    CRAppendWorldLayer(layer);
}
void CRInitUI() {
    if (cr_config->ui_layer_count != 0)
        return;
    CRLayer layer = CRNewLayer();
    CRInitGrid(&layer);
    CRAppendUILayer(layer);
}
void CRSetLayerFlags(CRLayer *layer, unsigned int flags) {
    layer->flags = flags;
}
void CRSetWorldFlags(unsigned int flags) {
    cr_config->world_layers[0].flags = flags;
}
void CRSetUIFlags(unsigned int flags) {
    cr_config->ui_layers[0].flags = flags;
}

// Mask
size_t CRNewMask(int width, int height, uint8_t flags, Vector2 position) {
    size_t index = cr_config->mask_count;
    if (index == 0)
        cr_config->masks = malloc(sizeof(CRMask));
    else
        cr_config->masks = realloc(cr_config->masks, sizeof(CRMask) * (index + 1));
    CRMask *mask = &cr_config->masks[index];
    size_t mask_size = width * height;
    mask->grid = malloc(sizeof(uint8_t) * mask_size);
    for (size_t i = 0; i < mask_size; i++)
        mask->grid[i] = 255;
    mask->width = width;
    mask->height = height;
    mask->flags = flags;
    mask->position = position;
    cr_config->mask_count++;
    return index;
}
void CRAddMaskToLayer(size_t mask_index, CRLayer *layer) {
    if (layer->mask_count == MAXLAYERMASKS) {
        printf("ERROR: Too many mask layers\n");
        return;
    }
    layer->mask_indexes[layer->mask_count] = mask_index;
    layer->mask_count++;
}
void CRSetWorldMask(Vector2 position, uint8_t mask_value) {
    if (cr_config->world_layer_count == 0) {
        printf("ERROR: Cannot mask when there are no world layers\n");
        return;
    }
    CRLayer *layer = &cr_config->world_layers[0];
    if (layer->mask_count == 0) {
        size_t new_mask = CRNewMask(layer->width, layer->height, 0b11, layer->position);
        CRAddMaskToLayer(new_mask, layer);
    }
    CRMask *mask = &cr_config->masks[layer->mask_indexes[0]];
    size_t mask_position = position.x + position.y * mask->width;
    mask->grid[mask_position] = mask_value;
}
void CRSetUIMask(Vector2 position, uint8_t mask_value) {
    if (cr_config->ui_layer_count == 0) {
        printf("ERROR: Cannot mask when there are no world layers\n");
        return;
    }
    CRLayer *layer = &cr_config->ui_layers[0];
    if (layer->mask_count == 0) {
        size_t new_mask = CRNewMask(layer->width, layer->height, 0b11, layer->position);
        CRAddMaskToLayer(new_mask, layer);
    }
    CRMask *mask = &cr_config->masks[layer->mask_indexes[0]];
    size_t mask_position = position.x + position.y * mask->width;
    mask->grid[mask_position] = mask_value;
}
uint8_t CRMaskTile(CRLayer *layer, Vector2 position, uint8_t flags) {
    // Position is the position on the layer
    // bit 0 of flags indicates of it's a grid, bit 1 indicates if it's an entity
    if (position.x < 0 || position.x > layer->width || position.y < 0 || position.y > layer->height) {
        return 255;
    }
    CRTile *tile = &layer->grid[(int) (position.x + position.y * layer->width)];
    if (tile->index.i == 0)
        return 255;
    uint8_t mask_value = 255;
    for (int i = 0; i < layer->mask_count; i++) {
        CRMask mask = cr_config->masks[i];
        // see if point is on the layer and mask, otherwise move to the next layer
        if (!(OnLayer(layer, position) && OnMask(&mask, position)))
            continue;
        // verify behavior from flags
        if (!CheckMaskFlags(flags, mask.flags))
            continue;
        // grab tile value on layer
        Vector2 shifted_position = position;
        shifted_position.x += mask.position.x;
        shifted_position.y += mask.position.y;
        size_t position_index = shifted_position.x + (shifted_position.y * mask.width);
        int value = mask_value;
        value -= 255 - mask.grid[position_index];
        if (value <= 0)
            return 0;
        mask_value = value;
    }
    return mask_value;
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
    if (cr_config->world_layer_count == 0) {
        printf("ERROR: Cannot add entity when world layer doesn't exist\n");
        return;
    }
    CRAddEntityToLayer(0, entity);
}
void CRAddEntityToLayer(CRLayer *layer, CREntity *entity) {
    if (layer == 0) {
        if (cr_config->world_layer_count == 0) {
            printf("ERROR: No world layer to add entity to\n");
            return;
        }
        layer = &cr_config->world_layers[0];
    }
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
CRTile CRDefaultTileConfig(int index) {
    CRTile tile;
    tile.index.i = index;
    tile.shift = (Vector2) {0, 0};
    tile.foreground = cr_config->default_foreground;
    tile.background = cr_config->default_background;
    tile.visibility = cr_config->default_visibility;
    return tile;
}
CRTile CRCTile(char *string) {
    CRTile tile = CRDefaultTileConfig(0);
    for (int i = 0; i <= 4; i++) {
        if (string[i] == 0)
            break;
        tile.index.c[i] = string[i];
    }
    return tile;
}
CRTile CRITile(int index) {
    CRTile tile = CRDefaultTileConfig(index);
    return tile;
}
void CRSetGridTile(CRTile *grid, CRTile tile, Vector2 position, int width, int height) {
    int x = position.x;
    int y = position.y;
    if (x > width || y > height) {
        printf("Error: Tile position (%d, %d) out of bounds\n", x, y);
        return;
    }
    grid[x + y * width] = tile;
}
void CRSetLayerTile(CRLayer *layer, CRTile tile, Vector2 position) {
    int width = layer->width;
    int height = layer->height;
    CRSetGridTile(layer->grid, tile, position, width, height);
}
void CRSetLayerTileChar(CRLayer *layer, char *string, Vector2 position) {
    CRTile tile = CRCTile(string);
    CRSetLayerTile(layer, tile, position);
}
void CRSetLayerTileIndex(CRLayer *layer, int index, Vector2 position) {
    CRTile tile = CRITile(index);
    CRSetLayerTile(layer, tile, position);
}
void CRSetWorldTile(CRTile tile, Vector2 position) {
    CRSetLayerTile(&cr_config->world_layers[0], tile, position);
}
void CRSetUITile(CRTile tile, Vector2 position) {
    CRSetLayerTile(&cr_config->ui_layers[0], tile, position);
}
void CRSetWorldTileIndex(int index, Vector2 position) {
    CRTile tile = CRITile(index);
    CRSetLayerTile(&cr_config->world_layers[0], tile, position);
}
void CRSetUITileIndex(int index, Vector2 position) {
    CRTile tile = CRITile(index);
    CRSetLayerTile(&cr_config->ui_layers[0], tile, position);
}
void CRSetWorldTileChar(char *character, Vector2 position) {
    CRTile tile = CRCTile(character);
    CRSetLayerTile(&cr_config->world_layers[0], tile, position);
}
void CRSetUITileChar(char *character, Vector2 position) {
    CRTile tile = CRCTile(character);
    CRSetLayerTile(&cr_config->ui_layers[0], tile, position);
}
void CRSetWorldLayerTile(int index, CRTile tile, Vector2 position) {
    if (cr_config->world_layer_count < index) {
        printf("ERROR: Index %d for world layer out of bounds\n", index);
        return;
    }
    CRSetLayerTile(cr_config->world_layers + index, tile, position);
}
void CRSetUILayerTile(int index, CRTile tile, Vector2 position) {
    if (cr_config->ui_layer_count < index) {
        printf("ERROR: Index %d for UI layer out of bounds\n", index);
        return;
    }
    CRSetLayerTile(cr_config->world_layers + index, tile, position);
}

// Draw Tiles
int CRCharToIndex(char *character) {
    int assoc_index = UTF8CharToInt(character);
    return cr_config->char_index_assoc[assoc_index].index;
}
int PreDrawTile(CRTileIndex index, uint8_t mask, Color *foreground_color, Color *background_color, char string_out[5]) {
    if (index.i == 0)
        return 1;

    float mask_multiplier = (float) mask/255.0f;

    float transparency = background_color->a;
    transparency *= mask_multiplier;
    background_color->a = transparency;

    transparency = foreground_color->a;
    transparency *= mask_multiplier;
    foreground_color->a = transparency;

    if (foreground_color->a == 0 && foreground_color->a == 0)
        return 1;

    for (int i = 0; i <= 4; i++)
        string_out[i] = index.c[i];

    return 0;
}
void CRDrawTile(CRTile *tile, uint8_t tilemap_flags, size_t index, float tile_size, Vector2 position, uint8_t mask) {
    if ((tilemap_flags & 0b1) == 0) {
        if (cr_config->font_count > index){
            CRDrawTileChar(tile, &cr_config->fonts[index], tile_size, position, mask);
        } else {
            CRDrawTileChar(tile, 0, tile_size, position, mask);
        }
    } else {
        int char_index = ((tilemap_flags & 0b10) >> 1);
        if (cr_config->tilemap_count > index) {
            CRDrawTileImage(tile, &cr_config->tilemaps[index], char_index, tile_size, position, mask);
        } else {
            CRDrawTileImage(tile, 0, char_index, tile_size, position, mask);
        }
    }
}
void CRDrawTileChar(CRTile *tile, Font *font, float tile_size, Vector2 position, uint8_t mask) {
    Color tile_color = tile->background;
    Color text_color = tile->foreground;
    char string_out[5];
    if (PreDrawTile(tile->index, mask, &text_color, &tile_color, string_out))
        return;

    DrawRectangle(position.x, position.y, tile_size, tile_size, tile_color);
#if GRID_OUTLINE
    DrawRectangleLines(position.x, position.y, tile_size, tile_size, RED);
#endif
    Vector2 shift = tile->shift;
    if (font != 0) {
        position = CenterTextEx(position, *font, tile_size, string_out);
        position = ShiftPosition(position, shift);
        DrawTextEx(*font, string_out, position, 24, 0, text_color);
    } else {
        position = CenterText(position, tile_size, string_out);
        position = ShiftPosition(position, shift);
        DrawText(string_out, position.x, position.y, 24, text_color);
    }
}
void CRDrawTileImage(CRTile *tile, CRTilemap *tilemap, int char_index, float tile_size, Vector2 position, uint8_t mask) {
    Color tile_color = tile->background;
    Color foreground_color = tile->foreground;
    char string_out[5];
    if (PreDrawTile(tile->index, mask, &foreground_color, &tile_color, string_out))
        return;

    DrawRectangle(position.x, position.y, tile_size, tile_size, tile->background);
#if GRID_OUTLINE
    DrawRectangleLines(position.x, position.y, tile_size, tile_size, RED);
#endif
    Vector2 shift = tile->shift;
    int index = tile->index.i;
    if (char_index) {
        index = CRCharToIndex(tile->index.c);
    }
    if (tilemap != 0) {
        position = ShiftPosition(position, shift);
        Rectangle rect = TileIndexRec(tilemap, index);
        Rectangle dest;
        dest.x = position.x;
        dest.y = position.y;
        dest.width = tile_size;
        dest.height = tile_size;
        Vector2 origin = {0.0,0.0};
        DrawTexturePro(tilemap->texture, rect, dest, origin, 0.0f, foreground_color);
    } else {
        // TODO handle the problem of no tilemap
    }
}
void CRDrawLayer(CRLayer *layer) {
    float tile_size = cr_config->tile_size;
    for (int row = 0; row < layer->height; row++) {
        for (int col = 0; col < layer->width; col++) {
            CRTile *tile = &layer->grid[col + row * layer->width];
            uint8_t mask = CRMaskTile(layer, (Vector2){col, row}, 0b01);
            CRDrawTile(tile, layer->flags, layer->tile_index, tile_size,
                    (Vector2) {tile_size * col, tile_size * row}, mask);
        }
    }
    if (layer->entities.head == 0)
        return;
    CREntity *itr = layer->entities.head;
    while (itr != 0) {
        CRTile *tile = &itr->tile;
        Vector2 position = itr->position;
        uint8_t mask = CRMaskTile(layer, position, 0b10);
        position.x *= tile_size;
        position.y *= tile_size;
        CRDrawTile(tile, layer->flags, layer->tile_index, tile_size, position, mask);
        itr = itr->next;
    }
}

// Shaders
size_t CRNewShader(const char *vsCode, const char *fsCode) {
    int shader_count = cr_config->shader_count;
    if (shader_count == 0) {
        cr_config->shaders = malloc(sizeof(Shader));
    } else {
        cr_config->shaders = realloc(cr_config->shaders, sizeof(Shader) * (shader_count + 1));
    }
    cr_config->shaders[shader_count] = LoadShader(vsCode, fsCode);
    cr_config->shader_count++;
    return 0;
}
size_t CRNewVertexShader(const char *vsCode) {
    CRNewShader(vsCode, 0);
    return 0;
}
size_t CRNewFragmentShader(const char *fsCode) {
    CRNewShader(0, fsCode);
    return 0;
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

// Shape functions
void CRDrawCharRectangle(CRLayer *layer, Vector2 top_left, Vector2 bottom_right, char *tl, char *t, char *tr, char *r, char *br, char *b, char *bl, char *l, char *fill) {
    int i, j;
    int width = bottom_right.x - top_left.x + 1;
    int height = bottom_right.y - top_left.y + 1;
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            if (i == 0 && j == 0) {// top left
                CRSetLayerTileChar(layer, tl, top_left);
            } else if (i == 0 && j == width-1) {// top right
                Vector2 position = bottom_right;
                position.y = top_left.y;
                CRSetLayerTileChar(layer, tr, position);
            } else if (i == height-1 && j == 0) {// bottom left
                Vector2 position = bottom_right;
                position.x = top_left.x;
                CRSetLayerTileChar(layer, bl, position);
            } else if (i == height-1 && j == width-1) {// bottom right
                Vector2 position = bottom_right;
                CRSetLayerTileChar(layer, br, position);
            } else if (i == 0 || i == height-1 || j == 0 || j == width-1) {
                Vector2 position = top_left;
                position.y += i;
                position.x += j;
                if (i == 0)
                    CRSetLayerTileChar(layer, t, position);
                else if (i == height-1)
                    CRSetLayerTileChar(layer, b, position);
                else if (j == 0)
                    CRSetLayerTileChar(layer, l, position);
                else
                    CRSetLayerTileChar(layer, r, position);
            }
        }
    }
}
void CRDrawTileRectangle(CRLayer *layer, Vector2 top_left, Vector2 bottom_right, CRTile tl, CRTile t, CRTile tr, CRTile r, CRTile br, CRTile b, CRTile bl, CRTile l, CRTile fill) {
    int i, j;
    int width = bottom_right.x - top_left.x + 1;
    int height = bottom_right.y - top_left.y + 1;
    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            if (i == 0 && j == 0) {// top left
                CRSetLayerTile(layer, tl, top_left);
            } else if (i == 0 && j == width-1) {// top right
                Vector2 position = bottom_right;
                position.y = top_left.y;
                CRSetLayerTile(layer, tr, position);
            } else if (i == height-1 && j == 0) {// bottom left
                Vector2 position = bottom_right;
                position.x = top_left.x;
                CRSetLayerTile(layer, bl, position);
            } else if (i == height-1 && j == width-1) {// bottom right
                Vector2 position = bottom_right;
                CRSetLayerTile(layer, br, position);
            } else if (i == 0 || i == height-1 || j == 0 || j == width-1) {
                Vector2 position = top_left;
                position.y += i;
                position.x += j;
                if (i == 0)
                    CRSetLayerTile(layer, t, position);
                else if (i == height-1)
                    CRSetLayerTile(layer, b, position);
                else if (j == 0)
                    CRSetLayerTile(layer, l, position);
                else
                    CRSetLayerTile(layer, r, position);
            }
        }
    }
}
void CRDrawUICharRectangle(Vector2 top_left, Vector2 bottom_right, char *tl, char *t, char *tr, char *r, char *br, char *b, char *bl, char *l, char *fill) {
    CRDrawCharRectangle(&cr_config->ui_layers[0], top_left, bottom_right, tl, t, tr, r, br, b, bl, l, fill);
}
void CRDrawUITileRectangle(Vector2 top_left, Vector2 bottom_right, CRTile tl, CRTile t, CRTile tr, CRTile r, CRTile br, CRTile b, CRTile bl, CRTile l, CRTile fill) {
    CRDrawTileRectangle(&cr_config->ui_layers[0], top_left, bottom_right, tl, t, tr, r, br, b, bl, l, fill);
}

// Text Rendering
void CRDrawTextString(CRLayer *layer, char *text, Color tile_color, Color text_color, Font *font, Vector2 start, float tile_size, int width, int height, int word_wrap) {
    Rectangle rec;
    rec.x = start.x;
    rec.y = start.y;
    rec.width = width;
    rec.height = height;
    DrawTextBoxed(font, layer, text, rec, 24, 2.0f, word_wrap, text_color);
}

// Window Information
Vector2 CRCameraOffset() {
    Camera2D *camera = CRGetMainCamera();
    Vector2 camera_offset = camera->offset;
    camera_offset.x /= cr_config->tile_size;
    camera_offset.y /= cr_config->tile_size;
    return camera_offset;
}
Vector2 CRScreenSize() {
    Vector2 size;

    size.x = GetRenderWidth() / cr_config->tile_size;
    size.y = GetRenderHeight() / cr_config->tile_size;

    return size;
}
