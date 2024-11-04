
#include <X11/Xlib.h>
#include <X11/Xos.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

class XMoveChecker {
	public:
		XMoveChecker( unsigned bsize, unsigned avgmove );
		virtual ~XMoveChecker();
		virtual	void	RollBall() = 0;
		double loop( unsigned long secs );

	protected:
		Display	*dpy;
		int		scr;
		Window	win;

		unsigned bsize;			// Ball-Size
		unsigned avgmove;			// durchschnittssprung
		unsigned dhelp;
};

// ==========================================================================

XMoveChecker::XMoveChecker( unsigned bsize_in, unsigned avg )
: bsize(bsize_in), avgmove(avg)
{
XSetWindowAttributes setwinattr;

	dpy   = XOpenDisplay( "" );
	scr   = DefaultScreen( dpy );

	if (avgmove>bsize-1)		avgmove=bsize-1;
	dhelp = 2*bsize-1;

	win   = XCreateSimpleWindow( dpy, RootWindow( dpy, scr ), 0, 0, dhelp, dhelp,
					0, BlackPixel( dpy,scr ), WhitePixel( dpy,scr ) );
	setwinattr.override_redirect = True;
	XChangeWindowAttributes( dpy, win, CWOverrideRedirect, &setwinattr );

	// map window and wait for it to be mapped.
XEvent	event;
	XSelectInput( dpy, win, ExposureMask );
	XMapRaised( dpy, win );
	XNextEvent( dpy, &event );
}

XMoveChecker::~XMoveChecker() {
	XDestroyWindow( dpy, win );
	XCloseDisplay( dpy );
}

double XMoveChecker::loop( unsigned long secs ) {
struct timeval start;
struct timeval current;
struct timeval	diff;
long	count=-1;	// Vorlauf

	if (secs<=5) {
		XGrabServer(dpy);
		XSync(dpy,0);
	}

	do {
		for (int i=0;i<16;i++)		RollBall();
		XSync( dpy, 0 );
		if (count++<0)	gettimeofday( &start, NULL );
		gettimeofday( &current, NULL );
		diff.tv_sec = current.tv_sec - start.tv_sec;
		if ( current.tv_usec >= start.tv_usec ) {
			diff.tv_usec = current.tv_usec - start.tv_usec;
		}
		else {
			diff.tv_usec = 1000000L + current.tv_usec - start.tv_usec;
			diff.tv_sec--;
		}
	} while( diff.tv_sec < secs );

	if (secs<=5) {
		XUngrabServer(dpy);
	}

	return ( (double)count / ((double)diff.tv_sec + diff.tv_usec/1000000.0)*(double)secs );
}

// ==========================================================================


class XMoveChecker1 : public XMoveChecker {
	public:
		XMoveChecker1( unsigned bsize, unsigned avg );
		virtual ~XMoveChecker1();
		virtual void RollBall();

	protected:
		Pixmap	help;				// Doublebuffer:		Depth 8  (2*bsize-1)
		Pixmap	bpix;				// Ball-Bitmap:		Depth 1     bsize
		Pixmap	spix;				// Shadow-Bitmap:		Depth 1     bsize
		Pixmap	rpix;				// Ring-Bitmap:		Depth 1     bsize
		GC			gc_bclear;		// reset help-pixmap
		GC			gc_ballwhite;	// create white ball
		GC			gc_lay2;			// add shades to ball
		GC			gc_ball;			// add colored rings
		GC			gc_bxor;			// copy help pixmap to screen
		GC			gc_bit;			// initialize bitmaps
};

XMoveChecker1::XMoveChecker1( unsigned bsize_in, unsigned avg )
: XMoveChecker( bsize_in, avg )
{
	help  = XCreatePixmap(dpy,win,dhelp,dhelp,DefaultDepth(dpy,scr));

	bpix  = XCreatePixmap(dpy,win,bsize,bsize,1);
	spix  = XCreatePixmap(dpy,win,bsize,bsize,1);
	rpix  = XCreatePixmap(dpy,win,bsize,bsize,1);

	gc_bit = XCreateGC( dpy, bpix, 0, 0 );
	XFillRectangle( dpy, bpix, gc_bit, 0, 0, bsize, bsize );
	XFillRectangle( dpy, spix, gc_bit, 0, 0, bsize, bsize );
	XFillRectangle( dpy, rpix, gc_bit, 0, 0, bsize, bsize );

//
// ball_mask:  0x03
// shade_mask: 0x04
//
unsigned long	value_mask;
XGCValues		values;

	value_mask =	GCPlaneMask | GCForeground | GCBackground | GCFunction
					|	GCGraphicsExposures;

	values.graphics_exposures = False;
	values.fill_style	= FillSolid;
	values.background = 0;

	values.foreground = 0x00;
	values.plane_mask = 0x07;		// Arbeiten in lay2
	values.function   = GXclear;
	gc_bclear    = XCreateGC( dpy, win, value_mask, &values );
	values.function   = GXxor;
	gc_bxor      = XCreateGC( dpy, win, value_mask, &values );

	values.function	= GXxor;
	values.foreground = 0x04;
	values.plane_mask = 0x04;
	gc_lay2      = XCreateGC( dpy, win, value_mask, &values );

	values.function	= GXxor;
	values.foreground = 0x01;
	values.plane_mask = 0x03;
	gc_ballwhite      = XCreateGC( dpy, win, value_mask, &values );
	values.foreground = 0x03;
	gc_ball           = XCreateGC( dpy, win, value_mask, &values );
}

