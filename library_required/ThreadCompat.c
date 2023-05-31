#include "ThreadCompat.h"

#if defined(_WIN32)
#if !defined(_CVI_)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <process.h>
#include <stdlib.h>

typedef struct ThreadHandle_WinStruct
{
	uintptr_t handle;
	int has_ended;
	CONDITION_VARIABLE join_wait;
	CRITICAL_SECTION wait_cs;
	ThreadProc entry;
	void* ctx;
}ThreadHandle_WinStruct;

static void ThreadWrapper(void* sts)
{
	ThreadHandle_WinStruct* handle_internal = (ThreadHandle_WinStruct*)sts;
	EnterCriticalSection(&handle_internal->wait_cs);
	handle_internal->entry(handle_internal->ctx);
	LeaveCriticalSection(&handle_internal->wait_cs);
	handle_internal->has_ended = 1;
	WakeConditionVariable(&handle_internal->join_wait);
}

ThreadHandle StartThread(ThreadProc proc, void* ctx)
{
	ThreadHandle_WinStruct* handle_internal = (ThreadHandle_WinStruct*)malloc(sizeof(ThreadHandle_WinStruct));
	if (handle_internal == NULL) {
		return 0;
	}
	handle_internal->ctx = ctx;
	handle_internal->entry = proc;
	handle_internal->has_ended = 0;
	InitializeConditionVariable(&handle_internal->join_wait);
	InitializeCriticalSection(&handle_internal->wait_cs);
	// use _beginthread so that TLS and other crt constructs are respected
	handle_internal->handle = _beginthread(ThreadWrapper, 0, handle_internal);
	if (handle_internal->handle < 0) {
		// error
		free(handle_internal);
		return 0;
	}
	
	return (ThreadHandle)handle_internal;
}

// TODO: wait for stop...
void JoinThread(ThreadHandle handle)
{
	ThreadHandle_WinStruct* handle_internal = (ThreadHandle_WinStruct*)handle;
	while (!handle_internal->has_ended) {
		if (SleepConditionVariableCS(&handle_internal->join_wait, &handle_internal->wait_cs, INFINITE) == 0) {
			// error
		}
	}
	DeleteCriticalSection(&handle_internal->wait_cs);
	free(handle_internal);
}
#else
// windows on cvi cannot use _beginthread for crt tls initialization. But CVI's CreateThread is tls safe
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

#include <stdlib.h>

typedef struct ThreadHandle_WinStruct
{
	uintptr_t handle;
	int has_ended;
	CONDITION_VARIABLE join_wait;
	CRITICAL_SECTION wait_cs;
	ThreadProc entry;
	void* ctx;
}ThreadHandle_WinStruct;

static DWORD __stdcall ThreadWrapper(void* sts)
{
	ThreadHandle_WinStruct* handle_internal = (ThreadHandle_WinStruct*)sts;
	EnterCriticalSection(&handle_internal->wait_cs);
	handle_internal->entry(handle_internal->ctx);
	LeaveCriticalSection(&handle_internal->wait_cs);
	handle_internal->has_ended = 1;
	WakeConditionVariable(&handle_internal->join_wait);
	return 0;
}

ThreadHandle StartThread(ThreadProc proc, void* ctx)
{
	ThreadHandle_WinStruct* handle_internal = (ThreadHandle_WinStruct*)malloc(sizeof(ThreadHandle_WinStruct));
	if (handle_internal == NULL) {
		return 0;
	}
	handle_internal->ctx = ctx;
	handle_internal->entry = proc;
	handle_internal->has_ended = 0;
	InitializeConditionVariable(&handle_internal->join_wait);
	InitializeCriticalSection(&handle_internal->wait_cs);
	// CVI can use CreateThread safely
	handle_internal->handle = CreateThread(NULL, 0, ThreadWrapper, handle_internal, 0, NULL);
	if (handle_internal->handle < 0) {
		// error
		free(handle_internal);
		return 0;
	}
	return (ThreadHandle)handle_internal;
}

// TODO: wait for stop...
void JoinThread(ThreadHandle handle)
{
	ThreadHandle_WinStruct* handle_internal = (ThreadHandle_WinStruct*)handle;
	while (!handle_internal->has_ended) {
		if (SleepConditionVariableCS(&handle_internal->join_wait, &handle_internal->wait_cs, INFINITE) == 0) {
			// error
		}
	}
	DeleteCriticalSection(&handle_internal->wait_cs);
	free(handle_internal);
}
#endif





#else
// pthread...
#endif

void GenricLock_LockRead(GenericLock_Wrapper* lck)
{
	switch (lck->type) {
		default: break;
		case MATRIX_DATA_LOCK_MUTEX:
			LOCK_MUTEX((MUTEX_T*)lck->plock);
			break;
		case MATRIX_DATA_LOCK_RWLOCK:
			RWLOCK_LOCK_READ((RWLOCK_T*)lck->plock);
			break;
	}
}
void GenricLock_UnLockRead(GenericLock_Wrapper* lck)
{
	switch (lck->type) {
		default: break;
		case MATRIX_DATA_LOCK_MUTEX:
			UNLOCK_MUTEX((MUTEX_T*)lck->plock);
			break;
		case MATRIX_DATA_LOCK_RWLOCK:
			RWLOCK_UNLOCK_READ((RWLOCK_T*)lck->plock);
			break;
	}
}

void GenricLock_LockWrite(GenericLock_Wrapper* lck)
{
	switch (lck->type) {
		default: break;
		case MATRIX_DATA_LOCK_MUTEX:
			LOCK_MUTEX((MUTEX_T*)lck->plock);
			break;
		case MATRIX_DATA_LOCK_RWLOCK:
			RWLOCK_LOCK_WRITE((RWLOCK_T*)lck->plock);
			break;
	}
}

_Requires_lock_held_(lck->plock)
void GenricLock_UnLockWrite(GenericLock_Wrapper* lck)
{
	switch (lck->type) {
		default: break;
		case MATRIX_DATA_LOCK_MUTEX:
			UNLOCK_MUTEX((MUTEX_T*)lck->plock);
			break;
		case MATRIX_DATA_LOCK_RWLOCK:
			RWLOCK_UNLOCK_WRITE((RWLOCK_T*)lck->plock);
			break;
	}
}