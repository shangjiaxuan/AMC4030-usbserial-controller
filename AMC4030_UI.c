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
		if (obj->readback_out) {
			pin_out = (out_stat & 0x01) != 0;
			errChk(SetCtrlVal(obj->controls.base.panel, obj->controls.OUT1, pin_out));
			pin_out = (out_stat & 0x02) != 0;
			errChk(SetCtrlVal(obj->controls.base.panel, obj->controls.OUT2, pin_out));
			pin_out = (out_stat & 0x04) != 0;
			errChk(SetCtrlVal(obj->controls.base.panel, obj->controls.OUT3, pin_out));
			pin_out = (out_stat & 0x08) != 0;
			errChk(SetCtrlVal(obj->controls.base.panel, obj->controls.OUT4, pin_out));
		}
		pin_out = (out_stat & 0x10) != 0;
		errChk(SetCtrlVal(obj->controls.base.panel, obj->controls.IN1, pin_out));
		pin_out = (out_stat & 0x20) != 0;
		errChk(SetCtrlVal(obj->controls.base.panel, obj->controls.IN2, pin_out));
		pin_out = (out_stat & 0x40) != 0;
		errChk(SetCtrlVal(obj->controls.base.panel, obj->controls.IN3, pin_out));
		pin_out = (out_stat & 0x80) != 0;
		errChk(SetCtrlVal(obj->controls.base.panel, obj->controls.IN4, pin_out));
		pin_out = (out_stat & 0x100) != 0;
		errChk(SetCtrlVal(obj->controls.base.panel, obj->controls.ORG1, pin_out));
		pin_out = (out_stat & 0x200) != 0;
		errChk(SetCtrlVal(obj->controls.base.panel, obj->controls.ORG2, pin_out));
		pin_out = (out_stat & 0x400) != 0;
		errChk(SetCtrlVal(obj->controls.base.panel, obj->controls.ORG3, pin_out));
	}
Error:
	return error;
}

int AMC4030_UI_Object_StartMoveAxis(AMC4030_UI_Object* obj, uint8_t axis, float speed, float distance)
{
	return AMC4030_usb_protocol_Jog(obj->monitor.connection, axis, distance, speed);
}
int AMC4030_UI_Object_StopAxis(AMC4030_UI_Object* obj, uint8_t axis_mask)
{
	return AMC4030_usb_protocol_StopByAxis(obj->monitor.connection, axis_mask);
}
int AMC4030_UI_Object_HomeAxis(AMC4030_UI_Object* obj, uint8_t axis_mask)
{
	return AMC4030_usb_protocol_HomeByAxis(obj->monitor.connection, axis_mask);
}

int AMC4030_UI_Object_StopAll(AMC4030_UI_Object* obj)
{
	return AMC4030_usb_protocol_StopAll(obj->monitor.connection);
}
int AMC4030_UI_Object_PauseAll(AMC4030_UI_Object* obj)
{
	return AMC4030_usb_protocol_PauseAll(obj->monitor.connection);
}

int AMC4030_UI_Object_ResumeAll(AMC4030_UI_Object* obj)
{
	return AMC4030_usb_protocol_ResumeAll(obj->monitor.connection);
}

int AMC4030_UI_Object_SetOutput(AMC4030_UI_Object* obj, uint8_t out_no, uint8_t val)
{
	return AMC4030_usb_protocol_SetCoil(obj->monitor.connection, out_no, val);
}

