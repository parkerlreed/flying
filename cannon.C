#ifndef _global_h
#	include "global.h"
#endif

#ifndef _cannon_h
#	include "cannon.h"
#endif
#ifndef _ball_h
#	include "ball.h"
#endif
#ifndef _wall_h
#	include "wall.h"
#endif
#ifndef _graph_h
#	include "graph.h"
#endif
#ifndef _mover_h
#	include "mover.h"
#endif

//
// Voreinstellungen
//

Cannon::Cannon(double wx, double wy) :
Billard(wx,wy)
{
	w1 = w2 = r = 0;

	InitArea( TableWidth, TableHeight );
	SelectTable(-1);
	red_col		= AddBallColor( "red4" );
	white_col	= AddBallColor( "bisque" );
	yellow_col	= AddBallColor( "DarkGoldenrod1" );
}

Cannon::~Cannon() {
	if (w1)		delete w1;
	if (w2)		delete w2;
	if (r)		delete r;
}

const Real & Cannon::GetNormalBallSize() const {
	return BallRadius;
}



void Cannon::InitPlayground() {

	Billard::InitPlayground();
	BallRadius = m->GetActRadius();
	Billard::InitTable();			// Wandbegrenzung

	w1_p = Vec2( AreaOffX()+AreaWidth()*0.25, AreaOffY()+AreaHeight()/2.0 );
	w1 = new Ball( w1_p, BallRadius );
	w1->state = new BallState( m, white_col, w1_p );

	w2_p = Vec2( AreaOffX()+AreaWidth()*0.25, AreaOffY()+AreaHeight()/4.0 );
	w2 = new Ball( w2_p, BallRadius );
	w2->state = new BallState( m, yellow_col, w2_p );
	r_p  = Vec2( AreaOffX()+AreaWidth()*0.75, AreaOffY()+AreaHeight()/2.0 );
	r  = new Ball( r_p, BallRadius );
	r->state = new BallState( m, red_col, r_p );
}

void Cannon::DrawBackground() const {
	Billard::DrawBackground();
	SetBgColor(table_line_col);
	FillCircle( AreaOffX()+AreaWidth()*0.25, AreaOffY()+AreaHeight()*0.25, 1.0 );
	FillCircle( AreaOffX()+AreaWidth()*0.25, AreaOffY()+AreaHeight()*0.50, 1.0 );
	FillCircle( AreaOffX()+AreaWidth()*0.25, AreaOffY()+AreaHeight()*0.75, 1.0 );
	FillCircle( AreaOffX()+AreaWidth()*0.50, AreaOffY()+AreaHeight()*0.50, 1.0 );
	FillCircle( AreaOffX()+AreaWidth()*0.75, AreaOffY()+AreaHeight()*0.50, 1.0 );
}

void Cannon::ResetGame() {
	w2->SetPV( w2_p );
	w1->SetPV( w1_p );
	r ->SetPV( r_p );

	Billard::ResetGame();
}
