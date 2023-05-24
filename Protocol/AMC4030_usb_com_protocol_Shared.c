#include "AMC4030_usb_com_protocol_Internal.h"

int AMC4030_usb_protocol_WriteU32(AMC4030_usb_protocol_context* obj, uint32_t data)
{
	if (obj->write_bytes + 4 > 532) {
		return AMC4030_USB_PROTO_INPUT_BUFFER_OVERFLOW;
	}
	uint16_t* dst = (uint16_t*)(obj->send_buffer + obj->write_bytes);
	memcpy(dst, &data, 4);
	obj->write_bytes += 4;
	return 0;
}

int AMC4030_usb_protocol_WriteU16(AMC4030_usb_protocol_context* obj, uint16_t data)
{
	if (obj->write_bytes + 2 > 532) {
		return AMC4030_USB_PROTO_INPUT_BUFFER_OVERFLOW;
	}
	uint16_t* dst = (uint16_t*)(obj->send_buffer + obj->write_bytes);
	memcpy(dst, &data, 2);
	obj->write_bytes += 2;
	return 0;
}

int AMC4030_usb_protocol_WriteU8(AMC4030_usb_protocol_context* obj, uint8_t data)
{
	if (obj->write_bytes + 1 > 532) {
		return AMC4030_USB_PROTO_INPUT_BUFFER_OVERFLOW;
	}
	obj->send_buffer[obj->write_bytes] = data;
	obj->write_bytes += 1;
	return 0;
}

int AMC4030_usb_protocol_Writefloat32(AMC4030_usb_protocol_context* obj, float data)
{
	if (obj->write_bytes + 4 > 532) {
		return AMC4030_USB_PROTO_INPUT_BUFFER_OVERFLOW;
	}
	float* dst = (float*)(obj->send_buffer + obj->write_bytes);
	memcpy(dst, &data, 4);
	obj->write_bytes += 4;
	return 0;
}

int AMC4030_usb_protocol_WriteBuffer(AMC4030_usb_protocol_context* obj, uint16_t len, const void* data)
{
	if (obj->write_bytes + len > 532) {
		return AMC4030_USB_PROTO_INPUT_BUFFER_OVERFLOW;
	}
	memcpy(obj->send_buffer + obj->write_bytes, data, len);
	obj->write_bytes += len;
	return 0;
}

int AMC4030_usb_protocol_StartCommand(AMC4030_usb_protocol_context* obj, uint16_t code)
{
	int error = 0;
	obj->current_command = code;
	// first two bytes is buffer size
	obj->write_bytes = 2;
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_WriteU16(obj, code));
FAIL:
	return error;
}

unsigned char AMC4030_usb_protocol_calculate_checksum(uint16_t count, const unsigned char* buffer)
{
	char sum = 0;
	for (uint16_t i = 0; i < count; ++i) {
		sum += buffer[i];
	}
	return -sum;
}

int AMC4030_usb_protocol_ReadFileData(AMC4030_usb_protocol_context* obj, uint8_t file_id, uint16_t start_offset, uint16_t len, void* dest)
{

	if (!obj || !dest) {
		return AMC4030_USB_PROTO_NULL_POINTER;
	}
	if (len > 500 || file_id < 0 || file_id >3) {
		return AMC4030_USB_PROTO_INPUT_PARAMETER_ERROR;
	}
	int error = 0;
	AMC4030_usb_protocol_LockContext(obj);
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_StartCommand(obj, 257));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_WriteU8(obj, file_id));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_WriteU16(obj, start_offset));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_WriteU16(obj, len));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_FinishCommand(obj));

	if (obj->read_bytes != len + 5) {
		GOTO_FAIL_IFNEG(AMC4030_USB_PROTO_RESPONSE_PARAMETER_ERROR);
	}
	memcpy(dest, obj->recv_buffer + 5, len);
FAIL:
	AMC4030_usb_protocol_UnLockContext(obj);
	return error;
}

int AMC4030_usb_protocol_WriteFileData(AMC4030_usb_protocol_context* obj, uint8_t file_id, uint16_t start_offset, uint16_t len, const void* data)
{
	if (!obj || !data) {
		return AMC4030_USB_PROTO_NULL_POINTER;
	}
	if (len > 500 || file_id<0 || file_id >3) {
		return AMC4030_USB_PROTO_INPUT_PARAMETER_ERROR;
	}
	int error = 0;
	AMC4030_usb_protocol_LockContext(obj);
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_StartCommand(obj, 258));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_WriteU8(obj, file_id));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_WriteU16(obj, start_offset));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_WriteU16(obj, len));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_WriteBuffer(obj, len, data));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_FinishCommand(obj));
FAIL:
	AMC4030_usb_protocol_UnLockContext(obj);
	return error;
}

