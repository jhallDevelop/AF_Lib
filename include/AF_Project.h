/*
===============================================================================
AF_PROJECT_H


===============================================================================
*/
#ifndef AF_PROJECT_H
#define AF_PROJECT_H
#include "AF_AppData.h"
#include "AF_Assets.h"
#include "AF_File.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}

#define PLATFORM_STRING "PLATFORM = "
#define MAX_PLATFORM_LEN 49 // Max length for the platform name buffer (50 - 1 for null terminator)


/*
================
AF_Project_Load
// Construct and return the platforms as an array of chars
================
*/
inline static bool AF_Project_Load(AF_AppData* _appData, const char* _filePath){
    // Open a file for binary reading
    FILE* file = AF_File_OpenFile(_filePath, "r");
    if (file == NULL) {
        AF_Log_Error("AF_Project_Load: FAILED to open file %s\n", _filePath);
        return false;
    } 
    // buffer reading the line
    char line[256];
    bool found = false;

    while(fgets(line, sizeof(line), file)){
        // split the string at "PLATFORM = "
        char* token = strstr(line, PLATFORM_STRING);
        if(token){
                // move pointer past "PLATFORM ="
                token += strlen(PLATFORM_STRING);
                // skip any leading whitespace
                while(*token == ' ' || *token =='\t') 
                {
                    token++;
                }

                // extract the word (stop at space or newline
                char word[MAX_PLATFORM_LEN + 1];
                int i = 0;
                // Copy while valid character, not whitespace/newline, and buffer has space
                while (token[i] && token[i] != ' ' && token[i] != '\n' && token[i] != '\t' && i < MAX_PLATFORM_LEN) {
                    word[i] = token[i];
                    i++;
                }
                word[i] = '\0'; // null terminator
                
                // check if we extracted something or just whitespace
                if(i > 0){
                    // if word found is a int value, its likely one of our platform build types
                    // if the value is larger than 16, its likely there is an error in the data being read.
                    if(isdigit(*word) == false){
                        return false;
                    }
                    
                    // convert from ascii char to int
                    int platformDigit = *word - '0';
                    // range check if the result is a mess
                    if(platformDigit < 0 || platformDigit > 16){
                        return false;
                    }
                    AF_Log("AF_Project_Load: save platform digit %i\n", platformDigit);
                    // save the platform type as a enum value
                    _appData->projectData.buildPlatformType = (AF_Platform_e)platformDigit;

                    found = true;
                    break; // Exit the loop once found (if only first occurrence is needed)
                }else {
                    // Found "PLATFORM = " but it was followed only by whitespace or nothing on the line
                    AF_Log_Warning("AF_Project_Load: Found '%s' but no value following it on the line in %s\n", PLATFORM_STRING, _filePath);
                    // Continue searching subsequent lines? Or treat as error? Depends on requirements.
                    // If finding the line but no value is an error, set found = false and break, or return false here.
               }

                
            }
        }

    // Close the file - *AFTER* the loop
    fclose(file);

    // Log error only if the loop finished without finding the string
    if (!found) {
        AF_Log_Error("AF_Project_Load: Could not find valid '%s' line in file %s\n", PLATFORM_STRING, _filePath);
    }

    // Return true only if the platform string was successfully found and extracted
    return found;
}






#endif


#endif // AF_PROJECT_H
