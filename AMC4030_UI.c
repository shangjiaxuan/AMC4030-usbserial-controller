#include "AMC4030_UI.h"

#include <userint.h>

#include "../toolbox.h"

void AMC4030_UI_Object_Destroy(AMC4030_UI_Object* obj)
{
	SerialDevice_UI_Object_Destroy(&obj->base);
	DiscardPanel(obj->controls.base.panel);
	obj->controls.base.panel = 0;
	CallbackChain_Remove(&obj->base.onStatusUpdate, &obj->updateHook);
}

int AMC4030_UI_Object_UpdateStatus(AMC4030_UI_Object* obj)
{
	int error = UIENoError;
	uint32_t status = obj->monitor.current_status.dwWorkStatus;
	errChk(SetCtrlVal(obj->controls.base.panel, obj->controls.paused, (status & 0x01) != 0));
	errChk(SetCtrlVal(obj->controls.base.panel, obj->controls.working, (status & 0x02) != 0));
	errChk(SetCtrlVal(obj->controls.base.panel, obj->controls.moving, (status & 0x04) != 0));
	errChk(SetCtrlVal(obj->controls.base.panel, obj->controls.returning, (status & 0x08) != 0));
	int alarm = (status & 0x10) != 0;
	if (alarm) {
		errChk(SetCtrlAttribute(obj->controls.base.panel, obj->controls.alert_code, ATTR_DIMMED, 0));
	}
	else {
		errChk(SetCtrlAttribute(obj->controls.base.panel, obj->controls.alert_code, ATTR_DIMMED, 1));
	}
	errChk(SetCtrlVal(obj->controls.base.panel, obj->controls.alert_present, alarm));
	errChk(SetCtrlVal(obj->controls.base.panel, obj->controls.alert_code, obj->monitor.current_status.nAlmCode));
	{
		double position;
		position = obj->monitor.current_status.nPos[0] / 100000.0;
		errChk(SetCtrlVal(obj->controls.base.panel, obj->controls.X_pos, position));
		position = obj->monitor.current_status.nPos[1] / 100000.0;
		errChk(SetCtrlVal(obj->controls.base.panel, obj->controls.Y_pos, position));
		position = obj->monitor.current_status.nPos[2] / 100000.0;
		errChk(SetCtrlVal(obj->controls.base.panel, obj->controls.Z_pos, position));
	}
	{
		uint16_t out_stat = obj->monitor.current_status.dwOutputStatus;
		int pin_out;
		pin_out = (out_stat & 0x01) != 0;
		errChk(SetCtrlVal(obj->controls.base.panel, obj->controls.IN1, pin_out));
		pin_out = (out_stat & 0x02) != 0;
		errChk(SetCtrlVal(obj->controls.base.panel, obj->controls.IN2, pin_out));
		pin_out = (out_stat & 0x08) != 0;
		errChk(SetCtrlVal(obj->controls.base.panel, obj->controls.IN3, pin_out));
		pin_out = (out_stat & 0x10) != 0;
		errChk(SetCtrlVal(obj->controls.base.panel, obj->controls.IN4, pin_out));
		pin_out = (out_stat & 0x20) != 0;
		errChk(SetCtrlVal(obj->controls.base.panel, obj->controls.ORG1, pin_out));
		pin_out = (out_stat & 0x40) != 0;
		errChk(SetCtrlVal(obj->controls.base.panel, obj->controls.ORG2, pin_out));
		pin_out = (out_stat & 0x80) != 0;
		errChk(SetCtrlVal(obj->controls.base.panel, obj->controls.ORG3, pin_out));
	}
	if (obj->readback_out) {

	}
Error:
	return error;
}

#include "../ui_shared.h"
#include "cvi_uir_AMC4030.h"

#include "AMC4030_DeviceEnumerator.h"

int AMC4030_UI_Object_Initialize(AMC4030_UI_Object* obj, const char* uir_file, int use_as_standalone)
{
	int error = UIENoError;
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

	obj->X_plus.onPress.hook.cb = ;


	obj->updateHook.hook.cb = AMC4030_UI_Object_UpdateStatus;
	obj->updateHook.hook.ctx = obj;
	CallbackChain_Append(&obj->base.onStatusUpdate, &obj->updateHook);


Error:
	return error;
}
