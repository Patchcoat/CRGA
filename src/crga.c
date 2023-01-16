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
} cr_fonts = {0,0};

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
void CRInitWindowSizeTitleFPS(const int screenWidth, const int screenHeight,
        const char *title, const int fps) {
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
    Vector2 fontPosition = {40.0f, 225.0f};
    while (!WindowShouldClose())
    {
        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawTextEx(cr_fonts.fonts[0], "Hello World!", fontPosition, 24, 0, BLACK);

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
