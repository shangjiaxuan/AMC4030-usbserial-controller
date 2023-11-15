#include "SerialDevice_Panel.h"

#include <stdlib.h>

#include <userint.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS
#define MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS 0
#endif
#include <Windows.h>
#include <utility.h>

#ifndef errChk
#define errChk(fCall) if (error = (fCall), error < 0) \
{goto Error;} else
#endif

void SerialDevice_UI_Object_Destroy(SerialDevice_UI_Object* obj)
{
	obj->monitor_funcs->Stop(obj->monitor);
	obj->connection_funcs->Delete(obj->monitor_funcs->GetConnection(obj->monitor));
	obj->monitor_funcs->SetConnection(obj->monitor, 0);
	obj->monitor_funcs->UnRegisterListener(obj->monitor, &obj->updateListener);
	obj->monitor_funcs->UnRegisterEndListener(obj->monitor, &obj->stopListener);
	if (obj->port_file_name != obj->port_file_short_buffer) {
		free(obj->port_file_name);
		obj->port_file_name = obj->port_file_short_buffer;
		obj->port_file_buffer_size = sizeof(obj->port_file_short_buffer);
	}
}

int SerialDevice_UI_Object_StartUI(SerialDevice_UI_Object* obj)
{
	if (!obj->use_as_standalone) {
		return UIEMustBeInTopLevelPanelThread;
	}
	obj->cvi_thread_id = CmtGetCurrentThreadID();
	obj->standalone_running = 1;
	int error = DisplayPanel(obj->controls->panel);
	if (error < 0) {
		return error;
	}
	return RunUserInterface();
}

static int  SerialDevice_UI_Object_Quit(SerialDevice_UI_Object* obj)
{
	int error = UIENoError;
	int res = ConfirmPopup("SerialDevice CVI UI", "Confirm Quit?");
	if (res == 1) {
		// execute on-exit things
		obj->standalone_running = 0;
		error = QuitUserInterface(0);
	}
	return error;
}

int SerialDevice_UI_Object_EndUI(SerialDevice_UI_Object* obj)
{
	if (!obj->use_as_standalone) {
		return UIEMustBeInTopLevelPanelThread;
	}
	if (obj->standalone_running) {
		return PostDeferredCallToThread(SerialDevice_UI_Object_Quit, obj, obj->cvi_thread_id);
	}
	return UIEOperationFailed;
}

int SerialDevice_UI_Object_RefreshDevices(SerialDevice_UI_Object* obj)
{
	int error = UIENoError;
	void* enumerator = NULL;
	int index = 0;
	int need_check_prev = 0;
	error = SerialDevice_UI_Object_GetSelectedCommDev(obj);
	if (error < 0 && error != UIENoItemsInList) {
		goto Error;
	}
	if (error == UIENoItemsInList) {
		error = UIENoError;
	}
	else {
		need_check_prev = 1;
	}
	if (obj->onDeviceRefresh) {
		errChk(CallbackChain_Invoke(obj->onDeviceRefresh));
	}
	errChk(ClearListCtrl(obj->controls->panel, obj->controls->device_select));
	enumerator =  obj->device_enumerator_funcs->Create();
	if (!enumerator) {
		error = UIEOperationFailed;
		goto Error;
	}
	obj->port_count = 0;
	const char* device_file = 0;
	char buffer[2048] = {0};
	do {
		device_file = obj->device_enumerator_funcs->Next(enumerator);
		if (device_file) {
			if (need_check_prev) {
				if (strcmp(device_file, obj->port_file_name) == 0) {
					need_check_prev = 0;
					obj->selected_index = index;
				}
			}
			errChk(obj->device_enumerator_funcs->GetCurrentFriendlyName(enumerator, buffer, sizeof(buffer)));
			errChk(InsertListItem(obj->controls->panel, obj->controls->device_select, index, buffer, device_file));
			++index;
		}
	} while (device_file);
	obj->port_count = index;
	if (obj->selected_index>=0 && index > obj->selected_index) {
		errChk(SetCtrlIndex(obj->controls->panel, obj->controls->device_select, obj->selected_index));
	}
	if (!obj->port_count) {
		errChk(SetCtrlAttribute(obj->controls->panel, obj->controls->connect, ATTR_DIMMED, 1));
	}
	else {
		errChk(SetCtrlAttribute(obj->controls->panel, obj->controls->connect, ATTR_DIMMED, 0));
		errChk(SerialDevice_UI_Object_GetSelectedCommDev(obj));
	}
Error:
	if (enumerator) {
		obj->device_enumerator_funcs->Delete(enumerator);
	}

	return error;
}

