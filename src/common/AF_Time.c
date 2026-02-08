#include "AF_Time.h"
#include "AF_Log.h"
/*
====================
AF_Time_Update
Update the time variables
====================
*/

void AF_Time_Update(AF_Time* _time){
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
