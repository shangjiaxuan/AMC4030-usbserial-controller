#include "AMC4030_UI_Internal.h"

#include <userint.h>

#ifndef errChk
#define errChk(fCall) if (error = (fCall), error < 0) \
{goto Error;} else
#endif

static int AMC4030_UI_Object_OnCommit_Home_X(AMC4030_UI_Object* obj)
{
	int error = UIENoError;
	errChk(AMC4030_UI_Object_HomeAxis(obj, AMC4030_USB_PROTO_MASK_AXIS_X));
Error:
	return error;
}

static int AMC4030_UI_Object_OnCommit_Stop_X(AMC4030_UI_Object* obj)
{
	int error = UIENoError;
	errChk(AMC4030_UI_Object_StopAxis(obj, AMC4030_USB_PROTO_MASK_AXIS_X));
Error:
	return error;
}

static int AMC4030_UI_Object_OnPress_X_plus(AMC4030_UI_Object* obj)
{
	int error = UIENoError;
	errChk(AMC4030_UI_Object_StartMoveAxis(obj,  0, obj->X_speed, 20000));
Error:
	return error;
}

static int AMC4030_UI_Object_OnRelease_X_plus(AMC4030_UI_Object* obj)
{
	int error = UIENoError;
	if (!obj->X_step_enabled) {
		errChk(AMC4030_UI_Object_StopAxis(obj, AMC4030_USB_PROTO_MASK_AXIS_X));
	}
	else {
		errChk(AMC4030_UI_Object_StartMoveAxis(obj, 0, obj->X_speed, obj->X_step));
	}
Error:
	return error;
}

static int AMC4030_UI_Object_OnPress_X_minus(AMC4030_UI_Object* obj)
{
	int error = UIENoError;
	errChk(AMC4030_UI_Object_StartMoveAxis(obj, 0, obj->X_speed, -20000));
Error:
	return error;
}

static int AMC4030_UI_Object_OnRelease_X_minus(AMC4030_UI_Object* obj)
{
	int error = UIENoError;
	if (!obj->X_step_enabled) {
		errChk(AMC4030_UI_Object_StopAxis(obj, AMC4030_USB_PROTO_MASK_AXIS_X));
	}
	else {
		errChk(AMC4030_UI_Object_StartMoveAxis(obj, 0, obj->X_speed, -obj->X_step));
	}
Error:
	return error;
}

static int AMC4030_UI_Object_OnCommit_Home_Y(AMC4030_UI_Object* obj)
{
	int error = UIENoError;
	errChk(AMC4030_UI_Object_HomeAxis(obj, AMC4030_USB_PROTO_MASK_AXIS_Y));
Error:
	return error;
}

static int AMC4030_UI_Object_OnCommit_Stop_Y(AMC4030_UI_Object* obj)
{
	int error = UIENoError;
	errChk(AMC4030_UI_Object_StopAxis(obj, AMC4030_USB_PROTO_MASK_AXIS_Y));
Error:
	return error;
}

static int AMC4030_UI_Object_OnPress_Y_plus(AMC4030_UI_Object* obj)
{
	int error = UIENoError;
	errChk(AMC4030_UI_Object_StartMoveAxis(obj, 1, obj->Y_speed, 20000));
Error:
	return error;
}

static int AMC4030_UI_Object_OnRelease_Y_plus(AMC4030_UI_Object* obj)
{
	int error = UIENoError;
	if (!obj->Y_step_enabled) {
		errChk(AMC4030_UI_Object_StopAxis(obj, AMC4030_USB_PROTO_MASK_AXIS_Y));
	}
	else {
		errChk(AMC4030_UI_Object_StartMoveAxis(obj, 1, obj->Y_speed, obj->Y_step));
	}
Error:
	return error;
}


static int AMC4030_UI_Object_OnPress_Y_minus(AMC4030_UI_Object* obj)
{
	int error = UIENoError;
	errChk(AMC4030_UI_Object_StartMoveAxis(obj, 1, obj->Y_speed, -20000));
Error:
	return error;
}

static int AMC4030_UI_Object_OnRelease_Y_minus(AMC4030_UI_Object* obj)
{
	int error = UIENoError;
	if (!obj->Y_step_enabled) {
		errChk(AMC4030_UI_Object_StopAxis(obj, AMC4030_USB_PROTO_MASK_AXIS_Y));
	}
	else {
		errChk(AMC4030_UI_Object_StartMoveAxis(obj, 1, obj->Y_speed, -obj->Y_step));
	}
Error:
	return error;
}

