#include <CRGA.h>

void DetaledInit() {
    CRGA_Config config_struct = {0};
    config_struct->height = 100;
    config_struct->width = 100;
    config_struct->fonts = my_font_list;
    config_struct->background_fill = CRGA_BLACK;
    config_struct->cell_type = 0;
    config_struct->cell_align = 0;
    config_struct->cell_flags = 0;
    config_struct->background_color = my_background_color;
    config_struct->foreground_color = my_foreground_color;
    CRGA_InitWithConfig(&config_struct);
}

void CellFunctions() {
    // #defines for setting the size of the type that holds the texture index. Affects how many cells the texture can hold
    #define CRGA_TEXTURE_INDEX_8
    #define CRGA_TEXTURE_INDEX_16 // default
    #define CRGA_TEXTURE_INDEX_32
    #define CRGA_TEXTURE_INDEX_64
    // Creates a new cell with the default config
    CRGA_Cell cell = CRGA_NewCharacterCell("@");// config from global config struct
    CRGA_Cell cell = CRGA_NewTextureCell(0, 16);// (texture map, map cell)
    CRGA_Cell cell = CRGA_NewCharacterCellOnLayer("@", layer);// config from layer
    CRGA_Cell cell = CRGA_NewTextureCellOnLayer(0, 16, layer);
    // New cell from nothing
    CRGA_Cell cell = {0};
    cell->bg_color = CRGA_BLACK;
    cell->fg_color = 0xFFFFFFFF;// RGBA color, stored in a uint32_t
    cell->type = CRGA_CELL_TEXT;// enum
    cell->align = CRGA_CELL_BASELINE;// enum
    cell->flags = 0;// bitflags
    /* CRGA_CHAR // 0
     * CRGA_TEXTURE
     * CRGA_TEXT
     */
    /* CRGA_CELL_CENTER // 0
     * CRGA_CELL_BASELINE // keep text aligned to the baseline
     */ 
    // Flags when in Char or Text mode vs Texture mode
    /* CRGA_BOLD
     * CRGA_ITALIC
     * CRGA_UNDERLINE
     * CRGA_STRIKETHROUGH
     * CRGA_INVERSE
     */
    /* CRGA_COLOR_OVERLAY
     * CRGA_COLOR_TINT
     * CRGA_COLOR_ADD
     * CRGA_COLOR_SUBTRACT
     * CRGA_COLOR_MULTIPLY
     * CRGA_COLOR_DIVIDE
     */
    // Cell_color_tint tints the texture with the color. Default is Cell_color_overlay
    // Cell_texture says this cell should draw a texture, not text
    CRGA_Cell* cells = (CRGA_Cell*) malloc(2048 * sizeof(CRGA_Cell));
    CRGA_ERROR error = CRGA_SetLayerCells(width, height, cells, layer);// Developer is expected to do his own memory management
    CRGA_Cells house[5][5] = {0};
    CRGA_ERROR error = CRGA_OverlayCellsOnLayer(x, y, width, height, house, layer);// x, y is local. Cells out of range are dropped
}

void TextureFunctions() {
    CRGA_Texture_Map map = {0};
    CRGA_LoadTexture("/assets/textures/texture.png", &map->texture, CRGA_TEXTURE_BILINEAR);
    int width, height;
    unsigned char *data = some_other_texture_loading_function("/assets/textures/texture.png", &width, &height);
    CRGA_LoadTextureData(data, width, height, &map->texture, CRGA_TEXTURE_SMOOTHPIXEL);
    some_other_texture_freeing_function(data);
    // CRGA_TEXTURE_BILINEAR, _NEAREST, _SMOOTHPIXEL
    map->cell_width = 16;
    map->cell_height = 16;
    map->cell_hz_gap = 0;
    map->cell_vt_gap = 0;
    map->margin_left = 0;
    map->margin_right = 0;
    map->margin_top = 0;
    map->margin_bottom = 0;
    int cells_wide = CRGA_TextureMapCellWidth(map);
    int cells_tall = CRGA_TextureMapCellHeight(map);
    size_t count = CRGA_TextureMapCellCount(map);
    uint32_t cell_index = CRGA_TextureMapCellCoordinatesToIndex(x, y, map);// returns 0 if out of bounds
    uint8_t map_index = CRGA_AddMap(&map);
    cell->texture_map = map_index;// pointer to the map
    cell->map_cell = cell_index;// Counting from 1 at the top left, then left to right, top to bottom
    // 0 is reserved for the null texture, meaning no texture and drawing is skipped
    cell->type = CRGA_CELL_TEXTURE;
    CRGA_ERROR error = CRGA_SetTextureIndex(x,y,16);
    CRGA_ERROR error = CRGA_SetLayerTextureIndex(x,y,16, layer);
    uint32_t line[] = {16,16,26,19,100};
    CRGA_ERROR error = CRGA_SetTexLine(x,y, line, length);
    CRGA_ERROR error = CRGA_SetLayerTextureIndex(x,y, line, length, layer);
}

