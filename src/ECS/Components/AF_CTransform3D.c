#include "ECS/Components/AF_CTransform3D.h"
#include "ECS/Components/AF_Component.h"

/*
====================
AF_CTransform3D_ZERO
Empty constructor
====================
*/
AF_CTransform3D AF_CTransform3D_ZERO(void){ //
	AF_CTransform3D returnTransform = {
        //.has = FALSE,
        .enabled = FALSE,
        .pos = {0, 0, 0},
        .localPos = {0, 0, 0},
        .rot = {0, 0, 0},
        .localRot = {0, 0, 0},
        .scale = {1, 1, 1},
        .localScale = {1, 1, 1},
        .orientation = {0,0,0,0},
        .modelMat = Mat4_ZERO()
    };
	return returnTransform;
}

/*
====================
AF_CTransform3D_ADD
Add component constructor for the component
====================
*/
AF_CTransform3D AF_CTransform3D_ADD(void){
	AF_CTransform3D returnTransform = {
        //.has = TRUE,
        .enabled = TRUE,
        .pos = {0, 0, 0},
        .localPos = {0, 0, 0},
        .rot = {0, 0, 0},
        .localRot = {0, 0, 0},
        .scale = {1, 1, 1},
        .localScale = {1, 1, 1},
        .orientation = {0,0,0,0},
        .modelMat = Mat4_ZERO()
	// Default position matrix
    };
	return returnTransform;
}

