/*
===============================================================================
AF_UI_H

Implimentation of helper functions for UI rendering
Used for text rendering
TODO: sprite and other UI component rendering
===============================================================================
*/
#include "ECS/Entities/AF_ECS.h"
#include "ECS/Components/AF_CText.h"
void AF_UI_Init(AF_ECS* _ecs);
void AF_UI_Renderer_Update(AF_CText* _text);
void AF_UI_Renderer_Finish(void);
void AF_UI_Renderer_Shutdown(void);