int AMC4030_usb_protocol_GetMachinStatus(AMC4030_usb_protocol_context* obj, AMC4030_MachinStatus* status)
{
	if (!obj || !status) {
		return AMC4030_USB_PROTO_NULL_POINTER;
	}
	int error = 0;
	AMC4030_usb_protocol_LockContext(obj);
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_StartCommand(obj, 259));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_FinishCommand(obj));
	if (obj->read_bytes != sizeof(AMC4030_MachinStatus) + 6) {
		GOTO_FAIL_IFNEG(AMC4030_USB_PROTO_RESPONSE_PARAMETER_ERROR);
	}
	memcpy(status, obj->recv_buffer + 5, sizeof(AMC4030_MachinStatus));
FAIL:
	AMC4030_usb_protocol_UnLockContext(obj);
	return error;
}

int AMC4030_usb_protocol_Jog(AMC4030_usb_protocol_context* obj, uint8_t axis_no, float distance, float speed)
{
	if (!obj) {
		return AMC4030_USB_PROTO_NULL_POINTER;
	}
	if (axis_no < 0 || axis_no > 2) {
		return AMC4030_USB_PROTO_INPUT_PARAMETER_ERROR;
	}
	int error = 0;
	AMC4030_usb_protocol_LockContext(obj);
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_StartCommand(obj, 260));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_WriteU8(obj, axis_no));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_Writefloat32(obj, distance));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_Writefloat32(obj, speed));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_FinishCommand(obj));
FAIL:
	AMC4030_usb_protocol_UnLockContext(obj);
	return error;
}

int AMC4030_usb_protocol_HomeByAxis(AMC4030_usb_protocol_context* obj, uint8_t homeMask)
{
	if (!obj) {
		return AMC4030_USB_PROTO_NULL_POINTER;
	}
	if (homeMask & AMC4030_USB_PROTO_MASK_AXIS_INVALID) {
		return AMC4030_USB_PROTO_INPUT_PARAMETER_ERROR;
	}
	int error = 0;
	AMC4030_usb_protocol_LockContext(obj);
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_StartCommand(obj, 261));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_WriteU8(obj, homeMask));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_FinishCommand(obj));
FAIL:
	AMC4030_usb_protocol_UnLockContext(obj);
	return error;
}


int AMC4030_usb_protocol_SetCoil(AMC4030_usb_protocol_context* obj, uint8_t coil, uint8_t state)
{
	if (!obj) {
		return AMC4030_USB_PROTO_NULL_POINTER;
	}
	if (coil < 1 || coil > 4) {
		return AMC4030_USB_PROTO_INPUT_PARAMETER_ERROR;
	}

	int error = 0;

	AMC4030_usb_protocol_LockContext(obj);
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_StartCommand(obj, 262));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_WriteU8(obj, coil));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_WriteU8(obj, state > 0));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_FinishCommand(obj));
FAIL:
	AMC4030_usb_protocol_UnLockContext(obj);
	return error;
}

int AMC4030_usb_protocol_StopAll(AMC4030_usb_protocol_context* obj)
{
	if (!obj) {
		return AMC4030_USB_PROTO_NULL_POINTER;
	}
	int error = 0;
	AMC4030_usb_protocol_LockContext(obj);
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_StartCommand(obj, 263));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_FinishCommand(obj));
FAIL:
	AMC4030_usb_protocol_UnLockContext(obj);
	return error;
}

int AMC4030_usb_protocol_StopByAxis(AMC4030_usb_protocol_context* obj, uint8_t stopMask)
{
	if (!obj) {
		return AMC4030_USB_PROTO_NULL_POINTER;
	}
	if (stopMask & AMC4030_USB_PROTO_MASK_AXIS_INVALID) {
		return AMC4030_USB_PROTO_INPUT_PARAMETER_ERROR;
	}
	int error = 0;
	AMC4030_usb_protocol_LockContext(obj);
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_StartCommand(obj, 264));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_WriteU8(obj, stopMask));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_FinishCommand(obj));
FAIL:
	AMC4030_usb_protocol_UnLockContext(obj);
	return error;
}

