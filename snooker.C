#ifndef _global_h
#	include "global.h"
#endif

#ifndef _snooker_h
#	include "snooker.h"
#endif
#ifndef _pocket_
#	include "pocket.h"
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


Snooker::Snooker(double wx, double wy) :
Pool(wx,wy)
{
	InitArea( TableWidth, TableHeight );

	SelectTable(-1);

#ifndef __TURBOC__
	red_col = AddBallColor( "red3" );
	cols[0] = AddBallColor( "yellow1" );
	cols[1] = AddBallColor( "green4" );
	cols[2] = AddBallColor( "brown4" );
	cols[3] = AddBallColor( "blue" );
	cols[4] = AddBallColor( "HotPink3" );
	cols[5] = AddBallColor( "black" );
#else
	red_col = AddBallColor( "red3" );
	floor   = CreateColorMix( table_col, table_line_col );

	cols[5] = AddBallColor( "black" );
	cols[0] = AddBallColor( "yellow1" );
	cols[1] = AddBallColor( "green4" );
	cols[2] = CreateColorMix( red_col, cols[5] );		// braun
	cols[3] = AddBallColor( "blue" );
	cols[4] = CreateColorMix( red_col, cue_col );	// Pink
#endif

	cueball=0;
	color_in_pocket = 0;
	reds_in_pocket  = 0;
}


Snooker::~Snooker() {
	if (cueball) {
		delete cueball;
		for (int c=0;c<6;c++)		delete colored[c];
		for (int r=0;r<15;r++)		delete red[r];
		cueball=0;
	}
}

const Real & Snooker::GetNormalBallSize() const {
	return BallRadius;
}


void Snooker::Triangle( double x, double y )
{
const int count = 5;
int		c=0;
double	cdist = sqrt( 3.*(GetNormalBallSize()+Offset)*(GetNormalBallSize()+Offset) );

//	x-=(cdist*(count-1)/2);
	for( int col=0; col<count; col++ ) {
		for( int row=0; row<=col; row++ ) {
			reddefs[c] = Vec2( x+col*cdist, y+(row-col/2.0)*2*(GetNormalBallSize()+Offset) ); 
			red[c]=new Ball( reddefs[c], GetNormalBallSize() );
			red[c]->state = new BallState( m, red_col, red[c]->P() );
			c++;
		}
	}
}

void Snooker::SetupBalls() {
	defs[0] = Vec2( AreaOffX()+AreaWidth()*0.25,  AreaOffY()+AreaHeight()/2.+14.6 );
	defs[1] = Vec2( AreaOffX()+AreaWidth()*0.25,  AreaOffY()+AreaHeight()/2.-14.6 );
	defs[2] = Vec2( AreaOffX()+AreaWidth()*0.25,  AreaOffY()+AreaHeight()*0.50 );
	defs[3] = Vec2( AreaOffX()+AreaWidth()*0.50,  AreaOffY()+AreaHeight()*0.50 );
	defs[4] = Vec2( AreaOffX()+AreaWidth()*0.75,  AreaOffY()+AreaHeight()*0.50 );
	defs[5] = Vec2( AreaOffX()+AreaWidth()-32.0, AreaOffY()+AreaHeight()*0.50 );

	for (int i=0;i<6;i++) {
		colored[i] = new Ball( defs[i], BallRadius );
		colored[i]->state = new BallState( m, cols[i], colored[i]->P() );
	}

	cuedef  = Vec2( AreaOffX()+AreaWidth()*0.125, AreaOffY()+AreaHeight()*0.375 );
	cueball = new Ball( cuedef, BallRadius );
	cueball->state = new BallState( m, cue_col, cueball->P() );

	tridef  = Vec2(	AreaOffX()+AreaWidth()*0.75+GetNormalBallSize()*2.5,
							AreaOffY()+AreaHeight()/2. );
	Triangle(tridef);
}

void Snooker::InitPlayground() {
	Billard::InitPlayground();
	BallRadius = m->GetActRadius();
	InitTable(6.0);
	SetupBalls();
}

