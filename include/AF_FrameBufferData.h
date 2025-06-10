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
    AF_FrameBufferData returnBufferData;
    returnBufferData.fbo = 0;
    returnBufferData.rbo = 0;
    returnBufferData.shaderID = 0;
    returnBufferData.textureID = 0;
    returnBufferData.textureWidth = 0;
    returnBufferData.textureHeight = 0;
    returnBufferData.vertPath = "\0";
    returnBufferData.fragPath = "\0";
    returnBufferData.shaderTextureName = "\0";
    returnBufferData.internalFormat = 0;
    returnBufferData.textureAttatchmentType = 0;
    returnBufferData.drawBufferType = 0;
    returnBufferData.readBufferType = 0;
    returnBufferData.minFilter = 0;
    returnBufferData.magFilter = 0;
    return returnBufferData;
}

#endif
