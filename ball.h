#ifndef _ball_h
#define _ball_h

//
// In dem Modul 'ball.h' sind alle Klassen vereint, die mit Bällen und Bögen
// zu tun haben. Dabei gelten die folgenden Ableitungen
//
//			   StaticArc			: Grundfunktionalität
//         /         \
//		 OuterArc	 InnerArc	: Bogen, der von außen/innen reflektiert
//        |       \   |
//		    |       ArcWall	 	: beidseitig reflektierend (Ableitung nur symbolisch)
//		StaticBall					: 360 Grad Bogen, nicht beweglich
//        |
//		  Ball						: Kugel (gleichzeitig ein DynObj)
//
//
// Eine Besonderheit stellt der Ball dar, welcher die Funktionalität des
// DynObj und des Object besitzt. Außerdem ist er ein Beispiel, wie durch
// spezielle Ausnutzung der HitFromBall-Routine die Kontrolle an ein Objekt
// gebracht werden kann. Um Schrittweise die Geschwindigkeit durch Reibung
// zu verringern, gibt er in der HitFromBall-Routine die Zeit des nächsten
// Verlangsamungsschrittes zurück, wenn er nach einer Kollision mit sich
// selbst befragt wird.
//

#ifndef _arcs_h
#	include "arcs.h"			// Oberklasse StaticBall
#endif
#ifndef _dynobj_h
#	include "dynobj.h"		// zweite Oberklasse DynObj
#endif

class BallStateTop;		// forward
class PBallTop;

//
// -------------------------------------------------------------------------
//   class Ball:  Fliegende Kugeln
// -------------------------------------------------------------------------
//

class Ball : public StaticBall, public DynObj {
	public:		// private
		Vec2		v;				// Geschwindigkeits-Vektor
		char			run_flag;	// Geschwingkeit ungleich null
		const Real	a;				// Absolutwert der Negativ-Beschleunigung
#if (SIM_SLOW)
		Real			next_slowstep;		// Zeitpunkt der nächsten Verlangsamung
		Real			last_slowstep;		// Zeitpunkt der letzten Verlangsamung
		Real			slow_granoff;		// Offset bei Start, damit Kugeln zu
												// unterschiedlicher Zeit gebremst werden.
		void			SlowStep();			// Kugel verlangsamen
#endif

		PBallTop	*pball;				// PBall, welcher den Lock hat
		int	Lock( PBallTop *pball );
		int	Unlock( PBallTop *pball );
		void	GoUnlocked();

		BallStateTop	*state;
		void SetState( BallStateTop *state_in )	{ state = state_in; }

	public:		// protected
		Real			m;			// Masse, wird vom RemotePBall veraendert !!!

#if (TIME_CACHE)
		virtual void CollisionCalc();
		virtual void HitYouAt( Object *obj, Real time );

		Object	*collision_object;
		Real		collision_time;
#endif

	public:
		Ball( double x, double y, double vx, double vy, double r, double m );
		Ball( double x, double y, double vx, double vy, double r );
		Ball( double x, double y, double vx, double vy );
		Ball( const Vec2 &v, double r, double m );
		Ball( const Vec2 &v, double r );
		~Ball();

		const Vec2 &V() const			{ return v; }
		const Real   &VX() const		{ return v.X(); }
		const Real   &VY() const		{ return v.Y(); }
//		Real   VXpix() const				{ return v.Xpix(); }
//		Real   VYpix() const				{ return v.Ypix(); }
		const	Real   &M() const			{ return m; }

		int IsRunning() const			{ return run_flag; }

		Vec2 Dir() const;
		Real Len() const					{ return V().Norm(); }

		void SetP( const Vec2 &new_p );	// Kugel auf Position setzen (mit Zeichnen)
		void SetV( const Vec2 &v_in );
		void SetPV( const Vec2 &new_p, const Vec2 &v_in=Vec2Zero )
												{ SetP( new_p ); SetV( v_in ); }
		void ChgV( const Vec2 &v_in ) {
			SetV( v_in );
#if (TIME_CACHE)
			CollisionCalc();
#endif
		}

		void TellPressed();

		Real HitFromBall( Ball *b );
		virtual Real NextCollision();
		void CollideWithBall( Ball *b );

		static Real FindClosest( const Ball *myself,  const Vec2 &pos, Ball **best );
		int FitsAt(const Vec2 &pos);
		int FitsNextTo(const Vec2 &pos, const Vec2 &delta, Vec2 *newpos);

		virtual void Move( Real t );
		virtual void Reset();
		virtual void Redraw();

		virtual void Info();
		virtual void WasHit(Object *);

		const BallStateTop *GetState()					{ return state; }

		virtual void Draw();

friend class PBallTop;
friend class LineKeeper;
friend class StackKeeper;
friend class Goal;
};


#endif
