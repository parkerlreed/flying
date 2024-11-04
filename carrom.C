#ifndef _global_h
#	include "global.h"
#endif

#ifndef _carrom_h
#	include "carrom.h"
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
#ifndef _arcs_h
#	include "arcs.h"
#endif
#ifndef _pocket_h
#	include "pocket.h"
#endif



void Carrom::InitArea( double wx, double wy ) {
	area_width	= wx;
	area_height	= wy;
	area_off_x = (MaxX()-ToPocket-PocketWidth-area_width)/2.0;
	area_off_y = (MaxY()-area_height)/2.0;
}


Carrom::Carrom(double wx, double wy) :
	Game(wx+2.0*FrameOffset+PocketWidth+ToPocket,wy+2.0*FrameOffset)
{
	InitArea(wx,wy);

	table_col	= SetMainBgColor( "Tan" );		// GlobalBackground

	table_black	= AddBgColor( "black" );
	table_red	= AddBgColor( "Brown" );
	table_green = AddBgColor( "DarkGreen" );
	pocket_col	= CreateColorMix( table_col, table_green );

#ifndef __TURBOC__
	white_col	= AddBallColor( "DarkGoldenrod" );
	black_col	= AddBallColor( "black" );
	queen_col	= AddBallColor( "red" );
	striker_col	= AddBallColor( "DeepSkyBlue" );
#else
	white_col	= AddBallColor( "DarkGoldenrod" );
	floor			= CreateColorMix( table_green, table_black );

	black_col	= AddBallColor( "black" );
	queen_col	= AddBallColor( "red" );
	striker_col	= CreateColorMix( white_col, black_col );
#endif

	AddShadeColor( "grey", 1, 2 );
	SetCursorColor( "White" );
	striker = 0;
	striker_in_pocket = 0;
	queen_in_pocket   = 0;
	whites_in_pocket  = 0;
	blacks_in_pocket  = 0;

	keeper = new StackKeeper(PocketWidth,PocketFrame,table_black);
}

Carrom::~Carrom() {
	if (striker) {
		delete queen;
		delete striker;
		for (int i=0;i<9;i++) {
			delete white[i];
			delete black[i];
		}
		striker=0;
	}

	for (int i=0;i<4;i++) {
		if (e[i][0])		delete e[i][0];
		if (e[i][1])		delete e[i][1];
		if (p[i])			delete p[i];
		if (w[i])			delete w[i];
	}

	if (mstone)		delete mstone;
	if (mstriker)	delete mstriker;
}

const Real & Carrom::GetPresetA() const			{ return PresetA; }
const Real & Carrom::GetPresetHaft() const		{ return PresetHaft; }
const Real & Carrom::GetSlowGranularity() const	{ return SlowGranularity; }

const Real & Carrom::GetNormalBallSize() const	{ return StoneRadius; }

const Real & Carrom::GetChargeGranularity() const	{ return ChargeGranularity; }
const Real & Carrom::GetChargeSpeed() const			{ return ChargeSpeed; }
const Real & Carrom::GetMaxCharge() const			{ return MaxCharge; }
const Real & Carrom::GetShootTime() const			{ return ShootTime; }

const Real Carrom::AreaOffX() const			{ return area_off_x; }
const Real Carrom::AreaOffY() const			{ return area_off_y; }
const Real Carrom::AreaWidth() const		{ return area_width; }
const Real Carrom::AreaHeight() const		{ return area_height; }

const Real Carrom::TAreaOffX() const		{ return area_off_x-Cushion; }
const Real Carrom::TAreaOffY() const		{ return area_off_y-Cushion; }
const Real Carrom::TAreaWidth() const		{ return area_width+2.0*Cushion; }
const Real Carrom::TAreaHeight() const		{ return area_height+2.0*Cushion; }

