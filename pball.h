#ifndef _pball_h
#define _pball_h

//
// In diesem Modul wird die Klasse PBallTop deklariert, die der
// rechnerspezifischen Klasse PBall als Oberklasse dient.
//
#ifndef _vec2_h
#	include "vec2.h"
#endif

class Ball;

typedef enum {
	BillardQueue,
	DiscSlider,
	DiscThrower
} PBallType;

class PBallTop {
	public:
		PBallTop(PBallType type=BillardQueue);
		virtual ~PBallTop();

		virtual void Warp( const Vec2 &dest ) = 0;

typedef void (PBallTop::*PBallFun)();
		static void ForAll( PBallFun fun );

		void CueMoved();								// Callback an der Kugel
		void CueWasHit();								// Callback an der Kugel

		virtual void Redraw();
		virtual void Update();

	protected:
		virtual void Press(int button);
		virtual void Release(int button);

		typedef enum {
			Unlocked		=   1,
			Moving		=   2,
			Aiming		=   4,
			Charging		=   8,
			OldCharging =  16,
			Shooting		=  32,
			OldShooting =  64,
			ChargeOrShoot	= 120,	// Charging|Shooting
			Locked			= 124,	// Aiming|Charging|Shooting
			LockedQueue = 128,
			UsingTool		= 252,	// Aiming|Charging|Shooting|LockedQueue
			PullMoving	= 256,
			ReadyToSleep	= 263		// Unlocked|Moving|Aiming|PullMoving
		} Mode;
		Mode					mode;
friend class PBallNorm;				// for WaitForEvents-Implementation

		PBallType			pball_type;

		Ball *cue;
		Ball *default_cue;
		Real FindPressed( Ball **best );			// sucht n‰chste Kugel
		Ball *FindCueBall();							// sucht n‰chstgelegene Kugel

		void StartAiming( Ball *cueball=0L );	// Zielen mit gew‰hlter Kugel
		void StartCharging( int old );			// Aufladen
		void RestartCharging();
		void StartShooting( int old );			// Animation des Stoﬂes starten
		void Shoot();									// Stoﬂ auslˆsen, Kugel loslassen

		void StartPullMoving( Ball *cueball=0L );// Kugel auf Tisch bewegen

		void StartMoving( Ball *cueball=0L );	// Kugel direkt bewegen
	public:
		void StopMoving();							// bewegte Kugel loslassen
	protected:
		void ReleaseBall();							// Kugel loslassen
		void GoUnlocked();							// ‹bergang in Unlock-Zustand

	// Pointerbewegung
		Vec2	dest;				  	  	// aktuelle Position der Maus
//		union {
			Real		target_time;		// abs. Zeit, wann Ziel erreicht sein soll
			Real		lock_time;			// lock-Zeit nach Schuﬂ
//		};
		Vec2	q_end;					// Spitze des Queue's (w‰hrend lock-Zeit)

	public:
		static Real WarpTime;			// Zeitspanne f¸r einen Maussprung
		static Real LockedQueueDelay;	// Nachlauf des gelock'ten Queues

	protected:
		void WarpRecalc();
		void PointerMoveTo( const Vec2 &pointer_position );

		virtual void RedrawPointer() = 0;				// Mausanzeige
		virtual void SetPointer( int x, int y ) = 0;	// Mausanzeige

	// Aiming Tool
		int	tool_is_visible;
		virtual void StartAimingTool();
		virtual void MoveAimingTool();
		virtual void EndAimingTool();
		virtual void DrawAimingTool(int charge_only=0);
		void		SaveAimingState();

		int	valid_queue_position;
		Vec2	q_end_s, q_s1_s, q_s2_s;
		virtual void StartQueue(const Vec2 &end, const Vec2 &s1, const Vec2 &s2);
		virtual void MoveQueue(const Vec2 &end, const Vec2 &s1, const Vec2 &s2);
		virtual void EndQueue();
		virtual void DrawQueue();

#define MAX_PREPOS	3
		void 		PreCalc();
		int		ncalc_pos;
		Vec2		calc_pos[MAX_PREPOS];
		Vec2		balls_dir;					// Richtung zur Zielkugel
		Vec2		my_dir;						// Richtung der Stoﬂkugel

	// Charger
		Real		next_chargestep;			// Zeit des n‰chsten Aufladens
		Real		last_chargestep;			// Zeit des letzten Aufladens
		Real		charge;						// Aktuelle Ladung
		Real		charge_increment;			// Aktuelle Einheiten pro Sekunde
		Real		save_charge;				// Ladung beim Abschuﬂ

		int		id;

		int		aim_hint;		// Spezialhilfe zum Zielen
		int		aim_mode;		// Pointer-Mode beim letzten Zeichnen
		Vec2		aim_p;			// Kugelposition beim letzten Zeichnen
		Real		aim_r;			// Kugelradius
	//	int		aim_x, aim_y;	// alte Pointerposition
		Vec2		old_dest;		// alte Pointerposition
		Real		aim_charge;
		int		int_charge;
		char		hint_valid;

		int		x_old, y_old;	// alte Pointerposition (von SetPointer)

	protected:
		static int					id_count;
		static class PBallTop	*pball_queue;
		class PBallTop				*next;
};

#ifndef __TURBOC__
#	include "xpball.h"
#else
#	include "dospball.h"
#endif

#endif
