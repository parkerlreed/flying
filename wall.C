#ifndef _global_h
#	include "global.h"
#endif

#ifndef _wall_h
#	include "wall.h"
#endif
#ifndef _ball_h
#	include "ball.h"
#endif
#ifndef _game_h
#	include "game.h"
#endif

//
// -------------------------------------------------------------------------
//   class Wall:  Begrenzungs-Objekt
// -------------------------------------------------------------------------
//

Wall::Wall( double x1, double y1, double x2, double y2 )
: p1(x1,y1), p2(x2,y2)
{
	d  = p2-p1;               	// Richtungsvektor der Wand
	ds = d.TurnLeft().Norm1();	// normierter senkrechter Vektor zur Richtung
}
Wall::Wall( const Vec2 &v1, const Vec2 &v2 )
: p1(v1), p2(v2)
{
	d  = p2-p1;               	// Richtungsvektor der Wand
	ds = d.TurnLeft().Norm1();	// normierter senkrechter Vektor zur Richtung
}


Wall::~Wall() { }

void Wall::Info() {
	printf( "%02d: Wall:       %08lx: (%4.1f,%4.1f) - (%4.1f,%4.1f)\n",
				Object::id, (unsigned long)this,
				(double)p1.X(), (double)p1.Y(),
				(double)p2.X(), (double)p2.Y() );
}

Real Wall::HitFromBall( Ball *b ) {
	if (b->IsIdle())		return MAX_TIME;

#if (EasyWall)
Real		t1, l1;
Vec2	direct_v;
Vec2	balls_edge;

	b->V().Split( ds, &direct_v );
	if (direct_v.X()==0.0&&direct_v.Y()==0.0)	return NO_HIT;
	balls_edge = b->P() + direct_v.Norm1()*b->R();

	if (Vec2::Solve( p1, d, balls_edge, direct_v, &t1 ))	return NO_HIT;
#if (ABORT_CALC_WALL)
	if	( t1>b->collision_time-current_time+EPS )				return NOT_REACHABLE;
#endif

	if (t1<-EPS)					return NO_HIT;
	if (d.X()!=0.0) {
		l1 = ( balls_edge.X() + t1*b->VX() - p1.X() ) / d.X();
	}
	else if (d.Y()!=0.0) {
		l1 = ( balls_edge.Y() + t1*b->VY() - p1.Y() ) / d.Y();
	}
	else 								return NO_HIT;	// Wand ohne Länge ??
	if ( l1>=0.0 && l1<=1.0 )	return t1;
	return NO_HIT;

#else
Real	A,B,C;	// Hilfsvariablen
Real	l1, t1;	// erste Lösung des Gleichungssystems
Real	l2, t2;	// zweite Lösung des Gleichungssystems

//
// erste Variante des Gleichungssystems, nur lösbar wenn VX() ungleich 0
//
	if ( d.X() != 0.0 ) {

		A = b->VY() - ( b->VX()/d.X()*d.Y() );
		if ( A!=0.0 )	{

			B = p1.Y()-b->PY() + ( b->PX()-p1.X() )/d.X()*d.Y();
			C = b->R() * (ds.Y() - (ds.X()/d.X()*d.Y()));
	//
	// eine Kollision findet statt, wenn beide Zeiten größer/gleich 0
	//
			t1 = (B+C)/A;
			if ( t1<-EPS )		return NO_HIT;
			t2 = (B-C)/A;
			if ( t2<-EPS )		return NO_HIT;
	//
	// für die kleinere Zeit muß l? zwischen 0 und 1 liegen
	//
			if ( t1<t2 ) {
#if (ABORT_CALC_WALL)
				if	( t1>b->collision_time-current_time+EPS ) {
					DBG2(AbortCalc,"    WallHit from %d to %d aborted.\n",b->Object::id,id);
					return NOT_REACHABLE;
				}
#endif
				l1 = ( b->PX() + t1*b->VX() - p1.X() - b->R()*ds.X() ) / d.X();
				if ( l1>=0.0 && l1<=1.0 )	return t1;
				else								return NO_HIT;
			}
			else {
#if (ABORT_CALC_WALL)
				if	( t2>b->collision_time-current_time+EPS ) {
					DBG2(AbortCalc,"    WallHit from %d to %d aborted.\n",b->Object::id,id);
					return NOT_REACHABLE;
				}
#endif
				l2 = ( b->PX() + t2*b->VX() - p1.X() + b->R()*ds.X() ) / d.X();
				if ( l2>=0.0 && l2<=1.0 )	return t2;
				else								return NO_HIT;
			}
		}
	}

//
// Zweite Variante des Gleichungssystems, nur lösbar, wenn VY() ungleich 0.
//
	if ( d.Y() != 0.0 ) {

		A = b->VX() - ( b->VY()/d.Y()*d.X() );
		if ( A!=0.0 )	{

			B = p1.X()-b->PX() + ( b->PY()-p1.Y() )/d.Y()*d.X();
			C = b->R() * (ds.X() - (ds.Y()/d.Y()*d.X()));
	//
	// eine Kollision findet statt, wenn beide Zeiten größer/gleich 0
	//
			t1 = (B+C)/A;
			if ( t1<-EPS )		return NO_HIT;
			t2 = (B-C)/A;
			if ( t2<-EPS )		return NO_HIT;
	//
	// für die kleinere Zeit muß l? zwischen 0 und 1 liegen.	//
			if ( t1<t2 ) {
				l1 = ( b->PY() + t1*b->VY() - p1.Y() - b->R()*ds.Y() ) / d.Y();
				if ( l1>=0.0 && l1<=1.0 )	return t1;
				else								return NO_HIT;
			}
			else {
				l2 = ( b->PY() + t2*b->VY() - p1.Y() + b->R()*ds.Y() ) / d.Y();
				if ( l2>=0.0 && l2<=1.0 )	return t2;
				else								return NO_HIT;
			}
		}
	}

//
// keine Lösung möglich ?
//
	return NO_HIT;
#endif
}


