#ifndef TABS_H
#define TABS_H

#if defined(_WIN32) || defined(_WIN64)
    #define PLATFORM_WINDOWS
#elif (defined(__APPLE__) && defined(__MACH__))
    #define PLATFORM_MAC
#else
    #define PLATFORM_UNIX
#endif

#include <stdint.h>

#ifdef PLATFORM_WINDOWS
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <process.h>
#endif

#if defined(PLATFORM_MAC) || defined(PLATFORM_UNIX)
    #include <pthread.h>
    #include <unistd.h>
    #include <sched.h>
#endif

#define TABS_SUCCESS 0
#define TABS_ERROR   -1

#ifdef PLATFORM_WINDOWS
    typedef HANDLE tabs_thread_t;
    typedef DWORD tabs_thread_id_t;
    typedef unsigned (__stdcall *tabs_thread_func_t)(void*);
    #define TABS_THREAD_RETURN_TYPE unsigned __stdcall
    #define TABS_THREAD_RETURN return 0
#else
    typedef pthread_t tabs_thread_t;
    typedef pthread_t tabs_thread_id_t;
    typedef void* (*tabs_thread_func_t)(void*);
    #define TABS_THREAD_RETURN_TYPE void*
    #define TABS_THREAD_RETURN return NULL
#endif

int tabs_create_thread(tabs_thread_t* thread, tabs_thread_func_t func, void* arg);
int tabs_join_thread(tabs_thread_t thread, int* exit_code);
int tabs_detach_thread(tabs_thread_t thread);
tabs_thread_id_t tabs_current_thread_id(void);
void tabs_sleep_ms(uint32_t milliseconds);
void tabs_yield(void);

#ifdef PLATFORM_WINDOWS
    typedef CRITICAL_SECTION tabs_mutex_t;
#else
    typedef pthread_mutex_t tabs_mutex_t;
#endif

int tabs_mutex_init(tabs_mutex_t* mutex);
int tabs_mutex_destroy(tabs_mutex_t* mutex);
int tabs_mutex_lock(tabs_mutex_t* mutex);
int tabs_mutex_trylock(tabs_mutex_t* mutex);
int tabs_mutex_unlock(tabs_mutex_t* mutex);

#ifdef TABS_IMPLEMENTATION

int tabs_create_thread(tabs_thread_t* thread, tabs_thread_func_t func, void* arg) {
    if (thread == NULL || func == NULL) {
        return TABS_ERROR;
    }

#ifdef PLATFORM_WINDOWS
    *thread = (HANDLE)_beginthreadex(NULL, 0, func, arg, 0, NULL);
    if (*thread == NULL) {
        return TABS_ERROR;
    }
    return TABS_SUCCESS;
#else
    int result = pthread_create(thread, NULL, func, arg);
    if (result != 0) {
        return TABS_ERROR;
    }
    return TABS_SUCCESS;
#endif
}

int tabs_join_thread(tabs_thread_t thread, int* exit_code) {
#ifdef PLATFORM_WINDOWS
    DWORD result = WaitForSingleObject(thread, INFINITE);
    if (result != WAIT_OBJECT_0) {
        return TABS_ERROR;
    }

    if (exit_code != NULL) {
        DWORD code;
        if (GetExitCodeThread(thread, &code)) {
            *exit_code = (int)code;
        }
    }

    CloseHandle(thread);
    return TABS_SUCCESS;
#else
    void* retval;
    int result = pthread_join(thread, &retval);
    if (result != 0) {
        return TABS_ERROR;
    }

    if (exit_code != NULL) {
        *exit_code = (int)(intptr_t)retval;
    }

    return TABS_SUCCESS;
#endif
}

int tabs_detach_thread(tabs_thread_t thread) {
#ifdef PLATFORM_WINDOWS
    if (CloseHandle(thread)) {
        return TABS_SUCCESS;
    }
    return TABS_ERROR;
#else
    int result = pthread_detach(thread);
    if (result != 0) {
        return TABS_ERROR;
    }
    return TABS_SUCCESS;
#endif
}

tabs_thread_id_t tabs_current_thread_id(void) {
#ifdef PLATFORM_WINDOWS
    return GetCurrentThreadId();
#else
    return pthread_self();
#endif
}

void tabs_sleep_ms(uint32_t milliseconds) {
#ifdef PLATFORM_WINDOWS
    Sleep(milliseconds);
#else
    usleep(milliseconds * 1000);
#endif
}

void tabs_yield(void) {
#ifdef PLATFORM_WINDOWS
    SwitchToThread();
#else
    sched_yield();
#endif
}

int tabs_mutex_init(tabs_mutex_t* mutex) {
    if (mutex == NULL) {
        return TABS_ERROR;
    }

#ifdef PLATFORM_WINDOWS
    InitializeCriticalSection(mutex);
    return TABS_SUCCESS;
#else
    int result = pthread_mutex_init(mutex, NULL);
    if (result != 0) {
        return TABS_ERROR;
    }
    return TABS_SUCCESS;
#endif
}

int tabs_mutex_destroy(tabs_mutex_t* mutex) {
    if (mutex == NULL) {
        return TABS_ERROR;
    }

#ifdef PLATFORM_WINDOWS
    DeleteCriticalSection(mutex);
    return TABS_SUCCESS;
#else
    int result = pthread_mutex_destroy(mutex);
    if (result != 0) {
        return TABS_ERROR;
    }
    return TABS_SUCCESS;
#endif
}

int tabs_mutex_lock(tabs_mutex_t* mutex) {
    if (mutex == NULL) {
        return TABS_ERROR;
    }

#ifdef PLATFORM_WINDOWS
    EnterCriticalSection(mutex);
    return TABS_SUCCESS;
#else
    int result = pthread_mutex_lock(mutex);
    if (result != 0) {
        return TABS_ERROR;
    }
    return TABS_SUCCESS;
#endif
}

int tabs_mutex_trylock(tabs_mutex_t* mutex) {
    if (mutex == NULL) {
        return TABS_ERROR;
    }

#ifdef PLATFORM_WINDOWS
    if (TryEnterCriticalSection(mutex)) {
        return TABS_SUCCESS;
    }
    return TABS_ERROR;
#else
    int result = pthread_mutex_trylock(mutex);
    if (result == 0) {
        return TABS_SUCCESS;
    }
    return TABS_ERROR;
#endif
}

int tabs_mutex_unlock(tabs_mutex_t* mutex) {
    if (mutex == NULL) {
        return TABS_ERROR;
    }

#ifdef PLATFORM_WINDOWS
    LeaveCriticalSection(mutex);
    return TABS_SUCCESS;
#else
    int result = pthread_mutex_unlock(mutex);
    if (result != 0) {
        return TABS_ERROR;
    }
    return TABS_SUCCESS;
#endif
}

#endif // TABS_IMPLEMENTATION
#endif // TABS_H
