#include "AF_Script_Engine.h"
#include "AF_File.h"
#include "AF_String.h"
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#if !defined(_WIN32)
#include <unistd.h>
#if defined(__APPLE__)
#include <mach-o/dyld.h>
#endif
#endif
#ifdef _WIN32
#include <windows.h>
#endif
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#ifdef _WIN32
static void AF_Script_NormalizePath(char* path) {
    if (!path) return;
    for (char* p = path; *p; ++p) {
        if (*p == '/') *p = '\\';
    }
}
#else
static void AF_Script_NormalizePath(char* path) {
    // POSIX uses '/' as path separator, nothing to normalize.
    (void)path;
}
#endif

static const char* AF_Script_GetBinaryExtension(void) {
#ifdef __EMSCRIPTEN__
    return ".so";
#elif defined(_WIN32)
    return ".dll";
#elif defined(__APPLE__)
    return ".dylib";
#else
    return ".so";
#endif
}

static void AF_Script_DeriveNameFromPath(AF_CScript* script) {
    if (script == NULL) {
        return;
    }
    if (script->scriptName[0] != '\0' || script->scriptFullPath[0] == '\0') {
        return;
    }

    const char* baseName = strrchr(script->scriptFullPath, '/');
    if (baseName == NULL) {
        baseName = strrchr(script->scriptFullPath, '\\');
    }
    baseName = (baseName == NULL) ? script->scriptFullPath : baseName + 1;

    snprintf(script->scriptName, sizeof(script->scriptName), "%s", baseName);
    script->scriptName[sizeof(script->scriptName) - 1] = '\0';

    char* extension = strrchr(script->scriptName, '.');
    if (extension != NULL) {
        *extension = '\0';
    }
}

static af_bool_t AF_Script_GetExecutableDir(char* outDir, uint32_t outDirSize) {
    if (outDir == NULL || outDirSize == 0) {
        return AF_FALSE;
    }
    outDir[0] = '\0';

#ifdef _WIN32
    char exePath[AF_MAX_PATH_CHAR_SIZE] = {0};
    if (GetModuleFileNameA(NULL, exePath, sizeof(exePath)) == 0) {
        return AF_FALSE;
    }
    char* lastSlash = strrchr(exePath, '\\');
    if (lastSlash == NULL) {
        lastSlash = strrchr(exePath, '/');
    }
    if (lastSlash == NULL) {
        return AF_FALSE;
    }
    *lastSlash = '\0';
    snprintf(outDir, outDirSize, "%s", exePath);
    return AF_TRUE;
#elif defined(__APPLE__)
    char exePath[AF_MAX_PATH_CHAR_SIZE] = {0};
    uint32_t size = (uint32_t)sizeof(exePath);
    if (_NSGetExecutablePath(exePath, &size) != 0) {
        return AF_FALSE;
    }

    char* lastSlash = strrchr(exePath, '/');
    if (lastSlash == NULL) {
        return AF_FALSE;
    }
    *lastSlash = '\0';
    snprintf(outDir, outDirSize, "%s", exePath);
    return AF_TRUE;
#else
    char exePath[AF_MAX_PATH_CHAR_SIZE] = {0};
    ssize_t len = readlink("/proc/self/exe", exePath, sizeof(exePath) - 1);
    if (len <= 0) {
        return AF_FALSE;
    }
    exePath[len] = '\0';

    char* lastSlash = strrchr(exePath, '/');
    if (lastSlash == NULL) {
        return AF_FALSE;
    }
    *lastSlash = '\0';
    snprintf(outDir, outDirSize, "%s", exePath);
    return AF_TRUE;
#endif
}

static int AF_Script_BuildPathCandidates(const char* _filePath, const char* _projectRoot, const char* _platformName, char candidates[][AF_MAX_PATH_CHAR_SIZE], int maxCandidates)
{
    if (!_filePath || _filePath[0] == '\0' || maxCandidates <= 0) {
        return 0;
    }

    int count = 0;

    // direct path as provided
    snprintf(candidates[count], AF_MAX_PATH_CHAR_SIZE, "%s", _filePath);
    ++count;

    // exe dir relatives
    char exeDir[AF_MAX_PATH_CHAR_SIZE] = {0};
    if (AF_Script_GetExecutableDir(exeDir, sizeof(exeDir)) == AF_TRUE) {
        if (count < maxCandidates) {
            snprintf(candidates[count], AF_MAX_PATH_CHAR_SIZE, "%s/%s", exeDir, _filePath);
            ++count;
        }
        if (count < maxCandidates) {
            snprintf(candidates[count], AF_MAX_PATH_CHAR_SIZE, "%s/scripts/%s", exeDir, _filePath);
            ++count;
        }
    }

    // current working directory / bin / platform
    char cwd[AF_MAX_PATH_CHAR_SIZE] = {0};
#if defined(_WIN32)
    if (GetCurrentDirectoryA(sizeof(cwd), cwd) > 0) {
#else
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
#endif
        const char* platformCandidates[] = { AF_SCRIPT_PLATFORM_DIR, "Win64", "OSX", "LINUX", "WEB", NULL };
        for (int i = 0; platformCandidates[i] != NULL && count < maxCandidates; ++i) {
            snprintf(candidates[count], AF_MAX_PATH_CHAR_SIZE, "%s/bin/%s/%s", cwd, platformCandidates[i], _filePath);
            ++count;
        }
    }

    // projectRoot, if provided
    if (_projectRoot && _projectRoot[0] != '\0') {
        if (count < maxCandidates) {
            snprintf(candidates[count], AF_MAX_PATH_CHAR_SIZE, "%s/%s", _projectRoot, _filePath);
            ++count;
        }

        const char* projectPlatforms[] = { _platformName && _platformName[0] != '\0' ? _platformName : AF_SCRIPT_PLATFORM_DIR, "Win64", "OSX", "LINUX", "WEB", NULL };
        for (int i = 0; projectPlatforms[i] != NULL && count < maxCandidates; ++i) {
            if (count >= maxCandidates) {
                break;
            }
            snprintf(candidates[count], AF_MAX_PATH_CHAR_SIZE, "%s/bin/%s/%s", _projectRoot, projectPlatforms[i], _filePath);
            ++count;
        }
    }

    return count;
}

#ifdef _WIN32
static const char* AF_Script_GetWinHostSubdir(const char* exeName) {
    if (exeName != NULL && strstr(exeName, "AF_Editor") != NULL) {
        return "editor";
    }
    return "game";
}
#endif

