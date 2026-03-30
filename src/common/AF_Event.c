#include "AF_Event.h"
#include "AF_Log.h"
#include "AF_AppData.h"
#include <string.h>

static void AF_Event_Dispatch(AF_EventRegistry_s* _registry, AF_Event_s* _event);


/*
================
AF_Event_ResetRegistry
================
*/
void AF_Event_ResetRegistry(AF_EventRegistry_s* _registry)
{
    if (_registry == NULL) {
        AF_Log_Error("AF_Event_ResetRegistry: _registry is NULL");
        return;
    }

    memset(_registry, 0, sizeof(AF_EventRegistry_s));
    AF_Log("AF_Event_ResetRegistry: Event registry cleared\n");
}


/*
================
AF_Event_Initialize
================
*/
void AF_Event_Initialize(AF_EventRegistry_s* _registry)
{
    AF_Event_ResetRegistry(_registry);
    AF_Log("AF_Event_Initialize: Event System Initialized\n");
}

/*
================
AF_Event_Shutdown
================
*/
void AF_Event_Shutdown(void){
    AF_Log("AF_Event_Shutdown: Event System Shutdown\n");
}



/*
================
AF_Event_RegisterListener
================
*/
uint32_t AF_Event_RegisterListener(AF_EventRegistry_s* _registry, AF_Event_Type_e _type, AF_EventListenerFuncPtr _listener){
    if (_type >= AF_EVENT_TYPE_TOTAL_TYPES) {
        AF_Log_Error("AF_Event_RegisterListener: Invalid event type.");
        return AF_FAIL;
    }
    if (_registry->registries[_type].listenerCount >= AF_EVENT_MAX_LISTENERS_PER_TYPE) {
        AF_Log_Error("AF_Event_RegisterListener: Max listeners reached for this event type.");
        return AF_FAIL;
    }

    _registry->registries[_type].listeners[_registry->registries[_type].listenerCount] = _listener;
    _registry->registries[_type].listenerCount++;
    return AF_SUCCESS;
}


/*
================
AF_Event_UnregisterListener
================
*/
uint32_t AF_Event_UnregisterListener(AF_EventRegistry_s* _registry, AF_Event_Type_e _type, AF_EventListenerFuncPtr _listener){
    if (_type >= AF_EVENT_TYPE_TOTAL_TYPES) {
        AF_Log_Error("AF_Event_UnregisterListener: Invalid event type.");
        return AF_FAIL;
    }

    for (uint32_t i = 0; i < _registry->registries[_type].listenerCount; i++) {
        if (_registry->registries[_type].listeners[i] == _listener) {
            for (uint32_t j = i; j < _registry->registries[_type].listenerCount - 1; j++) {
                _registry->registries[_type].listeners[j] = _registry->registries[_type].listeners[j+1];
            }
            _registry->registries[_type].listenerCount--;
            return AF_SUCCESS;
        }
    }
    
    AF_Log_Warning("AF_Event_UnregisterListener: Listener not found for event type %d.", _type);
    return AF_FAIL;
}



/*
================
AF_Event_Queue
================
*/
void AF_Event_Queue(AF_AppData* _appData, AF_Event_s _event)
{
    if (_appData == NULL) {
        AF_Log_Error("AF_Event_Queue: _appData is NULL");
        return;
    }

    if (_event.type <= AF_EVENT_TYPE_NONE || _event.type >= AF_EVENT_TYPE_TOTAL_TYPES) {
        AF_Log_Error("AF_Event_Queue: Invalid event type %u, dropping event.", (uint32_t)_event.type);
        return;
    }

    for (uint32_t i = 0; i < AF_EVENT_QUEUE_SIZE; i++) {
        if (_appData->events[i].type == AF_EVENT_TYPE_NONE) {
            _appData->events[i] = _event;
            return;
        }
    }
    AF_Log_Warning("AF_Event_Queue: Event queue is full. Event was dropped.");
}



/*
================
AF_Event_Update
================
*/
void AF_Event_Update(AF_AppData* _appData)
{
    if (_appData == NULL){
        AF_Log_Error("AF_Event_Update: _appData is NULL\n");
        return;
    }

    AF_Event_s eventQueueCopy[AF_EVENT_QUEUE_SIZE];
    memcpy(eventQueueCopy, _appData->events, sizeof(eventQueueCopy));

    for(uint32_t i = 0; i < AF_EVENT_QUEUE_SIZE; i++){
        _appData->events[i] = AF_Event_ZERO();
    }

    for(uint32_t i = 0; i < AF_EVENT_QUEUE_SIZE; i++){
        if(eventQueueCopy[i].type != AF_EVENT_TYPE_NONE){
            AF_Event_Dispatch(&_appData->eventRegistry, &eventQueueCopy[i]);
        }
    }
}

/*
================
AF_Event_Dispatch
================
*/
void AF_Event_Dispatch(AF_EventRegistry_s* _registry, AF_Event_s* _event){
    if(_registry == NULL){
        AF_Log_Error("AF_Event_Dispatch: _registry is NULL");
        return;
    }

    if(_event == NULL){
        AF_Log_Error("AF_Event_Dispatch: _event is NULL");
        return;
    }
    
    AF_Log("AF_Event_Dispatch: Event type=%u, AF_EVENT_TYPE_TOTAL_TYPES=%d", _event->type, AF_EVENT_TYPE_TOTAL_TYPES);
    if (_event->type >= AF_EVENT_TYPE_TOTAL_TYPES) {
        AF_Log_Error("AF_Event_Dispatch: Event type %u is out of bounds (max=%d)", _event->type, AF_EVENT_TYPE_TOTAL_TYPES);
        return;
    }
    
    AF_EventListenerList_s* registry = &_registry->registries[_event->type];
    for (uint32_t i = 0; i < registry->listenerCount; i++) {
        AF_EventListenerFuncPtr listener = registry->listeners[i];
        if (listener == NULL) {
            continue;
        }
        listener(_event);
    }
}

AF_Event_s AF_Event_ZERO(void)
{
    AF_Event_s event;
    event.type = AF_EVENT_TYPE_NONE;
    // ensure data is zeroed out
    memset(&event.windowResize, 0, sizeof(event.windowResize));
    memset(&event.keyboardKey, 0, sizeof(event.keyboardKey));
    memset(&event.mouseButton, 0, sizeof(event.mouseButton));
    memset(&event.mouseMove, 0, sizeof(event.mouseMove));
    return event;
}
