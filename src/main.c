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

#define GRIDHEIGHT 10
#define GRIDWIDTH 10

int main() {
    CRInitWindow();
    //CRLoadFont("resources/dejavu-sans.book.ttf");
    short grid[GRIDHEIGHT * GRIDWIDTH];
    CRInitTiles(grid, '\0', GRIDHEIGHT, GRIDWIDTH);
    CRSetTiles(grid, GRIDHEIGHT, GRIDWIDTH);
    grid[0] = 'A';
    grid[1] = 'b';
    grid[GRIDWIDTH] = 'B';
    CRLoop();
    CRClose();
    return 0;
}