XMoveChecker1::~XMoveChecker1() {
	XFreeGC( dpy, gc_ball );
	XFreeGC( dpy, gc_ballwhite );
	XFreeGC( dpy, gc_lay2 );
	XFreeGC( dpy, gc_bxor );
	XFreeGC( dpy, gc_bclear );
	XFreePixmap( dpy, help );
	XFreePixmap( dpy, bpix );
	XFreePixmap( dpy, spix );
	XFreePixmap( dpy, rpix );
}

// XMoveChecker1: simulates the original algorithm that consists of
//	- clearing the double buffer pixmap of depth 8
//	- copying old ball, constisting of 3 layers of depth 1
//	- copying new ball, consisting of 3 layers of depth 1
//	- copying the double buffer to the screen
// - 8 X-calls

void XMoveChecker1::RollBall() {

int	d  = bsize;
int	dx = avgmove;
int	dy = avgmove;

int absx = (dx>0)?dx:-dx;
int absy = (dy>0)?dy:-dy;

	int	width = d+absx;		// tats蘡hlicher Ausschnittgr无e der Pixmap
	int	height= d+absy;
	int	ox = (dx>0)?0:absx;	// relativer Abstand alte Position
	int	oy = (dy>0)?0:absy;
	int	nx = (dx>0)?absx:0;	// relativer Abstand neue Position
	int	ny = (dy>0)?absy:0;

	XFillRectangle(dpy,help,gc_bclear,0,0,width,height);

	XCopyPlane(dpy,bpix,help,gc_ballwhite,0,0,d,d,ox,oy,1);
	XCopyPlane(dpy,spix,help,gc_lay2,0,0,d,d,ox,oy,1);
	XCopyPlane(dpy,rpix,help,gc_ball,0,0,d,d,ox,oy,1);

	XCopyPlane(dpy,bpix,help,gc_ballwhite,0,0,d,d,nx,ny,1);
	XCopyPlane(dpy,spix,help,gc_lay2,0,0,d,d,nx,ny,1);
	XCopyPlane(dpy,rpix,help,gc_ball,0,0,d,d,nx,ny,1);

	XCopyArea(dpy,help,win,gc_bxor,0,0,width,height,0,0 );
}

// ==========================================================================

class XMoveChecker2 : public XMoveChecker {
	public:
		XMoveChecker2( unsigned bsize, unsigned avg );
		virtual ~XMoveChecker2();
		virtual void RollBall();

	protected:
		Pixmap	help;				// Doublebuffer:			Depth 8  (2*bsize-1)
		Pixmap	bpix;				// Ball/Shade-Bitmap:	Depth 8     bsize
		Pixmap	rpix;				// Ring-Bitmap:			Depth 1     bsize
		GC			gc_bcopy;		// preset help-pixmap
		GC			gc_ball;			// add colored rings
		GC			gc_bxor;			// copy help pixmap to screen
		GC			gc_bit;			// initialize bitmaps
};