static int AMC4030_UI_Object_OnCommit_Home_Z(AMC4030_UI_Object* obj)
{
	int error = UIENoError;
	errChk(AMC4030_UI_Object_HomeAxis(obj, AMC4030_USB_PROTO_MASK_AXIS_Z));
Error:
	return error;
}

static int AMC4030_UI_Object_OnCommit_Stop_Z(AMC4030_UI_Object* obj)
{
	int error = UIENoError;
	errChk(AMC4030_UI_Object_StopAxis(obj, AMC4030_USB_PROTO_MASK_AXIS_Z));
Error:
	return error;
}

static int AMC4030_UI_Object_OnPress_Z_plus(AMC4030_UI_Object* obj)
{
	int error = UIENoError;
	errChk(AMC4030_UI_Object_StartMoveAxis(obj, 2, obj->Z_speed, 20000));
Error:
	return error;
}

static int AMC4030_UI_Object_OnRelease_Z_plus(AMC4030_UI_Object* obj)
{
	int error = UIENoError;
	if (!obj->Z_step_enabled) {
		errChk(AMC4030_UI_Object_StopAxis(obj, AMC4030_USB_PROTO_MASK_AXIS_Z));
	}
	else {
		errChk(AMC4030_UI_Object_StartMoveAxis(obj, 2, obj->Z_speed, obj->Z_step));
	}
Error:
	return error;
}


static int AMC4030_UI_Object_OnPress_Z_minus(AMC4030_UI_Object* obj)
{
	int error = UIENoError;
	errChk(AMC4030_UI_Object_StartMoveAxis(obj, 2, obj->Z_speed, -20000));
Error:
	return error;
}

static int AMC4030_UI_Object_OnRelease_Z_minus(AMC4030_UI_Object* obj)
{
	int error = UIENoError;
	if (!obj->X_step_enabled) {
		errChk(AMC4030_UI_Object_StopAxis(obj, AMC4030_USB_PROTO_MASK_AXIS_Z));
	}
	else {
		errChk(AMC4030_UI_Object_StartMoveAxis(obj, 2, obj->Z_speed, -obj->Z_step));
	}
Error:
	return error;
}

static int AMC4030_UI_Object_Update_OUT1(AMC4030_UI_Object* obj)
{
	int error = UIENoError;
	errChk(AMC4030_UI_Object_SetOutput(obj, 1, obj->OUT1 > 0));
Error:
	return error;
}

static int AMC4030_UI_Object_Update_OUT2(AMC4030_UI_Object* obj)
{
	int error = UIENoError;
	errChk(AMC4030_UI_Object_SetOutput(obj, 2, obj->OUT2 > 0));
Error:
	return error;
}

static int AMC4030_UI_Object_Update_OUT3(AMC4030_UI_Object* obj)
{
	int error = UIENoError;
	errChk(AMC4030_UI_Object_SetOutput(obj, 3, obj->OUT3 > 0));
Error:
	return error;
}

static int AMC4030_UI_Object_Update_OUT4(AMC4030_UI_Object* obj)
{
	int error = UIENoError;
	errChk(AMC4030_UI_Object_SetOutput(obj, 4, obj->OUT4 > 0));
Error:
	return error;
}

#include "../ui_shared.h"
#include "cvi_uir_AMC4030.h"

#include "AMC4030_DeviceEnumerator.h"