void Carrom::Setup( double x, double y )
{
static int	nc[5] = { 3, 4, 5, 4, 3 };		// Steine pro Spalte
static int t[19] = { -1,  1, -2,					// Steinverteilung
					 2, -3,  3,  4,
				 -4,  5,  0, -5, -6,
					 6, -7,  7,  8,
					  -8,  9, -9 };

const int count = 5;
int		c=0;
double	cdist = sqrt( 3.*(GetNormalBallSize()+Offset)*(GetNormalBallSize()+Offset) );
Ball	*r;

	x-=(cdist*(count-1)/2.0);
	for( int col=0; col<count; col++ ) {
		for( int row=0; row<nc[col]; row++ ) {
			r=new Ball( x+col*cdist, y+(row-nc[col]/2.0+0.5)*2.0*(GetNormalBallSize()+Offset), 0.0, 0.0, GetNormalBallSize(), StoneWeight );
			if (t[c]>0) {
				r->state = new BallState( mstone, white_col, r->P() );
				white[t[c]-1] = r;
			}
			else if (t[c]<0) {
				r->state = new BallState( mstone, black_col, r->P() );
				black[-t[c]-1] = r;
			}
			else {
				r->state = new BallState( mstone, queen_col, r->P() );
				queen = r;
			}
			c++;
		}
	}
}




void Carrom::InitPlayground() {
int i;

	mstone = new DiscMover( GetNormalBallSize() );
	mstone->Init();
	DBG2(UnixTrace, "Carrom: Stone-Radius set back from %g to %g\n",
					(double)StoneRadius, (double)mstone->GetActRadius() );
	StoneRadius = mstone->GetActRadius();

	mstriker = new BallMover( StrikerRadius );
	mstriker->Init();
	DBG2(UnixTrace, "Carrom: Striker-Radius set back from %g to %g\n",
					(double)StrikerRadius, (double)mstriker->GetActRadius() );
	StrikerRadius = mstriker->GetActRadius();

	Game::InitPlayground();
#if (0)
#ifdef DEBUG
	if (debug&ShowLight) {
		mstone->CreateLightWindow();
	}
#endif
#endif

	p[0] = new Pocket( AreaOffX(), AreaOffY(), PocketRadius );
	p[1] = new Pocket( AreaOffX()+AreaWidth(), AreaOffY(), PocketRadius );
	p[2] = new Pocket( AreaOffX()+AreaWidth(), AreaOffY()+AreaHeight(), PocketRadius );
	p[3] = new Pocket( AreaOffX(), AreaOffY()+AreaHeight(),	PocketRadius );

	for (i=0;i<4;i++) {
		e[i][0] = new StaticBall( p[i]->P()+PocketRadius*Vec2(1.0,RealZero).TurnAngleDeg(-90.*i),	0.0 );
		e[i][1] = new StaticBall( p[i]->P()+PocketRadius*Vec2(1.0,RealZero).TurnAngleDeg(-90.*i-90.),0.0 );
	}
	for (i=0;i<4;i++) {
		w[i] = new Wall( e[i][0]->P(), e[(i+1)%4][1]->P() );
	}

	mid = Vec2( AreaOffX()+AreaWidth()/2.0, AreaOffY()+AreaHeight()/2.0 );
	Vec2 d(1.0,0.0);
	for (i=0;i<4;i++) {
		base[i][0] = mid+25.*d.TurnAngleDeg(-90.*i)+22.*d.TurnAngleDeg(90.*(i+1));
		base[i][1] = mid+25.*d.TurnAngleDeg(-90.*i)+22.*d.TurnAngleDeg(90.*(i-1));
	}
	Setup(AreaOffX()+AreaWidth()/2.0, AreaOffY()+AreaHeight()/2.0);

	striker = new Ball( (base[1][0]+base[1][1])/2.0, StrikerRadius, StrikerWeight );
	striker->state = new BallState( mstriker, striker_col, striker->P() );
}




