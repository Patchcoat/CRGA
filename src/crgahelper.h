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

static void DrawTextBoxed(Font *font, CRLayer *layer, const char *text, Rectangle rec, float fontSize, float spacing, int wordWrap, Color tint) {
    int length = TextLength(text);  // Total length in bytes of the text, scanned by codepoints in loop

    float textOffsetY = 0;          // Offset between lines (on line break '\n')
    float textOffsetX = 0.0f;       // Offset X to next character to draw

    float scaleFactor = fontSize/(float)font->baseSize;     // Character rectangle scaling factor

    // Word/character wrapping mechanism variables
    enum { MEASURE_STATE = 0, DRAW_STATE = 1 };
    int state = wordWrap? MEASURE_STATE : DRAW_STATE;

    int startLine = -1;         // Index where to begin drawing (where a line begins)
    int endLine = -1;           // Index where to stop drawing (where a line ends)
    int lastk = -1;             // Holds last value of the character position

    for (int i = 0, k = 0; i < length; i++, k++) {
        // Get next codepoint from byte string and glyph index in font
        int codepointByteCount = 0;
        int codepoint = GetCodepoint(&text[i], &codepointByteCount);
        int index = GetGlyphIndex(*font, codepoint);

        // NOTE: Normally we exit the decoding sequence as soon as a bad byte is found (and return 0x3f)
        // but we need to draw all of the bad bytes using the '?' symbol moving one byte
        if (codepoint == 0x3f)
            codepointByteCount = 1;
        i += (codepointByteCount - 1);

        float glyphWidth = 0;
        if (codepoint != '\n') {
            glyphWidth = (font->glyphs[index].advanceX == 0) ? font->recs[index].width*scaleFactor : font->glyphs[index].advanceX*scaleFactor;

            if (i + 1 < length)
                glyphWidth = glyphWidth + spacing;
        }

        // NOTE: When wordWrap is ON we first measure how much of the text we can draw before going outside of the rec container
        // We store this info in startLine and endLine, then we change states, draw the text between those two variables
        // and change states again and again recursively until the end of the text (or until we get outside of the container).
        // When wordWrap is OFF we don't need the measure state so we go to the drawing state immediately
        // and begin drawing on the next line before we can get outside the container.
        if (state == MEASURE_STATE) {
            if ((codepoint == ' ') || (codepoint == '\t') || (codepoint == '\n'))
                endLine = i;

            if ((textOffsetX + glyphWidth) > rec.width) {
                endLine = (endLine < 1)? i : endLine;
                if (i == endLine)
                    endLine -= codepointByteCount;
                if ((startLine + codepointByteCount) == endLine)
                    endLine = (i - codepointByteCount);

                state = !state;
            } else if ((i + 1) == length) {
                endLine = i;
                state = !state;
            } else if (codepoint == '\n')
                state = !state;

            if (state == DRAW_STATE) {
                textOffsetX = 0;
                i = startLine;
                glyphWidth = 0;

                // Save character position when we switch states
                int tmp = lastk;
                lastk = k - 1;
                k = tmp;
            }
        } else {
            if (codepoint == '\n') {
                if (!wordWrap) {
                    textOffsetY += (font->baseSize + font->baseSize/2.0f)*scaleFactor;
                    textOffsetX = 0;
                }
            } else {
                if (!wordWrap && ((textOffsetX + glyphWidth) > rec.width)) {
                    textOffsetY += (font->baseSize + font->baseSize/2.0f)*scaleFactor;
                    textOffsetX = 0;
                }

                // When text overflows rectangle height limit, just stop drawing
                if ((textOffsetY + font->baseSize*scaleFactor) > rec.height)
                    break;

                // Draw current character glyph
                if ((codepoint != ' ') && (codepoint != '\t')) {
                    DrawTextCodepoint(*font, codepoint, (Vector2){ rec.x + textOffsetX, rec.y + textOffsetY }, fontSize, tint);
                }
            }

            if (wordWrap && (i == endLine)) {
                textOffsetY += (font->baseSize + font->baseSize/2.0f)*scaleFactor;
                textOffsetX = 0;
                startLine = endLine;
                endLine = -1;
                glyphWidth = 0;
                k = lastk;

                state = !state;
            }
        }

        if ((textOffsetX != 0) || (codepoint != ' '))
            textOffsetX += glyphWidth;  // avoid leading spaces
    }
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

int UTF8CharToInt(char *character) {
    int out = 0;
    int i = 0;
    while (i < 4 && character[i] == 0) {
        int c = character[i] << (sizeof(char) * 8 * i);
        out += c;
        i++;
    }
    return out;
}

#endif
