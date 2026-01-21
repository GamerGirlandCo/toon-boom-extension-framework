#ifndef COMMON_H
#define COMMON_H
#pragma warning(disable : 4113)
#pragma warning(disable : 4311)
#pragma warning(disable : 4047)
#pragma warning(disable : 4024)
#pragma warning(disable : 4312)
#pragma warning(disable : 4020)
#pragma warning(disable : 4700)
#pragma warning(disable : 4133)
#pragma warning(disable : 4142)
#pragma warning(disable : 4005)
#pragma warning(disable : 4244)
#pragma warning(disable : 4267)
#pragma warning(disable : 4838)
#pragma warning(disable : 4430)
#pragma warning(disable : 4996)
#define APP_WINDOWS
#if defined(__BYTEBOOL__)
typedef unsigned char boolean;
typedef unsigned char byte;
#endif
#ifndef __cplusplus
#define true 1
#define false 0
#endif
#define MY_APP_FATAL_ERROR(ctx, message) { printf( "FATAL ERROR: %s\n", message ); MessageBoxA( 0, message, "Fatal Error!", MB_OK | MB_ICONSTOP ); _flushall(); }

#define APP_LOG(ctx, level, message) printf( "%s\n", message )
#ifdef __cplusplus
extern "C" {

void D_DoomMain(void);
#include <app_funcs.h>
#undef APP_FATAL_ERROR
#define APP_FATAL_ERROR MY_APP_FATAL_ERROR
#include <thread.h>
extern thread_mutex_t mus_mutex;
extern thread_signal_t vblank_signal;
extern thread_atomic_int_t app_running;
extern int maketic;
extern int gametic;
void M_StartControlPanel (void);
extern char** myargv;
extern int myargc;
int app_proc(app_t *app, void *user_data);
void M_QuitResponse(int ch);
}
#else
#include <app_funcs.h>
#undef APP_FATAL_ERROR
#define APP_FATAL_ERROR MY_APP_FATAL_ERROR
#endif
#endif