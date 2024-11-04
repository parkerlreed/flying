#ifndef _arcs_h
#define _arcs_h

//
// In dem Modul 'arcs.h' sind alle Klassen vereint, die mit Bällen und Bögen
// zu tun haben. Dabei gelten die folgenden Ableitungen
//
//			   StaticArc			: Grundfunktionalität
//         /         \
//		 OuterArc	 InnerArc	: Bogen, der von außen/innen reflektiert
//        |       \   |
//		    |       ArcWall	 	: beidseitig reflektierend (Ableitung nur symbolisch)
//		StaticBall					: 360 Grad Bogen, nicht beweglich
//******* | **************************************************************
//		  Ball						: Kugel (gleichzeitig ein DynObj)
//    (-->ball.c)
//

#ifndef _object_h
#	include "object.h"		// Oberklasse
#endif
#ifndef _vec2_h
#	include "vec2.h"		// Member-Klasse
#endif


//
// -------------------------------------------------------------------------
//   class StaticArc:  Bogen (analog zu Wall)
// -------------------------------------------------------------------------
//

class StaticArc : public Object {
	protected:
		Vec2	p;			// Mittelpunkt des Bogens, bzw. Position der Kugel
		Real		r;			// Radius der Kugel

	public:
		StaticArc( double x, double y, double r, double start=0.0, double angle=360.0 );
		StaticArc( const Vec2 &v, const Real &r, const Real &start, const Real &angle );

		virtual ~StaticArc();

		virtual void Info();

		const Vec2 &P() const				{ return p; }
		const Real   &PX() const			{ return p.X(); }
		const Real   &PY() const			{ return p.Y(); }
		const	Real   &R() const				{ return r; }

	protected:
		virtual Real HitFromBall( Ball *b ) = 0;
		virtual void CollideWithBall( Ball *b );


		int IsOnArc( const Vec2 &d );

		Real OuterHitFromBall( Ball *b, const Vec2 &dv );
		Real InnerHitFromBall( Ball *b, const Vec2 &dv );
		Real		start;
		Real		angle;

	private:
		int HitFromBallTwice( int outer_hit, Ball *b, const Vec2 &dv, Real md,
													Real *time );

friend class Pocket;
friend class Pool;
};


//
// -------------------------------------------------------------------------
//   class StaticArc:  Bogen (analog zu Wall)
// -------------------------------------------------------------------------
//

class OuterArc : public StaticArc {
	public:
		OuterArc( double x, double y, double ri, double st, double ang ) :
			StaticArc( x, y, ri, st, ang )		{}
		OuterArc( const Vec2 &v, const Real &ri, const Real &st, const Real &ang ) :
			StaticArc( v, ri, st, ang )			{}
		virtual ~OuterArc();

		virtual void Info();

	protected:
		virtual Real HitFromBall( Ball *b );

	private:
};


class InnerArc : public StaticArc {
	public:
		InnerArc( double x, double y, double ri, double st, double ang ) :
			StaticArc( x, y, ri, st, ang )		{}
		InnerArc( const Vec2 &v, const Real &ri, const Real &st, const Real &ang ) :
			StaticArc( v, ri, st, ang )			{}
		virtual ~InnerArc();

		virtual void Info();

	protected:
		virtual Real HitFromBall( Ball *b );

	private:
};



class ArcWall : public StaticArc {
	public:
		ArcWall( double x, double y, double ri, double st, double ang )
			:	StaticArc( x,y,ri, st, ang )											{ }
		virtual ~ArcWall();

		virtual void Info();

	protected:
		virtual Real HitFromBall( Ball *b );
};

//
// -------------------------------------------------------------------------
//   class Static Ball:  Statische Kugel
// -------------------------------------------------------------------------
//

class StaticBall : public OuterArc {
	public:
		StaticBall( double x, double y, double ri=2. )
			: OuterArc( x,y,ri,0.0,360.0 )			{ type=StaticBallObj; }
		StaticBall( const Vec2 &v, const Real &ri )
			: OuterArc( v,ri,0.0,360.0 )			{ type=StaticBallObj; }

		virtual ~StaticBall();

		virtual void Info();

	protected:

friend class Pocket;
friend class Goal;
};


#endif
