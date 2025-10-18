/*
===============================================================================
AF_CTEXT_H definitions

Definition for the camera component struct
and helper functions
===============================================================================
*/
#ifndef AF_CTEXT_H
#define AF_CTEXT_H

#include "AF_Math/AF_Vec2.h"
#include "AF_Math/AF_Vec4.h"
#include "AF_Lib_Define.h"
#include "AF_Font.h"
#include "ECS/Components/AF_CMesh.h"


#ifdef __cplusplus
extern "C" {    
#endif

// Static quad vertices for text rendering (positions + texCoords)
static AF_Vertex AF_CTEXT_QUAD_VERTS[6] = {
    // Triangle 1: top-left, bottom-left, bottom-right
    {{-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}, // top-left
    {{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}, // bottom-left
    {{ 1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}, // bottom-right
    
    // Triangle 2: top-left, bottom-right, top-right
    {{-1.0f,  1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}, // top-left
    {{ 1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}, // bottom-right
    {{ 1.0f,  1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}}  // top-right
};

typedef struct AF_CText {
    PACKED_CHAR enabled;	    // 1 byte
    af_bool_t isDirty;
    af_bool_t isShowing;
    uint8_t fontID;
    // TODO: move strings out to a separate struct or use a string library
    char fontPath[AF_MAX_PATH_CHAR_SIZE];
    char text[AF_MAX_PATH_CHAR_SIZE];
    Vec2 screenPos;
    Vec2 textBounds;
    uint8_t textColor[4];
    void* textData;
    AF_CMesh mesh;
    AF_Font font;
} AF_CText;

AF_CText AF_CText_ADD(void);
AF_CText AF_CText_ZERO(void);

#ifdef __cplusplus
}
#endif

#endif
