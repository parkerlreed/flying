#ifndef _global_h
#	include "global.h"
#endif

#ifndef _billard_h
#	include "billard.h"
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

void Billard::InitArea( double wx, double wy ) {
	area_width	= wx;
	area_height	= wy;
	if (area_width/2.0>area_height)		area_height = area_width/2.0;
	else										area_width  = area_height*2.0;
	area_off_x = (MaxX()-area_width)/2.0;
	area_off_y = (MaxY()-area_height)/2.0;
}

int Billard::SelectTable( int col ) {
	if (col<0) {
			if (color_flag>0)		col = color_flag - 1;
			else						col=((int)(GetCurrentTime()*10))%3;
	}
	if (col>2)	col=1;

	switch(col) {
	default:
		SetMainBgColor( "rgb:60/90/70" );				// SeaGreen-GlobalBackground
		ResetColor( table_line_col, "rgb:00/50/00" );	// DarkGreen
		ResetColor( table_light_col, "rgb:80/B0/90" );	// LimeGreen
	//	ResetColor( table_dark_col, "MediumForestGreen" );
		SetCursorColor( "PaleTurquoise" );
		break;
	case 2:
		SetMainBgColor( "rgb:80/A0/C0" );		// LightSteelBlue-GlobalBackground
		ResetColor( table_line_col, "rgb:40/50/80" );		// SteelBlue
		ResetColor( table_light_col, "rgb:A0/C0/E0" );	// LightSteelBlue3
	//	ResetColor( table_dark_col, "LightSteelBlue4" );
		SetCursorColor( "Maroon" );
		break;
	case 0:
		SetMainBgColor( "rgb:f0/80/60" );		// coral2-GlobalBackground
		ResetColor( table_line_col, "rgb:78/38/28" );	// coral4
		ResetColor( table_light_col, "rgb:ff/A0/80" );	// LightSalmon2
	//	ResetColor( table_dark_col, "coral3" );
		SetCursorColor( "Navy" );
		break;
	}
	return col;
}

void Billard::InitTable() {
	w[0] = new Wall( AreaOffX(),					AreaOffY(),
										AreaOffX()+AreaWidth(), AreaOffY() );
	w[1] = new Wall( AreaOffX()+AreaWidth(),	AreaOffY(),
										AreaOffX()+AreaWidth(), AreaOffY()+AreaHeight() );
	w[2] = new Wall( AreaOffX()+AreaWidth(),	AreaOffY()+AreaHeight(),
										AreaOffX(),					AreaOffY()+AreaHeight() );
	w[3] = new Wall( AreaOffX(),					AreaOffY()+AreaHeight(),
										AreaOffX(),					AreaOffY() );
}

Billard::Billard(double wx, double wy) :
	Game(wx+2.0*FrameOffset,wy+2.0*FrameOffset)
{
	for (int i=0;i<4;i++)	w[i]=0;

	InitArea(wx- 2.0*FrameOffset,wy- 2.0*FrameOffset);

	table_col			= SetMainBgColor( "SeaGreen" );	// GlobalBackground

	outer_cushion_col	= AddStatColor( "black" );
	marker_col			= AddStatColor( "brown" );

	table_line_col		= AddBgColor( "DarkGreen" );
	table_light_col	= AddBgColor( "LimeGreen" );
//	table_dark_col		= AddBgColor( "MediumForestGreen" );

	table_col         = CreateColorMix( 0, table_light_col, 3 );

	inner_cushion_col = CreateColorMix( table_line_col, table_light_col, 15 );

//	floor = AddStatColor( "LightGoldenrod" );
	floor = CreateColorMix( table_line_col, 0 );

	AddShadeColor( "White", 1, 1 );

	m = 0;
}

Billard::~Billard() {
	for (int i=0;i<4;i++)		if (w[i])		delete w[i];

	if (m) {
		delete m;
		m=0;
	}
}

const Real & Billard::GetPresetA() const {
	return PresetA;
}

