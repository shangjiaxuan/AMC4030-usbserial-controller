#include "ui_shared.h"

#include "Callbacks.h"

#include <userint.h>

#ifndef errChk
#define errChk(fCall) if (error = (fCall), error < 0) \
{goto Error;} else
#endif

#include <assert.h>

int CVICALLBACK Generic_Action(int panel, int control, int event, void* callbackData, int eventData1, int eventData2)
{
	int error = UIENoError;
	assert(callbackData != 0);
	ActionCallback ac = callbackData;
	switch (event) {
		case EVENT_COMMIT:
		{
			errChk(ac());
		}
			break;
	}
Error:
	if (error < 0) {
		MessagePopup("Error in generic action call:", GetUILErrorString(error));
	}
	return 0;
}

int CVICALLBACK Generic_AnyCall(int panel, int control, int event,
	void* callbackData, int eventData1, int eventData2)
{
	int error = UIENoError;
	assert(callbackData != 0);
	switch (event) {
		case EVENT_COMMIT:
			errChk(GetCtrlVal(panel, control, callbackData));
			break;
	}
Error:
	if (error < 0) {
		MessagePopup("Error in generic any call:", GetUILErrorString(error));
	}
	return 0;
}

int CVICALLBACK Generic_AnyHookCall(int panel, int control, int event,
	void* callbackData, int eventData1, int eventData2)
{
	int error = UIENoError;
	assert(callbackData != 0);
	DataHook* info = callbackData;
	switch (event) {
		case EVENT_COMMIT:
		{
			errChk(CallbackChain_Invoke(&info->pre_hook));
			if (info->pdata) {
				errChk(GetCtrlVal(panel, control, info->pdata));
			}
			errChk(CallbackChain_Invoke(&info->post_hook));
		}
		break;
	}
Error:
	if (error < 0) {
		MessagePopup("Error in generic any+hook call with hook:", GetUILErrorString(error));
	}
	return 0;
}

int CVICALLBACK Generic_HookCall(int panel, int control, int event,
	void* callbackData, int eventData1, int eventData2)
{
	int error = UIENoError;
	assert(callbackData != 0);
	CallbackChain* info = callbackData;
	assert(info != 0);
	switch (event) {
		case EVENT_COMMIT:
		{
			errChk(CallbackChain_Invoke(info));
		}

		break;
	}
Error:
	if (error < 0) {
		MessagePopup("Error in generic hook call with hook:", GetUILErrorString(error));
	}
	return 0;
}

int CVICALLBACK Generic_TimerCall(int panel, int control, int event,
	void* callbackData, int eventData1, int eventData2)
{
	int error = UIENoError;
	assert(callbackData != 0);
	CallbackChain* info = callbackData;
	assert(info != 0);
	switch (event) {
		case EVENT_TIMER_TICK:
		{
			errChk(CallbackChain_Invoke(info));
		}

		break;
	}
Error:
	if (error < 0) {
		MessagePopup("Error in generic timer call with hook:", GetUILErrorString(error));
	}
	return 0;
}

int CVICALLBACK Generic_ButtonLongCall(int panel, int control, int event, void* callbackData, int eventData1, int eventData2)
{
	int error = UIENoError;
	assert(callbackData != 0);
	ButtonLongCallback* info = callbackData;
	assert(info != 0);
	switch (event) {
		case EVENT_LEFT_CLICK:
			if (!*info->single_event_mode && !info->pressed) {
				int x, y, w, h;
				errChk(GetCtrlBoundingRect(panel, control, &y, &x, &h, &w));
				if (eventData2 >= x && eventData2 <= x + w
					&& eventData1 >= y && eventData1 <= y + h) {
					info->pressed = 1;
					errChk(CallbackChain_Invoke(&info->onPress));
				}
			}
			break;
		case EVENT_LEFT_CLICK_UP:
			if (!*info->single_event_mode && info->pressed) {
				info->pressed = 0;
				errChk(CallbackChain_Invoke(&info->onRelease));
			}
			break;
		case EVENT_COMMIT:
			if (*info->single_event_mode) {
				info->pressed = 0;
				errChk(CallbackChain_Invoke(&info->onRelease));
			}
			break;
	}
Error:
	if (error < 0) {
		MessagePopup("Error in generic long press button call with hook:", GetUILErrorString(error));
	}
	return 0;
}

int QuitAction(void* unused)
{
	int res = ConfirmPopup("CVI UI", "È·¶¨ÍË³öÂð£¿");
	if (res == 1) {
		// execute on-exit things

		QuitUserInterface(0);
	}
	return 0;
}
