#ifndef _wall_h
#define _wall_h

#ifndef _object_h
#	include "object.h"
#endif
#ifndef _vec2_h
#	include "vec2.h"
#endif

//
// -------------------------------------------------------------------------
//   class Wall:  Begrenzungs-Objekt
// -------------------------------------------------------------------------
//

class Wall : public Object {
	public:
		Wall( double x1, double y1, double x2, double y2 );
		Wall( const Vec2 &v1, const Vec2 &v2 );
		virtual ~Wall();

		virtual Real HitFromBall( Ball *b );
		virtual void CollideWithBall( Ball *b );
		virtual void Info();

	protected:
		Vec2	p1,p2;

		Vec2	d;			// p2-p1
		Vec2	ds;		// normierter Vektor senkrecht zu d

friend class Pool;
};

//
// -------------------------------------------------------------------------
//   class XWall:  Wand ohne Begrenzung
// -------------------------------------------------------------------------
//

class XWall : public Wall {
	public:
		XWall( double x, double y1, double y2 ) : Wall(x,y1,x,y2)	{ ; }
		virtual ~XWall();
		Real HitFromBall( Ball *b );
		void	 CollideWithBall( Ball *b );
};
//
// -------------------------------------------------------------------------
//   class YWall:  Wand ohne Begrenzung
// -------------------------------------------------------------------------
//

class YWall : public Wall {
	public:
		YWall( double x1, double x2, double y ) : Wall(x1,y,x2,y)	{ ; }
		virtual ~YWall();
		Real HitFromBall( Ball *b );
		void	 CollideWithBall( Ball *b );
};

#endif
