#ifndef _global_h
#	include "global.h"
#endif

#ifndef _arcs_h
#	include "arcs.h"
#endif
#ifndef _ball_h
#	include "ball.h"
#endif
#ifndef _game_h
#	include "game.h"
#endif

//
// ============================================================================
// class StaticArc
// ============================================================================
//

StaticArc::StaticArc( double x, double y, double r_in, double start_in, double angle_in )
: p( x, y )
{
	type			= StaticArcObj;
	this->r     = r_in;
	this->start = start_in;
	this->angle = angle_in;
}
StaticArc::StaticArc( const Vec2 &v, const Real &r_in,
										const Real &start_in, const Real &angle_in )
: p( v ), r(r_in), start(start_in), angle(angle_in)
{
	type			= StaticArcObj;
}
StaticArc::~StaticArc() { }

int StaticArc::HitFromBallTwice( int outer_hit, Ball *b, const Vec2 &dv,
											Real md, Real *time_out )
{
	Vec2	d  = P() - b->P();				// Abstandsvektor

#if (ABORT_CALC_BALL)
//
// Abstand mit maximal Geschwindigkeitsbetrag vergleichen
//
		if ( (d.Norm()-md)/sqrt(sq)>b->collision_time-current_time+EPS)
																						return 2;
#endif

	Real	m  = (d.X()*dv.X()+d.Y()*dv.Y());
			if (outer_hit&&(m<-EPS))		return 1;

	Real	sq = dv.SqrNorm();
			if (sq<EPS)							return 1;

			m /= sq;
	Real	f  = (md*md-d.SqrNorm())/sq + (m*m);
			if (f<=0.0)							return 1;
			f = sqrt(f);

	Real	f1 =  f+m;
	Real	f2 = -f+m;

			if (f1<f2) {	*time_out = (outer_hit)?f1:f2;	}
			else {			*time_out = (outer_hit)?f2:f1;	}

			return 0;
}

int StaticArc::IsOnArc( const Vec2 &d )
{
Real	a = P().AngleDeg(d);

		a-=start;
		if ( angle>=0.0 ) {
			if (a<0.0)		a+=360.0;
			return ( (a>=0.0)&&(a<=angle) )?1:0;
		}
		else {
			if (a>0.0)		a-=360.0;
			return ( (a<=0.0)&&(a>=angle) )?1:0;
		}
}

void StaticArc::CollideWithBall( Ball *b ) {
Vec2	x, y;
Vec2	dist=P()-b->P();

	g->HitWall(b);
	b->V().Split( dist, &x, &y );			// Ball b wird direkt zurückgestoßen
	b->SetV( y-x );
}

void StaticArc::Info() {
	printf( "%02d: StaticArc: %08lx: P()=(%4.1f,%4.1f), R()=%3.1f, %f-%f\n",
				id, (unsigned long)this,
				(double)PX(), (double)PY(), (double)R(),
				(double)start, (double)angle );
}

Real StaticArc::OuterHitFromBall( Ball *b, const Vec2 &dv ) {
Real	otime;

	switch( HitFromBallTwice( 1, b, dv, R()+b->R(), &otime ) ) {
	case 1:		return NO_HIT;
#if (ABORT_CALC_BALL)
	case 2:
				DBG2(AbortCalc,"    OuterHit from %d to %d aborted.\n", b->Object::id, id );
				return NOT_REACHABLE;
#endif
	default:
		if (otime>-EPS)	return otime;
		else					return NO_HIT;
	}
}

Real StaticArc::InnerHitFromBall( Ball *b, const Vec2 &dv ) {
Real	itime;

	if (b->R()>r)		return NO_HIT;	// Ball zu groß für inneren Treffer

	switch (HitFromBallTwice( 0, b, dv, R()-b->R(), &itime )) {
	case 1:		return NO_HIT;
#if (ABORT_CALC_BALL)
	case 2:
				DBG2(AbortCalc,"    InnerHit from %d to %d aborted.\n", b->Object::id, id );
				return NOT_REACHABLE;
#endif
	default:
		if (itime>-EPS)	return itime;
		else					return NO_HIT;
	}
}

//
// class OuterArc
//
OuterArc::~OuterArc()		{}

void OuterArc::Info() {
	printf( "%02d: OuterArc:   %08lx: P()=(%4.1f,%4.1f), R()=%3.1f, %f-%f\n",
				id, (unsigned long)this,
				(double)PX(), (double)PY(), (double)R(),
				(double)start, (double)angle );
}

Real OuterArc::HitFromBall( Ball *b ) {
	if (b->IsIdle())		return MAX_TIME;
	return OuterHitFromBall(b,b->V());
}

//
// class InnerArc
//
InnerArc::~InnerArc()	{}

void InnerArc::Info() {
	printf( "%02d: InnerArc:   %08lx: P()=(%4.1f,%4.1f), R()=%3.1f, %f-%f\n",
				id, (unsigned long)this,
				(double)PX(), (double)PY(), (double)R(),
				(double)start, (double)angle );
}

Real InnerArc::HitFromBall( Ball *b ) {
	if (b->IsIdle())		return MAX_TIME;
	return InnerHitFromBall(b,b->V());
}

//
// class ArcWall
//
ArcWall::~ArcWall()	{}

void ArcWall::Info() {
	printf( "%02d: ArcWall:    %08lx: P()=(%4.1f,%4.1f), R()=%3.1f, %f-%f\n",
				id, (unsigned long)this,
				(double)PX(), (double)PY(), (double)R(),
				(double)start, (double)angle );
}

Real ArcWall::HitFromBall( Ball *b ) {
Real	itime, otime;

	if (b->IsIdle())		return MAX_TIME;

	itime = InnerHitFromBall(b,b->V());
	if ( (itime<NO_HIT)&&(!IsOnArc(b->P()+itime*b->V())))	itime = NO_HIT;
	otime = OuterHitFromBall(b,b->V());
	if ( (otime<NO_HIT)&&(!IsOnArc(b->P()+otime*b->V())))	otime = NO_HIT;
	return( (itime<otime)?itime:otime );
}

//
// ============================================================================
// class StaticBall, unbewegliche Kugel (unendlicher Masse)
// ============================================================================
//
void StaticBall::Info() {
	printf( "%02d: StaticBall: %08lx: P()=(%4.1f,%4.1f), R()=%3.1f\n",
				id, (unsigned long)this,
				(double)PX(), (double)PY(), (double)R() );
}

StaticBall::~StaticBall()	{}
