#include "AF_Audio.h"

/*
====================
AF_Audio_INIT
Definition for Audio init
====================
*/
void AF_Audio_Init(void){

}

/*
====================
AF_Audio_UPDATE
Definition for Audio update
====================
*/
void AF_Audio_Update(AF_ECS* _ecs, const float _dt);


/*
====================
AF_Audio_EarlyUpdate
Definition for Audio early update
====================
*/
void AF_Audio_EarlyUpdate(AF_ECS* _ecs){
    if(_ecs == NULL){}
}

/*
====================
AF_Audio_LateUpdate
functino to be called during late update. allows rendering debug to occur and to occur after movmeent
======
*/
void AF_Audio_LateUpdate(AF_ECS* _ecs){
    if(_ecs == NULL){}
}

/*
====================
AF_Audio_LateRenderUpdate
functino to be called during late update. allows rendering debug to occur and to occur inbetween render calls, allowing debug rendering to occur
======
*/
void AF_Audio_LateRenderUpdate(AF_ECS* _ecs){
    if(_ecs == NULL){}
}

/*
====================
AF_Audio_PLAY
Definition for Audio playing 
====================
*/
void AF_Audio_Play(AF_CAudioSource* _audioSource, float _volume, af_bool_t _isLooping){
    if(_audioSource == NULL || _volume == false || _isLooping == false){
    }

}

/*
====================
AF_Audio_SHUTDOWN
Definition for Audio shutdown 
====================
*/
void AF_Audio_Shutdown(void){

}
