#ifndef AF_GL_UTIL_H
#define AF_GL_UTIL_H
#include "GL/glew.h"
//#include <GL/gl.h>
#include <stdio.h>
#define GL_SILENCE_DEPRECATION




#define AF_GL_DEBUG_LOG(msg) do { AF_GL_CheckError(__FILE__, __LINE__, msg); } while(0)

void AF_GL_CheckError(const char *message) {
    GLenum error = glGetError();
    while (error != GL_NO_ERROR) {
        const char *errorString;
        switch (error) {
            case GL_INVALID_ENUM:
                errorString = "GL_INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                errorString = "GL_INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                errorString = "GL_INVALID_OPERATION";
                break;
                /*
            case GL_STACK_OVERFLOW:
                errorString = "GL_STACK_OVERFLOW";
                break;
            case GL_STACK_UNDERFLOW:
                errorString = "GL_STACK_UNDERFLOW";
                break;
                */
            case GL_OUT_OF_MEMORY:
                errorString = "GL_OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                errorString = "GL_INVALID_FRAMEBUFFER_OPERATION";
                break;
            default:
                errorString = "UNKNOWN_ERROR";
        }
        fprintf(stderr, "OpenGL Error: %s | Message: %s\n", errorString, message);
        error = glGetError(); // Continue checking for additional errors
    }
}

#endif
