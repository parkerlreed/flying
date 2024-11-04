#ifndef _global_h
#	include "global.h"
#endif

#ifndef _object_h
#	include "object.h"
#endif
#ifndef _graph_h
#	include "graph.h"
#endif
#ifndef _pball_h
#	include "pball.h"
#endif

double	world_x, world_y;
int		max_x, max_y;
double	w2n;
int		wall_click = 0;



void Inside( Vec2 *v ) {
double x=v->X();
double y=v->Y();

	if (x<0)				x=0;
	if (x>max_x)		x=max_x;
	if (y<0)				y=0;
	if (y>max_y)		y=max_y;
	*v = Vec2(x,y);
}

static const int MAX_BG		= 7;
static const int MAX_BALL	= 15;
static const int MAX_SHADE = 3;
static const int MAX_STAT	= 128;

		 int				nbg_cols=0;
static const char 	*bg_col[MAX_BG];
		 unsigned long	bg_pix[MAX_BG];			// aus Planes zusammengesetzt
static unsigned long bg_mask;						// alle bg-Planes

		 int				nball_cols=1;
static const char 	*ball_col[MAX_BALL] = { "Background-Dummy" };
		 unsigned long	ball_pix[MAX_BALL];		// allokierte Pixel
static unsigned long ball_mask;

		 int				nball_ids =0;
		 ColorId			ball_ids[MAX_BALL];		// für Bälle verteilte Farb-Ids

       int           nstat_cols =0;
static const char		*stat_col[MAX_STAT];
       unsigned long stat_pix[MAX_STAT];		// Static-Colors (unter Cursor)

static int				nshade_cols=0;
static const char 	*shade_col[MAX_SHADE];
static int m[MAX_SHADE], d[MAX_SHADE];			// Anteil Original : Schatten
static unsigned long	shade_pix[MAX_SHADE];	// aus Planes zusammengesetzt
static unsigned long shade_mask;    			// alle Shade-Planes

static const char 	*cursor_col=0;
static unsigned long	cursor_mask;				// Cursor-Plane

void InitColors() {
	nbg_cols   = 0;
	nball_cols = 1;
	nstat_cols = 0;
	nball_ids  = 0;
	nshade_cols= 0;
}

unsigned long GetAllocatedPixel( const char *colorname ) {
int i;
	for (i=0;i<nbg_cols;i++)
		if	(!strcmp(bg_col[i],colorname))		return bg_pix[i]|ball_pix[0];
	for (i=0;i<nball_cols;i++)
		if	(!strcmp(ball_col[i],colorname))		return ball_pix[i];
	for (i=0;i<nstat_cols;i++)
		if	(!strcmp(stat_col[i],colorname))		return stat_pix[i];
	return 0;
}

unsigned long GetAllocatedPixel( ColorId col, int x, int y ) {
	if (col&MIX_MASK) {
		if ( (x^y)&0x01 ) {
			col = (col>>COLOR_BITS);
		}
	}
	if (col&BG_MASK)			return bg_pix[ (int)(col&COLOR_MASK) ];
	else							return ball_pix[ (int)(col&COLOR_MASK) ];
}

