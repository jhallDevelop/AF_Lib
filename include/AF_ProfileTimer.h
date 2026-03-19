#ifndef AF_ProfileTimer_H
#define AF_ProfileTimer_H

#include "AF_Lib_Define.h"
#include "AF_Log.h"

#ifdef __cplusplus
extern "C" {
#endif

// ====================
// AF_ProfileTimer
// struct to hold the data needed for monitoring time in the game
// ====================
typedef struct {
	// 4-byte types packed together
    AF_FLOAT fps;				// Frames per second
	AF_FLOAT fpsTimer;			// Timer to track FPS updates
	uint32_t frameCount;		// Count frames for FPS calculation
	uint32_t totalFrameCount;	// Total frames since the application started
} AF_ProfileTimer;



// ====================
// AF_Time_Update
// Update the profile Timer variables
// ====================
static inline void AF_ProfileTimer_Update(AF_ProfileTimer* _time, AF_FLOAT _deltaTime){
    assert(_time != NULL && "AF_ProfileTimer_Update: _time is NULL");
    
	_time->frameCount++;
	_time->totalFrameCount++;
	_time->fpsTimer += _deltaTime;

	// Update FPS once per second
	if (_time->fpsTimer >= 1.0f) {
		_time->fps = (AF_FLOAT)_time->frameCount;

		// subtract 1.0 to keep the timer from drifting over time, which can cause FPS updates to become less frequent
		_time->fpsTimer -= 1.0f;
		_time->frameCount = 0;
	}
}

static inline AF_ProfileTimer AF_ProfileTimer_ZERO(void){
	AF_ProfileTimer timer = {
		.fps = 0,
		.fpsTimer = 0,
		.frameCount = 0,
		.totalFrameCount = 0
	};
	return timer;
}	

#ifdef __cplusplus
}
#endif

#endif // AF_ProfileTimer_H 