void TextFunctions() {
    CRGA_Font font;
    uint8_t font_index = CRGA_LoadFont("/assets/fonts/font.ttf", &font);
    cell->font = font_index;
    cell->charCode = "|";// UTF-8
    const char* string = "Hello World!";
    cell->string = string;// cell->string stores a pointer to a string
    cell->string_length = 12;// length of string not including the null terminator
    CRGA_ERROR error = CRGA_CellString("Hello World!", cell);// makes a string that will be accessable out of scope
    CRGA_ERROR error = CRGA_SetString(x, y, "Hello World!");
    CRGA_ERROR error = CRGA_SetCharacter(x, y, "+");// x, y is relative to the layer
    CRGA_ERROR error = CRGA_SetLayerCharacter(x, y, "+", layer);// Sets a single cell
    CRGA_ERROR error = CRGA_SetLayerCharLine(x, y,"Hello World!", length, layer);// Sets a line of cells, left to right.
    // If length is shorter than the string, it stops eairly. If longer, it fills the remaining cells with '\0'.
    /* The following four functions work like the above, but run left to right, right to left, top to bottom, or bottom to top
     * respectively. The LR version is identical to the above function.
     */
    CRGA_ERROR error = CRGA_SetCharLineLR(x, y,"Hello World!", length, layer);
    CRGA_ERROR error = CRGA_SetCharLineRL(x, y,"Hello World!", length, layer);
    CRGA_ERROR error = CRGA_SetCharLineTB(x, y,"Hello World!", length, layer);
    CRGA_ERROR error = CRGA_SetCharLineBT(x, y,"Hello World!", length, layer);
    // Drawing a block of text
    const char* text;
    CRGA_TextBlock *text_block = CRGA_AddText(x,y, width, height, CRGA_TEXTALIGN_LEFT, CRGA_VERTALIGN_TOP, CRGA_SNAP_TL,
                                    CRGA_DISPLAY_CELL);
    CRGA_TextBlock *text_block = CRGA_AddLayerText(x,y, width, height, CRGA_TEXTALIGN_LEFT, CRGA_VERTALIGN_TOP, 
                                    CRGA_SNAP_TL, CRGA_DISPLAY_CELL, layer);
    text_block.current_line = 0;/* The line everything is relative to, counting from the top.
                                 * Everything above this is cropped out.
                                 */
    // See camera functions for more info on this
    /* enum
     * CRGA_TEXTALIGN_LEFT
     * CRGA_TEXTALIGN_CENTER
     * CRGA_TEXTALIGN_RIGHT
     * CRGA_TEXTALIGN_JUSTIFY
     */
    /* enum
     * CRGA_VERTALIGN_TOP
     * CRGA_VERTALIGN_CENTER
     * CRGA_VERTALIGN_BOTTOM
     */
    /* enum
     * CRGA_DISPLAY_CELL all measurements (x,y,width,height) are in cells
     * CRGA_DISPLAY_PIXEL all measurements are in pixels
     */
    CRGA_TextBlock *text_block_array[MAXTEXTBLOCKS] = {0};
    CRGA_SetLayerTextBlocks(text_block_array, MAXTEXTBLOCKS);// Lets the player manage his own text block array memory
}

void EntityFunctions() {
    CRGA_Entity player = CRGA_NewEntity("@");
    player.global = false;// position is always global
    player.inheret_flags = false;// User the flags of the layer the entity is on
    player.flags = CRGA_UI_LAYER;// Custom flags if the above is false, same options as the layer flags
    CRGA_Cell cell = CRGA_NewCell("@", layer);
    player.cell = cell;
    CRGA_AddEntity(x, y, player);// x and y is local when not specified
    CRGA_AddEntityGlobal(x, y, player);
    CRGA_AddEntityToLayer(x, y, player, layer);// Drawn over everything else in that layer
    CRGA_AddEntityToLayerGlobal(x, y, player, layer);
    player.pos_x = 0;
    player.pos_y = 0;
    int global_x, global_y;
    CRGA_GetEntityGlobalPosition(&player, &global_x, &global_y);
    CRGA_SetEntityGlobalPosition(&player, global_x, global_y);
    CRGA_MoveEntity(x, y, entity);// changes the entity position to be the x,y
    CRGA_MoveEntityGlobal(x, y, entity);
    CRGA_ShiftEntity(x, y, entity);// adds the x,y to the entity position
}

