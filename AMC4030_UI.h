#pragma once

#ifndef _AMC4030_UI_H_
#define _AMC4030_UI_H_

#include "../SerialDevice_Panel.h"

typedef struct AMC4030_UI_UIControls
{
	SerialDevice_UI_UIControls base;

	int config_button;
	
	int OUT1, OUT2, OUT3, OUT4;

	int X_minus, X_plus, X_home, X_stop;
	int X_speed, X_step, X_step_enabled;

	int Y_minus, Y_plus, Y_home, Y_stop;
	int Y_speed, Y_step, Y_step_enabled;

	int Z_minus, Z_plus, Z_home, Z_stop;
	int Z_speed, Z_step, Z_step_enabled;

	int pause_all, stop_all, resume_all;

	int paused, working, moving, returning, alert_present;
	int alert_code;

	int X_pos, Y_pos, Z_pos;

	int IN1, IN2, IN3, IN4;
	int ORG1, ORG2, ORG3;
}AMC4030_UI_UIControls;

#include "AMC4030_InstanceMonitor.h"

#include "../ui_shared.h"

typedef struct AMC4030_UI_Object
{
	SerialDevice_UI_Object base;
	AMC4030_UI_UIControls controls;

	AMC4030_InstanceMonitor monitor;

	CallbackChain updateHook;
	CallbackChain deviceRefreshHook;

	ButtonLongCallback X_plus;
	ButtonLongCallback X_minus;
	ButtonLongCallback Y_plus;
	ButtonLongCallback Y_minus;
	ButtonLongCallback Z_plus;
	ButtonLongCallback Z_minus;

	float X_speed;
	float X_step;
	float Y_speed;
	float Y_step;
	float Z_speed;
	float Z_step;

	DataHook X_speed_hook;
	DataHook X_step_hook;
	DataHook X_step_enable_hook;
	DataHook Y_speed_hook;
	DataHook Y_step_hook;
	DataHook Y_step_enable_hook;
	DataHook Z_speed_hook;
	DataHook Z_step_hook;
	DataHook Z_step_enable_hook;

	CallbackChain X_home;
	CallbackChain X_stop;
	CallbackChain Y_home;
	CallbackChain Y_stop;
	CallbackChain Z_home;
	CallbackChain Z_stop;

	int OUT1, OUT2, OUT3, OUT4;
	DataHook out1_hook;
	DataHook out2_hook;
	DataHook out3_hook;
	DataHook out4_hook;

	CallbackChain stop_all;
	CallbackChain pause_all;
	CallbackChain resume_all;

	int readback_out;
}AMC4030_UI_Object;


int AMC4030_UI_Object_Initialize(AMC4030_UI_Object* obj, const char* uir_file, int use_as_standalone);

void AMC4030_UI_Object_Destroy(AMC4030_UI_Object* obj);

int AMC4030_UI_Object_UpdateStatus(AMC4030_UI_Object* obj);

int AMC4030_UI_Object_StartMoveAxis(AMC4030_UI_Object* obj, uint8_t axis, float speed, float distance);
int AMC4030_UI_Object_StopAxis(AMC4030_UI_Object* obj, uint8_t axis_mask);
int AMC4030_UI_Object_HomeAxis(AMC4030_UI_Object* obj, uint8_t axis_mask);

int AMC4030_UI_Object_StopAll(AMC4030_UI_Object* obj);
int AMC4030_UI_Object_PauseAll(AMC4030_UI_Object* obj);
int AMC4030_UI_Object_ResumeAll(AMC4030_UI_Object* obj);

int AMC4030_UI_Object_SetOutput(AMC4030_UI_Object* obj, uint8_t out_no, uint8_t val);

int AMC4030_UI_Object_WaitStop(AMC4030_UI_Object* obj);

#endif
