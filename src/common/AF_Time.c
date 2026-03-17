#include "AF_Time.h"
#include <time.h>   // for clock_gettime
#include <assert.h>

#ifdef _WIN32
#include <windows.h>
#endif

// ====================
// AF_Time_Init
// Initialise the struct variables. 
// Don't need to take in any variables except the current time, 
// pass back a new copy of initialised data in the struct.
// ====================
AF_Time AF_Time_Init(void){
	AF_Time t = { .startTick = 0};						// Zero-initialize the struct
	t.startTick = AF_GetRawTicks();
	t.lastTick = t.startTick;
	return t;
}

// ====================
// AF_Time_Update
// Update the time variables
// ====================
void AF_Time_Update(AF_Time* _time){
    assert(_time != NULL && "AF_Time_Update: _time is NULL");

	uint64_t currentTick = AF_GetRawTicks(); 			// Get current time in tick

	// Integer math for precise delta in nanoseconds
	uint64_t deltaNS = currentTick - _time->lastTick;	// Delta time in nanoseconds
	_time->lastTick = currentTick;						// Update last tick to current tick

	// convert to seconds only when needed for game logic
	_time->deltaTime = (double)deltaNS * 1.0e-9;		// Convert delta to seconds
	_time->totalTime = (double)(currentTick - _time->startTick) * 1.0e-9; // Total time in seconds
}

// ====================
// AF_GetRawTicks
// Helper function to get the current time in ticks using clock_gettime for better precision
// ====================
uint64_t AF_GetRawTicks(void){
	#ifdef _WIN32
	LARGE_INTEGER counter;
	LARGE_INTEGER frequency;
	QueryPerformanceCounter(&counter);
	QueryPerformanceFrequency(&frequency);
	return (uint64_t)((counter.QuadPart * 1000000000ULL) / frequency.QuadPart);
	#else
	struct timespec timeSpec;
    // use CLOCK_MONOTONIC_RAW for the most precise timer available, unaffected by NTP adjustments
	clock_gettime(CLOCK_MONOTONIC_RAW, &timeSpec);
	return (uint64_t)timeSpec.tv_sec * 1000000000ULL + (uint64_t)timeSpec.tv_nsec;
	#endif
}	

// ====================
// AF_Time_GetTime
// Helper function to get the current time in seconds using clock_gettime for better precision
// ====================
double AF_Time_GetTime(void){
	#ifdef _WIN32
	LARGE_INTEGER counter;
	LARGE_INTEGER frequency;
	QueryPerformanceCounter(&counter);
	QueryPerformanceFrequency(&frequency);
	return (double)counter.QuadPart / (double)frequency.QuadPart;
	#else
	struct timespec timeSpec;
	clock_gettime(CLOCK_MONOTONIC, &timeSpec);
	return (double)(timeSpec.tv_sec) + (double)(timeSpec.tv_nsec) * 1.0e-9; // multiply nanoseconds by 1e-9 to convert to seconds
	#endif
}
