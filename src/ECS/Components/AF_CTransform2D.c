#include "ECS/Components/AF_CTransform2D.h"
#include "ECS/Components/AF_Component.h"
/*
====================
AF_CTransform2D_ZERO
Empty constructor
====================
*/
AF_CTransform2D AF_CTransform2D_ZERO(void){
	AF_CTransform2D returnTransform = {
        .enabled = AF_FALSE,
        .pos = {0, 0},
        .rot = {0, 0},
        .scale = {1, 1}
	// Default position matrix
    };
	return returnTransform;
}

/*
====================
AF_CTransform2D_ADD
Add component constructor for the component
====================
*/
AF_CTransform2D AF_CTransform2D_ADD(void){
	AF_CTransform2D returnTransform = {
        //.has = AF_TRUE,
        .enabled = AF_TRUE,
        .pos = {0, 0},
        .rot = {0, 0},
        .scale = {1, 1}
	// Default position matrix
    };
	return returnTransform;
}

