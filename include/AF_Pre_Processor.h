#ifndef AF_PRE_PROCESSOR_H
#define AF_PRE_PROCESSOR_H

#define AF_EDITOR_VAR
#include <stddef.h>
#include "AF_Lib_Define.h"

typedef enum AF_EDITOR_VAR_TYPE_e{
    AF_EDITOR_VAR_TYPE_INT,
    AF_EDITOR_VAR_TYPE_FLOAT,
    AF_EDITOR_VAR_TYPE_BOOL,
    AF_EDITOR_VAR_TYPE_STRING,
    AF_EDITOR_VAR_TYPE_VEC2,
    AF_EDITOR_VAR_TYPE_VEC3,
    AF_EDITOR_VAR_TYPE_EVENT,
    AF_EDITOR_VAR_TYPE_TOTAL_TYPES
} AF_EDITOR_VAR_TYPE_e;

typedef struct {
    char name[AF_MAX_PATH_CHAR_SIZE];
    enum AF_EDITOR_VAR_TYPE_e type;
    union {
        uint32_t intValue;
        AF_FLOAT floatValue;
        af_bool_t boolValue;
        char strValue[AF_MAX_PATH_CHAR_SIZE];
        AF_FLOAT vec2Value[2];
        AF_FLOAT vec3Value[3];
        uint32_t eventTypeValue;
    } data;
} AF_PropertyMetaData_s;


#endif      // AF_PRE_PROCESSOR_H
