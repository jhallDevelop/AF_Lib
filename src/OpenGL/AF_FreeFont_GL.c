#include "AF_Font.h"
#include <GL/glew.h>
#define GL_SILENCE_DEPRECATION
#include <ft2build.h>
#include FT_FREETYPE_H  // Include FreeType header
#include "AF_Util.h"
#include "ECS/Components/AF_CText.h"
#include "AF_Font.h"

#include "AF_File.h"


/*
====================
AF_LoadFont
Load a font using FreeType and create character textures
return font ID or -1 on failure
====================
AF_LoadFont
Load a font using FreeType and create character textures
return font ID or -1 on failure
====================
*/
af_bool_t AF_LoadFont(AF_Font* _font) {
    // FreeType
    // --------
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft)) {
        AF_Log_Error("AF_FreeFont: AF_LoadFont: ERROR::FREETYPE: Could not init FreeType Library\n");
        return AF_FALSE;
    }

    // find path to font
    char fontPathNameBuffer[AF_MAX_PATH_CHAR_SIZE];
    uint32_t getPathSuccess = AF_File_GetPathName(_font->fontPath, fontPathNameBuffer, AF_MAX_PATH_CHAR_SIZE);
    if (getPathSuccess == 0) {
        AF_Log_Error("AF_FreeFont: AF_LoadFont: ERROR::FREETYPE: Failed to find font path: %s\n", _font->fontPath);
        return AF_FALSE;
    }
    // Check for empty path
    if (fontPathNameBuffer[0] == '\0') {
        AF_Log_Error("AF_FreeFont: AF_LoadFont: ERROR::FREETYPE: Failed to find font path: %s\n", _font->fontPath);
        return AF_FALSE;
    }
    
    // load font as face
    FT_Face face;
    if (FT_New_Face(ft, _font->fontPath, 0, &face)) {
        AF_Log_Error("AF_FreeFont: AF_LoadFont: ERROR::FREETYPE: Failed to load font: %s\n", _font->fontPath);
        return AF_FALSE;
    }
   
    // set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, _font->fontSize);

    // disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // load first 128 characters of ASCII set
    for (unsigned char c = 0; c < AF_CHARACTER_SET_SIZE; c++) {
        // Load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            AF_Log_Warning("AF_FreeFont: AF_LoadFont: ERROR::FREETYPE: Failed to load Glyph: %c\n", c);
            continue;
        }

        // If glyph has no bitmap (e.g. space), don't generate a texture
        if (face->glyph->bitmap.width > 0 && face->glyph->bitmap.rows > 0) {
            // generate texture
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            #ifdef AF_WEB_BUILD
                // WebGL 1.0 requires GL_LUMINANCE for single-channel textures.
                // This is more compatible than GL_RED, which is only in WebGL 2.0.
                glTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    GL_LUMINANCE,
                    face->glyph->bitmap.width,
                    face->glyph->bitmap.rows,
                    0,
                    GL_LUMINANCE,
                    GL_UNSIGNED_BYTE,
                    face->glyph->bitmap.buffer
                );
            #else
                glTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    GL_RED,
                    face->glyph->bitmap.width,
                    face->glyph->bitmap.rows,
                    0,
                    GL_RED,
                    GL_UNSIGNED_BYTE,
                    face->glyph->bitmap.buffer
                );
            #endif
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            _font->characters[c].TextureID = texture;
        } else {
            _font->characters[c].TextureID = 0; // Mark as having no texture
        }
        
        // now store character metrics for later use
        _font->characters[c].Size.x = face->glyph->bitmap.width;
        _font->characters[c].Size.y = face->glyph->bitmap.rows;
        _font->characters[c].Bearing.x = face->glyph->bitmap_left;
        _font->characters[c].Bearing.y = face->glyph->bitmap_top;
        _font->characters[c].Advance = face->glyph->advance.x;
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    return AF_TRUE; // Successfully loaded font
}