void LayerFunctions() {
    // Layers
    CRGA_Layer* base_layer = CRGA_GetLayerByIdent(CRGA_BASE_LAYER);
    int index = CRGA_GetLayerIdent(layer);
    CRGA_Layer* layer = CRGA_AddNewLayer(width, height, -1);// Creates a new layer and appends it to the layer array. Inserts it so the array remains sorted by ident.
    CRGA_Layer new_layer = {0};
    new_layer.width = 32;
    new_layer.height = 32;
    new_layer.ident = 1;
    new_layer.cells = (CRGA_Cell*) malloc(1024 * sizeof(CRGA_Cell));
    CRGA_ERROR error = CRGA_AddLayer(new_layer);/* Developer is expected to do his own memory management for the cells, 
                                                 * but not the layer array
                                                 */
    layer->flags = CRGA_UI_LAYER | CRGA_CULL | CRGA_HIDE | CRGA_NODRAW; // Default is 0x0 = World, Draw Behind, Show, and Draw
    layer->cell_type = 0;// default cell type, alignment, and flags
    layer->cell_align = 0;
    layer->cell_flags = 0;
    config_struct->background_color = my_background_color;
    config_struct->foreground_color = my_foreground_color;
    /* Hide vs Nodraw
     * Nodraw layers are skipped when looping through all the layers in the Draw and DrawLayers functions.
     * Hidden layers won't be drawn by the DrawLayer function.
     * Hide is a "stronger" flag than Nodraw. 
     * Nodraw exists for layers that you don't want to be drawn all the time but you still want to be drawn with DrawLayer
     */
    CRGA_SetLayerIdent(-1, layer);// sets the ident then shifts the layer so that the list remains sorted.
    bool set_entity_flags = true;// Whether we should also set the flags of entities
    bool set_cell_flags = true;// Whether we should also set the flags of entities
    // Sets the flags variable for every cell on the base layer
    CRGA_Resize(width, height);// Any time a layer is not specified, it assumes the base layer
    CRGA_ResizeLayer(width, height, layer);// Performs a realloc, so not something you want to be doing every frame
    CRGA_SetLayerPosition(x, y, layer);// position is always global
    CRGA_SortLayers();// Sorts the array by ident. This should rarely be called.
    layer->crop_left = 3;// Used to hide edge cells without allocating memory
    CRGA_Layer layers[LAYERS_MAX];
    CRGA_NewLayerArray(layers, LAYERS_MAX);// Lets the developer entirely manage his own memory
    CRGA_HideLayer(layer);
    CRGA_ShowLayer(layer);

    // Cells
    CRGA_Cell* cells = (CRGA_Cell*) malloc(2048 * sizeof(CRGA_Cell));
    CRGA_ERROR error = CRGA_SetLayerCells(width, height, cells, layer);// Developer is expected to do his own memory management
    CRGA_Cells house[5][5] = {0};
    CRGA_ERROR error = CRGA_OverlayCellsOnLayer(x, y, width, height, house, layer);// x, y is local. Cells out of range are dropped

    // Entities
    CRGA_Entity player = CRGA_NewEntity("@");
    CRGA_AddEntity(x, y, player);// x and y is local when not specified

    while (Play()) {
        CRGA_DrawStart();
        CRGA_DrawLayers();// Draw every layer that isn't nodraw
        CRGA_DrawLayer(layer);
        CRGA_DrawLayerByIdent(CRGA_BASE_LAYER);
        CRGA_DrawEnd();
    }
}

void CameraFunctions() {
    CRGA_Camera* camera = &CRGA_main_camera;
    camera->pos_x = 0;// relative to the target if following the target, otherwise relative to the world
    camera->pos_y = 0;
    camera->offset_x = 0.5;// in screen widths
    camera->offset_y = 0.5;
    camera->follow_target = true;// ignored if the target is on the UI layer or is NULL
    camera->snap = CRGA_SNAP_TL;// bit flags
    /* There are 4 SNAP flags, TL, TR, BL, and BR where T is top, B is bottom, L is left and R is right
     * If more than one SNAP_CAMERA flag is set, then the camera snaps to the average (sort of). If none are set, 
     * it snaps to the top left. This gives 9 points the camera can snap to.
     * 
     *   TL---TL/TR---TR
     *   |             |
     *   |             |
     * TL/BL    C    TR/BR
     *   |             |
     *   |             |
     *   BL---BL/BR---BR
     * C = TL/BR, TR/BL, TL/TR/BL, TL/TR/BR, TL/BL/BR, TR/BR/BL, TL/TR/BL/BR
     */
    CRGA_Entity player;
    camera->target = &player;// CRGA_DrawStart() function will check if following a target and adjust the camera to it
}

int main() {
    // Init both with and without a config
    CRGA_Config *config_struct = CRGA_Init();

    while (Play()) {
        CRGA_Draw();// draws from one end of the layer array to the other. Indexes just exist for sorting.
    }

    CRGA_Close();
    return 0;
}

void DesignNotes() {
    // To save space, the following are kept in a union together
    cell->font && cell->texture_map && cell->string_len
    cell->character && cell->map_cell && cell->string
}

void Implementation() {
    cull
}