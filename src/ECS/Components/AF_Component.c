#include "ECS/Components/AF_Component.h"

/*
====================
AF_Component_SetHas
Function to set the has value
====================
*/
PACKED_CHAR AF_Component_SetHas(flag_t _component, BOOL _hasFlag){
	if(_hasFlag == TRUE){ 
		// Set the bit if the key is pressed
		return _component |= FLAG_HAS;
	}
	else{
		// clear the bit is clear if has is FALSE
		return _component &= ~FLAG_HAS;
	}
}

/*
====================
AF_Component_SetEnabled
Function to set the enabled value
====================
*/
PACKED_CHAR AF_Component_SetEnabled(PACKED_CHAR _component, BOOL _enabledFlag){
	if(_enabledFlag== TRUE){ 
		// Set the bit if the enabled frag is TRUE
		return _component |= FLAG_ENABLED;
	}
	else{
		// clear the bit is clear if has is FALSE
		return _component &= ~FLAG_ENABLED;
	}
}


/*
====================
AF_Component_GetEnabled
Function to decode the enabled value
====================
*/
BOOL AF_Component_GetEnabled(flag_t _flags){

	return (_flags & FLAG_ENABLED) != 0; 
}

/*
====================
AF_Component_GetHas
Function to get the has value
====================
*/
BOOL AF_Component_GetHas(flag_t _flags){

	return (_flags & FLAG_HAS) != 0;
}

/*
====================
AF_Component_GetHasEnabled
Function to determine if the flag is both enabled and has
using bitwise operations
====================
*/
BOOL AF_Component_GetHasEnabled(flag_t _flags){
	// Create a mask that has both FLAG_ENABLED and FLAG_HAS bits set
    const flag_t MASK_HAS_ENABLED = FLAG_ENABLED | FLAG_HAS;
    // Check if all bits in MASK_HAS_ENABLED are set in _flags
    return (_flags & MASK_HAS_ENABLED) == MASK_HAS_ENABLED;
}
