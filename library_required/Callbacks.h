#pragma once

#ifndef _CALLBACKS_H_
#define _CALLBACKS_H_

// for use with adavanced post and pre-procressing of simple data bind
typedef int (*ActionCallback)(void);
typedef int (*ContextedCallback)(void* p);

typedef struct CallbackInfo
{
	ContextedCallback cb;
	void* ctx;
} CallbackInfo;

typedef struct CallbackChain
{
	CallbackInfo hook;
	struct CallbackChain* pNext;
} CallbackChain;

typedef struct DataHook
{
	CallbackChain pre_hook;
	void* pdata;
	CallbackChain post_hook;
} DataHook;

typedef struct DataHook_Duplicates DataHook_Duplicates;
typedef struct DataHook_Duplicates
{
	CallbackInfo pre_hook;
	void* pdata;
	CallbackInfo post_hook;
	DataHook_Duplicates* pNext;
} DataHook_Duplicates;

// stops and returns first error
int CallbackChain_Invoke(CallbackChain* pcb);

// callback chaining algorithms, prepend+append
void CallbackChain_Insert(CallbackChain* target, CallbackChain* item);

// adds all
void CallbackChain_Append(CallbackChain** head, CallbackChain* item);
// adds all
void CallbackChain_Prepend(CallbackChain** head, CallbackChain* item);

// removes one item
void CallbackChain_Remove(CallbackChain** head, CallbackChain* item);

// adds all
void DataChain_Append(DataHook_Duplicates** head, DataHook_Duplicates* item);
// adds all
void DataChain_Prepend(DataHook_Duplicates** head, DataHook_Duplicates* item);

// removes one item
void DataChain_Remove(DataHook_Duplicates** head, DataHook_Duplicates* item);

inline int Callback_Invoke(CallbackInfo* hook){
	if (hook->cb) {
		return hook->cb(hook->ctx);
	}
	return 0;
}

typedef struct ButtonLongCallback
{
	int* single_event_mode;
	int pressed;
	CallbackChain onPress;
	CallbackChain onRelease;
}ButtonLongCallback;

#endif

