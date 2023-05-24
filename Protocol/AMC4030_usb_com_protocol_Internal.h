#pragma once
#ifndef _AMC4030_USB_COM_PROTOCOL_INTERNAL_H_
#define _AMC4030_USB_COM_PROTOCOL_INTERNAL_H_
#include "AMC4030_usb_com_protocol.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <stdint.h>

#define GOTO_FAIL_IFNEG(fCall) if (error = (fCall), error < 0) \
{goto FAIL;} else

typedef struct AMC4030_usb_protocol_context
{
#ifdef _WIN32
	HANDLE hFile;
	CRITICAL_SECTION lock;
#endif
	uint16_t current_command;
	uint16_t write_bytes;
	uint16_t read_bytes;
	unsigned char send_buffer[532];
	unsigned char recv_buffer[532];
}AMC4030_usb_protocol_context;

unsigned char AMC4030_usb_protocol_calculate_checksum(uint16_t count, const unsigned char* buffer);
int AMC4030_usb_protocol_FinishCommand(AMC4030_usb_protocol_context* obj);

void AMC4030_usb_protocol_LockContext(AMC4030_usb_protocol_context* obj);
void AMC4030_usb_protocol_UnLockContext(AMC4030_usb_protocol_context* obj);

#endif