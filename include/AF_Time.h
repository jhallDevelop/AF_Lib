/*
===============================================================================
AF_TIME_H

Implimentation of helper functions for time
Definition of the AF_Time struct
===============================================================================
*/
#ifndef AF_TIME_H
#define AF_TIME_H
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include "AF_Lib_Define.h"
#ifdef __cplusplus
extern "C" {    
#endif

/*
====================
AF_Time
struct to hold the data needed for monitoring time in the game
====================
*/
typedef struct {
	uint32_t currentFrame;		// The current frame.
	double deltaTime;	// Time in seconds since the last frame
	double currentTime;		// Time captured by the system clock. 
    double lastTime;		// record of the previous time captured
	AF_FLOAT fps; // Frames per second
	AF_FLOAT fpsTimer; // Timer to track FPS updates
	uint32_t frameCount; // Count frames for FPS calculation
} AF_Time;

static inline double AF_Time_GetTime(void){
	//return ((double)(clock()) / CLOCKS_PER_SEC);	// old incorrect way
	struct timespec timeSpec;
	clock_gettime(CLOCK_MONOTONIC, &timeSpec);
	return (double)(timeSpec.tv_sec) + (double)(timeSpec.tv_nsec) / 1e9;
}

/*
====================
AF_Time_Init
Initialise the struct variables. Don't need to take in any variables except the current time, 
pass back a new copy of initialised data in the struct.
====================
*/
static inline AF_Time AF_Time_ZERO(const double _currentTime){

	AF_Time returnTime;
	returnTime.currentFrame = 0;
	returnTime.deltaTime = 0;
	returnTime.currentTime = _currentTime;
	returnTime.lastTime = _currentTime;
	returnTime.fps = 0.0f;
	returnTime.fpsTimer = 0.0f;
	returnTime.frameCount = 0;

	return returnTime;
}

/*
====================
AF_Time_Update
Update the time variables
====================
*/

void AF_Time_Update(AF_Time* _time);



#ifdef __cplusplus
}
#endif

#endif //AF_TIME_H
