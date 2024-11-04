#ifndef _global_h
#	include "global.h"
#endif

#ifndef _mover_h
#	include "mover.h"
#endif
#ifndef _vec2_h
#	include "vec2.h"
#endif
#ifndef _graph_h
#	include "graph.h"
#endif

/*----------------------------------------------------------------------------*/

#ifndef __TURBOC__
#	include "xmover.C"
#else
#	include "dosmover.C"
#endif

/*----------------------------------------------------------------------------*/

BallStateTop::BallStateTop()										{}
BallStateTop::~BallStateTop()										{}
void BallStateTop::Redraw()										{}
void BallStateTop::Show()											{}
void BallStateTop::MoveTo( const Vec2& /*new_pos*/ )	{}
void BallStateTop::Hide()											{}

/*----------------------------------------------------------------------------*/


BallState::BallState( BallMover *m_in, ColorId col_in, const Vec2 &pos ) {
	vis = 0;
	m   = m_in;
	if (!m) {
		printf( "BallState: Error: BallMover not initialized\n" );
	}
	col = col_in;
	for (col_x=0;col_x<nball_ids;col_x++) {
		if (col==ball_ids[col_x])					break;
	}
	x = (int)(pos.X()*w2n);
	y = (int)(pos.Y()*w2n);
	DBG3( BState, "BallState(%ld,<%d,%d>)", col_in, x,y );
}

BallState::~BallState() {
}


void BallState::Redraw() {
	vis=1;
	m->DrawBallAt(x,y,col_x);
}

void BallState::Show() {
	if (!vis) {
		vis=1;
		m->DrawBallAt(x,y,col_x);
	}
}


void BallState::MoveTo( const Vec2 &pos ) {
int	newx;
int	newy;

	newx = (int)(pos.X()*w2n);
	newy = (int)(pos.Y()*w2n);
	if (newx!=x||newy!=y) {
		m->MoveBallOnScreen(x,y,newx,newy,col_x);
		x = newx;
		y = newy;
	}
}


void BallState::Hide() {
	if (vis) {
		vis=0;
		m->DrawBallAt(x,y,col_x);
	}
}

/*----------------------------------------------------------------------------*/


HalfBallState::HalfBallState( BallMover *m_in, ColorId col_in, const Vec2 &pos ) :
	BallState( m_in, col_in, pos )
{
	mh = (HalfBallMover *)m_in;
#ifndef __TURBOC__
	st=mh->AngVec2St(rand()%mh->vecs_l,rand()%mh->vecs_b);
#endif
}

HalfBallState::~HalfBallState() {
}

void HalfBallState::Redraw() {
	vis=1;
	mh->RollBallAt(x,y,st,col_x);
}

void HalfBallState::Show() {
	if (!vis) {
		vis=1;
		mh->RollBallAt(x,y,st,col_x);
	}
}


void HalfBallState::MoveTo( const Vec2 &pos ) {
int			newx;
int			newy;
RingState	nst;

	newx = (int)(pos.X()*w2n);
	newy = (int)(pos.Y()*w2n);
	if (newx!=x||newy!=y) {
		mh->RollBallOnScreen(x,y,st,newx,newy,&nst,col_x);
		x	= newx;
		y	= newy;
		st = nst;
	}
}


void HalfBallState::Hide() {
	if (vis) {
		vis=0;
		mh->RollBallAt(x,y,st,col_x);
	}
}
