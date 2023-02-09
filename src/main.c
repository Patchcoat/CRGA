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
 *         Author:  
 *   Organization:  
 *
 * =====================================================================================
 */
#include "crga.h"
#include <stdio.h>

#define GRIDHEIGHT 10
#define GRIDWIDTH 10

CREntity *movable;

void PreDraw() {
    if (IsKeyPressed(KEY_W)) {
        movable->position.y -= 1;
    } else if (IsKeyPressed(KEY_S)) {
        movable->position.y += 1;
    }
    if (IsKeyPressed(KEY_A)) {
        movable->position.x -= 1;
    } else if (IsKeyPressed(KEY_D)) {
        movable->position.x += 1;
    }
}

int main() {
    // Startup
    CRInit();
    CRPreDraw = &PreDraw;

    // Load font/tilemap
    CRLoadTilemap("resources/tilemap.png", 8, 8);
    //CRLoadFont("resources/dejavu-sans.book.ttf");

    // Set base world tiles
    CRSetWorldTileChar("A", (Vector2) {0,0});
    CRSetWorldTileChar("b", (Vector2) {1,0});
    CRSetWorldTileChar("B", (Vector2) {0,1});
    CRSetWorldMask((Vector2) {1,0}, 100);

    // Use tilemap
    CRSetWorldFlags(0b11);
    // Associate character with tile index
    CRSetCharAssoc("A", 1);
    CRSetCharAssoc("B", 17);
    CRSetCharAssoc("C", 20);
    CRSetCharAssoc("b", 4);
    CRSetCharAssoc("@", 5);

    // Create a new world layer
    CRLayer layer = CRInitLayer();
    CRSetLayerTileChar(&layer, "C", (Vector2){1, 1});
    CRAddWorldLayer(1, layer);
    CRSetLayerFlags(&layer, 0b00);

    // Create player entity
    CREntity player = CRNewEntity(CRCTile("@"), (Vector2) {0, 0});
    //player.tile.shift = (Vector2) {0, -3};
    CRAddEntity(&player);
    movable = &player;

    CRLoop();

    // Cleanup
    CRClose();
    return 0;
}
