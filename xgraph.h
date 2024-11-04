#ifndef _xgraph_h
#define _xgraph_h

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#ifndef _xsound_h
#	include "xsound.h"
#endif

extern Display	*dpy;
extern int		scr;
extern Window	win;
extern Window	new_root;
extern GC		gc_default;

extern GC		gc_col[];

extern GC		gc_lay1;
extern GC		gc_lay2;
extern GC		gc_cursor;

extern GC		gc_current;

extern GC		gc_bxor;
extern GC		gc_bclear;

extern GC		gc_ballwhite;		// für deluxe-Ball
extern GC		gc_ball[];

Window CreateWindow( const char *name, int width, int height );

#if (0)
#ifndef NO_DRAW
inline void FillCircle( double x, double y, double r ) {
	XFillArc( dpy, win, gc_current,
				(int)(x-r), (int)(y-r), (int)(2*r), (int)(2*r),
				0, (int)360*64 );
	XFillArc( dpy, win, gc_lay2,
				(int)((int)(x-r)+r*7/16), (int)((int)(y-r)+r*7/16),
				(int)(r*3/4), (int)(r*3/4),
				0, (int)360*64 );
}
#else
inline void FillCircle( GC *, double, double, double ) { }
#endif
#endif


inline void ClickWall() {
}
inline void ClickBall() {
	do_sound(40,80,1,0);
}
inline void ClickPocket() {
	do_sound(192,80,60,1 );
}

#endif
