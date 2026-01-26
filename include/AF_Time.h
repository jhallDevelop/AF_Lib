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
	double deltaTime;	// Time in ms since the last frame
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
static inline AF_Time AF_Time_ZERO(const float _currentTime){

	AF_Time returnTime;
	returnTime.currentFrame = 0;
	returnTime.deltaTime = 0;
	returnTime.currentTime = _currentTime;
	returnTime.lastTime = 0.0f;
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

static inline void AF_Time_Update(AF_Time* _time){
	_time->currentTime = AF_Time_GetTime();
    _time->deltaTime = _time->currentTime -_time->lastTime;
    _time->lastTime = _time->currentTime;

    _time->frameCount += 1;
    _time->fpsTimer += _time->deltaTime;
    
    // When one second has passed...
    if(_time->fpsTimer >= 1.0f){
        // Calculate and STORE the FPS in your new variable.
        _time->fps = (float)_time->frameCount / _time->fpsTimer;

        // Reset the counters for the next second.
        _time->fpsTimer = 0.0f;
        _time->frameCount = 0;
    }
}



#ifdef __cplusplus
}
#endif

#endif //AF_TIME_H
