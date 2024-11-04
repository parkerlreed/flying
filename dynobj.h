#ifndef _dynobj_h
#define _dynobj_h

//
// DynObj ist die Oberklasse für die beweglichen Objekte (zur Zeit nur Bälle)
//
// Funktionaltität:
//
//	- Verwaltung aller dynamischen Objekte (durch statische Liste)
// - statische Funktionen Steuerung der Kollisionsberechnung:
//		- void CollisionCalcAll()
//		- void Collide()
//						Aktuelle Kollision ausführen
//
// - Vorgeschriebene virtuelle Funktionen:
//		- Real NextCollision()
//						Abfrage des Zeitpunktes der nächsten Kollision
//		- void CollisionCalc()								(bei TIME-CACHE)
//						Anstoß der Kollisionsberechnung des Objektes, welches
//						alle Object's mit HitFromBall() nach der nächsten Kollision
//						befragt und das Ergebnis im TIME-CACHE festhält.
//		- void HitYouAt(Object *obj, Real time)		(bei TIME-CACHE)
//						Wenn im CollisionCalc beide Objekte Bälle sind, wird der
//						Wert dem anderen Ball mitgeteilt, damit auch dessen
//						TIME-CACHE auf dem aktuellsten Stand ist.
//

#ifndef _real_h
#	include "real.h"
#endif

#if (TIME_CACHE)
#	ifndef _objtab_h
#		include "objtab.h"
#	endif
#endif

class Ball;
class Object;

class DynObj {
	public:
		DynObj();
		virtual ~DynObj();

		static Real CollisionAll();				// Kollisionsbefragung
		static void Collide();						// aktuelle Kollision ausführen
		virtual Real NextCollision() = 0;		// nächste Berührung mit Ball
		virtual void CollisionCalc() = 0;

typedef void (DynObj::*DynObjFun)();
		static void ForAllDyn( DynObjFun fun );
typedef void (DynObj::*DynObjFunD)(Real);
		static void ForAllDyn( DynObjFunD fun, Real param );

		virtual void	Move( Real );	// Verschiebung des Objektes
		virtual void	Reset();			// Initialisierung des Slowsteps
		virtual void	Redraw();		// Initiales Zeichnes des Objekts

		static void DynInitAll();		// alle Initialisierungsroutinen aufrufen
		static void DynEndAll();

		int IsIdle()	{ return idle; }

	protected:
		static Ball		*hit_ball;		// an Kollision beteiligter Ball
		static Object	*hit_object;	// weiteres an Kollision beteiligtes Objekt
		int				id;

		virtual void DynInit();		  	// z.B. Erzeugung des TIME-CACHE
#if (TIME_CACHE)
		virtual void HitYouAt( Object *obj, Real time ) =0;
		int				col_objects;
		ObjTab			obj_list;		// legt Berechnungsreihenfolge fest
#endif

	private:
		static int				id_count;		// aktueller Objektzähler
		static class DynObj	*dyn_queue;
		class DynObj		 	*next;

		char						idle;				// keine Kollisionsberechnung mehr

friend class Ball;
friend class PBallTop;
};

#endif
