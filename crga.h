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

// Window Functions
void CRInitWindow();
void CRInitWindowTitle(const char *title);
void CRInitWindowSize(const int screenWidth, const int screenHeight);
void CRInitWindowSizeTitle(const int screenWidth, const int screenHeight, const char *title);
void CRInitWindowSizeFPS(const int screenWidth, const int screenHeight, const int fps);
void CRInitWindowSizeTitleFPS(const int screenWidth, const int screenHeight,
        const char *title, const int fps);

// Cleanup Functions
void CRClose();
void CRUnloadFonts();

// Drawing Loop
void CRLoop();

// Font Rendering
void CRLoadFont(const char *font_path);
void CRLoadFontSize(const char *font_path, int size);

#endif