XMoveChecker2::XMoveChecker2( unsigned bsize_in, unsigned avg )
: XMoveChecker( bsize_in, avg )
{
	help  = XCreatePixmap(dpy,win,dhelp,dhelp,DefaultDepth(dpy,scr));
	bpix  = XCreatePixmap(dpy,win,dhelp,dhelp,DefaultDepth(dpy,scr));
	XFillRectangle( dpy, bpix, DefaultGC(dpy,scr), 0, 0, dhelp, dhelp );

	rpix  = XCreatePixmap(dpy,win,bsize,bsize,1);

	gc_bit = XCreateGC( dpy, rpix, 0, 0 );
	XFillRectangle( dpy, rpix, gc_bit, 0, 0, bsize, bsize );

//
// ball_mask:  0x03
// shade_mask: 0x04
//
unsigned long	value_mask;
XGCValues		values;

	value_mask =	GCPlaneMask | GCForeground | GCBackground | GCFunction
					|	GCGraphicsExposures;

	values.graphics_exposures = False;
	values.fill_style	= FillSolid;
	values.background = 0;

	values.foreground = 0x00;
	values.plane_mask = 0x07;
	values.function   = GXcopy;
	gc_bcopy     = XCreateGC( dpy, win, value_mask, &values );
	values.function   = GXxor;
	gc_bxor      = XCreateGC( dpy, win, value_mask, &values );

	values.function	= GXxor;
	values.plane_mask = 0x03;
	values.foreground = 0x03;
	gc_ball           = XCreateGC( dpy, win, value_mask, &values );
}

XMoveChecker2::~XMoveChecker2() {
	XFreeGC( dpy, gc_ball );
	XFreeGC( dpy, gc_bxor );
	XFreeGC( dpy, gc_bcopy );
	XFreePixmap( dpy, help );
	XFreePixmap( dpy, bpix );
	XFreePixmap( dpy, rpix );
}


// XMoveChecker2: simulates the enhanced algorithm that consists of
//	- initialisation the double buffer with an oversized 8 bit shadowed image of the old ball
//   (a lot more memory is need in this approach than in the next similare one)
//	- copying the 8 bit shadow image of the new ball
//	- copying 2 bitmaps with the rings of the ball with 1 bit each
//	- copying the double buffer to the screen
//	- 5 X-calls

void XMoveChecker2::RollBall() {

int	d  = bsize;
int	dx = avgmove;
int	dy = avgmove;

int absx = (dx>0)?dx:-dx;
int absy = (dy>0)?dy:-dy;

	int	width = d+absx;		// tats蘡hlicher Ausschnittgr无e der Pixmap
	int	height= d+absy;
	int	ox = (dx>0)?0:absx;	// relativer Abstand alte Position
	int	oy = (dy>0)?0:absy;
	int	nx = (dx>0)?absx:0;	// relativer Abstand neue Position
	int	ny = (dy>0)?absy:0;

	XCopyArea(dpy,bpix,help,gc_bcopy,0,0,width,height,0,0);
	XCopyArea(dpy,bpix,help,gc_bxor,0,0,d,d,nx,ny);

	XCopyPlane(dpy,rpix,help,gc_ball,0,0,d,d,ox,oy,1);
	XCopyPlane(dpy,rpix,help,gc_ball,0,0,d,d,nx,ny,1);

	XCopyArea(dpy,help,win,gc_bxor,0,0,width,height,0,0 );
}

// ==========================================================================

class XMoveChecker3 : public XMoveChecker2 {
	public:
		XMoveChecker3( unsigned bsize, unsigned avg );
		virtual ~XMoveChecker3();
		virtual void RollBall();
};

XMoveChecker3::XMoveChecker3( unsigned bsize_in, unsigned avg )
: XMoveChecker2( bsize_in, avg ) {
}

XMoveChecker3::~XMoveChecker3() {
}

// XMoveChecker3: simulates the enhanced algorithm that consists of
//	- clearing the double buffer pixmap of depth 8
//	- copying the 8 bit shadow image of the balls with 8 bits each
//	- copying 2 bitmaps with the rings of the ball with 1 bit each
//	- copying the double buffer to the screen
//	- 6 X-calls

void XMoveChecker3::RollBall() {

int	d  = bsize;
int	dx = avgmove;
int	dy = avgmove;

int absx = (dx>0)?dx:-dx;
int absy = (dy>0)?dy:-dy;

	int	width = d+absx;		// tats蘡hlicher Ausschnittgr无e der Pixmap
	int	height= d+absy;
	int	ox = (dx>0)?0:absx;	// relativer Abstand alte Position
	int	oy = (dy>0)?0:absy;
	int	nx = (dx>0)?absx:0;	// relativer Abstand neue Position
	int	ny = (dy>0)?absy:0;

	XFillRectangle(dpy,help,gc_bcopy,0,0,width,height);

	XCopyArea(dpy,bpix,help,gc_bxor,0,0,d,d,ox,oy);
	XCopyArea(dpy,bpix,help,gc_bxor,0,0,d,d,nx,ny);

	XCopyPlane(dpy,rpix,help,gc_ball,0,0,d,d,ox,oy,1);
	XCopyPlane(dpy,rpix,help,gc_ball,0,0,d,d,nx,ny,1);

	XCopyArea(dpy,help,win,gc_bxor,0,0,width,height,0,0 );
}

