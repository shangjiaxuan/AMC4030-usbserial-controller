#include "AMC4030_usb_com_protocol_Internal.h"

void AMC4030_usb_protocol_LockContext(AMC4030_usb_protocol_context* obj)
{
	EnterCriticalSection(&obj->lock);
}

void AMC4030_usb_protocol_UnLockContext(AMC4030_usb_protocol_context* obj)
{
	LeaveCriticalSection(&obj->lock);
}

int AMC4030_usb_protocol_FinishCommand(AMC4030_usb_protocol_context* obj)
{
	// make checksum and write size header
	{
		if (obj->write_bytes + 1 > 532) {
			return AMC4030_USB_PROTO_INPUT_BUFFER_OVERFLOW;
		}
		obj->write_bytes += 1;
		memcpy(obj->send_buffer, &obj->write_bytes, 2);
		obj->send_buffer[obj->write_bytes - 1] = AMC4030_usb_protocol_calculate_checksum(obj->write_bytes - 1, obj->send_buffer);
	}
	// send and recieve
	{
		if (!PurgeComm(obj->hFile, PURGE_RXABORT | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_TXCLEAR)) {
			return AMC4030_USB_PROTO_COMM_WRITE_ERROR;
		}
		if (!ClearCommError(obj->hFile, NULL, NULL)) {
			return AMC4030_USB_PROTO_COMM_WRITE_ERROR;
		}
		DWORD written = 0;
		if (!WriteFile(obj->hFile, obj->send_buffer, obj->write_bytes, &written, NULL) || written != obj->write_bytes) {
			return AMC4030_USB_PROTO_COMM_WRITE_ERROR;
		}
		obj->read_bytes = 0;
		uint16_t expected_size = 5;
		int expected_size_read = 0;
		int four_byte_verified = 0;
		int fifth_byte_verified = 0;
		int poll_count = 0;
		while (poll_count < 200) {
			DWORD errors;
			COMSTAT stat;
			if (!ClearCommError(obj->hFile, &errors, &stat)) {
				return AMC4030_USB_PROTO_COMM_READ_ERROR;
			}
			if (!stat.cbInQue) {
				++poll_count;
				Sleep(1);
				continue;
			}
			DWORD read_bytes;
			if (!ReadFile(obj->hFile, obj->recv_buffer + obj->read_bytes, ((unsigned)512- obj->read_bytes< stat.cbInQue)? 512 - (unsigned)obj->read_bytes: stat.cbInQue, &read_bytes, NULL)) {
				return AMC4030_USB_PROTO_COMM_READ_ERROR;
			}
			obj->read_bytes += (uint16_t)read_bytes;
			if (!expected_size_read && obj->read_bytes > 2) {
				memcpy(&expected_size, obj->recv_buffer, 2);
				if (expected_size < 5) {
					return AMC4030_USB_PROTO_RESPONSE_PARAMETER_ERROR;
				}
				expected_size_read = 1;
			}
			if (!four_byte_verified && obj->read_bytes >= 4) {
				if (memcmp(obj->recv_buffer, "erro", 4) == 0) {
					return AMC4030_USB_PROTO_DEVICE_RECIEVE_ERROR;
				}
				uint16_t code;
				memcpy(&code, obj->recv_buffer + 2, 2);
				if (code != obj->current_command) {
					return AMC4030_USB_PROTO_RESPONSE_PARAMETER_ERROR;
				}
				four_byte_verified = 1;
			}
			if (!fifth_byte_verified && obj->read_bytes >= 5) {
				if (obj->recv_buffer[4] != 0) {
					return AMC4030_USB_PROTO_RESPONSE_PARAMETER_ERROR;
				}
				fifth_byte_verified = 1;
			}
			if (obj->read_bytes >= 512) {
				return AMC4030_USB_PROTO_COMM_READ_ERROR;
			}
		}
		if (obj->read_bytes < 5) {
			return AMC4030_USB_PROTO_COMM_READ_ERROR;
		}
		if (!expected_size_read || obj->read_bytes != expected_size) {
			return AMC4030_USB_PROTO_COMM_READ_ERROR;
		}
	}
	// validate response
	{
		if (obj->recv_buffer[obj->read_bytes - 1] != AMC4030_usb_protocol_calculate_checksum(obj->read_bytes - 1, obj->recv_buffer)) {
			return AMC4030_USB_PROTO_RESPONSE_FORMAT_ERROR;
		}
	}
	return 0;
}

