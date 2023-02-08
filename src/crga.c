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
#include <stdio.h>
#include <string.h>

CRConfig *cr_config;

// Init
void CRInit() {
    CRConfig *config = (CRConfig *) malloc(sizeof(CRConfig));
    CRInitConfig(config);
    CRSetConfig(config);
    CRInitCharIndexAssoc();
    CRInitWorld();
    CRInitWindow();
}
void CRInitConfig(CRConfig *config) {
    config->window_width = 800;
    config->window_height = 450;
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

    config->main_camera.target = (Vector2){0.0f, 0.0f};
    config->main_camera.offset = (Vector2){0.0f, 0.0f};
    config->main_camera.rotation = 0.0f;
    config->main_camera.zoom = 1.0f;

    config->background_color = BLACK;

    config->assocs = 0;
    config->assoc_count = 0;

    config->fonts = 0;
    config->font_count = 0;

    config->tilemaps = 0;
    config->tilemap_count = 0;
}
inline void CRSetConfig(CRConfig *config) {
    cr_config = config;
}
inline void CRInitCharIndexAssoc() {
    for (int i = 0; i < 255; i++) {
        int index = i;
        // set the character string to the ASCII value followed by three null terminators
        cr_config->char_index_assoc[i].character[0] = (uint32_t) 0;
        cr_config->char_index_assoc[i].character[0] = (char) i;
        // Shift all all ASCII control codes to the end
        index = (i > 0 && i < 32) ? i + 96 : index;
        // shift all (non extended ascii) characters to the front
        index = (i >= 32 && i <= 127) ? i - 31 : index;
        // extended ascii character number matches it's index
        cr_config->char_index_assoc[i].index = index;
        // set the linked list next to the null ptr
        cr_config->char_index_assoc[i].next = 0;
    }
}
inline void CRInitWindow() {
    InitWindow(cr_config->window_width, cr_config->window_height, cr_config->title);
    SetTargetFPS(cr_config->fps);
}

