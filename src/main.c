/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description: The Main file for the Classic Rogelike Graphics API. Used for testing. 
 *
 *        Version:  1.0
 *        Created:  01/14/2023 06:41:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Spencer Welch
 *   Organization:  BYU-I
 *
 * =====================================================================================
 */
#include "crga.h"

int main() {
    CRInitWindow();
    //CRLoadFont("resources/dejavu-sans.book.ttf");
    int height = 10;
    int width = 10;
    short grid[height * width];
    CRInitTiles(grid, '\0', height, width);
    CRSetTiles(grid, height, width);
    grid[0] = 'A';
    grid[1] = 'b';
    grid[width] = 'B';
    CRLoop();
    CRClose();
    return 0;
}
