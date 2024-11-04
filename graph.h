#ifndef _graph_h
#define _graph_h

#ifndef _object_h
#	include "object.h"
#endif
#ifndef _vec2_h
#	include "vec2.h"
#endif

extern double	world_x, world_y;		// Fenstergröße in Weltkoordinaten
extern int 		max_x, max_y;			// Fenstergröße in Pixelkoordinaten
extern double	w2n;						// Transformationsfaktor
extern int		wall_click;

inline double MaxX()		{	return world_x; }
inline double MaxY()		{	return world_y; }

void InitGraphic( double x=100., double y=80. );
void UnlockGraphic( void );
void CloseGraphic( void );

void SetBgColor( ColorId col );

#define	COLOR_BITS		10
#define	COLOR_MASK		0x000000ffL
#define	BG_MASK			0x00000100L
#define	STAT_MASK		0x00000200L
#define	RAND_SHIFT		20
#define	RAND_MASK		0x00F00000L
#define	MIX_MASK			0x80000000L

extern int nbg_cols;
extern unsigned long bg_pix[];
extern int nball_cols;
extern unsigned long ball_pix[];
extern int nball_ids;
extern ColorId			ball_ids[];
extern int nstat_cols;
extern unsigned long stat_pix[];

unsigned long GetAllocatedPixel( const char *name );
unsigned long GetAllocatedPixel( ColorId id, int x=0, int y=0 );

ColorId	AddBgColor( const char *name );
ColorId	AddBallColor( const char *name );
ColorId	AddStatColor( const char *name );
int		AddShadeColor( const char *name, int mult, int divi );
ColorId	SetMainBgColor( const char *name );
void		SetCursorColor( const char *name );

ColorId	CreateColorMix( ColorId c1, ColorId c2, int fact=0 );
void		ResetColor( ColorId col, const char *name );

int StoreColor( unsigned );
void StoreColors( void );
void InitColors( void );
void AllocColors( void );
void MapMainWindow( void );

void DrawLine( const Real& x1, const Real& y1, const Real& x2, const Real& y2 );
void DrawArc(const Real& x, const Real& y, const Real& r, const Real& from, const Real& angle );
void DrawCircle( const Real& x, const Real& y, const Real& r );
void FillArc(const Real& x, const Real& y, const Real& r, const Real& from, const Real& angle );
void FillCircle( const Real& x, const Real& y, const Real& r );
void FillRectangle( const Real &x, const Real& y, const Real& width, const Real& height );

void FillPoly( int n, Vec2 v[] );
void FillPoly( int n, ... );

inline void DrawLine( const Vec2 &p1, const Vec2 &p2 ) {
	DrawLine( p1.X(), p1.Y(), p2.X(), p2.Y() );
}
inline void DrawArc( const Vec2 &p, const Real& r, const Real& from, const Real& angle ) {
	DrawArc( p.X(), p.Y(), r, from, angle );
}
inline void DrawCircle( const Vec2 &p, const Real& r ) {
	DrawCircle( p.X(), p.Y(), r );
}
inline void FillArc( const Vec2 &p, const Real& r, const Real& from, const Real& angle ) {
	FillArc( p.X(), p.Y(), r, from, angle );
}
inline void FillCircle( const Vec2 &p, const Real& r ) {
	FillCircle( p.X(), p.Y(), r );
}
inline void FillRectangle( const Vec2 &p, const Vec2 &s ) {
	FillRectangle( p.X(), p.Y(), s.X(), s.Y() );
}


void SetClick( int pitch, int percent, int duration );
inline void ClickWall( void );
inline void ClickBall( void );
inline void ClickPocket( void );


double GetCurrentTime( void );			// aktuelle Uhrzeit in Sekunden
char GetKey( void );

void Inside( Vec2 *v );						// auf Bereich testen
void WarpPointer( const Vec2 &dest );	// Maus verschieben


#ifdef STATISTICS
typedef enum {
	CycleRate,
	MoveRate,
	PointerInfo,
	OffsetInfo
} StatType;

void showinfo( StatType stat, const char *info );
void graphinfo( int value );
#endif


#ifndef __TURBOC__
#	ifndef _xgraph_h
#		include "xgraph.h"
#	endif
#else
#	ifndef _dosgraph_h
#		include "dosgraph.h"
#	endif
#endif


#endif
