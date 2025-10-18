#ifndef AF_FONT_H
#define AF_FONT_H

#include "AF_Vec2.h"
#include "AF_Lib_Define.h"
#define AF_CHARACTER_SET_SIZE 128 // Standard ASCII character set size



/*
=========================
AF_Character
Holds all state information relevant to a character as loaded using FreeType
=========================
*/
typedef struct AF_Character {
    char character;  // Character represented
    uint32_t TextureID; // ID handle of the glyph texture
    Vec2   Size;      // Size of glyph
    Vec2   Bearing;   // Offset from baseline to left/top of glyph
    uint32_t Advance;   // Horizontal offset to advance to next glyph
} AF_Character;

/*
=========================
AF_Character_Zero
Initializes an AF_Character struct to zero/default values
=========================
*/
static inline AF_Character AF_Character_Zero(void) {
    Vec2 zeroVec = {0, 0};
    AF_Character character;
    character.character = 0;
    character.TextureID = 0;
    character.Size = zeroVec;
    character.Bearing = zeroVec;
    character.Advance = 0;
    return character;
}

/*
=========================
AF_Font
Holds all state information relevant to a font as loaded using FreeType
=========================
*/
typedef struct AF_Font {
    char fontPath[AF_MAX_PATH_CHAR_SIZE];          // Path to the font file
    uint32_t fontSize;          // Size of the font
    AF_Character characters[AF_CHARACTER_SET_SIZE]; // Array of characters
} AF_Font;

/* 
=========================
AF_Font_Zero
Initializes an AF_Font struct to zero/default values
=========================
*/
static inline AF_Font AF_Font_Zero(void) {
    AF_Font font;
    font.fontPath[0] = '\0';
    font.fontSize = 0;
    for (int i = 0; i < AF_CHARACTER_SET_SIZE; i++) {
        font.characters[i] = AF_Character_Zero();
    }
    return font;
}


#endif // AF_FONT_H
