#pragma once

#ifndef _AMC4030_MANAGER_INSTANCE_H_
#define _AMC4030_MANAGER_INSTANCE_H_

#include "Protocol/AMC4030_usb_com_protocol.h"

#include "../ThreadCompat.h"

#include "../Callbacks.h"

typedef struct AMC4030_InstanceMonitor
{
	AMC4030_MachinStatus current_status;
	int update_interval;

	int update_error;

	AMC4030_usb_protocol_context* connection;
	ThreadHandle monitorThread;
	int is_running;
	MUTEX_T listener_lock;
	// triggers on status update
	CallbackChain* status_listeners;
} AMC4030_InstanceMonitor;

int AMC4030_InstanceMonitor_Start(AMC4030_InstanceMonitor* obj);
void AMC4030_InstanceMonitor_Stop(AMC4030_InstanceMonitor* obj);

void AMC4030_InstanceMonitor_RegisterListener(AMC4030_InstanceMonitor* obj, CallbackChain* listener);
void AMC4030_InstanceMonitor_UnRegisterListener(AMC4030_InstanceMonitor* obj, CallbackChain* listener);

#endif