void Snooker::DrawBackground() const {
	Pool::DrawBackground();
	SetBgColor(table_line_col);
	DrawLine( defs[0], defs[1] );
	DrawArc( defs[2], 14.6, 90.0, 180.0 );
	for (int i=0;i<6;i++) 	FillCircle( defs[i], 1.0 );
}

void Snooker::ResetGame() {
int i;
	cueball->SetPV( cuedef );
	for (i=0;i<6;i++)			colored[i]->SetPV( defs[i] );
	for (i=0;i<15;i++)		red[i]->SetPV( reddefs[i] );

	reds_in_pocket  = 0;
	color_in_pocket = 0;

	Billard::ResetGame();
}

// -------------------------------------------------------------------------

SnookerDemo::~SnookerDemo() {}

const Real &SnookerDemo::GetPresetA() const			{ return PresetA; }
const Real &SnookerDemo::GetSlowGranularity() const	{ return SlowGranularity; }

void SnookerDemo::InitPlayground() {
	Billard::InitPlayground();
	BallRadius = m->GetActRadius();
	Billard::InitTable();
	SetupBalls();
	cueball->v = Vec2( shot_speed, shot_speed );
}

void SnookerDemo::DrawBackground() const {
	Billard::DrawBackground();
	SetBgColor(table_line_col);
	DrawLine( defs[0], defs[1] );
	DrawArc( defs[2], 14.6, 90.0, 180.0 );
	for (int i=0;i<6;i++) 	FillCircle( defs[i], 1.0 );
}

////////////////////////////////////////////////////////////////////////////

void Snooker::InPocket( Ball *b ) {
int i;
	for (i=0;i<6;i++) {
		if (b==colored[i]) {
			color_in_pocket |= (1<<i);
			return;
		}
	}
	for (i=0;i<15;i++) {
		if (b==red[i]) {
			reds_in_pocket  |= (1<<i);
			return;
		}
	}
	Pool::InPocket(b);
}

void Snooker::AllBallsStopped() {
	if (color_in_pocket) {
		int	mask=0;
		for (int i=0;i<6;i++) {
			if ((reds_in_pocket!=0x7fff)||((color_in_pocket&mask)!=mask)) {
				if (color_in_pocket & (1<<i)) {
					(void)IsSelectable(colored[i]);
				}
			}
			mask = (mask<<1)|1;
		}
	}
	Pool::AllBallsStopped();
}

int Snooker::IsSelectable(Ball *b) {
int i;
	for (i=0;i<6;i++) {
		if (b==colored[i]) {
			if (color_in_pocket & (1<<i)) {
				if (b->FitsAt(defs[i])) {
					b->SetP(defs[i]);		// reset to default position
				}
				else if (b->FitsAt(defs[5])) {
					b->SetP(defs[5]);		// reset to black position
				}
				else if (i<4) {
					Vec2	newpos;
					b->FitsNextTo(defs[i],Vec2(-1.0,RealZero),&newpos);
					b->SetP(newpos);		// closer to left wall
				}
				else {
					Vec2	newpos;
					b->FitsNextTo(defs[i],Vec2(1.0,RealZero),&newpos);
					b->SetP(newpos);		// closer to right wall
				}

				b->ChgV(Vec2Zero);
				color_in_pocket &= ~(1<<i);		// it's back in the game

				return 0;
			}
			else {
				return 1;
			}
		}
	}
	for (i=0;i<15;i++) {
		if (b==red[i]) {
			if (reds_in_pocket & (1<<i)) {
				Vec2	newpos;
				b->FitsNextTo(tridef,Vec2(1.0,RealZero),&newpos);
				b->SetP(newpos);					// closer to right wall

				b->ChgV(Vec2Zero);
				reds_in_pocket &= ~(1<<i);		// it's back in the game

				return 0;
			}
			else {
				return 1;
			}
		}
	}

	return Pool::IsSelectable(b);
}
