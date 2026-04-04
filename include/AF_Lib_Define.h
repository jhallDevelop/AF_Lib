/*
===============================================================================
AF_LIB_DEFINE_H
define extra types e.g. af_bool_t which doesn't exist in c

===============================================================================
*/
#ifndef AF_LIB_DEFINE_H
#define AF_LIB_DEFINE_H
#include <stdint.h>

// Define value often used
#define AF_MAX_PATH_CHAR_SIZE 1024    // Max path size for file paths
#define AF_ECS_TOTAL_ENTITIES 65	
#define AF_ECS_INVALID_INDEX 0xFFFFFFFF

// Define Static

// Define Bool
#define AF_TRUE 1
#define AF_FALSE 0
#define AF_SUCCESS 1
#define AF_FAIL 0
typedef char af_bool_t;		// 1 byte

// --- Cross-Platform "Safe" Strings & IO ---
#if defined(_WIN32)
	#include <string.h>
	#include <stdio.h>
	#define AF_STRNCPY_S(dest, destSize, src, count) strncpy_s(dest, destSize, src, count)
	#define AF_SSCANF_S sscanf_s
#else
	#include <string.h>
	#include <stdio.h>
	// POSIX strncpy doesn't take destSize, but we check bounds manually in the caller
	#define AF_STRNCPY_S(dest, destSize, src, count) (strncpy(dest, src, count), (dest)[(count) < (destSize) ? (count) : (destSize) - 1] = '\0')
	#define AF_SSCANF_S sscanf
#endif

typedef char PACKED_CHAR;	// 1 byte
typedef uint16_t PACKED_UINT16;	// 2 bytes
typedef uint32_t PACKED_UINT32;	// 4 bytes

#define AF_FLOAT float
#if USE_FIXED
    //#define AF_FLOAT int8_t //uint16_t
    //#define AF_EPSILON 1 << 10
#else
    //#define AF_FLOAT float
    #define AF_EPSILON 1e-6
#endif				


typedef enum AF_Anchor_e {
    AF_ANCHOR_TOP_LEFT,
    AF_ANCHOR_TOP_CENTRE,
    AF_ANCHOR_TOP_RIGHT,
    AF_ANCHOR_MIDDLE_LEFT,
    AF_ANCHOR_MIDDLE_CENTRE,
    AF_ANCHOR_MIDDLE_RIGHT,
    AF_ANCHOR_BOTTOM_LEFT,
    AF_ANCHOR_BOTTOM_CENTRE,
    AF_ANCHOR_BOTTOM_RIGHT,
    AF_ANCHOR_ENUM_COUNT
} AF_Anchor_e;


// enum to text mapping
typedef struct AF_AnchorComponentMap {
    AF_Anchor_e anchorEnum;
    const char* anchorName; 
} AF_AnchorComponentMap;

static AF_AnchorComponentMap AF_AnchorComponentMappings[] = {
    {AF_ANCHOR_TOP_LEFT, "Top Left"},
    {AF_ANCHOR_TOP_CENTRE, "Top Centre"},
    {AF_ANCHOR_TOP_RIGHT, "Top Right"},
    {AF_ANCHOR_MIDDLE_LEFT, "Middle Left"},
    {AF_ANCHOR_MIDDLE_CENTRE, "Middle Centre"},
    {AF_ANCHOR_MIDDLE_RIGHT, "Middle Right"},
    {AF_ANCHOR_BOTTOM_LEFT, "Bottom Left"},
    {AF_ANCHOR_BOTTOM_CENTRE, "Bottom Centre"},
    {AF_ANCHOR_BOTTOM_RIGHT, "Bottom Right"}
};

// Helper sizeof to find the size of an array, as sizeof only tells you the size of the array pointer
#endif

