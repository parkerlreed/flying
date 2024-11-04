#ifndef _object_h
#define _object_h

//
// Object ist die Oberklasse der Objekte, die von sich bewegenden Objekten
// berührt werden können.
//
// Funktionaltität:
//
//	- Verwaltung aller Objekte (durch statische Liste)
// - Identifikation der Objekte (durch spezielle ID's im member dyn_id)
// - Vorgeschriebene virtuelle Funktionen für die Collisionsberechnung:
//		- Real HitFromBall( Ball *b )
//						Kollisionszeitpunkt des Objektes mit Ball b bestimmen.
//		- void CollideWithBall( Ball *b )
// 					Ausführung der Kollision mit Änderung der
// 					Richtungsvektorn an sich selbst und dem beteiligten Ball.
//		- void WasHit( Object *o )
//						Nach CollideWithBall wird auch der beteiligte Ball über
//						die stattgefundene Kollision informiert.
//

#ifndef _global_h
#	include "global.h"
#endif
#ifndef _real_h
#	include "real.h"
#endif

//
// -------------------------------------------------------------------------
//   class Object:  Oberklasse
// -------------------------------------------------------------------------
//
class Ball;		// forward-declaration, da Object einen Klassenzeiger braucht

class Object {
	public:
		Object();
		virtual ~Object();

		virtual Real	Collision();

typedef	void (Object::*ObjFun)();
		static void ForAll( ObjFun fun );

		virtual void Info();

		static Real min_time;

		virtual void Draw();

		typedef enum {
			StaticArcObj,
			StaticBallObj,
			WallObj,
			BallObj,
			HandObj,		// für Hockey, um von Bällen zu unterscheiden
			PocketObj,
			GoalObj,
			UnknownObj
		} ObjType;

		int	id;

	protected:
		virtual Real HitFromBall( Ball *b )	= 0;
		virtual void CollideWithBall( Ball *b ) = 0;
		virtual void WasHit(Object*);

		static int		id_count;		// aktueller Objektzähler
		int	dyn_id;						// Id des DynObj (bei Object auf -1 gesetzt)
		static class Object *stat_queue;
		class Object *next;

		ObjType type;

private:

friend class DynObj;
friend class Ball;
friend class PBallTop;
friend class Game;
friend class Hockey;
};

#endif