ColorId AddBgColor( const char *colorname ) {
	for (int i=0;i<nbg_cols;i++)
		if	(!strcmp(bg_col[i],colorname)) {
			DBG2( ShowColors, "   BG%d: '%s' (reused)\n", i, colorname );
			return i|BG_MASK;
		}

	if (nbg_cols<MAX_BG) {
		DBG2( ShowColors, "   BG%d: '%s'\n", nbg_cols, colorname );
		bg_col[nbg_cols] = colorname;
		return (nbg_cols++)|BG_MASK;
	}
	else {
		DBG2( ShowColors, "   BG%d: '%s' *** TOO MANY COLORS\n",
																	nbg_cols, colorname );
		return -1;
	}
}
ColorId AddBallColor( const char *colorname ) {
	for (int i=0;i<nball_cols;i++)
		if	(!strcmp(ball_col[i],colorname)) {
			DBG2( ShowColors, " Ball%d: '%s' (reused)\n", i, colorname );
			return i;
		}

	if (nball_cols<MAX_BALL) {
		DBG2( ShowColors, " Ball%d: '%s'\n", nball_cols, colorname );
		ball_col[nball_cols]		= colorname;
		ball_ids[nball_ids++]	= nball_cols;
		return nball_cols++;
	}
	else {
		DBG2( ShowColors, " Ball%d: '%s' *** TOO MANY COLORS\n",
																	nball_cols, colorname );
		return -1;
	}
}
ColorId AddStatColor( const char *colorname ) {
	for (int i=0;i<nstat_cols;i++)
		if	(!strcmp(stat_col[i],colorname)) {
			DBG2( ShowColors, " Stat%d: '%s' (reused)\n", i, colorname );
			return i|STAT_MASK;
		}

	if (nstat_cols<MAX_STAT) {
		DBG2( ShowColors, " Stat%d: '%s'\n", nstat_cols, colorname );
		stat_col[nstat_cols]		= colorname;
		return (nstat_cols++)|STAT_MASK;
	}
	else {
		DBG2( ShowColors, " Stat%d: '%s' *** TOO MANY COLORS\n",
																	nstat_cols, colorname );
		return -1;
	}
}
int AddShadeColor( const char *colorname, int mult, int divi ) {
	for (int i=0;i<nshade_cols;i++)
		if	(!strcmp(shade_col[i],colorname))		return i;

	if (nshade_cols<MAX_SHADE) {
		DBG2( ShowColors, "Shade%d: '%s'\n", nshade_cols, colorname );
		shade_col[nshade_cols] = colorname;
		m[nshade_cols] = mult;
		d[nshade_cols] = divi;
		return nshade_cols++;
	}
	else {
		DBG2( ShowColors, "Shade%d: '%s' *** TOO MANY COLORS\n",
																	nball_cols, colorname );
		return -1;
	}
}
void SetCursorColor( const char *colorname ) {
	cursor_col = colorname;
}
ColorId SetMainBgColor( const char *colorname ) {
	ball_col[0] = colorname;
	DBG1( ShowColors, "MainBg: '%s' (Ball0)\n", colorname );
	return 0;
}

ColorId CreateColorMix( ColorId c1, ColorId c2, int fact ) {
	ColorId	mix = MIX_MASK | (c2<<COLOR_BITS) | c1
						| ((fact<<RAND_SHIFT)&RAND_MASK);

	if (!(c1&BG_MASK)&&!(c2&BG_MASK)) {
		ball_ids[nball_ids++]=mix;
		DBG4( ShowColors, "     %d: Mix %lx + %lx (%d:1)\n",
												nball_ids, c1, c2, fact );
	}
	else {
		DBG3( ShowColors, "         Mix %lx + %lx (%d:1)\n",
												c1, c2, fact );
	}
	return mix;
}


void ResetColor( ColorId id, const char *colorname ) {
	if (id&MIX_MASK) {
		printf( "error: mix-colors unchangeable\n" );
	}
	else {
		if (id&BG_MASK) {
			DBG2( ShowColors, "   BG%ld: '%s' (Reset)\n", id-BG_MASK, colorname );
			bg_col[(int)(id&COLOR_MASK)]=colorname;
		}
		else if (id&STAT_MASK) {
			DBG2( ShowColors, " Stat%ld: '%s' (Reset)\n", id, colorname );
			stat_col[(int)(id&COLOR_MASK)] = colorname;
		}
		else {
			DBG2( ShowColors, " Ball%ld: '%s' (Reset)\n", id, colorname );
			ball_col[(int)id] = colorname;
		}
	}
}

#ifndef __TURBOC__
#	include "xgraph.C"
#else
#	include "dosgraph.C"
#endif

