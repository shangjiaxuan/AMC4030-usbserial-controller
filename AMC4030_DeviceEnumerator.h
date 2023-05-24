#pragma once

#ifndef _AMC4030_DEVICE_ENUMERATOR_H_
#define _AMC4030_DEVICE_ENUMERATOR_H_

typedef struct AMC4030_DeviceEnumerator AMC4030_DeviceEnumerator;

AMC4030_DeviceEnumerator* AMC4030_DeviceEnumerator_Create();
void AMC4030_DeviceEnumerator_Destroy(AMC4030_DeviceEnumerator* obj);

// resets enumerator to first element
void AMC4030_DeviceEnumerator_Reset(AMC4030_DeviceEnumerator* obj);

// returns positive com ports one by one.
// stops at -1
int AMC4030_DeviceEnumerator_Next(AMC4030_DeviceEnumerator* obj);

int AMC4030_DeviceEnumerator_GetCurrentFriendlyName(AMC4030_DeviceEnumerator* obj, char* name_buffer, int buffer_size);

#endif
