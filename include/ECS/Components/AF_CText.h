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

#define AF_CTEXT_DEFAULT_TEXT_MESH_NAME "plane"
#define AF_CTEXT_DEFAULT_TEXT_VERT_PATH "text.vert"
#define AF_CTEXT_DEFAULT_TEXT_FRAG_PATH "text.frag"
//#define AF_CTEXT_DEFAULT_TEXT_TEXTURE_NAME "atlas.png"




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
    AF_FLOAT textColor[4];
    void* textData;
    AF_CMesh mesh;
    AF_Font font;
    AF_Anchor_e textAnchor;
    AF_Anchor_e textAlignment; // pivot point for text layout: which position on the text block aligns to screenPos
    uint32_t characterCount; // number of characters in the text
} AF_CText;

AF_CText AF_CText_ADD(void);
AF_CText AF_CText_ZERO(void);

#ifdef __cplusplus
}
#endif

#endif