// ==========================================================================

class XMoveChecker4 : public XMoveChecker2 {
	public:
		XMoveChecker4( unsigned bsize, unsigned avg );
		virtual ~XMoveChecker4();
		virtual void RollBall();
};

XMoveChecker4::XMoveChecker4( unsigned bsize_in, unsigned avg )
: XMoveChecker2( bsize_in, avg ) {
}

XMoveChecker4::~XMoveChecker4() {
}

// XMoveChecker4: simulates the enhanced algorithm that consists of
//	- initialisation of double-buffer parts, not covered from old ball
//	- initialisaion of the double-buffer part containing old ball (8 bit)
//	- copying the 8 bit shadow image of the new ball (8 bit)
//	- copying 2 bitmaps with the rings of the ball with 1 bit each
//	- copying the double buffer to the screen
//	- 6 X-calls

void XMoveChecker4::RollBall() {

int	d  = bsize;
int	dx = avgmove;
int	dy = avgmove;

int absx = (dx>0)?dx:-dx;
int absy = (dy>0)?dy:-dy;

	int	width = d+absx;		// tats蘡hlicher Ausschnittgr无e der Pixmap
	int	height= d+absy;
	int	ox = (dx>0)?0:absx;	// relativer Abstand alte Position
	int	oy = (dy>0)?0:absy;
	int	nx = (dx>0)?absx:0;	// relativer Abstand neue Position
	int	ny = (dy>0)?absy:0;

	XFillRectangle(dpy,help,gc_bcopy,0,d,width-d,height-d);
	XFillRectangle(dpy,help,gc_bcopy,d,0,width-d,height);

	XCopyArea(dpy,bpix,help,gc_bcopy,0,0,d,d,ox,oy);
	XCopyArea(dpy,bpix,help,gc_bxor,0,0,d,d,nx,ny);

	XCopyPlane(dpy,rpix,help,gc_ball,0,0,d,d,ox,oy,1);
	XCopyPlane(dpy,rpix,help,gc_ball,0,0,d,d,nx,ny,1);

	XCopyArea(dpy,help,win,gc_bxor,0,0,width,height,0,0 );
}

// ==========================================================================


void main_xbench(int argc, char **argv)
{
XMoveChecker	*xm=0;
unsigned	bsize   = 24;
unsigned avgmove = 8;
unsigned count = 3;
unsigned secs  = 3;

	for (int i=1;i<argc;i++) {
		if (!strncmp(argv[i],"-n",2))				count = atoi(argv[i]+2);
		else if (!strncmp(argv[i],"-s",2))		secs  = atoi(argv[i]+2);
		else if (!strncmp(argv[i],"-b",2))		bsize = atoi(argv[i]+2);
		else if (!strncmp(argv[i],"-m",2))		avgmove = atoi(argv[i]+2);
		else {
			printf( "usage: bench [-options]\n" );
			printf( "       -n<n>  number of tests       (def: %d)\n", count );
			printf( "       -s<n>  seconds of test       (def: %d)\n", secs );
			printf( "       -b<n>  size of pixmap        (def: %d)\n", bsize );
			printf( "       -m<n>  average move distance (def: %d)\n", avgmove );
			exit(0);
		}
	}

	printf( "\nBenchmark for different animation modes of flying -deluxe\n" );
	printf( "Mode 2 usually turns out to be the fastest and\n" );
	printf( "therefore is turned on by default in the game.\n\n" );
	printf( "    Mode 1        Mode 2       Mode 3\n" );
	printf( "------------------------------------------\n" );
	fflush( stdout );
	while( count-- > 0 ) {
		for (int j=1;j<=3;j++) {
			switch(j) {
			case 1: xm = new XMoveChecker1( bsize, avgmove );	break;
			case 2: xm = new XMoveChecker2( bsize, avgmove );	break;
			case 3: xm = new XMoveChecker3( bsize, avgmove );	break;
			case 4: xm = new XMoveChecker4( bsize, avgmove );	break;
			}

			printf( "%6.2f pics/s ", xm->loop(secs)/(double)secs );
			fflush(stdout);
			delete xm;
		}
		printf( "\n" );
	}
	printf("\nThe benchmark computes the number frames per second by\n" );
	printf( "assuming that all 16 balls are moving at constant speed.\n" );

	printf("\nby Helmut H鰊ig, May 1995\n\n" );
}