const Real & Billard::GetPresetHaft() const {
	return PresetHaft;
}

const Real & Billard::GetSlowGranularity() const {
	return SlowGranularity;
}

const Real Billard::AreaOffX() const		{ return area_off_x; }
const Real Billard::AreaOffY() const		{ return area_off_y; }
const Real Billard::AreaWidth() const		{ return area_width; }
const Real Billard::AreaHeight() const		{ return area_height; }

const Real Billard::PAreaOffX() const		{ return area_off_x-InnerCushion; }
const Real Billard::PAreaOffY() const		{ return area_off_y-InnerCushion; }
const Real Billard::PAreaWidth() const		{ return area_width+2.0*InnerCushion; }
const Real Billard::PAreaHeight() const	{ return area_height+2.0*InnerCushion; }

const Real Billard::TAreaOffX() const
							{ return area_off_x-InnerCushion-OuterCushion; }
const Real Billard::TAreaOffY() const
							{ return area_off_y-InnerCushion-OuterCushion; }
const Real Billard::TAreaWidth() const
							{ return area_width+2.0*(InnerCushion+OuterCushion); }
const Real Billard::TAreaHeight() const
							{ return area_height+2.0*(InnerCushion+OuterCushion); }

void Billard::InitPlayground() {
	if (!m) {
		m = new ShadedBallMover( GetNormalBallSize() );
		m->Init();
	}
	Game::InitPlayground();
#ifdef DEBUG
	if (debug&ShowLight) {
		m->CreateLightWindow();
	}
#endif
}

void Billard::DrawMarker( const Real &midx, const Real &midy, const Real &ox, const Real &oy ) const {
Vec2	v1( midx   , midy+oy );
Vec2	v2( midx-ox, midy    );
Vec2	v3( midx   , midy-oy );
Vec2	v4( midx+ox, midy    );

	FillPoly( 4, &v1, &v2, &v3, &v4 );
}

void Billard::DrawBackground() const {
double i;

	Game::DrawBackground();

	SetBgColor( outer_cushion_col );
	FillRectangle( TAreaOffX(), TAreaOffY(), TAreaWidth(), TAreaHeight() );

	SetBgColor(inner_cushion_col);
	FillRectangle( PAreaOffX(), PAreaOffY(), PAreaWidth(), PAreaHeight() );

	SetBgColor( table_col );
	FillRectangle( AreaOffX(), AreaOffY(), AreaWidth(), AreaHeight() );

#if (0)
	{
	int	px1 = w2n*(AreaOffX());
	int	py1 = w2n*(AreaOffY());
	int	px2 = w2n*(AreaOffX()+AreaWidth());
	int	py2 = w2n*(AreaOffY()+AreaHeight());
	int	px  = px2-px1;
	int	py  = py2-py1;
	int	pts = px*py;

		SetBgColor(table_light_col);
		for (int i=pts/8;i>0;i--) {
			XDrawPoint(dpy,win,gc_current,px1+rand()%px,py1+rand()%py);
		}
	}
#endif

double	outer = PAreaOffY()-TAreaOffY();
	SetBgColor( marker_col );
	for (i=1;i<8;i++) {
		DrawMarker( AreaOffX()+AreaWidth()*i/8.0, TAreaOffY()+outer/2.0,
																			outer/8.0, outer/4.0 );
		DrawMarker( AreaOffX()+AreaWidth()*i/8.0, TAreaOffY()+TAreaHeight()-outer/2,
																			outer/8.0, outer/4.0 );
	}
	for (i=1;i<4;i++) {
		DrawMarker( TAreaOffX()+outer/2.0, AreaOffY()+AreaHeight()*i/4.0,
																		outer/4.0, outer/8.0 );
		DrawMarker( TAreaOffX()+TAreaWidth()-outer/2.0, AreaOffY()+AreaHeight()*i/4.0,
																		outer/4.0, outer/8.0 );
	}
}
