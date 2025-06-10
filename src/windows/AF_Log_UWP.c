/*
===============================================================================
AF_LOG Implementation

Logging helper functions for the game
Calls vfprintf but adds some colour to text output
===============================================================================
*/
#include "AF_Log.h"
#include <windows.h>

/*
====================
AF_Log_Error
Log standard error to console
====================
*/
void AF_Log_Error(const char* _message, ...) {
    // A buffer to hold the user's formatted message.
    char user_message[2048];
    // A final buffer to hold the prefix + user message + newline.
    char final_output[2048];

    // 1. Handle the null input case first, sending it to the debugger.
    if (_message == NULL) {
        // Note: ANSI colors are removed as they don't work in the VS output window.
        OutputDebugStringA("Error: Invalid format string provided to AF_Log_Error.\n");
        return;
    }

    // 2. Safely create the formatted message from the variable arguments.
    // This replaces both the incorrect `fprintf` and the `vfprintf` calls.
    va_list args;
    va_start(args, _message);
    // Use vsnprintf to "print" the formatted string into our buffer, preventing overflows.
    vsnprintf(user_message, sizeof(user_message), _message, args);
    va_end(args);

    // 3. Construct the final output string with the "Error: " prefix and a newline.
    // We use sprintf_s for a safe way to combine the strings.
    sprintf_s(final_output, sizeof(final_output), "Error: %s\n", user_message);

    // 4. Send the complete, final string to the Visual Studio Output window.
    OutputDebugStringA(final_output);

}

/*
====================
AF_Log
Normal log to console
====================
*/
void AF_Log(const char* _message, ...) {
    // A buffer to hold the user's formatted message from the arguments.
    char user_message[2048];
    // A final buffer to hold the "LOG: " prefix + user message + newline.
    char final_output[2048];

    // Handle null input gracefully.
    if (_message == NULL) {
        OutputDebugStringA("LOG: Invalid format string provided to AF_Log.\n");
        return;
    }

    // 1. Safely create the formatted message from the variable arguments.
    // This is the correct replacement for the flawed OutputDebugStringA call.
    va_list args;
    va_start(args, _message);
    vsnprintf(user_message, sizeof(user_message), _message, args);
    va_end(args);

    // 2. Construct the final output string with a "LOG: " prefix and a newline.
    // The commented AFSaveLog function suggested using a "LOG: " prefix.
    sprintf_s(final_output, sizeof(final_output), "LOG: %s\n", user_message);

    // 3. Send the complete, final string to the Visual Studio Output window.
    OutputDebugStringA(final_output);
}




/*
====================
AF_Log_Warning
Log warning to console
====================
*/
void AF_Log_Warning(const char* _message, ...) {
    // Buffer to hold the user's formatted message.
    char user_message[2048];
    // Final buffer to hold the "Warning: " prefix + user message + newline.
    char final_output[2048];

    // Handle null input gracefully.
    if (_message == NULL) {
        OutputDebugStringA("Warning: Invalid format string provided to AF_Log_Warning.\n");
        return;
    }

    // 1. Safely create the formatted message from the variable arguments.
    // This is the correct way to handle variadic arguments for this purpose.
    va_list args;
    va_start(args, _message);
    vsnprintf(user_message, sizeof(user_message), _message, args);
    va_end(args);

    // 2. Construct the final output string with the "Warning: " prefix and a newline.
    sprintf_s(final_output, sizeof(final_output), "Warning: %s\n", user_message);

    // 3. Send the complete, final string to the Visual Studio Output window.
    OutputDebugStringA(final_output);
}
