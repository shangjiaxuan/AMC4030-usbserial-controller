#pragma once

#ifndef _AMC4030_USB_COM_PROTOCOL_H_
#define _AMC4030_USB_COM_PROTOCOL_H_

#if defined(_WIN32) && defined(AMC4030_USB_PROTO_DYNAMIC)
#define AMC4030_USB_PROTO_EXPORT __declspec(dllexport)
#define AMC4030_USB_PROTO_IMPORT __declspec(dllimport)
#ifdef AMC4030_USB_PROTO_BUILD
#define AMC4030_USB_PROTO_SYMBOL AMC4030_USB_PROTO_EXPORT
#else
#define AMC4030_USB_PROTO_SYMBOL AMC4030_USB_PROTO_IMPORT
#endif
#else
#define AMC4030_USB_PROTO_EXPORT
#define AMC4030_USB_PROTO_IMPORT
#define AMC4030_USB_PROTO_SYMBOL
#endif

#define AMC4030_USB_PROTO_NO_ERROR						0
#define AMC4030_USB_PROTO_DEVICE_RECIEVE_ERROR			-1
#define AMC4030_USB_PROTO_DEVICE_UNRECOGINZED_COMMAND	-2
#define AMC4030_USB_PROTO_INPUT_PARAMETER_ERROR			-3
#define AMC4030_USB_PROTO_INPUT_BUFFER_OVERFLOW			-4
#define AMC4030_USB_PROTO_RESPONSE_PARAMETER_ERROR		-5
#define AMC4030_USB_PROTO_RESPONSE_FORMAT_ERROR			-6
#define AMC4030_USB_PROTO_COMM_WRITE_ERROR				-7
#define AMC4030_USB_PROTO_COMM_READ_ERROR				-8
#define AMC4030_USB_PROTO_COMM_INIT_GENERIC_ERROR		-9
#define AMC4030_USB_PROTO_COMM_DISPOSE_ERROR			-10
#define AMC4030_USB_PROTO_NULL_POINTER					-11
#define AMC4030_USB_PROTO_COMM_INIT_FILE_NOT_FOUND		-12
#define AMC4030_USB_PROTO_COMM_INIT_FILE_IN_USE			-13
#define AMC4030_USB_PROTO_COMM_INIT_FILE_NO_PERM		-14
#define AMC4030_USB_PROTO_COMM_INIT_MODE_FAILED			-15
#define AMC4030_USB_PROTO_COMM_INIT_OUT_OF_MEMORY		-16

#define AMC4030_USB_PROTO_MASK_AXIS_X 1
#define AMC4030_USB_PROTO_MASK_AXIS_Y 2
#define AMC4030_USB_PROTO_MASK_AXIS_Z 4
#define AMC4030_USB_PROTO_MASK_AXIS_INVALID (0xff ^ (AMC4030_USB_PROTO_MASK_AXIS_X | AMC4030_USB_PROTO_MASK_AXIS_Y | AMC4030_USB_PROTO_MASK_AXIS_Z))

#include <stdint.h>

typedef struct AMC4030_MachinStatus
{
	uint32_t	dwWorkStatus;		// D0: paused D1£ºworking D2£ºpoint to point moving D3 returing to home D4 alarm
	uint8_t		dwHomeDone;			// If home is correct before. D0:X done before D1:Y done before D2:Z done before
	uint8_t		nID;				// device ID
	uint16_t	FirmVer;			// firmware version
	int32_t		nPos[3];			// 100000 times Axis position
	uint32_t	RealSpeed[3];		// 100000 times Axis speed
	uint32_t	nAlmCode;			// alarm code
	uint16_t	dwInputStatus;		// input coil status D0-IN1; D1-IN2; D3-IN3; D4-IN4; D5-ORG1; D6-ORG2; D7-ORG3;
	uint16_t 	dwOutputStatus;		// output coil status
	uint32_t	Rsv[4];
}AMC4030_MachinStatus;

typedef struct AMC4030_FileList
{
	char filename[11][10];
	int file_count;
}AMC4030_FileList;

typedef struct AMC4030_usb_protocol_context AMC4030_usb_protocol_context;

