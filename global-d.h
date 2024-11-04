#ifndef	_global_h
#define	_global_h

#define	DATA_DIRECTORY		"fly.dta"
#define	PRESET_FILE			"presets.txt"
#define	LOG_FILE				DATA_DIRECTORY"/logfile.txt"

//
// Der Time-Cache ist eine Optimierung innerhalb der DynObj's, die nun
// für alle anderen Objekte einen Zeitenspeicher anlegen, in welchem die
// berechneten Zeiten hinterlegt werden. Bei Kollisionen überarbeiten dann
// lediglich die von der Kollision betroffenen Objekte ihren Zeitspeicher.
// (Gegebenenfalls müssen sie allerdings auch andere Objekte von den neuen
// Werten in Kenntnis setzen, aber das Verfahren ist eine führt zu einer
// erheblichen Beschleunigung.
#define	TIME_CACHE		1

//
// Die Kollisionsberechnung wird frühzeitig abgebrochen, wenn die Zeit
// vorraussichtlich über der Minimalzeit liegen wird.
// Problem: - Bis zum Test zum Abbrechen wird bereits zuviel Rechenzeit benötigt.
//          - Durch den Time-Cache kann der größere Wert später noch einmal
//            von Bedeutung sein und die Berechnung muß nochmal gestartet werden.
#define	ABORT_CALC		0
#if (ABORT_CALC)
#	define	ABORT_CALC_WALL	0
#	define	ABORT_CALC_BALL	0
#endif

//
// Die den Vektoren unterlagerte Klasse Real kann ausgetauscht werden.
// float ist schneller, aber natürlich ungenauer, was allerdings in den
// meisten Fällen durch das gewählte EPS ausgeglichen werden kann.
#ifndef __TURBOC__
#	define	REAL_IS_FLOAT	0
#else
#	define	REAL_IS_FLOAT	1
#endif

//
// Bei einer Klasse wird eine eigene Real-Klasse angeboten, die alle
// notwendigen Rechenoperationen definiert. Bei TurboC++ funktioniert
// das aus ungeklärten Gründen allerdings nicht.
#define	REAL_IS_CLASS	0

//
// Als Test der allgemeinen Vektorklasse der Algebra-Bibliothek koennen
// die beiden Spezial-Klassen Vec2 und Vec3 auch von der allgemeinen Vektor-
// Klasse abgeleitet werden (bei entsprechendem Zeitverlust)
#define	Vec2IsVector	0
#define	Vec3IsVector	0

//
// Überschaubarer Wand-Algorithmus:
// Der Algorithmus in Wall::HitFromBall() wird über die allgemeinen Methoden
// Solve und Split der Vektor-Klasse realisiert und ist somit leichter
// verständlich (ist jedoch langsamer als die Speziallösung)
#define	EasyWall		0

//
// Konstante, die als Zeit für 'keine Kollision' in der
// Kollisionsberechnung verwendet wird.
//
#define	MAX_TIME				1e10
#define	NO_HIT				MAX_TIME
#define	NO_TARGET			MAX_TIME
#define	RUNNING_LOSE		4e10
#if (ABORT_CALC)
#	define	NOT_REACHABLE	2e10
#endif

//
// Konstante, die bei Vergleichen von Real-Werten die Rechenungenauigkeit
// kompensieren soll.
// Bei Verwendung von double 1e-10, floats 1e-3
//
#if (REAL_IS_FLOAT)
#	define	EPS		 	1e-4
#else
#	define	EPS		 	1e-10
#endif

//
// Variable mit der aktuellen Zeit der Rechnung
// (nicht zu verwechseln mit der Echtzeit aus GetCurrentTime())
//
extern double	current_time;

#ifndef __TURBOC__
#	define	_DEBUG
#endif
//
// Debugging ist an verschiedensten Stellen aktiv, wenn zusätzlich
// noch der entsprechend mode gesetzt wird.
//
#ifdef DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern long		debug;				// Variable der anzuzeigenden modes
#define	ShowLight			0x00000001l
#define	ShowColors			0x00000002l
#define	ShowRings			0x00000004l
#define	ObjectInfo			0x00000008l
#define	BeforCollision		0x00000010l
#define	AfterCollision		0x00000020l
#define	CheckBoundary		0x00000040l
#define	MoveAll				0x00000080l
#define	__Moves				0x000000f8l
#define	PBallHit				0x00000100l
#define	PointerMove			0x00000200l
#define	XWallHit          0x00000400l
#define	YWallHit				0x00000800l
#define	CollCalc				0x00001000l
#define	AbortCalc			0x00002000l
#define	AbortReCalc			0x00004000l
#define	StickLevel			0x00008000l
#define	GameState			0x00010000l
#define	BMover				0x00020000l
#define	BState				0x00040000l
#define	Sync					0x00080000l
#define	ShowTurns			0x00100000l
#define	Loops					0x00200000l
#define	__ShowAll			0x00100007l
#define	__Rings				0x00100004l
#define	Intro					0x00400000l
#define	xdb               0x00800000l
#define	xwd               0x02000000l
#define  ForceCalc         0x01000000l
#define  Motion				0x04000000l

#define	TCTrace				0x40000000l
#define	UnixTrace			0x80000000l

#define	ShowSubWindow		(ShowLight|ShowColors|ShowRings|ShowTurns)

#define	DBG0(m,f)			if (debug&m)	printf( f )
#define	DBG1(m,f,a)			if (debug&m)	printf( f,a )
#define	DBG2(m,f,a,b)		if (debug&m)	printf( f,a,b )
#define	DBG3(m,f,a,b,c)	if (debug&m)	printf( f,a,b,c )
#define	DBG4(m,f,a,b,c,d)	if (debug&m)	printf( f,a,b,c,d )

int set_debug( const char *flag_name );
void show_flags();

#else
extern "C" {
	int printf(const char *__format, ...);
	void exit(int __status);
};

#define	DBG0(m,f)
#define	DBG1(m,f,a)
#define	DBG2(m,f,a,b)
#define	DBG3(m,f,a,b,c)
#define	DBG4(m,f,a,b,c,d)

#endif

//
// Für Zeitmessungen kann das Zeichnen der sich bewegenden Bälle komplett
// unterdrückt werden, indem die DrawCircle()-Funktion durch eine leere
// Funktion ersetzt wird.
#define	STATISTICS
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

#define	ON_ROOT		-1


void show_defaults();
void load_konfi();

#endif
