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
    AF_EDITOR_VAR_TYPE_VEC3,
    AF_EDITOR_VAR_TYPE_TOTAL_TYPES
} AF_EDITOR_VAR_TYPE_e;

typedef struct {
    char name[AF_MAX_PATH_CHAR_SIZE];
    enum AF_EDITOR_VAR_TYPE_e type;
    size_t offset;
} AF_PropertyMetaData_s;


#endif      // AF_PRE_PROCESSOR_H
