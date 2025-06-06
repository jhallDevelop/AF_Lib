/*
===============================================================================
AF_CTEXT_H definitions

Definition for the camera component struct
and helper functions
===============================================================================
*/
#ifndef AF_CTEXT_H
#define AF_CTEXT_H

#include "AF_Vec2.h"
#include "AF_Vec4.h"
#include "AF_Lib_Define.h"



#ifdef __cplusplus
extern "C" {    
#endif

typedef struct AF_CText {
    PACKED_CHAR enabled;	    // 1 byte
    BOOL isDirty;
    BOOL isShowing;
    uint8_t fontID;
    const char* fontPath;
    const char* text;
    Vec2 screenPos;
    Vec2 textBounds;
    uint8_t textColor[4];
    void* textData;
} AF_CText;

AF_CText AF_CText_ADD(void);
AF_CText AF_CText_ZERO(void);

#ifdef __cplusplus
}
#endif

#endif