#ifdef __cplusplus
extern "C" {
#endif

AMC4030_USB_PROTO_SYMBOL AMC4030_usb_protocol_context* AMC4030_usb_protocol_Create(const char* commfile, int* error);

AMC4030_USB_PROTO_SYMBOL void AMC4030_usb_protocol_Destroy(AMC4030_usb_protocol_context* obj);

// command code 257
AMC4030_USB_PROTO_SYMBOL int AMC4030_usb_protocol_ReadFileData(AMC4030_usb_protocol_context* obj, uint8_t file_id, uint16_t start_offset, uint16_t len, void* dest);
// command code 258
AMC4030_USB_PROTO_SYMBOL int AMC4030_usb_protocol_WriteFileData(AMC4030_usb_protocol_context* obj, uint8_t file_id, uint16_t start_offset, uint16_t len, const void* data);
// command code 259
AMC4030_USB_PROTO_SYMBOL int AMC4030_usb_protocol_GetMachinStatus(AMC4030_usb_protocol_context* obj, AMC4030_MachinStatus* status);
// command code 260
AMC4030_USB_PROTO_SYMBOL int AMC4030_usb_protocol_Jog(AMC4030_usb_protocol_context* obj, uint8_t axis_no, float distance, float speed);
// command code 261
AMC4030_USB_PROTO_SYMBOL int AMC4030_usb_protocol_HomeByAxis(AMC4030_usb_protocol_context* obj, uint8_t homeMask);
// command code 262
AMC4030_USB_PROTO_SYMBOL int AMC4030_usb_protocol_SetCoil(AMC4030_usb_protocol_context* obj, uint8_t coil, uint8_t state);
// command code 263
AMC4030_USB_PROTO_SYMBOL int AMC4030_usb_protocol_StopAll(AMC4030_usb_protocol_context* obj);
// command code 264
AMC4030_USB_PROTO_SYMBOL int AMC4030_usb_protocol_StopByAxis(AMC4030_usb_protocol_context* obj, uint8_t stopMask);

// command code 265 unknown usage

// sends message with payload, fills all response
// reserved not used 
//	command code reserved 266 for no response payload write
//	command code reserved 267 for response payload write + read
AMC4030_USB_PROTO_SYMBOL int AMC4030_usb_protocol_InvokeUserCommand(AMC4030_usb_protocol_context* obj, uint16_t code, uint16_t send_len, const void* to_send, uint16_t* recv_len, void* to_recv);

/*
	command code 268 reserved for "DownloadSystemCfg"
		meaning: read ini -> convert to 928 byte binary structure -> send to device in two runs
	command code 269 reserved for "UploadSystemCfg"
		meaning: fetch device binary 928 byte structure in two runs -> convert to ini format -> write to file
	command code 270 reserved for "DowloadFile"
		meaning: read file (1 for ini config, 2 for elo compiled movement script, 3 for bin firmware data)
					-> if ini convert to 928 byte structure -> send content in multiple runs to device using command 258 `WriteFileData`
*/

// implemented but unsupported...
// command code 271
AMC4030_USB_PROTO_SYMBOL int AMC4030_usb_protocol_FastLine3(AMC4030_usb_protocol_context* obj, float a1, float a2, float a3, float a4, float a5);
// command code 272
AMC4030_USB_PROTO_SYMBOL int AMC4030_usb_protocol_PauseAll(AMC4030_usb_protocol_context* obj);
// command code 273
AMC4030_USB_PROTO_SYMBOL int AMC4030_usb_protocol_ResumeAll(AMC4030_usb_protocol_context* obj);
// command code 274
AMC4030_USB_PROTO_SYMBOL int AMC4030_usb_protocol_GetFilenames(AMC4030_usb_protocol_context* obj, AMC4030_FileList* filelist);
// command code 275
AMC4030_USB_PROTO_SYMBOL int AMC4030_usb_protocol_DeleteFile(AMC4030_usb_protocol_context* obj, uint8_t filename_len, char* filename);
// command code 276
AMC4030_USB_PROTO_SYMBOL int AMC4030_usb_protocol_StartAutoRun(AMC4030_usb_protocol_context* obj, uint8_t nType, uint8_t nID, uint8_t filename_len, const char* filename, uint8_t run_on_power);

#ifdef __cplusplus
}
#endif

#endif