void Wall::CollideWithBall( Ball *b ) {
Vec2	o, p;

	g->HitWall(b);
	b->V().Split(ds,&o,&p);
	b->SetV(p-o);
}

//
// -------------------------------------------------------------------------
//   class XWall:  Wand ohne Begrenzung
// -------------------------------------------------------------------------
//

XWall::~XWall()	{ }

Real XWall::HitFromBall( Ball *b )
{
	if (b->IsIdle())		return MAX_TIME;

Real	min_t;					// ausgerechnete Zeit
Real	sx=p1.X()-b->PX();		// zurückzulegende Strecke (ohne Radius)

	if (( sx > 0.0 ) && ( b->VX() > 0.0 )) {
		sx -= b->R();				// Kugelradius berücksichtigen
	}
	else if ( sx < 0.0 ) {
		if ( b->VX() >= 0.0 )		return NO_HIT;
		sx += b->R();				// Kugelradius berücksichtigen
	}
	else if (b->VX()==0.0)			return NO_HIT;

	min_t = sx / b->VX();

#ifdef DEBUG
	if (debug&XWallHit) {
		printf( "XWall-Hit in %g secs\n", (double)min_t );
		Info();
		b->Info();
	}
#endif

	if (min_t<-EPS)	return NO_HIT;
	else						return min_t;
}

void XWall::CollideWithBall( Ball *b )
{
	g->HitWall(b);
	b->SetV(Vec2( - b->VX(), b->VY() ));
}

//
// -------------------------------------------------------------------------
//   class YWall:  Wand ohne Begrenzung
// -------------------------------------------------------------------------
//

YWall::~YWall()	{ }

Real YWall::HitFromBall( Ball *b )
{
	if (b->IsIdle())		return MAX_TIME;

Real	min_t;					// ausgerechnete Zeit
Real	sy=p1.Y()-b->PY();		// zurückzulegende Strecke (ohne Radius)

	if (( sy > 0.0 ) && ( b->VY() > 0.0 )) {
		sy -= b->R();				// Kugelradius berücksichtigen
	}
	else if ( sy < 0.0 ) {
		if ( b->VY() >= 0.0 )		return NO_HIT;
		sy += b->R();				// Kugelradius berücksichtigen
	}
	else if (b->VY()==0.0)			return NO_HIT;

	min_t = sy / b->VY();

#ifdef DEBUG
	if (debug&YWallHit) {
		printf( "YWall-Hit in %g secs\n", (double)min_t );
		Info();
		b->Info();
	}
#endif

	if (min_t<-EPS)	return NO_HIT;
	else						return min_t;
}

void YWall::CollideWithBall( Ball *b )
{
	g->HitWall(b);
	b->SetV(Vec2( b->VX(), - b->VY() ));
}
