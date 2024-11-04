#ifndef	_global_h
#define	_global_h

// Directory, in which to put the bitmap files for deluxe-playing
#ifndef DATA_DIRECTORY
#	define	DATA_DIRECTORY		"/tmp"
#endif

// name of the file to look for static data of the classes
#define	PRESET_FILE			"presets.txt"

#define	LOG_FILE				DATA_DIRECTORY"/logfile.txt"

//
// FDS_TYPE should be a cast to the type that is needed for the filedescriptors
// mask in the select-call. Unfortunately I dont know how to examine this
// type automatically.
// (e.g. it has to be (int*) on HP-UX and can be left empty on linux-systems)
//

// #define	FDS_TYPE				(int*)

#ifndef FDS_TYPE
#	define FDS_TYPE
#endif

//
// As an optimization, every DynObj gets a storage for caching the calculated
// collision-times with other objects. At the moment of a collision, only
// the 2 objects taking part in the collision have to update their cache
// and probably inform other objects about their changes.
#define	TIME_CACHE		1

//
// The collision detection can be aborted at an early stage, when a minimum
// time can be estimated which lies beyond a limit.
// Problem: - The check, if to abort, already takes too much time.
//          - When time-caching is on, advanced collision times, which aren't
//            of any use at the moment of calculation, might be used later on.
#define	ABORT_CALC		0
#if (ABORT_CALC)
#	define	ABORT_CALC_WALL	0
#	define	ABORT_CALC_BALL	0
#endif

//
// The basic floating point class can be exchanged between doubles and
// floats. The latter one is faster on my 386.
#ifndef __TURBOC__
#	define	REAL_IS_FLOAT	0
#else
#	define	REAL_IS_FLOAT	1
#endif

//
// A real C++-Class can be used for real arithmetic. Unfortunately
// that really slows the calculation down, even though the whole class
// is defined inline.
#define	REAL_IS_CLASS	0

//
// There are some specialized vector classes for 2 and 3 dimensionaL
// vectors, which can also be realized by inheriting from an universaL
// vector-class (but again, that's expensive)
#define	Vec2IsVector	0
#define	Vec3IsVector	0

//
// Switch to pre-existing (better to understand) algorithms for
// collision detection, instead of the special algortithm (which is
// a bit faster).
#define	EasyWall		0

//
// special constants instead of the collision time
#define	MAX_TIME				1e10
#define	NO_HIT				MAX_TIME
#define	NO_TARGET			MAX_TIME
#define	RUNNING_LOSE		4e10
#if (ABORT_CALC)
#	define	NOT_REACHABLE	2e10
#endif

//
// constants to overcome the problem with unprecise real-arithmetics
#if (REAL_IS_FLOAT)
#	define	EPS		 	1e-4
#else
#	define	EPS		 	1e-10
#endif

//
// current time in calculation
// not to mix up with the realtime of GetCurrentTime()
//
extern double	current_time;

#ifndef __TURBOC__
#	define	_DEBUG
#endif
//
// debugging switches, the main switch DEBUG enables all successive
// options, mainly leading to traces on stdout
#ifdef DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern long		debug;				// Variable der anzuzeigenden modes
#define	ShowLight			0x00000001L
#define	ShowColors			0x00000002L
#define	ShowRings			0x00000004L
#define	ObjectInfo			0x00000008L
#define	BeforCollision		0x00000010L
#define	AfterCollision		0x00000020L
#define	CheckBoundary		0x00000040L
#define	MoveAll				0x00000080L
#define	__Moves				0x000000f8L
#define	PBallHit				0x00000100L
#define	PointerMove			0x00000200L
#define	XWallHit          0x00000400L
#define	YWallHit				0x00000800L
#define	CollCalc				0x00001000L
#define	AbortCalc			0x00002000L
#define	AbortReCalc			0x00004000L
#define	StickLevel			0x00008000L
#define	GameState			0x00010000L
#define	BMover				0x00020000L
#define	BState				0x00040000L
#define	Sync					0x00080000L
#define	ShowTurns			0x00100000L
#define	Loops					0x00200000L
#define	__ShowAll			0x00100007L
#define	__Rings				0x00100004L
#define	Intro					0x00400000L
#define	xdb               0x00800000L
#define	xwd               0x02000000L
#define  ForceCalc         0x01000000L
#define  Motion				0x04000000L

#define	TCTrace				0x40000000L
#define	UnixTrace			0x80000000L

#define	ShowSubWindow		(ShowLight|ShowColors|ShowRings|ShowTurns)

#define	DBG0(m,f)			if (debug&m)	printf( f )
#define	DBG1(m,f,a)			if (debug&m)	printf( f,a )
#define	DBG2(m,f,a,b)		if (debug&m)	printf( f,a,b )
#define	DBG3(m,f,a,b,c)	if (debug&m)	printf( f,a,b,c )
#define	DBG4(m,f,a,b,c,d)	if (debug&m)	printf( f,a,b,c,d )

int set_debug( const char *flag_name );
void show_flags();

#else

#define	DBG0(m,f)
#define	DBG1(m,f,a)
#define	DBG2(m,f,a,b)
#define	DBG3(m,f,a,b,c)
#define	DBG4(m,f,a,b,c,d)

#endif

#include "stdinc.h"

//
// Für Zeitmessungen kann das Zeichnen der sich bewegenden Bälle komplett
// unterdrückt werden, indem die DrawCircle()-Funktion durch eine leere
// Funktion ersetzt wird.
#ifdef DEBUG
#	define	STATISTICS
#endif
#define	_NO_DRAW

//
// Reibungsimplementierung, in dem schrittweise
// die Geschwindigkeit verkleinert wird. Das Problem ist allerdings, das mit
// jedem Schritt die Zeiten des TIME-CACHE ungültig werden, und daher
// nicht zu viele Schritte pro Sekunden eingelegt werden sollten.
// Die Parametrierung der Reibung und der Auflösung der Schrittberechnung
// geschieht durch virtuelle Funktionen der Game-Klasse
//
// Einschalten der Reibungssimulation durch stufige Verlangsamung:
#define SIM_SLOW				1

#if (SIM_SLOW)
#	define	SUPPRESS_SLOWSTEP		-1.0
#endif


// ===========================================================================
//   'Wissenswertes' aus verschiedenen Bereichen (reduziert Abhängigkeiten)
// ===========================================================================

extern double w2n;					// Fensterskalierung				(graph.C)

typedef long ColorId;


#define	SOUND_SUBSYSTEM	1
#if (SOUND_SUBSYSTEM)
extern	int sound_request;	// sound einschalten							(main.C)
#endif
extern	int size;				// gewünschte (aktuelle) Fenstergröße	(main.C)
extern	int deluxe;				// Luxus-Flag                          (main.C)
extern	int light_flag;		// Light-Sources								(main.C)
extern	int color_flag;		// table color									(main.C)
extern	int nohint_flag;		// aiming hint									(main.C)
extern	int enhanced_mover;	// flag for desired mover class        (main.C)
extern	int frames_per_sec;	// flag for limited frames             (main.C)

extern	int no_server_grabs; // never grab server                   (main.C)
extern	int no_override;		// pseudo root as normal window        (main.C)

#define	ON_ROOT		-1


void show_defaults();
void load_konfi();

#endif
