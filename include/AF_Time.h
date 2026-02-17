// ===============================================================================
// AF_TIME_H
// Definition & Implimentation of helper functions for time
// ===============================================================================
#ifndef AF_TIME_H
#define AF_TIME_H
#include "AF_Lib_Define.h"

#ifdef __cplusplus
extern "C" {    
#endif


// ====================
// AF_Time
// struct to hold the data needed for monitoring time in the game
// ====================
typedef struct {
	// 80byte aligned types first
	uint64_t startTick;			// The time when the application started, in ticks.
	uint64_t lastTick;			// The time at the last frame, in ticks.
	double totalTime;			// Total time since the application started, in seconds.
	double deltaTime;			// Time in seconds since the last frame.
} AF_Time;



// ====================
// AF_GetRawTicks
// Helper function to get the current time in ticks using clock_gettime for better precision
// ====================
uint64_t AF_GetRawTicks(void);

// ====================
// AF_Time_Init
// Initialise the struct variables. 
// Don't need to take in any variables except the current time, 
// pass back a new copy of initialised data in the struct.
// ====================
AF_Time AF_Time_Init(void);

// ====================
// AF_Time_Update
// Update the time variables
// ====================
void AF_Time_Update(AF_Time* _time);

// ====================
// AF_Time_GetTime
// Helper function to get the current time in seconds using clock_gettime for better precision
// ====================
double AF_Time_GetTime(void);



#ifdef __cplusplus
}
#endif

#endif //AF_TIME_H
