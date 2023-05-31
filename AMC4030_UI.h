#pragma once

#ifndef _AMC4030_UI_H_
#define _AMC4030_UI_H_

#if defined(_WIN32) && defined(AMC4030_UI_DYNAMIC)
#define AMC4030_UI_EXPORT __declspec(dllexport)
#define AMC4030_UI_IMPORT __declspec(dllimport)
#ifdef AMC4030_UI_BUILD
#define AMC4030_UI_SYMBOL AMC4030_UI_EXPORT
#else
#define AMC4030_UI_SYMBOL AMC4030_UI_IMPORT
#endif
#else
#define AMC4030_UI_EXPORT
#define AMC4030_UI_IMPORT
#define AMC4030_UI_SYMBOL
#endif

typedef struct AMC4030_UI_Object AMC4030_UI_Object;

#include <stdint.h>

AMC4030_UI_SYMBOL AMC4030_UI_Object* AMC4030_UI_Object_Create(const char* uir_file, int use_as_standalone, int* error);
AMC4030_UI_SYMBOL void AMC4030_UI_Object_Delete(AMC4030_UI_Object* obj);

AMC4030_UI_SYMBOL int AMC4030_UI_Object_StartUI(AMC4030_UI_Object* obj);
AMC4030_UI_SYMBOL int AMC4030_UI_Object_EndUI(AMC4030_UI_Object* obj);

AMC4030_UI_SYMBOL int AMC4030_UI_Object_ShowUI(AMC4030_UI_Object* obj);
AMC4030_UI_SYMBOL int AMC4030_UI_Object_HideUI(AMC4030_UI_Object* obj);

AMC4030_UI_SYMBOL int AMC4030_UI_Object_UpdateStatus(AMC4030_UI_Object* obj);

AMC4030_UI_SYMBOL int AMC4030_UI_Object_StartMoveAxis(AMC4030_UI_Object* obj, uint8_t axis, float speed, float distance);
AMC4030_UI_SYMBOL int AMC4030_UI_Object_StopAxis(AMC4030_UI_Object* obj, uint8_t axis_mask);
AMC4030_UI_SYMBOL int AMC4030_UI_Object_HomeAxis(AMC4030_UI_Object* obj, uint8_t axis_mask);

AMC4030_UI_SYMBOL int AMC4030_UI_Object_StopAll(AMC4030_UI_Object* obj);
AMC4030_UI_SYMBOL int AMC4030_UI_Object_PauseAll(AMC4030_UI_Object* obj);
AMC4030_UI_SYMBOL int AMC4030_UI_Object_ResumeAll(AMC4030_UI_Object* obj);

AMC4030_UI_SYMBOL int AMC4030_UI_Object_SetOutput(AMC4030_UI_Object* obj, uint8_t out_no, uint8_t val);

// not implemented yet
int AMC4030_UI_Object_WaitStop(AMC4030_UI_Object* obj);

#endif
