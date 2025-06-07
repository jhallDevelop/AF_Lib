/*
===============================================================================
AF_CAudioSource_H definitions

Definition for the audio source component struct
and audio helper functions
===============================================================================
*/
#ifndef AF_CAudioSource_H
#define AF_CAudioSource_H
#include "AF_AudioClip.h"
#include "AF_Lib_Define.h"
#include "AF_Component.h"
#ifdef __cplusplus
extern "C" {    
#endif

typedef struct AF_CAudioSource {
    PACKED_CHAR enabled;	
    AF_AudioClip clip;
    uint8_t channel;
    af_bool_t loop;
    af_bool_t isPlaying;
    void* clipData; // special void* for data if needed .e.g wav64 format for n64/libdragon
} AF_CAudioSource;

AF_CAudioSource AF_CAudioSource_ZERO(void);
AF_CAudioSource AF_CAudioSource_ADD(void);

#ifdef __cplusplus
}
#endif


#endif // AF_CAudioSource_H
