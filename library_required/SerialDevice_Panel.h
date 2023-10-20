#pragma once

#ifndef _SERIAL_DEVICE_PANEL_H_
#define _SERIAL_DEVICE_PANEL_H_

typedef struct SerialDevice_UI_UIControls
{
	int panel;
	int device_select;
	int device_refresh;
	int connect;

	int select_mode_array;
	int connected_mode_array;

	int hide, quit;
}SerialDevice_UI_UIControls;

#include "Callbacks.h"

#include "SerialDevice_Vtable.h"

#include <stdint.h>

typedef struct SerialDevice_UI_Object
{
	SerialDevice_UI_UIControls* controls;

	const SerialDevice_InstanceMonitor_Vtable* monitor_funcs;
	const SerialDevice_Connection_Vtable* connection_funcs;
	const SerialDevice_Enumerator_Vtable* device_enumerator_funcs;

	void* monitor;

	CallbackChain updateListener;
	CallbackChain stopListener;
	int connection_need_cleanup;

	uint16_t port_count;
	CallbackChain selectHook;

	int is_connected;
	DataHook connectHook;

	CallbackChain refreshHook;

	CallbackChain panelHideHook;
	CallbackChain quitUIHook;

	// windows have the format \\.\COM1
	// linux and mac have format
	// pcie device:		/dev/ttyS0 /dev/tty.XXXXXXXX /dev/cu.XXXXXXXX
	// usb serial:		/dev/ttyUSB0 /dev/ttyACM0
	// bluetooth:		/dev/rfcomm0
	char port_file_short_buffer[30];
	size_t port_file_buffer_size;
	// if not equal to port_file_short_buffer, then is malloced
	char* port_file_name;

	CallbackChain* onDeviceRefresh;
	CallbackChain* onStatusUpdate;

	int use_as_standalone;
	int standalone_running;
	unsigned int cvi_thread_id;

	int need_device_refresh;

	int disallow_user_control;
}SerialDevice_UI_Object;


int SerialDevice_UI_Object_Initialize(SerialDevice_UI_Object* obj, int use_as_standalone);

void SerialDevice_UI_Object_Destroy(SerialDevice_UI_Object* obj);

int SerialDevice_UI_Object_StartUI(SerialDevice_UI_Object* obj);

int SerialDevice_UI_Object_EndUI(SerialDevice_UI_Object* obj);

int SerialDevice_UI_Object_RefreshDevices(SerialDevice_UI_Object* obj);

int SerialDevice_UI_Object_GetSelectedCommDev(SerialDevice_UI_Object* obj);

int SerialDevice_UI_Object_Connect(SerialDevice_UI_Object* obj);

int SerialDevice_UI_Object_DisConnect(SerialDevice_UI_Object* obj);

int SerialDevice_UI_Object_UpdateThreadEnding(SerialDevice_UI_Object* obj);

int SerialDevice_UI_Object_UpdateStatus(SerialDevice_UI_Object* obj);

int SerialDevice_UI_Object_ShowUI(SerialDevice_UI_Object* obj);
int SerialDevice_UI_Object_HideUI(SerialDevice_UI_Object* obj);


int SerialDevice_UI_Object_Lock(SerialDevice_UI_Object* obj);
int SerialDevice_UI_Object_Unlock(SerialDevice_UI_Object* obj);

int SerialDevice_UI_Object_SetUserControl(SerialDevice_UI_Object* obj);

#endif
