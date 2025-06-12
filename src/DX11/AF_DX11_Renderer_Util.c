#include "AF_Renderer_Util.h"
#include "AF_Log.h"


void AF_Renderer_CheckError(const char* message) {
	AF_Log_Warning("AF_Renderer_CheckError: DX11 not implemented yet\n");

    /*
    GLenum error = glGetError();
    while (error != GL_NO_ERROR) {
        const char* errorString;
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
    */
}

