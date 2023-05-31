#pragma once
#ifndef _SERIAL_DEVICE_VTABLE_H_
#define _SERIAL_DEVICE_VTABLE_H_

#include "Callbacks.h"

typedef struct SerialDevice_InstanceMonitor_Vtable
{
	int (*Initialize)(void* obj);
	void (*Destroy)(void* obj);
	int (*Start)(void* obj);
	int (*Stop)(void* obj);

	void (*RegisterListener)(void* obj, CallbackChain* listener);
	void (*UnRegisterListener)(void* obj, CallbackChain* listener);
	void (*RegisterEndListener)(void* obj, CallbackChain* listener);
	void (*UnRegisterEndListener)(void* obj, CallbackChain* listener);

	void (*SetConnection)(void* obj, void* connection);
	void* (*GetConnection)(void* obj);
}SerialDevice_InstanceMonitor_Vtable;

typedef struct SerialDevice_Connection_Vtable
{
	void* (*Create)(const char* filename, int* error);
	void (*Delete)(void* obj);
}SerialDevice_Connection_Vtable;

typedef struct SerialDevice_Enumerator_Vtable
{
	void* (*Create)();
	void (*Delete)(void* obj);

	void (*Reset)(void* obj);
	// returns positive com ports one by one.
	// stops at NULL
	const char* (*Next)(void* obj);

	int (*GetCurrentFriendlyName)(void* obj, char* name_buffer, int buffer_size);
}SerialDevice_Enumerator_Vtable;

#endif


