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
} AF_Time;

static inline double AF_Time_GetTime(void){
	return ((double)(clock()) / CLOCKS_PER_SEC);
}

/*
====================
AF_Time_Init
Initialise the struct variables. Don't need to take in any variables except the current time, 
pass back a new copy of initialised data in the struct.
====================
*/
static inline AF_Time AF_Time_ZERO(const float _currentTime){

	AF_Time returnTime = {
	.currentFrame = 0,
	.deltaTime = 0,
	.currentTime = _currentTime,
	.lastTime = 0.0f,
	};

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
	_time->currentFrame += 1,
	_time->lastTime = _time->currentTime;
}



#ifdef __cplusplus
}
#endif

#endif //AF_TIME_H
