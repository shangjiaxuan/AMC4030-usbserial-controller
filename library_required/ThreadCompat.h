#pragma once

#ifndef _THREAD_COMPAT_H_
#define _THREAD_COMPAT_H_

#include <stdint.h>

typedef uintptr_t ThreadHandle;
typedef void (*ThreadProc)(void* data);
ThreadHandle StartThread(ThreadProc proc, void* ctx);
void JoinThread(ThreadHandle handle);

#if defined(_WIN32)
#ifndef MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS
#define MICROSOFT_WINDOWS_WINBASE_H_DEFINE_INTERLOCKED_CPLUSPLUS_OVERLOADS 0
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#define MUTEX_T CRITICAL_SECTION
#define INIT_MUTEX(mtx) InitializeCriticalSection(mtx)
#define LOCK_MUTEX(mtx) EnterCriticalSection(mtx)
#define UNLOCK_MUTEX(mtx) LeaveCriticalSection(mtx)
#define DESTROY_MUTEX(mtx) DeleteCriticalSection(mtx)

#define RWLOCK_T SRWLOCK
#define RWLOCK_INITIALIZE(mtx) InitializeSRWLock(mtx)
#define RWLOCK_LOCK_READ(mtx) AcquireSRWLockShared(mtx)
#define RWLOCK_UNLOCK_READ(mtx) ReleaseSRWLockShared(mtx)
#define RWLOCK_LOCK_WRITE(mtx) AcquireSRWLockExclusive(mtx)
#define RWLOCK_UNLOCK_WRITE(mtx) ReleaseSRWLockExclusive(mtx)
#define RWLOCK_DESTROY(mtx) (mtx)


#define COND_VAR CONDITION_VARIABLE
#define COND_INIT(cond) InitializeConditionVariable(cond)
#define COND_WAIT_MTX(cond, mtx) SleepConditionVariableCS(cond, mtx, INFINITE)
#define COND_WAIT_READ_RWL(cond, mtx) SleepConditionVariableSRW(cond, mtx, INFINITE, CONDITION_VARIABLE_LOCKMODE_SHARED)
#define COND_WAIT_WRITE_RWL(cond, mtx) SleepConditionVariableSRW(cond, mtx, INFINITE, 0)
#define COND_NOTIFY_ONE(cond) WakeConditionVariable(cond)
#define COND_NOTIFY_ALL(cond) WakeAllConditionVariable(cond)

#else

#endif


typedef enum Matrix_DataLockType
{
	MATRIX_DATA_LOCK_NONE = 0,
	MATRIX_DATA_LOCK_MUTEX = 1,
	MATRIX_DATA_LOCK_RWLOCK = 2
}Matrix_DataLockType;

typedef struct GenericLock_Wrapper
{
	Matrix_DataLockType type;
	void* plock;
} GenericLock_Wrapper;

void GenricLock_LockRead(GenericLock_Wrapper* lck);
void GenricLock_UnLockRead(GenericLock_Wrapper* lck);

void GenricLock_LockWrite(GenericLock_Wrapper* lck);
void GenricLock_UnLockWrite(GenericLock_Wrapper* lck);

#endif

