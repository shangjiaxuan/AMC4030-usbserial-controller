#include "AMC4030_DeviceEnumerator.h"

#ifndef MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS
#define MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS 0
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#include <SetupAPI.h>

#include <stdlib.h>

#include <stdio.h>

typedef struct AMC4030_DeviceEnumerator
{
	HDEVINFO hDevInfo;
	DWORD index;
	char name_buffer[4 + 3 + 3 + 1];
}AMC4030_DeviceEnumerator;

AMC4030_DeviceEnumerator* AMC4030_DeviceEnumerator_Create()
{
	HDEVINFO hDevInfo = SetupDiGetClassDevsW(NULL, L"USB", NULL, DIGCF_PRESENT | DIGCF_ALLCLASSES);
	if (hDevInfo == INVALID_HANDLE_VALUE) {
		return NULL;
	}
	AMC4030_DeviceEnumerator* result = (AMC4030_DeviceEnumerator*)calloc(sizeof(AMC4030_DeviceEnumerator), 1);
	if (result) {
		result->hDevInfo = hDevInfo;
		return result;
	}
	SetupDiDestroyDeviceInfoList(hDevInfo);
	return NULL;
}

void AMC4030_DeviceEnumerator_Destroy(AMC4030_DeviceEnumerator* obj)
{
	if (!obj) {
		return;
	}
	SetupDiDestroyDeviceInfoList(obj->hDevInfo);
	free(obj);
}

void AMC4030_DeviceEnumerator_Reset(AMC4030_DeviceEnumerator* obj)
{
	obj->index = 0;
}

const char* AMC4030_DeviceEnumerator_Next(AMC4030_DeviceEnumerator* obj)
{
	char buffer[2048] = {0};
	SP_DEVINFO_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.cbSize = sizeof(data);
	for (;SetupDiEnumDeviceInfo(obj->hDevInfo, obj->index, &data); ++obj->index) {
		if (!SetupDiGetDeviceRegistryPropertyA(obj->hDevInfo, &data, SPDRP_HARDWAREID, NULL, 
			(PBYTE)buffer, sizeof(buffer), NULL)) {
			break;
		}
		for (int i = 0; i < 1024 && buffer[i]; ++i) {
			buffer[i] = tolower(buffer[i]);
		}
		if (!strstr(buffer, "vid_1a86")) {
			continue;
		}
		if (!strstr(buffer, "pid_7523")) {
			continue;
		}

		if (!SetupDiGetDeviceRegistryPropertyA(obj->hDevInfo, &data, SPDRP_CLASS, NULL,
			(PBYTE)buffer, sizeof(buffer), NULL)) {
			break;
		}
		for (int i = 0; i < 1024 && buffer[i]; ++i) {
			buffer[i] = tolower(buffer[i]);
		}
		if (!strstr(buffer, "ports")) {
			continue;
		}

		if (!SetupDiGetDeviceRegistryPropertyA(obj->hDevInfo, &data, SPDRP_FRIENDLYNAME, NULL,
			(PBYTE)buffer, sizeof(buffer), NULL)) {
			break;
		}
		const char com_pattern[] = "(COM";
		char* start = strstr(buffer, com_pattern);
		if (!start) {
			continue;
		}
		start += (sizeof(com_pattern)/sizeof(char)) - 1;
		int num = atoi(start);
		if (num<0 || num > 256) {
			continue;
		}
		snprintf(obj->name_buffer, sizeof(obj->name_buffer), "\\\\.\\COM%d", num);
		++obj->index;
		return obj->name_buffer;
	}
	obj->index = 0;
	return NULL;
}


int AMC4030_DeviceEnumerator_GetCurrentFriendlyName(AMC4030_DeviceEnumerator* obj, char* name_buffer, int buffer_size)
{
	if (obj->index <= 0) {
		return -1;
	}
	SP_DEVINFO_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.cbSize = sizeof(data);
	if (!SetupDiEnumDeviceInfo(obj->hDevInfo, obj->index - 1, &data)) {
		return -1;
	}
	if (!name_buffer || !buffer_size) {
		DWORD size;
		if (!SetupDiGetDeviceRegistryPropertyA(obj->hDevInfo, &data, SPDRP_FRIENDLYNAME, NULL,
			NULL, 0,  &size)) {
			return -1;
		}
		return size;
	}
	else {
		if (!SetupDiGetDeviceRegistryPropertyA(obj->hDevInfo, &data, SPDRP_FRIENDLYNAME, NULL,
			name_buffer, buffer_size, NULL)) {
			return -1;
		}
	}
	return 0;
}