// unknown function
int AMC4030_usb_protocol_FastLine3(AMC4030_usb_protocol_context* obj, float a1, float a2, float a3, float a4, float a5)
{
	if (!obj) {
		return AMC4030_USB_PROTO_NULL_POINTER;
	}
	int error = 0;
	AMC4030_usb_protocol_LockContext(obj);
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_StartCommand(obj, 271));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_Writefloat32(obj, a1));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_Writefloat32(obj, a2));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_Writefloat32(obj, a3));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_Writefloat32(obj, a4));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_Writefloat32(obj, a5));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_FinishCommand(obj));
FAIL:
	AMC4030_usb_protocol_UnLockContext(obj);
	return error;
}

int AMC4030_usb_protocol_PauseAll(AMC4030_usb_protocol_context* obj)
{
	if (!obj) {
		return AMC4030_USB_PROTO_NULL_POINTER;
	}
	int error = 0;
	AMC4030_usb_protocol_LockContext(obj);
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_StartCommand(obj, 272));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_FinishCommand(obj));
FAIL:
	AMC4030_usb_protocol_UnLockContext(obj);
	return error;
}

int AMC4030_usb_protocol_ResumeAll(AMC4030_usb_protocol_context* obj)
{
	if (!obj) {
		return AMC4030_USB_PROTO_NULL_POINTER;
	}
	int error = 0;
	AMC4030_usb_protocol_LockContext(obj);
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_StartCommand(obj, 273));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_FinishCommand(obj));
FAIL:
	AMC4030_usb_protocol_UnLockContext(obj);
	return error;
}

int AMC4030_usb_protocol_GetFilenames(AMC4030_usb_protocol_context* obj, AMC4030_FileList* filelist)
{
	if (!obj || !filelist) {
		return AMC4030_USB_PROTO_NULL_POINTER;
	}
	int error = 0;
	AMC4030_usb_protocol_LockContext(obj);
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_StartCommand(obj, 274));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_FinishCommand(obj));

	if (obj->read_bytes != sizeof(AMC4030_FileList) + 6) {
		GOTO_FAIL_IFNEG(AMC4030_USB_PROTO_RESPONSE_PARAMETER_ERROR);
	}
	memcpy(filelist, obj->recv_buffer + 5, sizeof(AMC4030_FileList));
FAIL:
	AMC4030_usb_protocol_UnLockContext(obj);
	return error;
}

int AMC4030_usb_protocol_DeleteFile(AMC4030_usb_protocol_context* obj, uint8_t filename_len, char* filename)
{
	if (!obj || !filename) {
		return AMC4030_USB_PROTO_NULL_POINTER;
	}
	int error = 0;
	AMC4030_usb_protocol_LockContext(obj);
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_StartCommand(obj, 275));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_WriteU8(obj, filename_len));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_WriteBuffer(obj, filename_len, filename));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_FinishCommand(obj));
FAIL:
	AMC4030_usb_protocol_UnLockContext(obj);
	return error;
}

int AMC4030_usb_protocol_StartAutoRun(AMC4030_usb_protocol_context* obj, uint8_t nType, uint8_t nID, uint8_t filename_len, const char* filename, uint8_t run_on_power)
{
	if (!obj || !filename) {
		return AMC4030_USB_PROTO_NULL_POINTER;
	}
	int error = 0;
	AMC4030_usb_protocol_LockContext(obj);
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_StartCommand(obj, 276));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_WriteU8(obj, nType));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_WriteU8(obj, nID));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_WriteU8(obj, filename_len));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_WriteU8(obj, run_on_power > 0));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_WriteBuffer(obj, filename_len, filename));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_FinishCommand(obj));
FAIL:
	AMC4030_usb_protocol_UnLockContext(obj);
	return error;
}

int AMC4030_usb_protocol_InvokeUserCommand(AMC4030_usb_protocol_context* obj, uint16_t code, uint16_t send_len, const void* to_send, uint16_t* recv_len, void* to_recv)
{
	// SendData (code 266) and ReadData (code 267)
	if (!obj || !to_send) {
		return AMC4030_USB_PROTO_NULL_POINTER;
	}
	int error = 0;
	if (send_len >= 532 - 5) {
		return AMC4030_USB_PROTO_INPUT_PARAMETER_ERROR;
	}
	AMC4030_usb_protocol_LockContext(obj);
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_StartCommand(obj, code));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_WriteBuffer(obj, send_len, to_send));
	GOTO_FAIL_IFNEG(AMC4030_usb_protocol_FinishCommand(obj));
	if (recv_len) {
		*recv_len = obj->read_bytes;
	}
	if (to_recv) {
		memcpy(to_recv, obj->recv_buffer, obj->read_bytes);
	}
FAIL:
	AMC4030_usb_protocol_UnLockContext(obj);
	return error;
}