// ===============================================================================
// AF_Script_Load
// Take in a file path and attempt to load the script .o object as a shared object.AF_File_CloseFile
// Return the pointer to the shared object
// ===============================================================================
void* AF_Script_Load(const char* _filePath){
    void* scriptPtr = NULL;
    if(_filePath == NULL || _filePath[0] == '\0'){
        AF_Log_Error("AF_Script_Load: file path is empty\n");
        return NULL;
    }
#ifdef _WIN32
    HMODULE handle = NULL;

    // Try loading with redirected search path (prefers module directory for dependencies)
    handle = LoadLibraryExA(_filePath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

    if (!handle) {
        char candidates[16][AF_MAX_PATH_CHAR_SIZE] = {{0}};
        const char* projectRoot = NULL;
        const char* platformName = AF_SCRIPT_PLATFORM_DIR;
        int candidateCount = AF_Script_BuildPathCandidates(_filePath, projectRoot, platformName, candidates, 16);
        for (int i = 0; i < candidateCount && !handle; ++i) {
            char candidatePath[AF_MAX_PATH_CHAR_SIZE] = {0};
            snprintf(candidatePath, sizeof(candidatePath), "%s", candidates[i]);
            AF_Script_NormalizePath(candidatePath);
            handle = LoadLibraryExA(candidatePath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
            if (handle) {
                AF_Log("AF_Script_Load: Resolved %s -> %s\n", _filePath, candidatePath);
                break;
            }
        }
    }

    // If still no handle, try resolving with common project/bin layout, then SetDllDirectory
    if (!handle) {
        char cwd[AF_MAX_PATH_CHAR_SIZE] = {0};
        if (GetCurrentDirectoryA(sizeof(cwd), cwd) > 0) {
            // Try runtime build editor/game scripts paths
            const char* candidatePlatforms[] = {"Win64", "OSX", "LINUX", "WEB", NULL};
            for (int i = 0; candidatePlatforms[i] != NULL && !handle; ++i) {
                char candidatePath[AF_MAX_PATH_CHAR_SIZE] = {0};
                snprintf(candidatePath, sizeof(candidatePath), "%s/bin/%s/%s", cwd, candidatePlatforms[i], _filePath);
                AF_Script_NormalizePath(candidatePath);
                handle = LoadLibraryExA(candidatePath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
                if (handle) {
                    AF_Log("AF_Script_Load: Resolved %s -> %s\n", _filePath, candidatePath);
                }
            }
        }
    }

    // If still no handle, try setting DLL directory to the script's folder and reloading.
    if (!handle) {
        char scriptDir[AF_MAX_PATH_CHAR_SIZE] = {0};
        const char* lastSlash = strrchr(_filePath, '\\');
        if (!lastSlash) {
            lastSlash = strrchr(_filePath, '/');
        }
        if (lastSlash) {
            size_t dirLen = (size_t)(lastSlash - _filePath);
            if (dirLen > 0 && dirLen < sizeof(scriptDir)) {
                memcpy(scriptDir, _filePath, dirLen);
                scriptDir[dirLen] = '\0';
            }
        }

        if (scriptDir[0] != '\0') {
            if (SetDllDirectoryA(scriptDir)) {
                handle = LoadLibraryExA(_filePath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
                SetDllDirectoryA(NULL);
                if (handle) {
                    AF_Log("AF_Script_Load: Resolved with SetDllDirectory %s\n", _filePath);
                }
            } else {
                AF_Log_Error("AF_Script_Load: SetDllDirectoryA failed for %s (error %lu)\n", scriptDir, (unsigned long)GetLastError());
            }
        }
    }

    if (!handle) {
        AF_Log_Error("AF_Script_Load: LoadLibraryA failed for %s (error %lu)\n", _filePath, (unsigned long)GetLastError());
        return NULL;
    }

    scriptPtr = (void*)handle;
#else
    char candidates[16][AF_MAX_PATH_CHAR_SIZE] = {{0}};
    const char* projectRoot = NULL; // optional, could be exposed from _appData in future
    const char* platformName = AF_SCRIPT_PLATFORM_DIR;
    int candidateCount = AF_Script_BuildPathCandidates(_filePath, projectRoot, platformName, candidates, 16);

    for (int i = 0; i < candidateCount; ++i) {
        const char* candidate = candidates[i];
        if (!candidate || candidate[0] == '\0') {
            continue;
        }

        if (AF_File_FileExists(candidate) != AF_TRUE) {
            continue;
        }

#ifdef __EMSCRIPTEN__
            scriptPtr = dlopen(candidate, RTLD_LOCAL | RTLD_LAZY);
            if (scriptPtr) {
                AF_Log("AF_Script_Load: Web resolved %s -> %s\n", _filePath, candidate);
                break;
            }
            AF_Log_Error("AF_Script_Load: Web failed to open shared object %s: %s\n", candidate, dlerror());
#else
            scriptPtr = dlopen(candidate, RTLD_LOCAL | RTLD_LAZY);
            if (scriptPtr) {
                AF_Log("AF_Script_Load: Resolved %s -> %s\n", _filePath, candidate);
                break;
            }
            AF_Log_Error("AF_Script_Load: Failed to open shared object %s: %s\n", candidate, dlerror());
#endif
    }

    if (!scriptPtr) {
        AF_Log_Error("AF_Script_Load: All POSIX candidates failed for %s\n", _filePath);
    }
#endif

    return scriptPtr;
}


// ===============================================================================
// AF_Script_Bind_Functions
// Take in a script component, and the loaded script shared object
// Bind the script shared objects start, update, and destroy function points to the component
// ===============================================================================
uint32_t AF_Script_Bind_Functions(AF_CScript* _script, void* _scriptSharedObjPtr){
    if(_script == NULL || _scriptSharedObjPtr == NULL){
        AF_Log_Error("AF_Script_Bind_Functions: passed a null reference\n");
        return AF_FAIL;
    }

    char startFuncName[MAX_FUNCTION_NAME];
    char updateFuncName[MAX_FUNCTION_NAME];
    char lateUpdateFuncName[MAX_FUNCTION_NAME];
    char destroyFuncName[MAX_FUNCTION_NAME];
    snprintf(startFuncName,      MAX_FUNCTION_NAME, "Start_%s",      _script->scriptName);
    snprintf(updateFuncName,     MAX_FUNCTION_NAME, "Update_%s",     _script->scriptName);
    snprintf(lateUpdateFuncName, MAX_FUNCTION_NAME, "LateUpdate_%s", _script->scriptName);
    snprintf(destroyFuncName,    MAX_FUNCTION_NAME, "Destroy_%s",    _script->scriptName);

#ifdef _WIN32
    HMODULE handle = (HMODULE)_scriptSharedObjPtr;
    _script->startFuncPtr      = (ScriptFuncPtr)GetProcAddress(handle, startFuncName);
    _script->updateFuncPtr     = (ScriptFuncPtr)GetProcAddress(handle, updateFuncName);
    _script->lateUpdateFuncPtr = (ScriptFuncPtr)GetProcAddress(handle, lateUpdateFuncName);
    _script->destroyFuncPtr    = (ScriptFuncPtr)GetProcAddress(handle, destroyFuncName);
    if (!_script->startFuncPtr || !_script->updateFuncPtr || !_script->lateUpdateFuncPtr || !_script->destroyFuncPtr) {
        AF_Log_Error("AF_Script_Bind_Functions: GetProcAddress failed for %s (error %lu)\n", _script->scriptName, (unsigned long)GetLastError());
        return AF_FAIL;
    }
#else
    dlerror(); // clear any existing error
    _script->startFuncPtr = (ScriptFuncPtr)dlsym(_scriptSharedObjPtr, startFuncName);
    if (dlerror() != NULL) { AF_Log_Error("AF_Script_Bind_Functions: dlsym failed for %s\n", startFuncName); return AF_FAIL; }
    _script->updateFuncPtr = (ScriptFuncPtr)dlsym(_scriptSharedObjPtr, updateFuncName);
    if (dlerror() != NULL) { AF_Log_Error("AF_Script_Bind_Functions: dlsym failed for %s\n", updateFuncName); return AF_FAIL; }
    _script->lateUpdateFuncPtr = (ScriptFuncPtr)dlsym(_scriptSharedObjPtr, lateUpdateFuncName);
    if (dlerror() != NULL) { AF_Log_Error("AF_Script_Bind_Functions: dlsym failed for %s\n", lateUpdateFuncName); return AF_FAIL; }
    _script->destroyFuncPtr = (ScriptFuncPtr)dlsym(_scriptSharedObjPtr, destroyFuncName);
    if (dlerror() != NULL) { AF_Log_Error("AF_Script_Bind_Functions: dlsym failed for %s\n", destroyFuncName); return AF_FAIL; }
#endif

    return AF_SUCCESS;
}


// ===============================================================================
// AF_Script_Bind_Functions_To_Components
// Take in all the entities, and and loop through 
// If an entity has a script component with a valid path, then load the script, and then bind
// binding the start, update and destroy function ptrs
// ===============================================================================
void AF_Script_Load_And_Bind_Functions(AF_AppData* _appData){
    if (_appData == NULL) {
        AF_Log_Error("AF_Script_Load_And_Bind_Functions: _appData is NULL\n");
        return;
    }

#ifdef __EMSCRIPTEN__
    AF_Log_Warning("AF_Script_Load_And_Bind_Functions: Web build binding scripts from main module symbols.\n");
#endif

    AF_ECS* _ecs = &_appData->ecs;
    const char* projectRoot = (_appData->projectData.projectRoot[0] != '\0') ? _appData->projectData.projectRoot : NULL;
    const char* platformName = "";
    if (_appData->projectData.platformData.platformType >= 0 && _appData->projectData.platformData.platformType < AF_PLATFORM_COUNT) {
        platformName = AF_Platform_Mappings[_appData->projectData.platformData.platformType].name;
    }

    char exeDir[AF_MAX_PATH_CHAR_SIZE] = {0};
    char exeName[AF_MAX_PATH_CHAR_SIZE] = {0};
    const char* scriptExt = AF_Script_GetBinaryExtension();
#ifdef _WIN32
    const char* winHostSubdir = "game";
#endif

#ifdef _WIN32
    char exePath[AF_MAX_PATH_CHAR_SIZE] = {0};
    if (GetModuleFileNameA(NULL, exePath, sizeof(exePath)) > 0) {
        const char* fileName = strrchr(exePath, '\\');
        if (!fileName) {
            fileName = strrchr(exePath, '/');
        }
        if (fileName != NULL) {
            snprintf(exeName, sizeof(exeName), "%s", fileName + 1);
            exeName[sizeof(exeName) - 1] = '\0';
        }

        char *lastSlash = strrchr(exePath, '\\');
        if (!lastSlash) {
            lastSlash = strrchr(exePath, '/');
        }
        if (lastSlash) {
            *lastSlash = '\0';
            snprintf(exeDir, sizeof(exeDir), "%s", exePath);
        }
    }
    winHostSubdir = AF_Script_GetWinHostSubdir(exeName);
#endif

#if !defined(_WIN32)
    AF_Script_GetExecutableDir(exeDir, sizeof(exeDir));
#endif

    for(uint32_t i = 0; i < _ecs->entitiesCount; i++){
        for(uint32_t j = 0; j < AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY; j++){
            uint32_t scriptID = (i * AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY) + j;
            AF_CScript* script = &_ecs->scripts[scriptID];

            // Reset script function pointers and module handle before (re)loading.
            script->startFuncPtr      = NULL;
            script->updateFuncPtr     = NULL;
            script->lateUpdateFuncPtr = NULL;
            script->destroyFuncPtr    = NULL;
            script->loadedScriptPtr   = NULL;

            if(AF_Component_GetHasEnabled(script->enabled) == AF_FALSE){
                continue;
            }

            AF_Script_DeriveNameFromPath(script);

#ifdef __EMSCRIPTEN__
            if (script->scriptName[0] == '\0') {
                AF_Log_Error("AF_Script_Load_And_Bind_Functions: Web script has no scriptName (Entity: %u, Script: %u)\n", i, scriptID);
                continue;
            }

            char startFuncName[MAX_FUNCTION_NAME] = {0};
            char updateFuncName[MAX_FUNCTION_NAME] = {0};
            char lateUpdateFuncName[MAX_FUNCTION_NAME] = {0};
            char destroyFuncName[MAX_FUNCTION_NAME] = {0};
            snprintf(startFuncName,      MAX_FUNCTION_NAME, "Start_%s",      script->scriptName);
            snprintf(updateFuncName,     MAX_FUNCTION_NAME, "Update_%s",     script->scriptName);
            snprintf(lateUpdateFuncName, MAX_FUNCTION_NAME, "LateUpdate_%s", script->scriptName);
            snprintf(destroyFuncName,    MAX_FUNCTION_NAME, "Destroy_%s",    script->scriptName);

            dlerror();
            script->startFuncPtr = (ScriptFuncPtr)dlsym(RTLD_DEFAULT, startFuncName);
            script->updateFuncPtr = (ScriptFuncPtr)dlsym(RTLD_DEFAULT, updateFuncName);
            script->lateUpdateFuncPtr = (ScriptFuncPtr)dlsym(RTLD_DEFAULT, lateUpdateFuncName);
            script->destroyFuncPtr = (ScriptFuncPtr)dlsym(RTLD_DEFAULT, destroyFuncName);

            if (script->startFuncPtr != NULL &&
                script->updateFuncPtr != NULL &&
                script->lateUpdateFuncPtr != NULL &&
                script->destroyFuncPtr != NULL) {
                // Non-null sentinel so call paths treat the script as loaded.
                script->loadedScriptPtr = (void*)script;
                AF_Log("AF_Script_Load_And_Bind_Functions: Web bound static script '%s' (Entity: %u, Script: %u)\n", script->scriptName, i, scriptID);
                continue;
            }

            AF_Log_Warning("AF_Script_Load_And_Bind_Functions: Web static symbols not found for '%s'; trying dynamic module path\n", script->scriptName);
                script->startFuncPtr      = NULL;
                script->updateFuncPtr     = NULL;
                script->lateUpdateFuncPtr = NULL;
                script->destroyFuncPtr    = NULL;
                script->loadedScriptPtr   = NULL;
#endif

            af_bool_t scriptPathResolved = AF_FALSE;

            // Prefer an existing user-provided script path if valid
            if (script->scriptFullPath[0] != '\0' && AF_File_FileExists(script->scriptFullPath) == AF_TRUE) {
                scriptPathResolved = AF_TRUE;
            }

            // Try project root paths when running inside the editor or from a workspace root.
            if (scriptPathResolved == AF_FALSE && projectRoot != NULL && script->scriptFullPath[0] != '\0') {
                char candidate[AF_MAX_PATH_CHAR_SIZE] = {0};
                snprintf(candidate, sizeof(candidate), "%s/%s", projectRoot, script->scriptFullPath);
                AF_Script_NormalizePath(candidate);
                if (AF_File_FileExists(candidate) == AF_TRUE) {
                    snprintf(script->scriptFullPath, AF_MAX_PATH_CHAR_SIZE, "%s", candidate);
                    scriptPathResolved = AF_TRUE;
                }
            }

            if (scriptPathResolved == AF_FALSE && projectRoot != NULL && script->scriptFullPath[0] != '\0') {
                const char* searchPlatforms[] = {platformName, "Win64", "OSX", "LINUX", "WEB", NULL};
                for (int p = 0; searchPlatforms[p] != NULL && scriptPathResolved == AF_FALSE; ++p) {
                    const char* platformToTry = searchPlatforms[p];
                    if (platformToTry == NULL || platformToTry[0] == '\0') {
                        continue;
                    }

                    char candidate[AF_MAX_PATH_CHAR_SIZE] = {0};
                    snprintf(candidate, sizeof(candidate), "%s/bin/%s/%s", projectRoot, platformToTry, script->scriptFullPath);
                    AF_Script_NormalizePath(candidate);
                    if (AF_File_FileExists(candidate) == AF_TRUE) {
                        snprintf(script->scriptFullPath, AF_MAX_PATH_CHAR_SIZE, "%s", candidate);
                        scriptPathResolved = AF_TRUE;
                        AF_Log("AF_Script_Load_And_Bind_Functions: Resolved %s -> %s via project bin/%s\n", script->scriptName, script->scriptFullPath, platformToTry);
                    }
                }
            }

            if (scriptPathResolved == AF_FALSE && script->scriptName[0] != '\0') {
                char candidate[AF_MAX_PATH_CHAR_SIZE] = {0};

#ifdef _WIN32
                if (exeDir[0] != '\0') {
                    snprintf(candidate, sizeof(candidate), "%s/scripts/%s/%s%s", exeDir, winHostSubdir, script->scriptName, scriptExt);
                    AF_Script_NormalizePath(candidate);
                    if (AF_File_FileExists(candidate) == AF_TRUE) {
                        snprintf(script->scriptFullPath, AF_MAX_PATH_CHAR_SIZE, "%s", candidate);
                        scriptPathResolved = AF_TRUE;
                    }
                }

                if (scriptPathResolved == AF_FALSE && exeDir[0] != '\0') {
                    snprintf(candidate, sizeof(candidate), "%s/scripts/%s%s", exeDir, script->scriptName, scriptExt);
                    AF_Script_NormalizePath(candidate);
                    if (AF_File_FileExists(candidate) == AF_TRUE) {
                        snprintf(script->scriptFullPath, AF_MAX_PATH_CHAR_SIZE, "%s", candidate);
                        scriptPathResolved = AF_TRUE;
                    }
                }

                if (scriptPathResolved == AF_FALSE) {
                    snprintf(candidate, sizeof(candidate), "scripts/%s/%s%s", winHostSubdir, script->scriptName, scriptExt);
                    AF_Script_NormalizePath(candidate);
                    if (AF_File_FileExists(candidate) == AF_TRUE) {
                        snprintf(script->scriptFullPath, AF_MAX_PATH_CHAR_SIZE, "%s", candidate);
                        scriptPathResolved = AF_TRUE;
                    }
                }

                if (scriptPathResolved == AF_FALSE) {
                    snprintf(candidate, sizeof(candidate), "scripts/%s%s", script->scriptName, scriptExt);
                    AF_Script_NormalizePath(candidate);
                    if (AF_File_FileExists(candidate) == AF_TRUE) {
                        snprintf(script->scriptFullPath, AF_MAX_PATH_CHAR_SIZE, "%s", candidate);
                        scriptPathResolved = AF_TRUE;
                    }
                }
#else
                if (exeDir[0] != '\0') {
                    snprintf(candidate, sizeof(candidate), "%s/scripts/%s%s", exeDir, script->scriptName, scriptExt);
                    if (AF_File_FileExists(candidate) == AF_TRUE) {
                        snprintf(script->scriptFullPath, AF_MAX_PATH_CHAR_SIZE, "%s", candidate);
                        scriptPathResolved = AF_TRUE;
                    }
                }

                if (scriptPathResolved == AF_FALSE) {
                    snprintf(candidate, sizeof(candidate), "scripts/%s%s", script->scriptName, scriptExt);
                    if (AF_File_FileExists(candidate) == AF_TRUE) {
                        snprintf(script->scriptFullPath, AF_MAX_PATH_CHAR_SIZE, "%s", candidate);
                        scriptPathResolved = AF_TRUE;
                    }
                }
#endif
            }

            if (scriptPathResolved == AF_FALSE && script->scriptName[0] != '\0') {
#ifdef _WIN32
                snprintf(script->scriptFullPath, AF_MAX_PATH_CHAR_SIZE, "scripts/%s/%s%s", winHostSubdir, script->scriptName, scriptExt);
#else
                snprintf(script->scriptFullPath, AF_MAX_PATH_CHAR_SIZE, "scripts/%s%s", script->scriptName, scriptExt);
#endif
            }

            // attempt to load the script
            AF_Log("AF_Script_Load_And_Bind_Functions: loading script: %s (Entity: %u, Script: %u)\n", script->scriptFullPath, i, scriptID);
            script->loadedScriptPtr = AF_Script_Load(script->scriptFullPath);

            if (!script->loadedScriptPtr) {
                AF_Log_Error("AF_Script_Load_And_Bind_Functions: failed to load script: %s (Entity: %u, Script: %u)\n", script->scriptFullPath, i, scriptID);
                script->startFuncPtr      = NULL;
                script->updateFuncPtr     = NULL;
                script->lateUpdateFuncPtr = NULL;
                script->destroyFuncPtr    = NULL;
                script->loadedScriptPtr   = NULL;
                continue;
            }

            // attempt the bind the scripts functions to this component
            uint32_t scriptBindSuccess = AF_Script_Bind_Functions(script, script->loadedScriptPtr);
            if(scriptBindSuccess == AF_FAIL){
                AF_Log_Error("AF_Script_Load_And_Bind_Functions: failed to bind script: Entity: %u, Script: %u\n", i, scriptID);
                AF_Script_UnLoad(script->loadedScriptPtr);
                script->startFuncPtr      = NULL;
                script->updateFuncPtr     = NULL;
                script->lateUpdateFuncPtr = NULL;
                script->destroyFuncPtr    = NULL;
                script->loadedScriptPtr   = NULL;
            }
        }
    }
    AF_Log("AF_Script_Load_And_Bind_Functions: Loaded and bound all scripts, Finished \n");
}

// ===============================================================================
// AF_Script_UnLoad
// Unload all the loaded script objects
// Return the pointer to the shared object
// ===============================================================================
void AF_Script_UnLoad(void* _scriptSharedObjPtr){
    
    if(_scriptSharedObjPtr == NULL){
        //AF_Log_Error("AF_Script_UnLoad: Failed to unload scriptSharedObjPtr due to passing null reference\n");
        return;
    }

#ifdef _WIN32
    FreeLibrary((HMODULE)_scriptSharedObjPtr);
#elif defined(__EMSCRIPTEN__)
    // No-op on Web build for dynamic module unloading. Side module loading is not supported in this path.
    (void)_scriptSharedObjPtr;
#else
    int eret = dlclose(_scriptSharedObjPtr);
    if (eret != 0) {
        AF_Log_Error("AF_Script_UnLoad: Failed to close shared object: %s\n", dlerror());
    }
#endif
}


// ===============================================================================
// AF_Script_UnloadScripts
// Take in all the entities, and and loop through 
// If an entity has a script component with a valid path, then unload the script, 
// ===============================================================================
void AF_Script_UnloadScripts(AF_ECS* _ecs){
    for(uint32_t i = 0; i < _ecs->entitiesCount; i++){
        for(uint32_t j = 0; j < AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY; j++){
            uint32_t scriptID = (i * AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY)  +j;
            AF_CScript* script = &_ecs->scripts[scriptID];

            // Unload regardless of enabled state to avoid stale handles/pointers.
            if (script->loadedScriptPtr != NULL) {
                AF_Script_UnLoad(script->loadedScriptPtr);
            }

            script->loadedScriptPtr   = NULL;
            script->startFuncPtr      = NULL;
            script->updateFuncPtr     = NULL;
            script->lateUpdateFuncPtr = NULL;
            script->destroyFuncPtr    = NULL;
            // Keep scriptEditorVarCount so editor-set values are preserved across play/unload.
            // Keep scriptName/scriptFullPath as is; useful for reload path calculation.
        }
    }
    AF_Log("AF_Script_UnloadScripts: Unload Finished \n");
}


// ===============================================================================
// AF_GetScriptFuncPtr
// Take in a script func name, and loaded shared object ptr, and attempt to find the function in the .o object as a shared object.
// Return the pointer to the shared object
// ===============================================================================
ScriptFuncPtr AF_GetScriptFuncPtr(void* _sharedObjectPtr, const char* _funcName){
    // Load functions from the shared objects
    ScriptFuncPtr scriptFunctPtr1 = NULL;
#ifdef _WIN32
    (void)_funcName; // Suppress unused variable warning
    (void)_sharedObjectPtr; // Suppress unused variable warning
    AF_Log_Error("AF_GetScriptFuncPtr: Windows not defined\n");
#else
    scriptFunctPtr1 = (ScriptFuncPtr) dlsym(_sharedObjectPtr, _funcName);
    char *error = dlerror(); // Check for any error after dlsym
    if (error != NULL) {
        AF_Log_Error("Game_App_Awake: Failed to load %s: %s\n", _funcName, error);
    }
#endif
    return scriptFunctPtr1;
}


// ===============================================================================
// AF_Script_Call_Start
// Loop through all script components that have valid script func pointers and call start
// ===============================================================================
void AF_Script_Call_Start(AF_AppData* _appData){
	if (_appData == NULL) {
		AF_Log_Error("AF_Script_Call_Start: _appData is NULL\n");
		return;
	}
	AF_ECS* ecs = &_appData->ecs;
    for(uint32_t i = 0; i < _appData->ecs.entitiesCount; i++){
        
        
        for(uint32_t j = 0; j < AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY; j++){
            uint32_t scriptID = (i * AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY)  +j;
            AF_CScript* script = &ecs->scripts[scriptID];// entity->scripts[j];
            if(AF_Component_GetHasEnabled(script->enabled) == AF_FALSE){
                continue;
            }
            
            if(script->loadedScriptPtr == NULL){
                continue;
            }

            if(script->startFuncPtr == NULL){
                //AF_Log_Error("AF_CallScriptStart: script startFuncPtr is null. Forgot to set it\n");
                continue;
            }

            // Apply editor var values to the DLL's globals before Start runs
            AF_Log("AF_Script_Call_Start: calling script Start for entity %u script '%s' (id %u)\n", i, script->scriptName, scriptID);
            AF_Script_ApplyEditorVars(script);

            // Call the function
            // Cast to special func ptr
            ScriptFuncPtr scriptFunctPtr = (ScriptFuncPtr)script->startFuncPtr;
            // Call it safely to prevent a broken script from crashing the host.
#if defined(_MSC_VER) && !defined(__clang__)
            __try {
                scriptFunctPtr(i, _appData);
            } __except(EXCEPTION_EXECUTE_HANDLER) {
                AF_Log_Error("AF_Script_Call_Start: script '%s' crashed in Start for entity %u\n", script->scriptName, i);
            }
#else
            scriptFunctPtr(i, _appData);
#endif
            AF_Log("AF_Script_Call_Start: script '%s' started successfully for entity %u script slot %u\n", script->scriptName, i, j);
        }
    }
}

// ===============================================================================
// AF_Script_Call_Update
// Loop through all script components that have valid script func pointers and call Update
// ===============================================================================
void AF_Script_Call_Update(AF_AppData* _appData){
	if (_appData == NULL) {
		return;
	}
	AF_ECS* ecs = &_appData->ecs;
    for(uint32_t i = 0; i < _appData->ecs.entitiesCount; i++){
        // Run all the scripts
        for(uint32_t j = 0; j < AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY; j++){
            uint32_t scriptID = (i * AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY)  +j;
            AF_CScript* script = &ecs->scripts[scriptID];
            if(AF_Component_GetHasEnabled(script->enabled) == AF_FALSE){
                continue;
            }
        
            if(script->loadedScriptPtr == NULL){
                continue;
            }

            if(script->updateFuncPtr == NULL){
                //AF_Log_Error("AF_CallScriptUpdate: script updateFuncPtr is null. Forgot to set it\n");
                continue;
            }

            // Call the function
            // Cast to special func ptr
            ScriptFuncPtr scriptFunctPtr = (ScriptFuncPtr)script->updateFuncPtr;
#if defined(_MSC_VER) && !defined(__clang__)
            __try {
                scriptFunctPtr(i, _appData);
            } __except(EXCEPTION_EXECUTE_HANDLER) {
                AF_Log_Error("AF_Script_Call_Update: script '%s' crashed in Update for entity %u\n", script->scriptName, i);
            }
#else
            scriptFunctPtr(i, _appData);
#endif
        }
    }
}

// ===============================================================================
// AF_Script_Call_LateUpdate
// Loop through all script components that have valid script func pointers and call Update
// ===============================================================================
void AF_Script_Call_LateUpdate(AF_AppData* _appData){
	if (_appData == NULL) {
		return;
	}
	AF_ECS* ecs = &_appData->ecs;
    for(uint32_t i = 0; i < _appData->ecs.entitiesCount; i++){
        // Run all the scripts
        for(uint32_t j = 0; j < AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY; j++){
            uint32_t scriptID = (i * AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY)  +j;
            AF_CScript* script = &ecs->scripts[scriptID];
            if(AF_Component_GetHasEnabled(script->enabled) == AF_FALSE){
                continue;
            }
        
            if(script->loadedScriptPtr == NULL){
                continue;
            }

            if(script->lateUpdateFuncPtr == NULL){
                //AF_Log_Error("AF_CallScriptUpdate: script updateFuncPtr is null. Forgot to set it\n");
                continue;
            }

            // Call the function
            // Cast to special func ptr
            ScriptFuncPtr scriptFunctPtr = (ScriptFuncPtr)script->lateUpdateFuncPtr;
#if defined(_MSC_VER) && !defined(__clang__)
            __try {
                scriptFunctPtr(i, _appData);
            } __except(EXCEPTION_EXECUTE_HANDLER) {
                AF_Log_Error("AF_Script_Call_LateUpdate: script '%s' crashed in LateUpdate for entity %u\n", script->scriptName, i);
            }
#else
            scriptFunctPtr(i, _appData);
#endif
        }
    }
}


// ===============================================================================
// AF_Script_Call_Destroy
// Loop through all script components that have valid script func pointers and call Destroy
// ===============================================================================
void AF_Script_Call_Destroy(AF_AppData* _appData){
	if (_appData == NULL) {
		return;
	}
	AF_ECS* ecs = &_appData->ecs;
    for(uint32_t i = 0; i < _appData->ecs.entitiesCount; i++){
        for(uint32_t j = 0; j < AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY; j++){
            uint32_t scriptID = (i * AF_ENTITY_TOTAL_SCRIPTS_PER_ENTITY)  +j;
            AF_CScript* script = &ecs->scripts[scriptID];
            if(AF_Component_GetHasEnabled(script->enabled) == AF_FALSE){
                continue;
            }
    
            if(script->loadedScriptPtr == NULL){
                continue;
            }

            if(script->destroyFuncPtr == NULL){
                //AF_Log_Error("AF_CallScriptDestroy: script destroyFuncPtr is null. Forgot to set it\n");
                continue;
            }

            // Call the function
            // Cast to special func ptr
            ScriptFuncPtr scriptFunctPtr = (ScriptFuncPtr)script->destroyFuncPtr;
#if defined(_MSC_VER) && !defined(__clang__)
            __try {
                scriptFunctPtr(i, _appData);
            } __except(EXCEPTION_EXECUTE_HANDLER) {
                AF_Log_Error("AF_Script_Call_Destroy: script '%s' crashed in Destroy for entity %u\n", script->scriptName, i);
            }
#else
            scriptFunctPtr(i, _appData);
#endif
        }
    }
}

// ===============================================================================
// AF_Script_ApplyEditorVars
// Write the values stored in scriptEditorVarData back into the loaded DLL/SO
// globals before Start() is called, so scripts see editor-set values.
// ===============================================================================
void AF_Script_ApplyEditorVars(AF_CScript* _script) {
    if (_script == NULL || _script->loadedScriptPtr == NULL) {
        return;
    }

    uint32_t varCount = _script->scriptEditorVarCount;
    if (varCount > MAX_EDITOR_VARS_PER_SCRIPT) {
        AF_Log_Error("AF_Script_ApplyEditorVars: clip scriptEditorVarCount from %u to %u for script %s\n", varCount, MAX_EDITOR_VARS_PER_SCRIPT, _script->scriptName);
        varCount = MAX_EDITOR_VARS_PER_SCRIPT;
    }

    for (uint32_t i = 0; i < varCount; i++) {
        AF_PropertyMetaData_s* var = &_script->scriptEditorVarData[i];
        void* sym = NULL;

#ifdef _WIN32
        HMODULE handle = (HMODULE)_script->loadedScriptPtr;
        sym = (void*)GetProcAddress(handle, var->name);
#else
        sym = dlsym(_script->loadedScriptPtr, var->name);
#endif
        if (sym == NULL) {
            AF_Log_Warning("AF_Script_ApplyEditorVars: Symbol '%s' not found in script '%s'. This is expected for non-exported or removed editor vars.\n", var->name, _script->scriptName);
            continue;
        }

        switch (var->type) {
            case AF_EDITOR_VAR_TYPE_INT:
                *(uint32_t*)sym = var->data.intValue;
            break;
            case AF_EDITOR_VAR_TYPE_FLOAT:
                *(AF_FLOAT*)sym = var->data.floatValue;
            break;
            case AF_EDITOR_VAR_TYPE_BOOL:
                *(af_bool_t*)sym = var->data.boolValue;
            break;
            case AF_EDITOR_VAR_TYPE_STRING:
                snprintf((char*)sym, AF_MAX_PATH_CHAR_SIZE, "%s", var->data.strValue);
            break;
            case AF_EDITOR_VAR_TYPE_VEC2:
                ((AF_FLOAT*)sym)[0] = var->data.vec2Value[0];
                ((AF_FLOAT*)sym)[1] = var->data.vec2Value[1];
            break;
            case AF_EDITOR_VAR_TYPE_VEC3:
                ((AF_FLOAT*)sym)[0] = var->data.vec3Value[0];
                ((AF_FLOAT*)sym)[1] = var->data.vec3Value[1];
                ((AF_FLOAT*)sym)[2] = var->data.vec3Value[2];
            break;
            case AF_EDITOR_VAR_TYPE_EVENT:
            {
                uint32_t eventValue = var->data.eventTypeValue;
                AF_Event_Type_e mappedEventType = AF_EVENT_TYPE_NONE;

                // Editor combo stores compact indices; map them to runtime event enums.
                switch (eventValue) {
                    case 0: mappedEventType = AF_EVENT_TYPE_NONE; break;
                    case 1: mappedEventType = AF_EVENT_TYPE_GAME_WIN; break;
                    case 2: mappedEventType = AF_EVENT_TYPE_GAME_LOSE; break;
                    default:
                        if (eventValue < AF_EVENT_TYPE_TOTAL_TYPES) {
                            mappedEventType = (AF_Event_Type_e)eventValue;
                        } else {
                            AF_Log_Warning("AF_Script_ApplyEditorVars: invalid event value %u for script '%s' var '%s', defaulting to NONE\n", eventValue, _script->scriptName, var->name);
                        }
                    break;
                }

                assert(mappedEventType >= AF_EVENT_TYPE_NONE && mappedEventType < AF_EVENT_TYPE_TOTAL_TYPES);

                AF_Log("AF_Script_ApplyEditorVars: Setting event var '%s' in script '%s' from editor value %u to mapped enum %u\n", var->name, _script->scriptName, eventValue, mappedEventType);
                *(uint32_t*)sym = (uint32_t)mappedEventType;
            }
            break;
            default: break;
        }
    }
}

// ===============================================================================
// AF_Script_SerialiseEditorVars
// serialise script editor vars
// ===============================================================================
void AF_Script_SerialiseEditorVars(const char *_scriptPath, AF_CScript *_scriptComponent)
{       
    uint32_t scriptSize = AF_File_GetFileSize(_scriptPath);
    if(scriptSize == 0){
        AF_Log_Error("AF_Script_SerialiseEditorVars: Failed to get script file size for path: %s\n", _scriptPath);
        return;
    }
    // fileopen the script.
    uint32_t scriptBufferSize = scriptSize*sizeof(char) + 1;
    char* scriptBuffer = malloc(scriptBufferSize);
    if(scriptBuffer == NULL){
        AF_Log_Error("AF_Script_SerialiseEditorVars: Failed to allocate memory for script buffer\n");
        return;
    }
    af_bool_t readSuccess = AF_File_ReadFile(scriptBuffer, scriptBufferSize, _scriptPath, "r");

    // search the script line by line for EDITOR_VAR tags.
    if(readSuccess == AF_FALSE){
        AF_Log_Error("AF_Script_SerialiseEditorVars: Failed to read script file at path: %s\n", _scriptPath);
        free(scriptBuffer);
        scriptBuffer = NULL;
        return;
    }

    // Capture existing values from JSON before we scan the file for the layout
    AF_PropertyMetaData_s existingVars[MAX_EDITOR_VARS_PER_SCRIPT];
    uint32_t existingVarCount = _scriptComponent->scriptEditorVarCount;
    memcpy(existingVars, _scriptComponent->scriptEditorVarData, sizeof(AF_PropertyMetaData_s) * existingVarCount);

    // Reset the component's var count as we are about to rebuild the list from the file
    _scriptComponent->scriptEditorVarCount = 0;

    // If found Tag, line under EDITOR_VAR is the variable. 
    // space delimiter
    char delimiter[] = " \t\r\n";

    // declare empty string to store token
    char* token = NULL;
    char* tokenContext = NULL;

    // Get the first token
#ifdef _WIN32
    token = strtok_s(scriptBuffer, delimiter, &tokenContext);
#else
    token = AF_StrtokR(scriptBuffer, delimiter, &tokenContext);
#endif

    // continue upto the last token
    while(token != NULL){
            
        // pass null to get next token
    #ifdef _WIN32
        token = strtok_s(NULL, delimiter, &tokenContext);
    #else
        token = AF_StrtokR(NULL, delimiter, &tokenContext);
    #endif
        if (token != NULL && strcmp(token, "AF_EDITOR_VAR") == 0) {
            // Get the NEXT token which should be the type (int, float, etc)
    #ifdef _WIN32
            token = strtok_s(NULL, delimiter, &tokenContext);
    #else
            token = AF_StrtokR(NULL, delimiter, &tokenContext);
    #endif
            if (token) {
                
                AF_EDITOR_VAR_TYPE_e varType = AF_Script_MapStringToEditorVarType(token);
                
                if(varType == AF_EDITOR_VAR_TYPE_TOTAL_TYPES){
                    AF_Log_Error("AF_Script_SerialiseEditorVars: Failed to map var type string to enum for var: %s\n", token);
                    continue;   
                }

                // set the variable type
                _scriptComponent->scriptEditorVarData[_scriptComponent->scriptEditorVarCount].type = varType;

                AF_Log("Type: %s ", token);
#ifdef _WIN32
                token = strtok_s(NULL, delimiter, &tokenContext);
#else
                token = AF_StrtokR(NULL, delimiter, &tokenContext);
#endif
                
                if (token == NULL) {
                    continue;
                }

                // progress the token to get the var name
                AF_Log("Name: %s ", token);
                
                // Track if we found a match in the JSON data
                af_bool_t foundInJson = AF_FALSE;
            for (uint32_t k = 0; k < existingVarCount; k++) {
                    if (strcmp(existingVars[k].name, token) == 0) {
                        // Found matching variable in JSON, copy it over to the new slot
                        _scriptComponent->scriptEditorVarData[_scriptComponent->scriptEditorVarCount] = existingVars[k];
                        foundInJson = AF_TRUE;
                        break;
                    }
                }

                // If not found in JSON, initialise it with the default from the file
                if (!foundInJson) {
                    snprintf(_scriptComponent->scriptEditorVarData[_scriptComponent->scriptEditorVarCount].name, AF_MAX_PATH_CHAR_SIZE, "%s", token);
                    _scriptComponent->scriptEditorVarData[_scriptComponent->scriptEditorVarCount].type = varType;

                    char* nameToken = token;
#ifdef _WIN32
                    token = strtok_s(NULL, delimiter, &tokenContext);
#else
                    token = AF_StrtokR(NULL, delimiter, &tokenContext);
#endif

                    if(token == NULL){
                        AF_Log_Error("AF_Script_SerialiseEditorVars: Failed to get var assignment token for %s\n", nameToken);
                        continue;
                    }

                    // If the token is "=", skip it to get the actual value
                    if (strcmp(token, "=") == 0) {
#ifdef _WIN32
                        token = strtok_s(NULL, delimiter, &tokenContext);
#else
                        token = AF_StrtokR(NULL, delimiter, &tokenContext);
#endif
                        if (token == NULL) {
                            AF_Log_Error("AF_Script_SerialiseEditorVars: Failed to get value after '=' for %s\n", nameToken);
                            continue;
                        }
                    }

                    switch(varType){
                        case AF_EDITOR_VAR_TYPE_INT:
                            _scriptComponent->scriptEditorVarData[_scriptComponent->scriptEditorVarCount].data.intValue = atoi(token);
                        break;

                        case AF_EDITOR_VAR_TYPE_FLOAT:
                            _scriptComponent->scriptEditorVarData[_scriptComponent->scriptEditorVarCount].data.floatValue = atof(token);
                        break;

                        case AF_EDITOR_VAR_TYPE_BOOL:
                            _scriptComponent->scriptEditorVarData[_scriptComponent->scriptEditorVarCount].data.boolValue = (strcmp(token, "true") == 0) ? AF_TRUE : AF_FALSE;
                        break;

                        case AF_EDITOR_VAR_TYPE_STRING:
                            snprintf(_scriptComponent->scriptEditorVarData[_scriptComponent->scriptEditorVarCount].data.strValue, AF_MAX_PATH_CHAR_SIZE, "%s", token);
                        break;

                        case AF_EDITOR_VAR_TYPE_VEC2:
                        {
#ifdef _WIN32
                            char* nextToken = strtok_s(NULL, delimiter, &tokenContext);
#else
                            char* nextToken = AF_StrtokR(NULL, delimiter, &tokenContext);
#endif
                            if (nextToken == NULL) {
                                AF_Log_Error("AF_Script_SerialiseEditorVars: Failed to parse Vec2 second token from: %s\n", token);
                                break;
                            }

                            char vec2Buffer[128];
                            snprintf(vec2Buffer, sizeof(vec2Buffer), "%s %s", token, nextToken);
                            if (AF_SSCANF_S(vec2Buffer, "{%f, %f};",
                                &_scriptComponent->scriptEditorVarData[_scriptComponent->scriptEditorVarCount].data.vec2Value[0],
                                &_scriptComponent->scriptEditorVarData[_scriptComponent->scriptEditorVarCount].data.vec2Value[1]) != 2) {
                                AF_Log_Error("AF_Script_SerialiseEditorVars: Failed to parse Vec2 value from token: %s\n", vec2Buffer);
                            }
                        }
                        break;

                        case AF_EDITOR_VAR_TYPE_VEC3:
                            // Expecting format Vec3(x, y, z)
                            if (AF_SSCANF_S(token, "Vec3(%f,%f,%f)", &_scriptComponent->scriptEditorVarData[_scriptComponent->scriptEditorVarCount].data.vec3Value[0],
                                &_scriptComponent->scriptEditorVarData[_scriptComponent->scriptEditorVarCount].data.vec3Value[1],
                                &_scriptComponent->scriptEditorVarData[_scriptComponent->scriptEditorVarCount].data.vec3Value[2]) != 3) {
                                AF_Log_Error("AF_Script_SerialiseEditorVars: Failed to parse Vec3 value from token: %s\n", token);
                            }
                        break;

                        case AF_EDITOR_VAR_TYPE_EVENT:
                            // Expecting format AF_Event_Type_e(EVENT_TYPE)
                            if (AF_SSCANF_S(token, "AF_Event_Type_e(%d)", (int*)&_scriptComponent->scriptEditorVarData[_scriptComponent->scriptEditorVarCount].data.eventTypeValue) != 1) {
                                AF_Log_Error("AF_Script_SerialiseEditorVars: Failed to parse Event Type value from token: %s\n", token);
                            }
                        break;
                        default: break;
                    }
                }
                
                _scriptComponent->scriptEditorVarCount++;
            }
        }
    }

    free(scriptBuffer);
    scriptBuffer = NULL;
}


AF_EDITOR_VAR_TYPE_e AF_Script_MapStringToEditorVarType(const char* _typeString){
    if(strcmp(_typeString, "uint32_t") == 0 || strcmp(_typeString, "int") == 0){
        return AF_EDITOR_VAR_TYPE_INT;
    } else if(strcmp(_typeString, "float") == 0 || strcmp(_typeString, "AF_FLOAT") == 0){
        return AF_EDITOR_VAR_TYPE_FLOAT;
    }
    else if(strcmp(_typeString, "af_bool_t") == 0 || strcmp(_typeString, "bool") == 0){
        return AF_EDITOR_VAR_TYPE_BOOL;
    }
    else if(strcmp(_typeString, "char*") == 0 || strcmp(_typeString, "char") == 0){
        return AF_EDITOR_VAR_TYPE_STRING;
    }else if(strcmp(_typeString, "Vec3") == 0){
        return AF_EDITOR_VAR_TYPE_VEC3;
    } else if(strcmp(_typeString, "Vec2") == 0){
        return AF_EDITOR_VAR_TYPE_VEC2;
    }
    else if(strcmp(_typeString, "AF_Event_Type_e") == 0){
        return AF_EDITOR_VAR_TYPE_EVENT;
    } 
    else {
        AF_Log_Error("AF_Script_MapStringToEditorVarType: Var Type %s not recognised\n", _typeString);
        return AF_EDITOR_VAR_TYPE_TOTAL_TYPES; // Return an invalid type to indicate error
    }
}