int SerialDevice_UI_Object_GetSelectedCommDev(SerialDevice_UI_Object* obj)
{
	int error = UIENoError;
	int string_len;
	{
		errChk(GetCtrlIndex(obj->controls->panel, obj->controls->device_select, &obj->selected_index));
		if (obj->selected_index < 0) {
			return UIENoItemsInList;
		}
		errChk(GetValueLengthFromIndex(obj->controls->panel, obj->controls->device_select, obj->selected_index, &string_len));
	}
	if ((size_t)string_len > obj->port_file_buffer_size - 1) {
		if (obj->port_file_name != obj->port_file_short_buffer) {
			char* new_buffer = (char*)malloc((size_t)string_len + 1);
			free(obj->port_file_name);
			if (new_buffer) {
				obj->port_file_name = new_buffer;
				obj->port_file_buffer_size = (size_t)string_len + 1;
			}
			else {
				obj->port_file_name = obj->port_file_short_buffer;
				obj->port_file_buffer_size = sizeof(obj->port_file_short_buffer);
			}
		}
	}
	if (obj->port_file_buffer_size > (size_t)string_len) {
		errChk(GetCtrlVal(obj->controls->panel, obj->controls->device_select, obj->port_file_name));
	}
	else {
		error = UIEOutOfMemory;
	}
Error:
	return error;
}

int SerialDevice_UI_Object_SetupConnectStateUI(SerialDevice_UI_Object* obj)
{
	int error = UIENoError;
	if (obj->is_connected) {
		errChk(SetCtrlVal(obj->controls->panel, obj->controls->connect, 1));
		errChk(SetCtrlArrayAttribute(obj->controls->select_mode_array, ATTR_DIMMED, 1));
		errChk(SetCtrlArrayAttribute(obj->controls->connected_mode_array, ATTR_DIMMED, 0));
	}
	else {
		errChk(SetCtrlVal(obj->controls->panel, obj->controls->connect, 0));
		errChk(SetCtrlArrayAttribute(obj->controls->select_mode_array, ATTR_DIMMED, 0));
		errChk(SetCtrlArrayAttribute(obj->controls->connected_mode_array, ATTR_DIMMED, 1));
		if (obj->need_device_refresh) {
			errChk(SerialDevice_UI_Object_RefreshDevices(obj));
		}
	}
	Error:
	return error;
}

int SerialDevice_UI_Object_Connect(SerialDevice_UI_Object* obj)
{
	obj->is_connected = 1;
	return SerialDevice_UI_Object_Connect_Do(obj);
}
int SerialDevice_UI_Object_DisConnect(SerialDevice_UI_Object* obj)
{
	obj->is_connected = 0;
	return SerialDevice_UI_Object_DisConnect_Do(obj);
}


int SerialDevice_UI_Object_Connect_Do(SerialDevice_UI_Object* obj)
{
	int error = UIENoError;
	errChk(SerialDevice_UI_Object_DisConnect_Do(obj));
	void* connection = obj->connection_funcs->Create(obj->port_file_name, &error);
	obj->monitor_funcs->SetConnection(obj->monitor, connection);
	if (connection) {
		errChk(obj->monitor_funcs->Start(obj->monitor));
	}
	else {
		obj->is_connected = 0;
		// report error?
		error = UIEOperationFailed;
	}
	errChk(SerialDevice_UI_Object_SetupConnectStateUI(obj));
Error:
	return error;
}

int SerialDevice_UI_Object_DisConnect_Do(SerialDevice_UI_Object* obj)
{
	int error = UIENoError;
	// must destroy anyway, not errChk
	obj->monitor_funcs->Stop(obj->monitor);
	obj->connection_funcs->Delete(obj->monitor_funcs->GetConnection(obj->monitor));
	obj->monitor_funcs->SetConnection(obj->monitor, 0);
	//obj->is_connected = 0;
	errChk(SerialDevice_UI_Object_SetupConnectStateUI(obj));
Error:
	return error;
}

int SerialDevice_UI_Object_UpdateConnected(SerialDevice_UI_Object* obj)
{
	if (obj->is_connected) {
		return SerialDevice_UI_Object_Connect_Do(obj);
	}
	else {
		return SerialDevice_UI_Object_DisConnect_Do(obj);
	}
}

int SerialDevice_UI_Object_SetUserControl(SerialDevice_UI_Object* obj)
{
	int error = UIENoError;
	if (obj->disallow_user_control) {
		errChk(SetCtrlAttribute(obj->controls->panel, obj->controls->connect, ATTR_DIMMED, 1));
		errChk(SetCtrlArrayAttribute(obj->controls->select_mode_array, ATTR_DIMMED, 1));
		errChk(SetCtrlArrayAttribute(obj->controls->connected_mode_array, ATTR_DIMMED, 1));
	}
	else {
		errChk(SetCtrlAttribute(obj->controls->panel, obj->controls->connect, ATTR_DIMMED, 0));
		errChk(SerialDevice_UI_Object_SetupConnectStateUI(obj));
	}
Error:
	return error;
}

