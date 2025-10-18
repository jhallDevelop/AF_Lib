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
    af_bool_t returnSuccess = AF_FALSE;
    // --------
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
    {
        AF_Log_Error("AF_FreeFont: AF_LoadFont: ERROR::FREETYPE: Could not init FreeType Library");
        return -1;
    }

	// find path to font
    //std::string font_name = FileSystem::getPath("resources/fonts/Antonio-Bold.ttf");
    char fontPathNameBuffer[AF_MAX_PATH_CHAR_SIZE];
    uint32_t getPathSuccess = AF_File_GetPathName(_font->fontPath, fontPathNameBuffer, AF_MAX_PATH_CHAR_SIZE);
    if(getPathSuccess == 0){
        AF_Log_Error("AF_FreeFont: AF_LoadFont: ERROR::FREETYPE: Failed to find font path: %s", _font->fontPath);
        return -1;
    }
    // Check for empty path
    if (fontPathNameBuffer[0] == '\0')
    {
        AF_Log_Error("AF_FreeFont: AF_LoadFont: ERROR::FREETYPE: Failed to find font path: %s", _font->fontPath);
        return -1;
    }
    
	// load font as face
    FT_Face face;
    if (FT_New_Face(ft, fontPathNameBuffer, 0, &face)) {
        AF_Log_Error("AF_FreeFont: AF_LoadFont: ERROR::FREETYPE: Failed to load font");
        return -1;
    }
   
    // set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, 48);

    // disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // load first 128 characters of ASCII set
    AF_Character characters[AF_CHARACTER_SET_SIZE];
    for (unsigned char c = 0; c < AF_CHARACTER_SET_SIZE; c++)
    {
        // Load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            AF_Log_Error("AF_FreeFont: AF_LoadFont: ERROR::FREETYPE: Failed to load Glyph");
            continue;
        }

        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
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
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Vec2 size = {face->glyph->bitmap.width, face->glyph->bitmap.rows};
        Vec2 bearing = {face->glyph->bitmap_left, face->glyph->bitmap_top};
        AF_Character character = AF_Character_Zero();
        character.character = c;
        character.TextureID = texture;
        character.Size = size;
        character.Bearing = bearing;
        character.Advance = face->glyph->advance.x;

        characters[c] = character;
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // Copy characters to font
    for (unsigned char c = 0; c < AF_CHARACTER_SET_SIZE; c++)
    {
        _font->characters[c] = characters[c];
    }

    returnSuccess = AF_TRUE; // Successfully loaded font
    return returnSuccess;
}


