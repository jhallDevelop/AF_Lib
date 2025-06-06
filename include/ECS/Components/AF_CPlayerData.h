/*
===============================================================================
AF_CPLAYERDATA_H definitions

Definition for the camera component struct
and helper functions
===============================================================================
*/
#ifndef AF_CPLAYERDATA_H
#define AF_CPLAYERDATA_H
#include "AF_Component.h"
#include "AF_Vec3.h"
#ifdef __cplusplus
extern "C" {    
#endif

enum PLAYER_FACTION {
	DEFAULT = 0,
	PLAYER = 1,
	ENEMY1 = 2,
	ENEMY2 = 3,
	ENEMY3 = 4,
	ENEMY4 = 5
};
/*
====================
AF_CSprite
====================
*/
// size is 64 bytes
typedef struct  {
	//BOOL has;
	//BOOL enabled;
	PACKED_CHAR enabled;	    // 1 byte
	BOOL isHuman;
	BOOL isCarrying;
	BOOL isCarried;
    int carryingEntity;
    float health;
    BOOL isAlive;
	BOOL isAttacking;
	BOOL isJumping;
	float movementSpeed;
	int score;
	Vec3 startPosition;
	Vec3 targetDestination;
	float spawnTime;
	enum PLAYER_FACTION faction;
	
} AF_CPlayerData;

AF_CPlayerData AF_CPlayerData_ZERO(void);
AF_CPlayerData AF_CPlayerData_ADD(void);

#ifdef __cplusplus
}
#endif

#endif
