#include "AF_Font.h"
#include "AF_File.h"
#include "AF_Log.h"

#include <GL/glew.h>
#include <math.h>
#include <stdlib.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

/*
====================
AF_LoadFont
Load a font using stb_truetype and create character textures
====================
*/
af_bool_t AF_LoadFont(AF_Font* _font) {
    if (_font == NULL || _font->fontPath[0] == '\0') {
        AF_Log_Error("AF_FreeFont: AF_LoadFont: invalid font input\n");
        return AF_FALSE;
    }

    FILE* fontFile = AF_File_Open(_font->fontPath, "rb");
    if (fontFile == NULL) {
        AF_Log_Error("AF_FreeFont: AF_LoadFont: failed to open font %s\n", _font->fontPath);
        return AF_FALSE;
    }

    fseek(fontFile, 0, SEEK_END);
    long fileSize = ftell(fontFile);
    fseek(fontFile, 0, SEEK_SET);
    if (fileSize <= 0) {
        AF_File_Close(fontFile);
        AF_Log_Error("AF_FreeFont: AF_LoadFont: invalid file size for %s\n", _font->fontPath);
        return AF_FALSE;
    }

    unsigned char* ttfBuffer = (unsigned char*)malloc((size_t)fileSize);
    if (ttfBuffer == NULL) {
        AF_File_Close(fontFile);
        AF_Log_Error("AF_FreeFont: AF_LoadFont: out of memory for %s\n", _font->fontPath);
        return AF_FALSE;
    }

    size_t readBytes = fread(ttfBuffer, 1, (size_t)fileSize, fontFile);
    AF_File_Close(fontFile);
    if (readBytes != (size_t)fileSize) {
        free(ttfBuffer);
        AF_Log_Error("AF_FreeFont: AF_LoadFont: failed reading %s\n", _font->fontPath);
        return AF_FALSE;
    }

    stbtt_fontinfo fontInfo;
    if (!stbtt_InitFont(&fontInfo, ttfBuffer, 0)) {
        free(ttfBuffer);
        AF_Log_Error("AF_FreeFont: AF_LoadFont: stbtt init failed for %s\n", _font->fontPath);
        return AF_FALSE;
    }

    uint32_t fontSize = (_font->fontSize > 0) ? _font->fontSize : 48;
    float scale = stbtt_ScaleForPixelHeight(&fontInfo, (float)fontSize);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < AF_CHARACTER_SET_SIZE; ++c) {
        int width = 0;
        int height = 0;
        int xoff = 0;
        int yoff = 0;
        int advanceWidth = 0;
        int leftSideBearing = 0;

        unsigned char* bitmap = stbtt_GetCodepointBitmap(
            &fontInfo,
            scale,
            scale,
            c,
            &width,
            &height,
            &xoff,
            &yoff
        );

        stbtt_GetCodepointHMetrics(&fontInfo, c, &advanceWidth, &leftSideBearing);

        if (bitmap != NULL && width > 0 && height > 0) {
            uint32_t textureID = 0;
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                #ifdef AF_WEB_BUILD
                GL_R8,
                #else
                GL_RED,
                #endif
                width,
                height,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                bitmap
            );
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            _font->characters[c].TextureID = textureID;
        } else {
            _font->characters[c].TextureID = 0;
        }

        _font->characters[c].character = (char)c;
        _font->characters[c].Size.x = (AF_FLOAT)width;
        _font->characters[c].Size.y = (AF_FLOAT)height;
        _font->characters[c].Bearing.x = (AF_FLOAT)xoff;
        _font->characters[c].Bearing.y = (AF_FLOAT)(-yoff);
        _font->characters[c].Advance = (uint32_t)lroundf((double)advanceWidth * (double)scale * 64.0);

        if (bitmap != NULL) {
            stbtt_FreeBitmap(bitmap, NULL);
        }
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    free(ttfBuffer);
    return AF_TRUE;
}


