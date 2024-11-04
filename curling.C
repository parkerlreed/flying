#ifndef _global_h
#	include "global.h"
#endif

#ifndef _curling_h
#	include "curling.h"
#endif
#ifndef _arcs_h
#	include "arcs.h"
#endif
#ifndef _ball_h
#	include "ball.h"
#endif
#ifndef _goal_h
#	include "goal.h"
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

//#undef CURLS
//#define CURLS 1

void Curling::InitTable() {
	mid = Vec2( AreaOffX()+AreaWidth()*7.0/8.0, AreaOffY()+AreaHeight()/2.0 );
	goal= new Goal(	Vec2( AreaOffX()+AreaWidth(), AreaOffY() ),
							Vec2( AreaOffX()+AreaWidth(), AreaOffY()+AreaHeight() ) );

	w[0] = new Wall( Edge(2), Edge(3) );
	w[1] = new Wall( Edge(3), Edge(0) );
	w[2] = new Wall( Edge(0), Edge(1) );
}


void Curling::InitArea( double wx, double wy ) {
	area_width  = wx;
	area_height = wy;
	area_off_x = (MaxX()-area_width)/2.;
	area_off_y = (MaxY()-KeeperHeight-area_height)/2.;
}


Curling::Curling(double wx, double wy) :
	Game(wx+2.*FrameOffset,wy+2.*FrameOffset + KeeperHeight)
{
	goal = 0;

	InitArea(wx,wy);

	table_col			= SetMainBgColor( "white" );
	red_bg_col			= AddBgColor( "red" );
	blue_bg_col			= AddBgColor( "blue" );

	table_dark_col		= AddBgColor( "gainsboro" );
	table_col			= CreateColorMix( 0, table_dark_col, 3 );
	floor					= AddStatColor( "black" );

	curl_col[0]			= AddBallColor( "Maroon" );
	curl_col[1]			= AddBallColor( "MediumAquamarine" );

	AddShadeColor( "White", 1, 1 );
	SetCursorColor( "brown" );

	keeper = new LineKeeper(KeeperHeight,KeeperFrame,blue_bg_col,table_col);

	mcurl     = 0;
	curl[0][0] = 0;
}

Curling::~Curling() {
int i,j;

	if (goal) {
		delete goal;
		for (j=0;j<3;j++)			delete w[j];
	}
	if (mcurl)		delete mcurl;

	if (curl[0][0]) {
		for (i=0;i<TEAMS;i++) {
			for (j=0;j<CURLS;j++)		delete curl[i][j];
		}
	}
}


const Real & Curling::GetPresetA() const {	return PresetA;}
const Real & Curling::GetPresetHaft() const {	return PresetHaft; }
const Real & Curling::GetSlowGranularity() const { return SlowGranularity; }

const Real & Curling::GetNormalBallSize() const  { return CurlRadius; }

const Real Curling::AreaOffX() const			{ return area_off_x; }
const Real Curling::AreaOffY() const			{ return area_off_y; }
const Real Curling::AreaWidth() const		{ return area_width; }
const Real Curling::AreaHeight() const		{ return area_height; }



void Curling::InitPlayground() {
	mcurl = new DiscMover( CurlRadius );
	mcurl->Init();
	DBG2(UnixTrace, "Curling: Radius set back from %g to %g\n",
					(double)CurlRadius, (double)mcurl->GetActRadius() );
	CurlRadius = mcurl->GetActRadius();


//	Game::InitPlayground();
	InitTable();

	for (int i=0;i<TEAMS;i++) {
		for (int j=0;j<CURLS;j++) {
			switch(i) {
			case 0:	curl_p[i][j]=Edge(0)+Vec2(CurlRadius+Offset,(2.0*CurlRadius+Offset)*(double)j+CurlRadius+Offset);
						break;
			case 1:	curl_p[i][j]=Edge(3)+Vec2(CurlRadius+Offset,-(2.0*CurlRadius+Offset)*(double)j-CurlRadius-Offset);
						break;
			};

			curl[i][j] = new Ball( curl_p[i][j], CurlRadius, CurlWeight );
			curl[i][j]->state
						 = new BallState( mcurl, curl_col[i], curl[i][j]->P() );
		}
	}
}


void Curling::DrawBackground() const {
	Game::DrawBackground();

	SetBgColor( table_col );
	FillRectangle( AreaOffX(), AreaOffY(), AreaWidth(), AreaHeight() );

	SetBgColor( blue_bg_col );			FillCircle( mid, 10.0 );
	SetBgColor( table_col );         FillCircle( mid, 9.5 );
	SetBgColor( blue_bg_col );
	for (int i=1;i<4;i++) {
		FillRectangle( AreaOffX()+(double)i*AreaWidth()/4.0-0.25,AreaOffX(),0.5,AreaHeight() );
	}
	SetBgColor( red_bg_col );        FillCircle( mid, 3.0 );

#if (0)
	for (int i=0;i<4;i++) {
		Vec2	cmid = (mid+Edge(i))/2.0;
		SetBgColor( blue_bg_col );			FillCircle( cmid, 5.0 );
		SetBgColor( table_col );         FillCircle( cmid, 4.5 );
	}
	SetBgColor( red_bg_col );
	for (i=0;i<4;i++) {
		FillCircle( (mid+Edge(i))/2.0, 2.0 );
	}

	SetBgColor( blue_bg_col );
	FillRectangle( AreaOffX()+AreaWidth()/4.0-0.25,AreaOffX(),0.5,AreaHeight() );
	FillRectangle( AreaOffX()+AreaWidth()*3.0/4.0-0.25,AreaOffX(),0.5,AreaHeight() );
#endif
}

void Curling::ResetGame() {
	for (int i=0;i<TEAMS;i++) {
		for (int j=0;j<CURLS;j++) {
			curl[i][j]->SetPV( curl_p[i][j] );
		}
	}
}

