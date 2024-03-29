#pragma once
#ifndef _AMC4030_UI_INTERNAL_H_
#define _AMC4030_UI_INTERNAL_H_

#include "AMC4030_UI.h"


#include "library_required/SerialDevice_Panel.h"

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

#include "library_required/ThreadCompat.h"

typedef struct AMC4030_UI_Object
{
	SerialDevice_UI_Object base;
	AMC4030_UI_UIControls controls;

	AMC4030_InstanceMonitor monitor;

	COND_VAR wait_stop;
	MUTEX_T wait_stop_mtx;

	CallbackChain updateHook;
	CallbackChain deviceRefreshHook;

	int X_step_enabled, Y_step_enabled, Z_step_enabled;
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

	int allow_control;
}AMC4030_UI_Object;

int AMC4030_UI_Object_Initialize(AMC4030_UI_Object* obj, const char* uir_file, int use_as_standalone);
void AMC4030_UI_Object_Destroy(AMC4030_UI_Object* obj);

#endif