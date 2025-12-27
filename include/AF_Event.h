#ifndef AF_EVENT_H
#define AF_EVENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "AF_Lib_Define.h"

#define AF_EVENT_MAX_LISTENERS_PER_TYPE 32
#define AF_EVENT_QUEUE_SIZE 32

// Event Enum
typedef enum AF_Event_Type_e{
    AF_EVENT_TYPE_NONE = 0,
    AF_EVENT_TYPE_WINDOW_CLOSE,
    AF_EVENT_TYPE_WINDOW_RESIZE,
    AF_EVENT_TYPE_KEYBOARD_KEY_PRESS,
    AF_EVENT_TYPE_KEYBOARD_KEY_RELEASE,
    AF_EVENT_TYPE_MOUSE_BUTTON_PRESS,
    AF_EVENT_TYPE_MOUSE_BUTTON_RELEASE,
    AF_EVENT_TYPE_MOUSE_MOVE,
    AF_EVENT_TYPE_TOTAL_TYPES // For array sizing
} AF_Event_Type_e;

// Forward-declare the event struct
struct AF_Event_s;
struct AF_AppData;

// Listener function pointer. The data passed will be a pointer to AF_Event_s.
typedef void (*AF_EventListenerFuncPtr)(const struct AF_Event_s* _event);

typedef struct {
	AF_EventListenerFuncPtr listeners[AF_EVENT_MAX_LISTENERS_PER_TYPE];
	uint32_t listenerCount;
} AF_EventListenerList_s;

typedef struct {
    AF_EventListenerList_s registries[AF_EVENT_TYPE_TOTAL_TYPES];
} AF_EventRegistry_s;


// Event struct for event instances.
// Contains the type and all possible data payloads.
typedef struct AF_Event_s {
	AF_Event_Type_e type;

	// Event-specific data payloads.
	// Only one of these will be valid, depending on the 'type'.
	struct {
		uint32_t width;
		uint32_t height;
	} windowResize;

	struct {
		uint32_t keyCode;
	} keyboardKey;

	struct {
		uint32_t button;
	} mouseButton;
	
	struct {
		float x;
		float y;
	} mouseMove;
} AF_Event_s;


// Event manager 
void AF_Event_Initialize(AF_EventRegistry_s* _registry);
void AF_Event_Shutdown(void);
uint32_t AF_Event_RegisterListener(AF_EventRegistry_s* _registry, AF_Event_Type_e _type, AF_EventListenerFuncPtr _listener);
uint32_t AF_Event_UnregisterListener(AF_EventRegistry_s* _registry, AF_Event_Type_e _type, AF_EventListenerFuncPtr _listener);
void AF_Event_Queue(struct AF_AppData* _appData, AF_Event_s _event);
void AF_Event_Update(struct AF_AppData* _appData);
AF_Event_s AF_Event_ZERO(void);

#ifdef __cplusplus
}
#endif

#endif // AF_EVENT_H