// Cleanup Functions
void CRClose() {
    CRUnloadFonts();
    CRUnloadTilemaps();
    CRUnloadCharIndexAssoc();
    CRUnloadLayers();
    CRUnloadMasks();
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
inline void CRUnloadCharIndexAssoc() {
    if (cr_config->assoc_count == 0)
        return;
    free(cr_config->assocs);
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

// Tilemap Loading
int cmpstr(char *s1, char *s2, size_t size) {
    for (int i = 0; i < size; i++) {
        if (s1[i] != s2[i])
            return 0;
        if (s1[i] == '\0')
            break;
    }
    return 1;
}
Rectangle TileIndexRec(CRTilemap *tilemap, int index) {
    index--;
    int tile_width = tilemap->width;
    int tile_height = tilemap->height;
    Rectangle rect;
    rect.x = 0;
    rect.y = 0;
    rect.width = tile_width;
    rect.height = tile_height;
    if (index > tilemap->tile_count)
        return rect;

    int tiles_h = tilemap->texture.width / tile_width;

    rect.x = (index % tiles_h) * tile_width;
    int y_index = (float) index / (float) tiles_h;
    rect.y = ((int) y_index) * tile_height;

    return rect;
}
void CRLoadTilemap(const char *tilemap_path, int tile_width, int tile_height) {
    if (cr_config->tilemap_count == 255) {
        // there are too many tiles, exit out
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
    // Convert string to index
    int assoc_index = character[0];
    // use index to access value in array
    CRCharIndexAssoc *assoc = &cr_config->char_index_assoc[assoc_index];
    do {
        // if the current value is the character we're looking for, replace it
        if (cmpstr(assoc->character, character, 4)) {
            assoc->index = index;
            return;
        }
        // otherwise, move on to the next 
        assoc = assoc->next;
    } while(assoc != 0);
    // if it doesn't exist, add add a new association
    size_t assoc_count = cr_config->assoc_count;
    if  (assoc_count == 0)
        cr_config->assocs = malloc(sizeof(CRCharIndexAssoc));
    else
        cr_config->assocs = realloc(cr_config->assocs, 
                sizeof(CRCharIndexAssoc) * (assoc_count + 1));
    CRCharIndexAssoc *new_assoc = &cr_config->assocs[assoc_count];
    // copy character over
    for (int i = 0; i < 4; i++)
        new_assoc->character[i] = character[i];
    // assign index
    new_assoc->index = index;
    // insert new_assoc
    new_assoc->next = cr_config->char_index_assoc[assoc_index].next;
    cr_config->char_index_assoc[assoc_index].next = new_assoc;
    // increment the counter
    cr_config->assoc_count++;
}

// Layers
int OnLayer(CRLayer *layer, Vector2 position) {
    position.x += layer->position.x;
    position.y += layer->position.y;

    if (position.x < 0 || position.x > layer->width || 
            position.y < 0 || position.y > layer->height) {
        return 0;
    }
    return 1;
}
int OnMask(CRMask *mask, Vector2 position) {
    position.x += mask->position.x;
    position.y += mask->position.y;

    if (position.x < 0 || position.x > mask->width || 
            position.y < 0 || position.y > mask->height) {
        return 0;
    }
    return 1;
}
CRLayer CRInitLayer() {
    CRLayer layer = CRNewLayer();
    CRInitGrid(&layer);
    return layer;
}
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
void NewWorldLayer() {
    if (cr_config->world_layer_count == 0) {
        cr_config->world_layers = (CRLayer *) malloc(sizeof(CRLayer));
    } else {
        cr_config->world_layers = (CRLayer *) realloc(cr_config->world_layers, 
                sizeof(CRLayer) * (cr_config->world_layer_count + 1));
    }
}
void NewUILayer() {
    if (cr_config->ui_layer_count == 0) {
        cr_config->ui_layers = (CRLayer *) malloc(sizeof(CRLayer));
    } else {
        cr_config->ui_layers = (CRLayer *) realloc(cr_config->ui_layers, 
                sizeof(CRLayer) * (cr_config->ui_layer_count + 1));
    }
}
void CRAddWorldLayer(int index, CRLayer layer) {
    if (index > cr_config->world_layer_count)
        return;// TODO out of bounds error
    if (index == cr_config->world_layer_count) {
        CRAppendWorldLayer(layer);
        return;
    }
    NewWorldLayer();
    for (int i = cr_config->world_layer_count - 1; i >= index; i--) {
        cr_config->world_layers[i+1] = cr_config->world_layers[i];
    }
    cr_config->world_layer_count++;
    cr_config->world_layers[index] = layer;
}
void CRAddUILayer(int index, CRLayer layer) {
    if (index > cr_config->ui_layer_count)
        return;// TODO out of bounds error
    if (index == cr_config->ui_layer_count) {
        CRAppendUILayer(layer);
        return;
    }
    NewUILayer();
    for (int i = cr_config->ui_layer_count - 1; i >= index; i--) {
        cr_config->ui_layers[i+1] = cr_config->ui_layers[i];
    }
    cr_config->ui_layer_count++;
    cr_config->ui_layers[index] = layer;
}
void CRAppendWorldLayer(CRLayer layer) {
    NewWorldLayer();
    cr_config->world_layers[cr_config->world_layer_count] = layer;
    cr_config->world_layer_count++;
}
void CRAppendUILayer(CRLayer layer) {
    NewUILayer();
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
void CRSetLayerFlags(CRLayer *layer, int flags) {
    layer->flags = flags;
}
void CRSetWorldFlags(int flags) {
    cr_config->world_layers[0].flags = flags;
}
void CRSetUIFlags(int flags) {
    cr_config->ui_layers[0].flags = flags;
}

// Mask
int CheckMaskFlags(uint8_t flags1, uint8_t flags2) {
    int match = 0;
    if ((flags1 & 0b0) == (flags2 & 0b0))
        match = 1;
    else if ((flags1 & 0b1) == (flags2 & 0b1))
        match = 1;

    return match;
}
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
    if (layer->mask_count == MAXLAYERMASKS)
        return; // TODO out of bounds exception
    layer->mask_indexes[layer->mask_count] = mask_index;
    layer->mask_count++;
}
void CRSetWorldMask(Vector2 position, uint8_t mask_value) {
    if (cr_config->world_layer_count == 0)
        return; // TODO no world layer found
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
    if (cr_config->ui_layer_count == 0)
        return; // TODO no ui layer found
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
CRTile CRITile(int index) {
    CRTile tile = DefaultTileConfig(index);
    return tile;
}
void CRSetGridTile(CRTile *grid, CRTile tile, Vector2 position, int width, int height) {
    int x = position.x;
    int y = position.y;
    if (x > width || y > height)
        return; // TODO return out of bounds error
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
    if (cr_config->world_layer_count < index)
        return; // TODO return out of bounds error
    CRSetLayerTile(cr_config->world_layers + index, tile, position);
}
void CRSetUILayerTile(int index, CRTile tile, Vector2 position) {
    if (cr_config->ui_layer_count < index)
        return; // TODO return out of bounds error
    CRSetLayerTile(cr_config->world_layers + index, tile, position);
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
int CRCharToIndex(char *character) {
    // Convert string to index
    int assoc_index = character[0];
    // use index to access value in array
    CRCharIndexAssoc *assoc = &cr_config->char_index_assoc[assoc_index];
    do {
        // if the current value is the character we're looking for, replace it
        if (cmpstr(assoc->character, character, 4)) {
            break;
        }
        // otherwise, move on to the next 
        assoc = assoc->next;
    } while(assoc != 0);
    if (assoc == 0)
        return 0;
    return assoc->index;
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
    if (tile->index.i == 0)
        return;

    Color tile_color = tile->background;
    Color text_color = tile->foreground;
    float mask_multiplier = (float) mask/255.0f;

    float transparency = tile_color.a;
    transparency *= mask_multiplier;
    tile_color.a = transparency;

    transparency = text_color.a;
    transparency *= mask_multiplier;
    text_color.a = transparency;

    if (tile_color.a == 0 && text_color.a == 0)
        return;

    char string_out[5];
    for (int i = 0; i <= 4; i++)
        string_out[i] = tile->index.c[i];

    float x_pos = position.x;
    float y_pos = position.y;

    DrawRectangle(x_pos, y_pos, tile_size, tile_size, tile_color);
#if GRID_OUTLINE
    DrawRectangleLines(x_pos, y_pos, tile_size, tile_size, RED);
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
    if (tile->index.i == 0)
        return;

    Color tile_color = tile->background;
    Color foreground_color = tile->foreground;
    float mask_multiplier = (float) mask/255.0f;

    float transparency = tile_color.a;
    transparency *= mask_multiplier;
    tile_color.a = transparency;

    transparency = foreground_color.a;
    transparency *= mask_multiplier;
    foreground_color.a = transparency;

    if (tile_color.a == 0 && foreground_color.a == 0)
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
