#ifndef _global_h
#	include "global.h"
#endif

#ifndef _pool_h
#	include "pool.h"
#endif
#ifndef _pcon_h
#	include "pcon.h"
#endif
#ifndef _pocket_h
#	include "pocket.h"
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
#ifndef _keeper_h
#	include "keeper.h"
#endif


//
// Voreinstellungen
//


const int  Pool::nPockets		= 6;


const Real & Pool::GetNormalBallSize() const {
	return BallRadius;
}

void Pool::InitArea( double wx, double wy ) {
	area_width  = wx;
	area_height = wy;
	if (area_width/2.>area_height)   area_height = area_width/2.;
	else                             area_width  = area_height*2.;
	area_off_x = (MaxX()-area_width)/2.;
	area_off_y = (MaxY()-PocketHeight-area_height)/2.;
}

int Pool::SelectTable( int col ) {
	col = Billard::SelectTable(col);
	pocket_col  = CreateColorMix( 0, table_line_col );
	return col;
}

Pool::Pool(double wx, double wy) :
Billard(wx,wy+PocketHeight)
{
	InitArea(wx,wy);
	pcon = 0;
	for (int i=0;i<nPockets;i++) 		p[i] = 0;

// SetMainBgColor( "SeaGreen" );		// GlobalBackground
	cue_col		= AddBallColor( "ivory" );
	pocket_col  = CreateColorMix( 0, table_line_col );

	keeper = new LineKeeper(PocketHeight,PocketFrame,inner_cushion_col,table_col);

	cue_in_pocket = 0;
}

Pool::~Pool() {
	if (pcon)		delete [] pcon;
	for (int i=0;i<6;i++)		if (p[i])	delete p[i];
}


void Pool::InitTable( double rad ) {
	po = PocketRadius/sqrt(2);
//	po=0;

	p[0] = new Pocket( PAreaOffX()+po,					 PAreaOffY()+po, PocketRadius );
	p[1] = new Pocket( PAreaOffX()+PAreaWidth()/2.,  PAreaOffY()+po, PocketRadius );
	p[2] = new Pocket( PAreaOffX()+PAreaWidth()-po, PAreaOffY()+po, PocketRadius );
	p[3] = new Pocket( PAreaOffX()+PAreaWidth()-po, PAreaOffY()+PAreaHeight()-po,
																				PocketRadius );
	p[4] = new Pocket( PAreaOffX()+PAreaWidth()/2.,	PAreaOffY()+PAreaHeight()-po,
																				PocketRadius );
	p[5] = new Pocket( PAreaOffX()+po,					PAreaOffY()+PAreaHeight()-po,
																				PocketRadius );

	pcon = new PocketConnector[nPockets];
	pcon[0].Init( *p[0], EdgeAngle, *p[1],  MidAngle, InnerCushion-po, rad );
	pcon[1].Init( *p[1],  MidAngle, *p[2], EdgeAngle, InnerCushion-po, rad );
	pcon[2].Init( *p[2], EdgeAngle, *p[3], EdgeAngle, InnerCushion-po, rad );
	pcon[3].Init( *p[3], EdgeAngle, *p[4],  MidAngle, InnerCushion-po, rad );
	pcon[4].Init( *p[4],  MidAngle, *p[5], EdgeAngle, InnerCushion-po, rad );
	pcon[5].Init( *p[5], EdgeAngle, *p[0], EdgeAngle, InnerCushion-po, rad );
}


void Pool::InitPlayground() {
	Billard::InitPlayground();
	BallRadius = m->GetActRadius();

	InitTable();
}

void Pool::DrawBackground() const {
	Billard::DrawBackground();

// Undraw middle marker
	SetBgColor( outer_cushion_col );
	double	outer = PAreaOffY()-TAreaOffY();
	int		i=4;
	DrawMarker( AreaOffX()+AreaWidth()*Real(i)/8., TAreaOffY()+outer/2.,
																		outer/8., outer/4. );
	DrawMarker( AreaOffX()+AreaWidth()*Real(i)/8., TAreaOffY()+TAreaHeight()-outer/2.,
																		outer/8., outer/4. );

// Draw Pockets
	for (i=0;i<nPockets;i++) {
		SetBgColor( table_col );
		FillPoly(4, &pcon[i].w1->p1, &pcon[i].w2->p1,
						&pcon[(i+nPockets-1)%nPockets].w2->p2,
						&pcon[(i+nPockets-1)%nPockets].w3->p2 );
		SetBgColor(pocket_col);
		FillCircle( p[i]->P(), p[i]->R() );
	}
	for (i=0;i<nPockets;i++) {
		SetBgColor(inner_cushion_col);
		FillArc( pcon[i].b1->p, pcon[i].b1->r, pcon[i].b1->start, pcon[i].b1->angle );
		FillArc( pcon[i].b2->p, pcon[i].b2->r, pcon[i].b2->start, pcon[i].b2->angle );
		FillPoly(6,	&pcon[i].w1->p1, &pcon[i].w1->p2,
						&pcon[i].w2->p1, &pcon[i].w2->p2,
						&pcon[i].w3->p1, &pcon[i].w3->p2	);
	}
}


//////////////////////////////////////////////////////////////////////////////

void Pool::ResetGame() {
	cueball->SetPV( cuedef );
	cue_in_pocket = 0;
	Billard::ResetGame();
}

void Pool::InPocket( Ball *b ) {
	Billard::InPocket(b);
	if (b==cueball)	cue_in_pocket = 1;
}

void Pool::AllBallsStopped() {
	Billard::AllBallsStopped();
	(void)IsSelectable(cueball);		// back on the table
}

int Pool::IsSelectable(Ball *b) {
	if (b==cueball) {
		if (cue_in_pocket) {
			Vec2	newpos;

			b->FitsNextTo(cuedef,Vec2(-1.0,RealZero),&newpos);
			cue_in_pocket = 0;
			b->SetP(newpos);
			b->ChgV(Vec2Zero);

			return 0;
		}
		else {
			return 1;
		}
	}

	return Billard::IsSelectable(b);
}
