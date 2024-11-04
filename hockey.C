#ifndef _global_h
#	include "global.h"
#endif

#ifndef _hockey_h
#	include "hockey.h"
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

void Hockey::InitTable() {
	mid = Vec2( AreaOffX()+AreaWidth()/2.0, AreaOffY()+AreaHeight()/2.0 );
	post[0][0] = new StaticBall( AreaOffX(),AreaOffY()+(AreaHeight()-GoalSize)/2.0 );
	post[0][1] = new StaticBall( AreaOffX(),AreaOffY()+(AreaHeight()+GoalSize)/2.0 );
//	goal[0]    = new Goal( post[0][0]->P(), post[0][1]->P() );
	goal[0] = new Goal(	Vec2( AreaOffX()-GetNormalBallSize(), AreaOffY() ),
								Vec2( AreaOffX()-GetNormalBallSize(), AreaOffY()+AreaHeight() ) );
	post[1][0] = new StaticBall( AreaOffX()+AreaWidth(),AreaOffY()+(AreaHeight()-GoalSize)/2.0 );
	post[1][1] = new StaticBall( AreaOffX()+AreaWidth(),AreaOffY()+(AreaHeight()+GoalSize)/2.0 );
//	goal[1]    = new Goal( post[1][0]->P(), post[1][1]->P() );

	goal[1] = new Goal(	Vec2( AreaOffX()+AreaWidth()+GetNormalBallSize(), AreaOffY() ),
								Vec2( AreaOffX()+AreaWidth()+GetNormalBallSize(), AreaOffY()+AreaHeight() ) );

	w[0][0] = new Wall( post[0][0]->P(), Edge(0) );
	w[0][1] = new Wall( Edge(0), Edge(1) );
	w[0][2] = new Wall( Edge(1), post[1][0]->P() );

	w[1][0] = new Wall( post[0][1]->P(), Edge(3) );
	w[1][1] = new Wall( Edge(3), Edge(2) );
	w[1][2] = new Wall( Edge(2), post[1][1]->P() );
}


void Hockey::InitArea( double wx, double wy ) {
	area_width  = wx;
	area_height = wy;
	area_off_x = (MaxX()-area_width)/2.;
	area_off_y = (MaxY()-GoalHeight-area_height)/2.;
}


Hockey::Hockey(double wx, double wy) :
	Game(wx+2.*FrameOffset,wy+2.*FrameOffset+GoalHeight)
{
	goal[0] = goal[1] = 0;

	InitArea(wx,wy);

	table_col			= SetMainBgColor( "white" );
	red_bg_col			= AddBgColor( "red" );
	blue_bg_col			= AddBgColor( "blue" );
	table_dark_col    = AddBgColor( "gainsboro" );

	disc_col				= AddBallColor( "black" );
	hand_col				= AddBallColor( "MidnightBlue" );

	table_col         = CreateColorMix( 0, table_dark_col, 3 );
	floor = AddStatColor( "black" );

	AddShadeColor( "White", 1, 1 );
	SetCursorColor( "brown" );

	keeper = new LineKeeper(GoalHeight,GoalFrame,blue_bg_col,table_col,CLOSE_LEFT|CLOSE_RIGHT);

	mdisc = 0;
	mhand = 0;
	disc = hand1 = hand2 = 0;
	disc_in_goal = 0;
}

Hockey::~Hockey() {
int i,j;

	if (goal[0]) {
		for (i=0;i<2;i++) {
			delete goal[i];
			for (j=0;j<2;j++)			delete post[i][j];
			for (j=0;j<3;j++)			delete w[i][j];
		}
	}
	if (mdisc)		delete mdisc;
	if (mhand)		delete mhand;

	if (disc)		delete disc;
	if (hand1)		delete hand1;
	if (hand2)		delete hand2;
}


const Real & Hockey::GetPresetA() const {	return PresetA;}
const Real & Hockey::GetPresetHaft() const {	return PresetHaft; }
const Real & Hockey::GetSlowGranularity() const { return SlowGranularity; }

const Real & Hockey::GetNormalBallSize() const  { return DiscRadius; }

const Real Hockey::AreaOffX() const			{ return area_off_x; }
const Real Hockey::AreaOffY() const			{ return area_off_y; }
const Real Hockey::AreaWidth() const		{ return area_width; }
const Real Hockey::AreaHeight() const		{ return area_height; }



