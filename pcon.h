#ifndef _pcon_h
#define _pcon_h

#ifndef _real_h
#	include "real.h"
#endif

class Vec2;
class Pocket;
class Ball;
class StaticArc;
class Wall;

class PocketConnector {
	public:
		PocketConnector()		{ w1=w2=w3=0; b1=b2=0; }
		~PocketConnector();

		void RoundedSegment( const Vec2 &e1, const Vec2 &e2,
									Vec2 *st,	const Real &r,
									Wall **w, StaticArc **a );
		int Init( const Pocket &p1, const Real &angle1,
			const Pocket &p2, const Real &angle2, const Real &cushion, const Real &rad );

	private:
		Wall			*w1;
		StaticArc	*b1;
		Wall			*w2;
		StaticArc	*b2;
		Wall			*w3;

friend class Pool;
};

#endif