void Carrom::DrawBackground() const {
int i;
int j;

const Real	w8  =  45.0;
const Real	w16 =  w8/2.0;
const Real	w32 = w16/2.;
const Real	w64 = w32/2.;

Vec2	d(1.0,0.0);
Vec2	p1,p2,p3,p4;

	Game::DrawBackground();

// normales Spielfeld
	SetBgColor( table_black );
	FillRectangle( TAreaOffX(), TAreaOffY(), TAreaWidth(), TAreaHeight() );
	SetBgColor(0);
	FillRectangle( AreaOffX(), AreaOffY(), AreaWidth(), AreaHeight() );

//
// Randelemente und Taschen aufbauen (4er-Schleife)
//
	for (i=0;i<4;i++) {
		SetBgColor(pocket_col);
		FillCircle( p[i]->P(), p[i]->R() );
	Real	ang = w8*Real(2*i+1);
		SetBgColor(table_black);
		DrawLine( mid+16.0*d.TurnAngleDeg(ang), mid+42.0*d.TurnAngleDeg(ang) );
		DrawArc(  mid+19.5*d.TurnAngleDeg(ang), 3.5, ang+30.0, 300. );

		SetBgColor( table_black );
		p1 = base[i][0] + 1.8*d.TurnAngleDeg(90.*i);
		p2 = base[i][0] + 1.3*d.TurnAngleDeg(90.*i);
		p3 = base[i][1] + 1.3*d.TurnAngleDeg(90.*i);
		p4 = base[i][1] + 1.8*d.TurnAngleDeg(90.*i);
		FillPoly( 4, &p1, &p2, &p3, &p4 );
		p1 = base[i][0] - 1.8*d.TurnAngleDeg(90.*i);
		p2 = base[i][0] - 1.6*d.TurnAngleDeg(90.*i);
		p3 = base[i][1] - 1.6*d.TurnAngleDeg(90.*i);
		p4 = base[i][1] - 1.8*d.TurnAngleDeg(90.*i);
		FillPoly( 4, &p1, &p2, &p3, &p4 );

		for (j=0;j<2;j++) {
			SetBgColor( table_black );			FillCircle( base[i][j], 3.6/2 );
			SetBgColor( table_col );			FillCircle( base[i][j], 3.3/2 );
			SetBgColor( table_red );			FillCircle( base[i][j], 3.5/2 );
		}
	}

//
// Kreise in der Mitte ziehen
//
	SetBgColor( table_black );  	FillCircle( mid, 21.5 / 2 );
	SetBgColor( table_red );  		FillCircle( mid, 21.2 / 2 );
	SetBgColor( table_black );  	FillCircle( mid, 20.8 / 2 );
	SetBgColor( table_col );	  	FillCircle( mid, 20.5 / 2 );

	SetBgColor( table_black );  	FillCircle( mid, 16.1 / 2 );
	SetBgColor( table_col );  		FillCircle( mid, 15.9 / 2 );

	SetBgColor( table_black );  	FillCircle( mid, 15.6 / 2 );
	SetBgColor( table_red );  		FillCircle( mid, 15.5 / 2 );
	SetBgColor( table_black );  	FillCircle( mid, 15.1 / 2 );
	SetBgColor( table_col );	  	FillCircle( mid, 15.0 / 2 );

	SetBgColor( table_black );  	FillCircle( mid, 14.60 / 2 );
	SetBgColor( table_col );  		FillCircle( mid, 14.35 / 2 );

//
// mittleren Kreis in 8 Segmenten aufbauen
//
	for (i=0;i<8;i++) {
		d = Vec2(1.0,RealZero).TurnAngleDeg( w8 * (double)i );

		SetBgColor(table_black);
		for (double r=0.;r<4.;r++) {
			p1 = mid+8.0*d.TurnAngleDeg(r*w32);
			p2 = mid+8.7*d.TurnAngleDeg(r*w32+5.0);
			p3 = mid+8.7*d.TurnAngleDeg(r*w32-5.0);
			FillPoly(3, &p1, &p2, &p3 );
			Real	ang=w8*Real(i)+w32*r+w64;
			DrawArc(mid+9.*Vec2(1.0,RealZero).TurnAngleDeg(ang),0.95,ang-90.,180.);
		}

		SetBgColor(table_red);
		FillCircle( mid+3.0*d.TurnAngleDeg(w16), 0.7 );
		for (j=1;j<8;j+=4)	FillCircle(mid+9.0*d.TurnAngleDeg(w64*j),0.6);
		SetBgColor(table_green);
		for (j=3;j<8;j+=4)	FillCircle(mid+9.0*d.TurnAngleDeg(w64*j),0.6);

			p1=mid+3.0*d;
			p2=mid+4.0*d.TurnAngleDeg( 10.0);
			p3=mid+8.5*d;
			p4=mid+4.0*d.TurnAngleDeg(-10.0);
		FillPoly( 4, &p1, &p2, &p3, &p4 );
			p2 = mid+1.5*d.TurnAngleDeg( 5.0*w16);
			p3 = mid+1.5*d.TurnAngleDeg(-5.0*w16);
		FillPoly( 3, &p1, &p2, &p3 );

		Vec2	s[10];		// Stern
			s[0] = mid+4.7*d.TurnAngleDeg(w16);
			s[1] = mid+4.3*d.TurnAngleDeg(w16+12.);
			s[2] = mid+5.2*d.TurnAngleDeg(w16+9.);
			s[3] = mid+6.2*d.TurnAngleDeg(w16+13.);
			s[4] = mid+6.2*d.TurnAngleDeg(w16+5.);

			s[5] = mid+7.2*d.TurnAngleDeg(w16);
			s[6] = mid+6.2*d.TurnAngleDeg(w16-5.);
			s[7] = mid+6.2*d.TurnAngleDeg(w16-13.);
			s[8] = mid+5.2*d.TurnAngleDeg(w16-9.);
			s[9] = mid+4.3*d.TurnAngleDeg(w16-12.);
		FillPoly( 10, s );
	}

	SetBgColor(table_red);
	FillCircle(mid,1.2);
}

