#ifndef AF_FRAMEBUFFERDATA_H
#define AF_FRAMEBUFFERDATA_H

#include <stdint.h>

typedef struct AF_FrameBufferData{
    uint32_t fbo;
    uint32_t rbo;
    uint32_t shaderID;
    uint32_t textureID;
    uint16_t textureWidth;
    uint16_t textureHeight;
    const char* vertPath;
    const char* fragPath;
    const char* shaderTextureName;
    uint32_t internalFormat;
    uint32_t textureAttatchmentType;
    uint32_t drawBufferType;
    uint32_t readBufferType;
    uint32_t minFilter;
    uint32_t magFilter;
} AF_FrameBufferData;

static inline AF_FrameBufferData AF_FrameBufferData_ZERO(void){
    AF_FrameBufferData returnBufferData = {
        .fbo = 0,
        .rbo = 0,
        .shaderID = 0,
        .textureID = 0,
        .textureWidth = 0,
        .textureHeight = 0,
        .vertPath = "\0",
        .fragPath = "\0",
        .shaderTextureName = "\0",
        .internalFormat = 0,
        .textureAttatchmentType = 0,
        .drawBufferType = 0,
        .readBufferType = 0,
        .minFilter = 0,
        .magFilter = 0
    };
    return returnBufferData;
}

#endif