int AMC4030_UI_Object_Initialize(AMC4030_UI_Object* obj, const char* uir_file, int use_as_standalone)
{
	int error = UIENoError;
	{
		obj->controls.base.hide = AMC4030_P_HIDE;
		obj->controls.base.quit = AMC4030_P_QUIT;
		obj->controls.base.device_select = AMC4030_P_PORT_SELECT;
		obj->controls.base.device_refresh = AMC4030_P_REFRESH_DEVICES;
		obj->controls.base.connect = AMC4030_P_CONNECT;

		obj->controls.config_button = AMC4030_P_CONFIG;
		obj->controls.OUT1 = AMC4030_P_OUT1;
		obj->controls.OUT2 = AMC4030_P_OUT2;
		obj->controls.OUT3 = AMC4030_P_OUT3;
		obj->controls.OUT4 = AMC4030_P_OUT4;
		obj->controls.IN1 = AMC4030_P_IN1;
		obj->controls.IN2 = AMC4030_P_IN2;
		obj->controls.IN3 = AMC4030_P_IN3;
		obj->controls.IN4 = AMC4030_P_IN4;
		obj->controls.ORG1 = AMC4030_P_ORG1;
		obj->controls.ORG2 = AMC4030_P_ORG2;
		obj->controls.ORG3 = AMC4030_P_ORG3;

		obj->controls.paused = AMC4030_P_PAUSED;
		obj->controls.working = AMC4030_P_WORKING;
		obj->controls.moving = AMC4030_P_MOVING;
		obj->controls.returning = AMC4030_P_HOME_MOVING;
		obj->controls.alert_present = AMC4030_P_ALERT;
		obj->controls.alert_code = AMC4030_P_ALERT_CODE;

		obj->controls.X_pos = AMC4030_P_POS_X;
		obj->controls.Y_pos = AMC4030_P_POS_Y;
		obj->controls.Z_pos = AMC4030_P_POS_Z;

		obj->controls.stop_all = AMC4030_P_STOP_ALL;
		obj->controls.pause_all = AMC4030_P_PAUSE_ALL;
		obj->controls.resume_all = AMC4030_P_RESUME_ALL;

		obj->controls.X_minus = AMC4030_P_MINUS_X;
		obj->controls.X_plus = AMC4030_P_PLUS_X;
		obj->controls.X_home = AMC4030_P_HOME_X;
		obj->controls.X_stop = AMC4030_P_STOP_X;
		obj->controls.X_speed = AMC4030_P_SPEED_X;
		obj->controls.X_step_enabled = AMC4030_P_USE_STEP_X;
		obj->controls.X_step = AMC4030_P_STEP_X;

		obj->controls.Y_minus = AMC4030_P_MINUS_Y;
		obj->controls.Y_plus = AMC4030_P_PLUS_Y;
		obj->controls.Y_home = AMC4030_P_HOME_Y;
		obj->controls.Y_stop = AMC4030_P_STOP_Y;
		obj->controls.Y_speed = AMC4030_P_SPEED_Y;
		obj->controls.Y_step_enabled = AMC4030_P_USE_STEP_Y;
		obj->controls.Y_step = AMC4030_P_STEP_Y;

		obj->controls.Z_minus = AMC4030_P_MINUS_Z;
		obj->controls.Z_plus = AMC4030_P_PLUS_Z;
		obj->controls.Z_home = AMC4030_P_HOME_Z;
		obj->controls.Z_stop = AMC4030_P_STOP_Z;
		obj->controls.Z_speed = AMC4030_P_SPEED_Z;
		obj->controls.Z_step_enabled = AMC4030_P_USE_STEP_Z;
		obj->controls.Z_step = AMC4030_P_STEP_Z;

		obj->base.connection_funcs = &AMC4030_USB_Connection_Vtable;
		obj->base.monitor_funcs = &AMC4030_InstanceMonitor_Vtable;
		obj->base.device_enumerator_funcs = &ch340_usbserial_port_enumerator;

		obj->base.monitor = &obj->monitor;

		int panelHandle;
		if ((panelHandle = LoadPanel(0, uir_file, AMC4030_P)) < 0)
			return panelHandle;

		obj->controls.base.panel = panelHandle;

		obj->controls.base.select_mode_array = GetCtrlArrayFromResourceID(obj->controls.base.panel, DEV_SEL_CTRLS);
		errChk(obj->controls.base.select_mode_array);
		obj->controls.base.connected_mode_array = GetCtrlArrayFromResourceID(obj->controls.base.panel, CONNECTED_CTRLS);
		errChk(obj->controls.base.connected_mode_array);

		obj->controls.base.panel = panelHandle;
		obj->base.controls = &obj->controls.base;
		errChk(SerialDevice_UI_Object_Initialize(&obj->base, use_as_standalone));
	}


	{
		obj->X_plus.onPress.hook.cb = AMC4030_UI_Object_OnPress_X_plus;
		obj->X_plus.onPress.hook.ctx = obj;
		obj->X_plus.onRelease.hook.cb = AMC4030_UI_Object_OnRelease_X_plus;
		obj->X_plus.onRelease.hook.ctx = obj;
		obj->X_plus.single_event_mode = &obj->X_step_enabled;
		errChk(InstallCtrlCallback(obj->controls.base.panel, obj->controls.X_plus, Generic_ButtonLongCall, &obj->X_plus));
		obj->X_minus.onPress.hook.cb = AMC4030_UI_Object_OnPress_X_minus;
		obj->X_minus.onPress.hook.ctx = obj;
		obj->X_minus.onRelease.hook.cb = AMC4030_UI_Object_OnRelease_X_minus;
		obj->X_minus.onRelease.hook.ctx = obj;
		obj->X_minus.single_event_mode = &obj->X_step_enabled;
		errChk(InstallCtrlCallback(obj->controls.base.panel, obj->controls.X_minus, Generic_ButtonLongCall, &obj->X_minus));
		obj->X_stop.hook.cb = AMC4030_UI_Object_OnCommit_Stop_X;
		obj->X_stop.hook.ctx = obj;
		errChk(InstallCtrlCallback(obj->controls.base.panel, obj->controls.X_stop, Generic_HookCall, &obj->X_stop));
		obj->X_home.hook.cb = AMC4030_UI_Object_OnCommit_Home_X;
		obj->X_home.hook.ctx = obj;
		errChk(InstallCtrlCallback(obj->controls.base.panel, obj->controls.X_home, Generic_HookCall, &obj->X_home));
		errChk(InstallCtrlCallback(obj->controls.base.panel, obj->controls.X_step_enabled, Generic_AnyCall, &obj->X_step_enabled));
		errChk(InstallCtrlCallback(obj->controls.base.panel, obj->controls.X_speed, Generic_AnyCall, &obj->X_speed));
		errChk(InstallCtrlCallback(obj->controls.base.panel, obj->controls.X_step, Generic_AnyCall, &obj->X_step));

		errChk(GetCtrlVal(obj->controls.base.panel, obj->controls.X_step_enabled, &obj->X_step_enabled));
		errChk(GetCtrlVal(obj->controls.base.panel, obj->controls.X_speed, &obj->X_speed));
		errChk(GetCtrlVal(obj->controls.base.panel, obj->controls.X_step, &obj->X_step));
	}

	{
		obj->Y_plus.onPress.hook.cb = AMC4030_UI_Object_OnPress_Y_plus;
		obj->Y_plus.onPress.hook.ctx = obj;
		obj->Y_plus.onRelease.hook.cb = AMC4030_UI_Object_OnRelease_Y_plus;
		obj->Y_plus.onRelease.hook.ctx = obj;
		obj->Y_plus.single_event_mode = &obj->Y_step_enabled;
		errChk(InstallCtrlCallback(obj->controls.base.panel, obj->controls.Y_plus, Generic_ButtonLongCall, &obj->Y_plus));
		obj->Y_minus.onPress.hook.cb = AMC4030_UI_Object_OnPress_Y_minus;
		obj->Y_minus.onPress.hook.ctx = obj;
		obj->Y_minus.onRelease.hook.cb = AMC4030_UI_Object_OnRelease_Y_minus;
		obj->Y_minus.onRelease.hook.ctx = obj;
		obj->Y_minus.single_event_mode = &obj->Y_step_enabled;
		errChk(InstallCtrlCallback(obj->controls.base.panel, obj->controls.Y_minus, Generic_ButtonLongCall, &obj->Y_minus));
		obj->Y_stop.hook.cb = AMC4030_UI_Object_OnCommit_Stop_Y;
		obj->Y_stop.hook.ctx = obj;
		errChk(InstallCtrlCallback(obj->controls.base.panel, obj->controls.Y_stop, Generic_HookCall, &obj->Y_stop));
		obj->Y_home.hook.cb = AMC4030_UI_Object_OnCommit_Home_Y;
		obj->Y_home.hook.ctx = obj;
		errChk(InstallCtrlCallback(obj->controls.base.panel, obj->controls.Y_home, Generic_HookCall, &obj->Y_home));
		errChk(InstallCtrlCallback(obj->controls.base.panel, obj->controls.Y_step_enabled, Generic_AnyCall, &obj->Y_step_enabled));
		errChk(InstallCtrlCallback(obj->controls.base.panel, obj->controls.Y_speed, Generic_AnyCall, &obj->Y_speed));
		errChk(InstallCtrlCallback(obj->controls.base.panel, obj->controls.Y_step, Generic_AnyCall, &obj->Y_step));

		errChk(GetCtrlVal(obj->controls.base.panel, obj->controls.Y_step_enabled, &obj->Y_step_enabled));
		errChk(GetCtrlVal(obj->controls.base.panel, obj->controls.Y_speed, &obj->Y_speed));
		errChk(GetCtrlVal(obj->controls.base.panel, obj->controls.Y_step, &obj->Y_step));
	}

	{
		obj->Z_plus.onPress.hook.cb = AMC4030_UI_Object_OnPress_Z_plus;
		obj->Z_plus.onPress.hook.ctx = obj;
		obj->Z_plus.onRelease.hook.cb = AMC4030_UI_Object_OnRelease_Z_plus;
		obj->Z_plus.onRelease.hook.ctx = obj;
		obj->Z_plus.single_event_mode = &obj->Z_step_enabled;
		errChk(InstallCtrlCallback(obj->controls.base.panel, obj->controls.Z_plus, Generic_ButtonLongCall, &obj->Z_plus));
		obj->Z_minus.onPress.hook.cb = AMC4030_UI_Object_OnPress_Z_minus;
		obj->Z_minus.onPress.hook.ctx = obj;
		obj->Z_minus.onRelease.hook.cb = AMC4030_UI_Object_OnRelease_Z_minus;
		obj->Z_minus.onRelease.hook.ctx = obj;
		obj->Z_minus.single_event_mode = &obj->Z_step_enabled;
		errChk(InstallCtrlCallback(obj->controls.base.panel, obj->controls.Z_minus, Generic_ButtonLongCall, &obj->Z_minus));
		obj->Z_stop.hook.cb = AMC4030_UI_Object_OnCommit_Stop_Z;
		obj->Z_stop.hook.ctx = obj;
		errChk(InstallCtrlCallback(obj->controls.base.panel, obj->controls.Z_stop, Generic_HookCall, &obj->Z_stop));
		obj->Z_home.hook.cb = AMC4030_UI_Object_OnCommit_Home_Z;
		obj->Z_home.hook.ctx = obj;
		errChk(InstallCtrlCallback(obj->controls.base.panel, obj->controls.Z_home, Generic_HookCall, &obj->Z_home));
		errChk(InstallCtrlCallback(obj->controls.base.panel, obj->controls.Z_step_enabled, Generic_AnyCall, &obj->Z_step_enabled));
		errChk(InstallCtrlCallback(obj->controls.base.panel, obj->controls.Z_speed, Generic_AnyCall, &obj->Z_speed));
		errChk(InstallCtrlCallback(obj->controls.base.panel, obj->controls.Z_step, Generic_AnyCall, &obj->Z_step));

		errChk(GetCtrlVal(obj->controls.base.panel, obj->controls.Z_step_enabled, &obj->Z_step_enabled));
		errChk(GetCtrlVal(obj->controls.base.panel, obj->controls.Z_speed, &obj->Z_speed));
		errChk(GetCtrlVal(obj->controls.base.panel, obj->controls.Z_step, &obj->Z_step));
	}

	{
		obj->stop_all.hook.cb = AMC4030_UI_Object_StopAll;
		obj->stop_all.hook.ctx = obj;
		errChk(InstallCtrlCallback(obj->controls.base.panel, obj->controls.stop_all, Generic_HookCall, &obj->stop_all));
		obj->pause_all.hook.cb = AMC4030_UI_Object_PauseAll;
		obj->pause_all.hook.ctx = obj;
		errChk(InstallCtrlCallback(obj->controls.base.panel, obj->controls.pause_all, Generic_HookCall, &obj->pause_all));
		obj->resume_all.hook.cb = AMC4030_UI_Object_ResumeAll;
		obj->resume_all.hook.ctx = obj;
		errChk(InstallCtrlCallback(obj->controls.base.panel, obj->controls.resume_all, Generic_HookCall, &obj->resume_all));
	}

	{
		obj->out1_hook.pdata = &obj->OUT1;
		obj->out1_hook.post_hook.hook.cb = AMC4030_UI_Object_Update_OUT1;
		obj->out1_hook.post_hook.hook.ctx = obj;
		errChk(InstallCtrlCallback(obj->controls.base.panel, obj->controls.OUT1, Generic_AnyHookCall, &obj->out1_hook));
		obj->out2_hook.pdata = &obj->OUT2;
		obj->out2_hook.post_hook.hook.cb = AMC4030_UI_Object_Update_OUT2;
		obj->out2_hook.post_hook.hook.ctx = obj;
		errChk(InstallCtrlCallback(obj->controls.base.panel, obj->controls.OUT2, Generic_AnyHookCall, &obj->out2_hook));
		obj->out3_hook.pdata = &obj->OUT3;
		obj->out3_hook.post_hook.hook.cb = AMC4030_UI_Object_Update_OUT3;
		obj->out3_hook.post_hook.hook.ctx = obj;
		errChk(InstallCtrlCallback(obj->controls.base.panel, obj->controls.OUT3, Generic_AnyHookCall, &obj->out3_hook));
		obj->out4_hook.pdata = &obj->OUT4;
		obj->out4_hook.post_hook.hook.cb = AMC4030_UI_Object_Update_OUT4;
		obj->out4_hook.post_hook.hook.ctx = obj;
		errChk(InstallCtrlCallback(obj->controls.base.panel, obj->controls.OUT4, Generic_AnyHookCall, &obj->out4_hook));
	}

	obj->updateHook.hook.cb = AMC4030_UI_Object_UpdateStatus;
	obj->updateHook.hook.ctx = obj;
	CallbackChain_Append(&obj->base.onStatusUpdate, &obj->updateHook);

	obj->readback_out = 1;
Error:
	return error;
}
