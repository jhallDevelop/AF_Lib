#include "ECS/Components/AF_Component.h"
#include "AF_Flags.h"

/*
====================
AF_Component_SetHas
Function to set the has value
====================
*/
PACKED_CHAR AF_Component_SetHas(AF_Flag8_t _component, af_bool_t _hasFlag){
	if(_hasFlag == AF_TRUE){ 
		// Set the bit if the key is pressed
		return _component |= FLAG_HAS;
	}
	else{
		// clear the bit is clear if has is AF_FALSE
		return _component &= ~FLAG_HAS;
	}
}

/*
====================
AF_Component_SetEnabled
Function to set the enabled value
====================
*/
PACKED_CHAR AF_Component_SetEnabled(PACKED_CHAR _component, af_bool_t _enabledFlag){
	if(_enabledFlag== AF_TRUE){ 
		// Set the bit if the enabled frag is AF_TRUE
		return _component |= FLAG_ENABLED;
	}
	else{
		// clear the bit is clear if has is AF_FALSE
		return _component &= ~FLAG_ENABLED;
	}
}


/*
====================
AF_Component_GetEnabled
Function to decode the enabled value
====================
*/
af_bool_t AF_Component_GetEnabled(AF_Flag8_t _flags){

	return (_flags & FLAG_ENABLED) != 0; 
}

/*
====================
AF_Component_GetHas
Function to get the has value
====================
*/
af_bool_t AF_Component_GetHas(AF_Flag8_t _flags){

	return (_flags & FLAG_HAS) != 0;
}


// ====================
// AF_Component_GetHasEnabled
// Function to determine if the flag is both enabled and has
// using bitwise operations
// ====================
af_bool_t AF_Component_GetHasEnabled(AF_Flag8_t _flags){
	// Create a mask that has both FLAG_ENABLED and FLAG_HAS bits set
    const AF_Flag8_t MASK_HAS_ENABLED = FLAG_ENABLED | FLAG_HAS;
    // Check if all bits in MASK_HAS_ENABLED are set in _flags
    return (_flags & MASK_HAS_ENABLED) == MASK_HAS_ENABLED;
}


