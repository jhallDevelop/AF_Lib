/*
===============================================================================
AF_COMPONENT_H definitions

Definition for helper functions for components
===============================================================================
*/
#ifndef AF_COMPONENT_H
#define AF_COMPONENT_H

#include "AF_Lib_Define.h"
#ifdef __cplusplus
extern "C" {
#endif

PACKED_CHAR AF_Component_SetHas(flag_t _component, af_bool_t _hasFlag);
PACKED_CHAR AF_Component_SetEnabled(PACKED_CHAR _component, af_bool_t _enabledFlag);
af_bool_t AF_Component_GetEnabled(flag_t _flags);
af_bool_t AF_Component_GetHasEnabled(flag_t _flags);
af_bool_t AF_Component_GetHas(flag_t _flags);

#ifdef __cplusplus
}
#endif

#endif  // AF_COMPONENT_H
