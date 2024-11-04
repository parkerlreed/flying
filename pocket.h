#ifndef _pocket_h
#define _pocket_h

#ifndef _ball_h
#	include "ball.h"
#endif
#ifndef _keeper_h
#	include "keeper.h"
#endif


class Pocket : public StaticArc {
	public:
		Pocket( double x, double y, double r=2. );
		~Pocket();

		void Info();
		void CollideWithBall( Ball *b );

	protected:
		Real HitFromBall( Ball *b );
};

#endif