#include <stdlib.h>
#include <stdio.h>

AMC4030_usb_protocol_context* AMC4030_usb_protocol_Create(const char* commfile, int* error)
{
	AMC4030_usb_protocol_context* obj = NULL;
	HRESULT res = S_OK;
	int mapped_error = AMC4030_USB_PROTO_NO_ERROR;
	HANDLE hFile = CreateFileA(commfile, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		res = GetLastError();
		switch (HRESULT_CODE(res)) {
			default:
				mapped_error = AMC4030_USB_PROTO_COMM_INIT_GENERIC_ERROR;
				break;
			case ERROR_ACCESS_DENIED:
				mapped_error = AMC4030_USB_PROTO_COMM_INIT_FILE_NO_PERM;
				break;
			case ERROR_SHARING_VIOLATION:
				mapped_error = AMC4030_USB_PROTO_COMM_INIT_FILE_IN_USE;
				break;
			case ERROR_FILE_NOT_FOUND:
				mapped_error = AMC4030_USB_PROTO_COMM_INIT_FILE_NOT_FOUND;
				break;
		}
		goto CLEANUP;
	}
	COMMTIMEOUTS timeouts = {0};
	// reading timeout = timeoutconstant + multipler*bytes
	// between read timeout = interval
	timeouts.ReadIntervalTimeout = MAXWORD;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant = 0;
	timeouts.WriteTotalTimeoutMultiplier = 20;
	timeouts.WriteTotalTimeoutConstant = 100;
	if (!SetCommTimeouts(hFile, &timeouts)) {
		mapped_error = AMC4030_USB_PROTO_COMM_INIT_MODE_FAILED;
		goto CLEANUP;
	}
	if (!SetCommMask(hFile, EV_RXCHAR)) {
		mapped_error = AMC4030_USB_PROTO_COMM_INIT_MODE_FAILED;
		goto CLEANUP;
	}
	DCB dcb_info = { 0 };
	if (!GetCommState(hFile, &dcb_info)) {
		mapped_error = AMC4030_USB_PROTO_COMM_INIT_MODE_FAILED;
		goto CLEANUP;
	}
	if (!BuildCommDCBA("baud=115200 parity=N data=8 stop=1", &dcb_info)) {
		mapped_error = AMC4030_USB_PROTO_COMM_INIT_MODE_FAILED;
		goto CLEANUP;
	}
	if (!SetCommState(hFile, &dcb_info)) {
		mapped_error = AMC4030_USB_PROTO_COMM_INIT_MODE_FAILED;
		goto CLEANUP;
	}
	obj = (AMC4030_usb_protocol_context*)calloc(1, sizeof(AMC4030_usb_protocol_context));
	if (obj) {
		obj->hFile = hFile;
		InitializeCriticalSection(&obj->lock);
	}
	else {
		mapped_error = AMC4030_USB_PROTO_COMM_INIT_OUT_OF_MEMORY;
		goto CLEANUP;
	}
	if (error) {
		*error = mapped_error;
	}
	return obj;
CLEANUP:
	if (hFile != INVALID_HANDLE_VALUE) {
		CloseHandle(hFile);
	}
	if (obj) {
		free(obj);
	}
	if (error) {
		*error = mapped_error;
	}
	return NULL;
}

void AMC4030_usb_protocol_Destroy(AMC4030_usb_protocol_context* obj)
{
	if (!obj) {
		return;
	}
	DeleteCriticalSection(&obj->lock);
	CloseHandle(obj->hFile);
	free(obj);
}

