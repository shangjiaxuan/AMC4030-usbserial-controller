#pragma once

#ifndef _AMC4030_MANAGER_INSTANCE_H_
#define _AMC4030_MANAGER_INSTANCE_H_

#include "Protocol/AMC4030_usb_com_protocol.h"

#include "library_required/ThreadCompat.h"

#include "library_required/Callbacks.h"

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
	CallbackChain* onEnd;
} AMC4030_InstanceMonitor;

int AMC4030_InstanceMonitor_Initialize(AMC4030_InstanceMonitor* obj);
void AMC4030_InstanceMonitor_Destroy(AMC4030_InstanceMonitor* obj);


int AMC4030_InstanceMonitor_Start(AMC4030_InstanceMonitor* obj);
int AMC4030_InstanceMonitor_Stop(AMC4030_InstanceMonitor* obj);

void AMC4030_InstanceMonitor_SetConnection(AMC4030_InstanceMonitor* obj, AMC4030_usb_protocol_context* connection);
AMC4030_usb_protocol_context* AMC4030_InstanceMonitor_GetConnection(AMC4030_InstanceMonitor* obj);

void AMC4030_InstanceMonitor_RegisterListener(AMC4030_InstanceMonitor* obj, CallbackChain* listener);
void AMC4030_InstanceMonitor_UnRegisterListener(AMC4030_InstanceMonitor* obj, CallbackChain* listener);

void AMC4030_InstanceMonitor_RegisterEndListener(AMC4030_InstanceMonitor* obj, CallbackChain* listener);
void AMC4030_InstanceMonitor_UnRegisterEndListener(AMC4030_InstanceMonitor* obj, CallbackChain* listener);

#include "../SerialDevice_Vtable.h"

extern const SerialDevice_InstanceMonitor_Vtable AMC4030_InstanceMonitor_Vtable;
extern const SerialDevice_Connection_Vtable AMC4030_USB_Connection_Vtable;

#endif
