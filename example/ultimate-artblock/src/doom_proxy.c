
#ifndef __cplusplus
// typedef enum {false, true} boolean;
#define false 0
#define true 1
typedef unsigned char byte;
#endif
#define MY_APP_FATAL_ERROR(ctx, message) { printf( "FATAL ERROR: %s\n", message ); MessageBoxA( 0, message, "Fatal Error!", MB_OK | MB_ICONSTOP ); _flushall(); }

#define APP_LOG(ctx, level, message) printf( "%s\n", message )
// #include <app.h>
// #undef APP_IMPLEMENTATION
#define APP_IMPLEMENTATION
#include <app.h>
#undef APP_IMPLEMENTATION
#ifdef _WIN32
#define strcasecmp stricmp
#define strncasecmp strnicmp
#else
#include <strings.h>	
#endif

#define CONCAT_IMPL( x, y ) x##y
#define CONCAT( x, y ) CONCAT_IMPL( x, y )
#define rcsid CONCAT( rcsid, __COUNTER__ )


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

int doom_access( char const* _FileName, int _AccessMode ) {
    FILE* f = fopen( _FileName, "rb" );
    if( f ) {
        fclose(f);
        return 0;
    }
    return 1;
}
#ifdef _WIN32
	#include <unistd.h>
#else 
	#include <unistd.h>
#endif

#undef access
#define access doom_access

#define open doom_open
#define close doom_close

#undef LoadMenu
#include <am_map.c>
#include <doomdef.c>
#include <doomstat.c>
#include <dstrings.c>
#include <d_items.c>
#include <d_main.c>
#include <d_net.c>
#include <f_finale.c>
#include <f_wipe.c>
#include <g_game.c>
#include <hu_lib.c>
#include <hu_stuff.c>
#include <info.c>
#include <m_argv.c>
#include <m_bbox.c>
#include <m_cheat.c>
#include <m_fixed.c>
#include <m_random.c>
#include <m_swap.c>
#include <p_ceilng.c>
#include <p_doors.c>
#include <p_enemy.c>
#include <p_floor.c>
#include <p_inter.c>
#include <p_lights.c>
#include <p_map.c>
#include <p_maputl.c>
#include <p_mobj.c>
#include <p_plats.c>
#include <p_pspr.c>
#include <p_saveg.c>
#include <p_setup.c>
#include <p_sight.c>
#include <p_spec.c>
#include <p_switch.c>
#include <p_telept.c>
#include <p_tick.c>
#include <p_user.c>
#include <r_bsp.c>
#include <r_data.c>
#include <r_draw.c>
#include <r_main.c>
#include <r_plane.c>
#include <r_segs.c>
#include <r_sky.c>
#include <r_things.c>
#include <sounds.c>
#undef BG
#include <st_lib.c>
#include <st_stuff.c>
#define channels xchannels
#include <s_sound.c>
#undef channels
#include <tables.c>
#include <v_video.c>
#define anim_t wi_anim_t
#define anims wi_anims
#define time wi_time
#include <wi_stuff.c>
#undef anims
#undef anim_t
#undef time
#include <z_zone.c>

#undef open
#undef close

#include <m_menu.c>
#include <m_misc.c>
#define strupr xstrupr
#include <w_wad.c>
#undef strupr

#include <i_net.c>

#undef MAXCHAR
#undef MAXSHORT
#undef MAXINT
#undef MAXLONG
#undef MINCHAR
#undef MINSHORT
#undef MININT
#undef MINLONG

#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#ifdef __wasm__
#define APP_WASM
#define WA_CORO_IMPLEMENT_NANOSLEEP
#else
#define APP_WINDOWS
#endif
#define boolean HACK_TO_MAKE_BOOLEAN_NOT_BE_DEFINED

#define FRAMETIMER_IMPLEMENTATION
#include <frametimer.h>

#define CRTEMU_IMPLEMENTATION
#include <crtemu.h>

#ifndef __wasm__
#define THREAD_IMPLEMENTATION
#if defined( __TINYC__ )
	typedef struct _RTL_CONDITION_VARIABLE { PVOID Ptr; } RTL_CONDITION_VARIABLE, *PRTL_CONDITION_VARIABLE;
	typedef RTL_CONDITION_VARIABLE CONDITION_VARIABLE, *PCONDITION_VARIABLE;
	static VOID (*InitializeConditionVariable)( PCONDITION_VARIABLE );
	static VOID (*WakeConditionVariable)( PCONDITION_VARIABLE );
	static BOOL (*SleepConditionVariableCS)( PCONDITION_VARIABLE, PCRITICAL_SECTION, DWORD );
#endif
#include <thread.h>
#undef THREAD_IMPLEMENTATION
#endif

#undef boolean 

#define MUS_IMPLEMENTATION
#include <mus.h>

#define TSF_IMPLEMENTATION
#define TSF_POW     pow
#define TSF_POWF    (float)pow
#define TSF_EXPF    (float)exp
#define TSF_LOG     log
#define TSF_TAN     tan
#define TSF_LOG10   log10
#define TSF_SQRT   (float)sqrt
#define TSF_SQRTF   (float)sqrt
#include <math.h>
#include <tsf.h>

#include <soundfont.c>

#include <i_sound.c>
#include <i_video.c>
#include <i_system.c>