//////////////////////////////////////////////////////////////////////////////

void Carrom::InPocket( Ball *b ) {
	Game::InPocket(b);
	if (b==striker)		{	striker_in_pocket = 1;	return;	}
	if (b==queen)			{	queen_in_pocket   = 1;	return;	}
	for (int i=0;i<9;i++) {
		if (b==white[i])	{	whites_in_pocket |= (1<<i);	return;	}
		if (b==black[i])	{	blacks_in_pocket |= (1<<i);	return;	}
	}
}

void Carrom::AllBallsStopped() {
	Game::AllBallsStopped();
	(void)IsSelectable(striker);		// back on the table
}

void Carrom::ResetToCenter(Ball *b) {
Vec2	center(AreaOffX()+AreaWidth()/2.0, AreaOffY()+AreaHeight()/2.0);

	Vec2	newpos;
	b->FitsNextTo(center,Vec2(1.0,RealZero),&newpos);
	b->SetP(newpos);					// closer to right wall

	b->ChgV(Vec2Zero);
}

int Carrom::IsSelectable(Ball *b) {
	if (b==striker) {
		if (striker_in_pocket) {
			Vec2	inspos(AreaOffX()+AreaWidth()-2.0*StrikerRadius-EPS,
														AreaOffY()+AreaHeight()/2.0);
			Vec2	newpos;

			b->FitsNextTo(inspos,Vec2(-1.0,RealZero),&newpos);
			striker_in_pocket = 0;
			b->SetP(newpos);
			b->ChgV(Vec2Zero);

			return 0;
		}
		else {
			return 1;
		}
	}
	for (int i=0;i<9;i++) {
		if ( b==white[i] ) {
			if (whites_in_pocket&(1<<i)) {
					ResetToCenter(b);
					whites_in_pocket &= ~(1U<<i);
					return 0;
			}
			else	return 1;
		}
		if ( b==black[i] ) {
			if (blacks_in_pocket&(1<<i)) {
					ResetToCenter(b);
					blacks_in_pocket &= ~(1U<<i);
					return 0;
			}
			else	return 1;
		}
	}
	if ( b==queen ) {
		if (queen_in_pocket) {
				ResetToCenter(b);
				queen_in_pocket = 0;
				return 0;
		}
		else	return 1;
	}

	return Game::IsSelectable(b);
}

// -------------------------------------------------------------------------

CarromDemo::~CarromDemo() {}

const Real &CarromDemo::GetPresetA() const			{ return PresetA; }
const Real &CarromDemo::GetSlowGranularity() const	{ return SlowGranularity; }

void CarromDemo::InitPlayground() {
	Carrom::InitPlayground();
	for (int i=0;i<4;i++) {
		delete e[i][0];	e[i][0]=0L;
		delete e[i][1];	e[i][1]=0L;
		Vec2	pos( p[i]->P() );
		delete p[i];
		p[i] = new StaticBall( pos, PocketRadius );
	}

	striker->v = Vec2(1.0,-shot_speed);
}

void CarromDemo::DrawBackground() const {
	Carrom::DrawBackground();
	SetBgColor(table_black);
	for (int i=0;i<4;i++) {
		FillCircle( p[i]->P(), p[i]->R() );
	}
}

