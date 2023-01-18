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

struct CRFonts {
    Font *fonts;
    unsigned char count;
} cr_fonts = {0, 0};

struct CRGrid {
    short *grid;
    int width;
    int height;
} cr_grid = {0, 0, 0};

// Window Functions
void CRInitWindow() {
    CRInitWindowSizeTitleFPS(800, 450, "CRGA Basic Window", 60);
}
void CRInitWindowTitle(const char *title) { CRInitWindowSizeTitleFPS(800, 450, title, 60);
}
void CRInitWindowSize(const int screenWidth, const int screenHeight) {
    CRInitWindowSizeTitleFPS(screenWidth, screenHeight, "CRGA Basic Window", 60);
}
void CRInitWindowSizeTitle(const int screenWidth, const int screenHeight, const char *title) {
    CRInitWindowSizeTitleFPS(screenWidth, screenHeight, title, 60);
}
void CRInitWindowSizeFPS(const int screenWidth, const int screenHeight, const int fps) {
    CRInitWindowSizeTitleFPS(screenWidth, screenHeight, "CRGA Basic Window", fps);
}
void CRInitWindowSizeTitleFPS(const int screenWidth, const int screenHeight, const char *title, const int fps) {
    InitWindow(screenWidth, screenHeight, title);

    SetTargetFPS(fps);
}

// Cleanup Functions
void CRClose() {
    CRUnloadFonts();
    CloseWindow();
}
inline void CRUnloadFonts() {
    for (int i = cr_fonts.count-1; i >= 0; i--) {
        UnloadFont(cr_fonts.fonts[i]);
    }
    free(cr_fonts.fonts);
}

// Drawing Loop
void CRLoop() {
    while (!WindowShouldClose())
    {
        BeginDrawing();

            ClearBackground(BLACK);

            CRDrawTiles();

        EndDrawing();
    }
}

// Font Rendering
inline void CRLoadFont(const char *font_path) {
    CRLoadFontSize(font_path, 96);
}

void CRLoadFontSize(const char *font_path, int size) {
    if (cr_fonts.count == 255) {
        // there are too many fonts, exit out
        return;
    } else if (cr_fonts.count == 0) {
        cr_fonts.fonts = (Font *) malloc(sizeof(Font));
    } else {
        cr_fonts.fonts = (Font *) realloc(cr_fonts.fonts, cr_fonts.count + 1);
    }
    unsigned char index = cr_fonts.count;
    cr_fonts.count += 1;
    
    cr_fonts.fonts[index] = LoadFontEx(font_path, size, 0, 0);

    GenTextureMipmaps(&cr_fonts.fonts[index].texture);
    SetTextureFilter(cr_fonts.fonts[index].texture, TEXTURE_FILTER_POINT);
}

// Tile Rendering
char IndexToChar(short index) {
    char charList[] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0','\0',' ','!','"','#','$','%','&','\'','(',')','*','+',',','-','.','/','0','1','2','3','4','5','6','7','8','9',':',';','<','=','>','?','@','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','[','\\',']','^','_','`','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','{','|','}','~','\0'};
    return charList[index];
}
void CRInitTiles(short *grid, short value, int height, int width){
    for (int i = 0; i < height * width; i++) {
        grid[i] = value;
    }
}
void CRSetTiles(short *grid, int width, int height) {
    cr_grid.grid = grid;
    cr_grid.width = width;
    cr_grid.height = height;
}
void CRDrawTiles() {
    float size = 20.0f;
    for (int row = 0; row < cr_grid.height; row++) {
        for (int col = 0; col < cr_grid.width; col++) {
            short index = cr_grid.grid[col * cr_grid.width + row];
            if (index == 0)
                continue;
            float x_pos = size * row;
            float y_pos = size * col;
            DrawRectangle(x_pos, y_pos, size, size, BLACK);
#if GRID_OUTLINE
            DrawRectangleLines(x_pos, y_pos, size, size, RED);
#endif
            char character[2] = {IndexToChar(index),'\0'};
            Vector2 position = {x_pos, y_pos};
            if (cr_fonts.count > 0) {
                position.x += size/2.0f - MeasureTextEx(cr_fonts.fonts[0], character, 24, 0).x/2.0f;
                DrawTextEx(cr_fonts.fonts[0], character, position, 24, 0, WHITE);
            } else {
                position.x += size/2.0f - MeasureText(character, 24)/2.0f;
                DrawText(character, position.x, position.y, 24, WHITE);
            }
        }
    }
}
