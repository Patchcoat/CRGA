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
    Vector2 camera_offset = CRCameraOffset();
    Vector2 size = CRScreenSize();
    if (IsKeyPressed(KEY_W)) {
        movable->position.y -= 1;
        if (movable->position.y < -camera_offset.y) {
            CRShiftCameraOffset(CRGetMainCamera(), (Vector2){0,1});
        }
    } else if (IsKeyPressed(KEY_S)) {
        movable->position.y += 1;
        if (movable->position.y >= -camera_offset.y + size.y) {
            CRShiftCameraOffset(CRGetMainCamera(), (Vector2){0,-1});
        }
    }
    if (IsKeyPressed(KEY_A)) {
        movable->position.x -= 1;
        if (movable->position.x < -camera_offset.x) {
            CRShiftCameraOffset(CRGetMainCamera(), (Vector2){1,0});
        }
    } else if (IsKeyPressed(KEY_D)) {
        movable->position.x += 1;
        if (movable->position.x >= -camera_offset.x + size.x) {
            CRShiftCameraOffset(CRGetMainCamera(), (Vector2){-1,0});
        }
    }
}

int main() {
    // Startup
    CRInit();
    CRSetPreDraw(&PreDraw);

    // Load font/tilemap
    CRLoadTilemap("resources/tilemap.png", 8, 8);
    //CRLoadFont("resources/dejavu-sans.book.ttf");

    // Set base world tiles
    CRSetWorldTileChar("A", (Vector2) {0,0});
    CRSetWorldTileChar("B", (Vector2) {1,0});
    CRSetWorldTileChar("B", (Vector2) {2,0});
    CRSetWorldTileChar("C", (Vector2) {3,0});
    CRSetWorldTileChar("b", (Vector2) {0,1});
    CRSetWorldTileChar("b", (Vector2) {0,2});
    CRSetWorldTileChar("c", (Vector2) {3,1});
    CRSetWorldTileChar("c", (Vector2) {3,2});
    CRSetWorldTileChar("D", (Vector2) {0,3});
    CRSetWorldTileChar("E", (Vector2) {3,3});
    CRSetWorldTileChar("B", (Vector2) {1,3});
    CRSetWorldTileChar("T", (Vector2) {2,3});
    CRSetWorldTileChar("T", (Vector2) {-1, -1});
    //CRSetWorldMask((Vector2) {1,0}, 100);

    // Use tilemap
    CRSetWorldFlags(0b01);
    // Associate character with tile index
    CRSetCharAssoc("A", 1);
    CRSetCharAssoc("B", 2);
    CRSetCharAssoc("T", 3);
    CRSetCharAssoc("C", 4);
    CRSetCharAssoc("D", 33);
    CRSetCharAssoc("E", 36);
    CRSetCharAssoc("b", 17);
    CRSetCharAssoc("M", 200);
    CRSetCharAssoc("00", 16);
    CRSetCharAssoc("c", 20);
    CRSetCharAssoc("@", 5);

    CRDrawUICharRectangle((Vector2){10,10}, (Vector2){20,20}, "+", "=", "+", "|", "+", "=", "+", "|", " ");

    // Create a new world layer
    CRLayer layer = CRInitLayer();
    CRSetLayerTileChar(&layer, "C", (Vector2){1, 1});
    CRSetLayerTileChar(&layer, "C", (Vector2){2, 2});
    CRSetLayerTileChar(&layer, "C", (Vector2){1, 2});
    CRSetLayerTileChar(&layer, "C", (Vector2){2, 1});
    CRAddWorldLayer(1, layer);
    CRSetLayerFlags(&layer, 0b00);

    // Create player entity
    CREntity player = CRNewEntity(CRCTile("@"), (Vector2) {5, 5});
    //player.tile.shift = (Vector2) {0, -3};
    CRAddEntity(&player);
    movable = &player;

    CRLoop();

    // Cleanup
    CRClose();
    return 0;
}
