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
#include <stdio.h>

#define GRIDHEIGHT 10
#define GRIDWIDTH 10

void pre_draw() {
    Vector2 camera_position = (Vector2) {0, 0};
    int change = 0;
    if (IsKeyPressed(KEY_W)) {
        change = 1;
        camera_position.y -= 1;
    } else if (IsKeyPressed(KEY_S)) {
        change = 1;
        camera_position.y += 1;
    }
    if (IsKeyPressed(KEY_A)) {
        change = 1;
        camera_position.x -= 1;
    } else if (IsKeyPressed(KEY_D)) {
        change = 1;
        camera_position.x += 1;
    }
    if (change) {
        CRShiftCameraOffset(CRGetMainCamera(), camera_position);
    }
}

int main() {
    printf("hello\n");
    CRInit();

    //CRLoadFont("resources/dejavu-sans.book.ttf");
    short grid[GRIDHEIGHT * GRIDWIDTH] = {0};
    printf("hello\n");
    CRSetWorldTiles(grid, GRIDHEIGHT, GRIDWIDTH);
    grid[0] = 'A';
    grid[1] = 'b';
    grid[GRIDWIDTH] = 'B';
    printf("hello\n");
    CRSetPredrawFunction(pre_draw);
    CRLoop();

    CRClose();
    return 0;
}
