#include "AF_Time.h"
#include <assert.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#endif

#if defined(__EMSCRIPTEN__) || defined(AF_WEB_BUILD)
#include <emscripten/emscripten.h>
#endif

#ifdef __APPLE__
#include <mach/mach_time.h>
#endif

// ====================
// AF_Time_Init
// Initialise the struct variables.
// ====================
AF_Time AF_Time_Init(void) {
    AF_Time t = {0};
    t.startTick = AF_GetRawTicks();
    t.lastTick = t.startTick;
    t.totalTime = 0.0;
    t.deltaTime = 0.0;
    return t;
}

// ====================
// AF_Time_Update
// Update the time variables
// ====================
void AF_Time_Update(AF_Time* _time) {
    assert(_time != NULL && "AF_Time_Update: _time is NULL");

    uint64_t currentTick = AF_GetRawTicks();
    uint64_t deltaNS = currentTick - _time->lastTick;
    _time->lastTick = currentTick;

    _time->deltaTime = (double)deltaNS * 1.0e-9;
    _time->totalTime = (double)(currentTick - _time->startTick) * 1.0e-9;
}

// ====================
// AF_GetRawTicks
// Platform-abstracted precise tick counter (nanoseconds)
// ====================
uint64_t AF_GetRawTicks(void) {
#if defined(__EMSCRIPTEN__) || defined(AF_WEB_BUILD)
    double tMs = emscripten_get_now();
    return (uint64_t)(tMs * 1.0e6);
#elif defined(_WIN32)
    LARGE_INTEGER counter;
    LARGE_INTEGER frequency;
    QueryPerformanceCounter(&counter);
    QueryPerformanceFrequency(&frequency);
    return (uint64_t)((counter.QuadPart * 1000000000ULL) / frequency.QuadPart);
#elif defined(__APPLE__)
    static mach_timebase_info_data_t timebase = {0, 0};
    if (timebase.denom == 0) {
        mach_timebase_info(&timebase);
    }
    uint64_t t = mach_absolute_time();
    return (t * timebase.numer) / timebase.denom;
#else
    struct timespec timeSpec;
#if defined(CLOCK_MONOTONIC_RAW)
    clock_gettime(CLOCK_MONOTONIC_RAW, &timeSpec);
#else
    clock_gettime(CLOCK_MONOTONIC, &timeSpec);
#endif
    return (uint64_t)timeSpec.tv_sec * 1000000000ULL + (uint64_t)timeSpec.tv_nsec;
#endif
}

// ====================
// AF_Time_GetTime
// Platform-abstracted current time in seconds
// ====================
double AF_Time_GetTime(void) {
#if defined(__EMSCRIPTEN__) || defined(AF_WEB_BUILD)
    return emscripten_get_now() * 1.0e-3;
#elif defined(_WIN32)
    LARGE_INTEGER counter;
    LARGE_INTEGER frequency;
    QueryPerformanceCounter(&counter);
    QueryPerformanceFrequency(&frequency);
    return (double)counter.QuadPart / (double)frequency.QuadPart;
#else
    struct timespec timeSpec;
    clock_gettime(CLOCK_MONOTONIC, &timeSpec);
    return (double)timeSpec.tv_sec + (double)timeSpec.tv_nsec * 1.0e-9;
#endif
}