void Hockey::InitPlayground() {
	mdisc = new DiscMover( DiscRadius );
	mdisc->Init();
	DBG2(UnixTrace, "Hockey: Radius set back from %g to %g\n",
					(double)DiscRadius, (double)mdisc->GetActRadius() );
	DiscRadius = mdisc->GetActRadius();

	mhand = new DiscMover( HandRadius );
	mhand->Init();
	DBG2(UnixTrace, "Hockey: Radius set back from %g to %g\n",
					(double)HandRadius, (double)mhand->GetActRadius() );
	HandRadius = mhand->GetActRadius();

	Game::InitPlayground();
	InitTable();

	disc = new Ball( mid, DiscRadius, DiscWeight );
	disc->state = new BallState( mdisc, disc_col, disc->P() );

	hand1 = new Ball( AreaOffX()+AreaWidth()/4., AreaOffY()+AreaHeight()/2.0,
								0.0, 0.0, HandRadius, HandWeight );
	hand1->state = new BallState( mhand, hand_col, hand1->P() );
	hand1->type  = Object::HandObj;

	hand2 = new Ball( AreaOffX()+AreaWidth()*3./4., AreaOffY()+AreaHeight()/2.0,
								0.0, 0.0, HandRadius, HandWeight );
	hand2->state = new BallState( mhand, hand_col, hand2->P() );
	hand2->type  = Object::HandObj;
}


void Hockey::DrawBackground() const {
int i;
	Game::DrawBackground();

	SetBgColor( table_col );
	FillRectangle( AreaOffX(), AreaOffY(), AreaWidth(), AreaHeight() );
	FillRectangle( post[0][0]->PX()-GetNormalBallSize(), post[0][0]->PY(),
							AreaWidth()+2.*GetNormalBallSize(), GoalSize );

	SetBgColor( blue_bg_col );			FillCircle( mid, 10.0 );
	SetBgColor( table_col );         FillCircle( mid, 9.5 );
	SetBgColor( blue_bg_col );
	FillRectangle( AreaOffX()+AreaWidth()/2.0-0.25,AreaOffX(),0.5,AreaHeight() );
	SetBgColor( red_bg_col );        FillCircle( mid, 3.0 );

	for (i=0;i<4;i++) {
		Vec2	cmid = (mid+Edge(i))/2.0;
		SetBgColor( blue_bg_col );			FillCircle( cmid, 5.0 );
		SetBgColor( table_col );         FillCircle( cmid, 4.5 );
	}
	SetBgColor( blue_bg_col );
	FillRectangle( AreaOffX()+AreaWidth()/4.0-0.25,AreaOffX(),0.5,AreaHeight() );
	FillRectangle( AreaOffX()+AreaWidth()*3.0/4.0-0.25,AreaOffX(),0.5,AreaHeight() );
	SetBgColor( red_bg_col );
	for (i=0;i<4;i++) {
		FillCircle( (mid+Edge(i))/2.0, 2.0 );
	}
}


void Hockey::InPocket( Ball *b ) {
	Game::InPocket(b);
	if (b==disc) {
		disc_in_goal = 1;
	}
}

void Hockey::StopBall( Ball *b ) {
	Game::StopBall(b);
	if (b==disc && disc_in_goal) {
		disc_in_goal = 0;

Real	to;	// Endpunkt für aktuelle Kugel in der Pocketline
Real	from;	// Startpunkt für aktuelle Kugel in der Pocketline
Real	vy;	// Startgeschwindigkeit, damit Kugel in Mitte (to) liegenbleibt

		to		= AreaOffY() + AreaHeight() / 2.0;
		from	= AreaOffY() + AreaHeight() - g->GetNormalBallSize()/2.0;
		vy		= sqrt( 2.0*fabs((to-from)*b->a) );

		b->SetP( Vec2( AreaOffX() + AreaWidth()/2.0, from ) );
		b->SetV( Vec2(RealZero,-vy) );
	}
}

//

TestHockey::TestHockey(double s) :
	Hockey(), shot_speed(s)
{
	w[0] = 0;
}

TestHockey::~TestHockey() {
	if (w[0])	for(int i=0;i<4;i++)		delete w[i];
}

const Real &TestHockey::GetPresetA() const				{ return PresetA; }
const Real &TestHockey::GetSlowGranularity() const	{ return SlowGranularity; }

#if (0)
void TestHockey::InitTable() {
	w[0] = new Wall( AreaOffX(),					AreaOffY(),
										AreaOffX()+AreaWidth(), AreaOffY() );
	w[1] = new Wall( AreaOffX()+AreaWidth(),	AreaOffY(),
										AreaOffX()+AreaWidth(), AreaOffY()+AreaHeight() );
	w[2] = new Wall( AreaOffX()+AreaWidth(),	AreaOffY()+AreaHeight(),
										AreaOffX(),					AreaOffY()+AreaHeight() );
	w[3] = new Wall( AreaOffX(),					AreaOffY()+AreaHeight(),
										AreaOffX(),					AreaOffY() );
}
#endif

void TestHockey::InitPlayground() {
	mdisc = new DiscMover( DiscRadius );
	mdisc->Init();
	DBG2(UnixTrace, "Hockey: Radius set back from %g to %g\n",
					(double)DiscRadius, (double)mdisc->GetActRadius() );
	DiscRadius = mdisc->GetActRadius();

	InitTable();

	disc = new Ball( Mid(3), DiscRadius, DiscWeight );
	disc->state = new BallState( mdisc, disc_col, disc->P() );
	disc->v = Vec2(shot_speed,RealZero);
}

