#ifndef _goal_h
#define _goal_h

#ifndef _wall_h
#	include "wall.h"
#endif

class Vec2;


class Goal : public Wall {
	public:
		Goal( const Vec2 &v1, const Vec2 &v2 );
		virtual ~Goal();

		virtual void Info();

	protected:
//		virtual Real HitFromBall( Ball *b );
		virtual void CollideWithBall( Ball *b );
};

#endif
