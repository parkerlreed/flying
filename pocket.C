#ifndef _global_h
#	include "global.h"
#endif

#ifndef _pocket_h
#	include "pocket.h"
#endif
#ifndef _game_h
#	include "game.h"		// zum Ansprechen des Keepers
#endif

Pocket::Pocket(double x, double y, double r)
: StaticArc( x,y,r,0.0,360.0 ) {
	type   = PocketObj;
	dyn_id = -2;		// Kennzeichnung der Pockets fuer PreCalc des Pointers
}


Pocket::~Pocket()	{}


void Pocket::Info() {
	printf( "%02d: Pocket:     %08lx: P()=(%4.1f,%4.1f), R()=%3.1f\n",
				id, (unsigned long)this,
				(double)PX(), (double)PY(), (double)R() );
}


Real Pocket::HitFromBall( Ball *b ) {
Real	time;
	if (b->IsIdle())	return MAX_TIME;

	switch( HitFromBallTwice( 1, b, b->V(), R(), &time ) ) {
	case 1:		return NO_HIT;
#if (ABORT_CALC_BALL)
	case 2:
#ifdef DEBUG
		if (debug&AbortCalc) {
			printf( "    PocketHit from %d to %d aborted.\n", b->Object::id, id );
		}
#endif
		return NOT_REACHABLE;
#endif
	default:
		if (time>-EPS)	return time;
		else				return NO_HIT;
	}
}


void Pocket::CollideWithBall( Ball *b ) {
	g->TakeOffBoard(b);
}
