#include "AMC4030_InstanceMonitor.h"

int AMC4030_InstanceMonitor_Initialize(AMC4030_InstanceMonitor* obj)
{
	INIT_MUTEX(&obj->listener_lock);
	return 0;
}

void AMC4030_InstanceMonitor_Destroy(AMC4030_InstanceMonitor* obj)
{
	AMC4030_InstanceMonitor_Stop(obj);
	DESTROY_MUTEX(&obj->listener_lock);
}

void AMC4030_InstanceMonitor_SetConnection(AMC4030_InstanceMonitor* obj, AMC4030_usb_protocol_context* connection)
{
	AMC4030_usb_protocol_context* prev_connection = obj->connection;
	if (prev_connection != connection) {
		obj->connection = connection;
	}
}
AMC4030_usb_protocol_context* AMC4030_InstanceMonitor_GetConnection(AMC4030_InstanceMonitor* obj)
{
	return obj->connection;
}


void AMC4030_InstanceMonitorThreadProc(AMC4030_InstanceMonitor* obj)
{
	while (obj->is_running && obj->update_error >= 0) {
		obj->update_error = AMC4030_usb_protocol_GetMachinStatus(obj->connection, &obj->current_status);
		LOCK_MUTEX(&obj->listener_lock);
		if (obj->status_listeners) {
			CallbackChain_Invoke(obj->status_listeners);
		}
		UNLOCK_MUTEX(&obj->listener_lock);
		if (!obj->is_running || obj->update_error < 0) {
			break;
		}
		// perhaps a condition variable in alertable state is better
		Sleep(obj->update_interval);
	}
	obj->is_running = 0;
	if (obj->onEnd) {
		CallbackChain_Invoke(obj->onEnd);
	}
}

int AMC4030_InstanceMonitor_Start(AMC4030_InstanceMonitor* obj)
{
	if (obj->is_running) {
		return -1;
	}
	obj->is_running = 1;
	obj->update_error = 0;
	obj->monitorThread = StartThread(AMC4030_InstanceMonitorThreadProc, obj);
	if (!obj->monitorThread) {
		obj->is_running = 0;
	}
	return obj->monitorThread == 0 ? -2 : 0;
}

int AMC4030_InstanceMonitor_Stop(AMC4030_InstanceMonitor* obj)
{
	obj->is_running = 0;
	if (obj->monitorThread) {
		JoinThread(obj->monitorThread);
		obj->monitorThread = 0;
	}
	return 0;
}

void AMC4030_InstanceMonitor_RegisterListener(AMC4030_InstanceMonitor* obj, CallbackChain* listener)
{
	LOCK_MUTEX(&obj->listener_lock);
	CallbackChain_Append(&obj->status_listeners, listener);
	UNLOCK_MUTEX(&obj->listener_lock);
}
void AMC4030_InstanceMonitor_UnRegisterListener(AMC4030_InstanceMonitor* obj, CallbackChain* listener)
{
	LOCK_MUTEX(&obj->listener_lock);
	CallbackChain_Remove(&obj->status_listeners, listener);
	UNLOCK_MUTEX(&obj->listener_lock);
}

void AMC4030_InstanceMonitor_RegisterEndListener(AMC4030_InstanceMonitor* obj, CallbackChain* listener)
{
	LOCK_MUTEX(&obj->listener_lock);
	CallbackChain_Append(&obj->onEnd, listener);
	UNLOCK_MUTEX(&obj->listener_lock);
}
void AMC4030_InstanceMonitor_UnRegisterEndListener(AMC4030_InstanceMonitor* obj, CallbackChain* listener)
{
	LOCK_MUTEX(&obj->listener_lock);
	CallbackChain_Remove(&obj->onEnd, listener);
	UNLOCK_MUTEX(&obj->listener_lock);
}

const SerialDevice_InstanceMonitor_Vtable AMC4030_InstanceMonitor_Vtable = {
	AMC4030_InstanceMonitor_Initialize,
	AMC4030_InstanceMonitor_Destroy,
	AMC4030_InstanceMonitor_Start,
	AMC4030_InstanceMonitor_Stop,
	AMC4030_InstanceMonitor_RegisterListener,
	AMC4030_InstanceMonitor_UnRegisterListener,
	AMC4030_InstanceMonitor_RegisterEndListener,
	AMC4030_InstanceMonitor_UnRegisterEndListener,
	AMC4030_InstanceMonitor_SetConnection,
	AMC4030_InstanceMonitor_GetConnection
};

const SerialDevice_Connection_Vtable AMC4030_USB_Connection_Vtable = {
	AMC4030_usb_protocol_Create,
	AMC4030_usb_protocol_Destroy
};
