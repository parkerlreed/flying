#ifndef _global_h
#	include "global.h"
#endif

#ifndef _goal_h
#	include "goal.h"
#endif
#ifndef _ball_h
#	include "ball.h"
#endif
#ifndef _game_h
#	include "game.h"
#endif



Goal::Goal(const Vec2 &v1, const Vec2 &v2 )
: Wall(v1,v2) {
	type   = GoalObj;
	dyn_id = -2;		// Kennzeichnung der Goals fuer PreCalc des Pointers
}


Goal::~Goal()	{}


void Goal::Info() {
	printf( "%02d: Goal:       %08lx: (%4.1f,%4.1f) - (%4.1f,%4.1f)\n",
				Object::id, (unsigned long)this,
				(double)p1.X(), (double)p1.Y(),
				(double)p2.X(), (double)p2.Y() );
}


void Goal::CollideWithBall( Ball *b ) {
	if (b->type==BallObj) {
		g->TakeOffBoard(b);
	}
	else {
		Wall::CollideWithBall(b);
	}
}

