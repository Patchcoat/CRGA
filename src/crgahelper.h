/*
 * =====================================================================================
 *
 *       Filename:  crga-helper.h
 *
 *    Description: a list of CRGA helper functions 
 *
 *        Version:  1.0
 *        Created:  02/08/2023 04:42:41 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef CRGA_HELPER_HEADER
#define CRGA_HELPER_HEADER

#include "crga.h"
#include <raylib.h>

int get_utf8_char_length(char *string) {
    char c = *string;
    if (((c >> 8) & 1) == 0)
        return 1;
    int char_length = -1;
    int char_flag = 1;
    int shift = 8;
    while(char_flag == 1) {
        char_flag = (c >> shift) & 1;
        shift--;
        char_length++;
    }
    return char_length;
}
char *get_next_utf8_char(char *string) {
    return string + get_utf8_char_length(string);
}

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

int CheckMaskFlags(uint8_t flags1, uint8_t flags2) {
    int match = 0;
    if ((flags1 & 0b0) == (flags2 & 0b0))
        match = 1;
    else if ((flags1 & 0b1) == (flags2 & 0b1))
        match = 1;

    return match;
}

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

#endif
