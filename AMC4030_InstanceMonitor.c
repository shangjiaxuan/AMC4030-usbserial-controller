#include "AMC4030_InstanceMonitor.h"

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
}

int AMC4030_InstanceMonitor_Start(AMC4030_InstanceMonitor* obj)
{
	if (obj->is_running) {
		return -1;
	}
	obj->is_running = 1;
	INIT_MUTEX(&obj->listener_lock);
	obj->monitorThread = StartThread(AMC4030_InstanceMonitorThreadProc, obj);
	return obj->monitorThread == 0 ? -2 : 0;
}

void AMC4030_InstanceMonitor_Stop(AMC4030_InstanceMonitor* obj)
{
	obj->is_running = 0;
	JoinThread(obj->monitorThread);
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
