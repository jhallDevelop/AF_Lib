// ===============================================================================
// AF_FLAGS_H
// Define flag helpers
// ===============================================================================
#ifndef AF_FLAGS_H
#define AF_FLAGS_H

typedef char AF_Flag8_t;		// 1 byte char to hold up to 8 flags

// We can store up to 8 bit flags in a char
#define FLAG_HAS 	    0x01 	// Has bit flag 	0000 0001 // used to check if a component exists on an entity, separate from enabled/disabled state
#define FLAG_ENABLED 	0x02 	// Enabled bit flag	0000 0010 // enabled in editor or game state, inverse is disabled
#define FLAG_EXIST      0x04    // Alive bit flag   0000 0100 // exists or doesn't exist
#define FLAG_ACTIVE_IN_HIERARCHY 0x08 // Active in hierarchy 0000 1000 // locally enabled AND parent is active

//#define FLAG_5 0x10		// Flag 5		0001 0000
//#define FLAG_6 0x20		// Flag 6		0010 0000
//#define FLAG_7 0x40		// Flag 7 		0100 0000
//#define FLAG_8 0x80		// Flag 8		1000 0000


// ====================
// AF_Flags_SetHas
// Function to set the has value
// ====================
inline static AF_Flag8_t AF_Flags_SetHas(AF_Flag8_t _flagVar, char _flagType, af_bool_t _hasFlag){
	if(_hasFlag){
		return _flagVar | _flagType;
	}else{
		return _flagVar & ~_flagType;
	}
}

/*
====================
AF_Component_GetHas
Function to get the has value
====================
*/
inline static af_bool_t AF_Flags_GetHas(AF_Flag8_t _flagVar, char _flagType){
	return (_flagVar & _flagType) != 0;
}

#endif // AF_FLAGS_H
