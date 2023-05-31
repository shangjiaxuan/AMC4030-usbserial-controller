#pragma once

#ifndef _UI_SHARED_H_
#define _UI_SHARED_H_

// for CVICALLBACK
#include <cvidef.h>

int QuitAction(void* unused);

// for simple binding of data to control
int CVICALLBACK Generic_Action(int panel, int control, int event, void* callbackData, int eventData1, int eventData2);
int CVICALLBACK Generic_AnyCall(int panel, int control, int event, void* callbackData, int eventData1, int eventData2);
int CVICALLBACK Generic_HookCall(int panel, int control, int event, void* callbackData, int eventData1, int eventData2);
int CVICALLBACK Generic_AnyHookCall(int panel, int control, int event, void* callbackData, int eventData1, int eventData2);
int CVICALLBACK Generic_ManyHookCall(int panel, int control, int event, void* callbackData, int eventData1, int eventData2);

// hook call responding to timer event instead of commit
int CVICALLBACK Generic_TimerCall(int panel, int control, int event, void* callbackData, int eventData1, int eventData2);


#include "Callbacks.h"

int CVICALLBACK Generic_ButtonLongCall(int panel, int control, int event, void* callbackData, int eventData1, int eventData2);

#endif