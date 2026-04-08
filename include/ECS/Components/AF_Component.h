/*
===============================================================================
AF_COMPONENT_H definitions

Definition for helper functions for components
===============================================================================
*/
#ifndef AF_COMPONENT_H
#define AF_COMPONENT_H

#include "AF_Lib_Define.h"
#include "AF_Lib_API.h"
#include "AF_Flags.h"
#ifdef __cplusplus
extern "C" {
#endif

AF_LIB_API PACKED_CHAR AF_Component_SetHas(AF_Flag8_t _component, af_bool_t _hasFlag);
AF_LIB_API PACKED_CHAR AF_Component_SetEnabled(PACKED_CHAR _component, af_bool_t _enabledFlag);
AF_LIB_API af_bool_t AF_Component_GetEnabled(AF_Flag8_t _flags);
AF_LIB_API af_bool_t AF_Component_GetHasEnabled(AF_Flag8_t _flags);
AF_LIB_API af_bool_t AF_Component_GetHas(AF_Flag8_t _flags);

#ifdef __cplusplus
}
#endif

#endif  // AF_COMPONENT_H