int SerialDevice_UI_Object_Lock(SerialDevice_UI_Object* obj)
{
	obj->disallow_user_control = 1;
	return SerialDevice_UI_Object_SetUserControl(obj);
}

int SerialDevice_UI_Object_Unlock(SerialDevice_UI_Object* obj)
{
	obj->disallow_user_control = 0;
	return SerialDevice_UI_Object_SetUserControl(obj);
}

int SerialDevice_UI_Object_UpdateThreadEnding(SerialDevice_UI_Object* obj)
{
	obj->connection_funcs->Delete(obj->monitor_funcs->GetConnection(obj->monitor));
	obj->monitor_funcs->SetConnection(obj->monitor, 0);
	obj->connection_need_cleanup = 1;
	int error = UIENoError;
	errChk(SetCtrlVal(obj->controls->panel, obj->controls->connect, 0));
	errChk(SetCtrlArrayAttribute(obj->controls->select_mode_array, ATTR_DIMMED, 0));
	errChk(SetCtrlArrayAttribute(obj->controls->connected_mode_array, ATTR_DIMMED, 1));
	errChk(SerialDevice_UI_Object_RefreshDevices(obj));
Error:
	return error;
}

int SerialDevice_UI_Object_UpdateStatus(SerialDevice_UI_Object* obj)
{
	if (obj->onStatusUpdate) {
		return CallbackChain_Invoke(obj->onStatusUpdate);
	}
	return UIENoError;
}

int SerialDevice_UI_Object_ShowUI(SerialDevice_UI_Object* obj)
{
	return DisplayPanel(obj->controls->panel);
}
int SerialDevice_UI_Object_HideUI(SerialDevice_UI_Object* obj)
{
	return HidePanel(obj->controls->panel);
}

#include "ui_shared.h"

int SerialDevice_UI_Object_Initialize(SerialDevice_UI_Object* obj, int use_as_standalone)
{
	int error = UIENoError;

	obj->port_file_name = obj->port_file_short_buffer;
	obj->port_file_buffer_size = sizeof(obj->port_file_short_buffer);

	obj->connectHook.pdata = &obj->is_connected;
	obj->connectHook.post_hook.hook.cb = SerialDevice_UI_Object_UpdateConnected;
	obj->connectHook.post_hook.hook.ctx = obj;
	errChk(InstallCtrlCallback(obj->controls->panel, obj->controls->connect, Generic_AnyHookCall, &obj->connectHook));

	obj->selectHook.hook.cb = SerialDevice_UI_Object_GetSelectedCommDev;
	obj->selectHook.hook.ctx = obj;
	errChk(InstallCtrlCallback(obj->controls->panel, obj->controls->device_select, Generic_HookCall, &obj->selectHook));

	obj->refreshHook.hook.cb = SerialDevice_UI_Object_RefreshDevices;
	obj->refreshHook.hook.ctx = obj;
	errChk(InstallCtrlCallback(obj->controls->panel, obj->controls->device_refresh, Generic_HookCall, &obj->refreshHook));

	errChk(obj->monitor_funcs->Initialize(obj->monitor));

	obj->updateListener.hook.cb = SerialDevice_UI_Object_UpdateStatus;
	obj->updateListener.hook.ctx = obj;
	obj->monitor_funcs->RegisterListener(obj->monitor, &obj->updateListener);

	obj->stopListener.hook.cb = SerialDevice_UI_Object_UpdateThreadEnding;
	obj->stopListener.hook.ctx = obj;
	obj->monitor_funcs->RegisterEndListener(obj->monitor, &obj->stopListener);

	errChk(SerialDevice_UI_Object_RefreshDevices(obj));

	obj->panelHideHook.hook.cb = SerialDevice_UI_Object_HideUI;
	obj->panelHideHook.hook.ctx = obj;
	errChk(InstallCtrlCallback(obj->controls->panel, obj->controls->hide, Generic_HookCall, &obj->panelHideHook));


	obj->quitUIHook.hook.cb = SerialDevice_UI_Object_EndUI;
	obj->quitUIHook.hook.ctx = obj;
	errChk(InstallCtrlCallback(obj->controls->panel, obj->controls->quit, Generic_HookCall, &obj->quitUIHook));

	obj->use_as_standalone = use_as_standalone;
	if (use_as_standalone) {
		errChk(SetPanelAttribute(obj->controls->panel, ATTR_CLOSE_CTRL, obj->controls->quit));
	}
	else {
		errChk(SetPanelAttribute(obj->controls->panel, ATTR_CLOSE_CTRL, obj->controls->hide));
	}
Error:
	return error;
}
