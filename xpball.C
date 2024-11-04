
extern Pixmap	pointer_pmap;							// aus Modul graph.C ...
extern int		pointer_off_x, pointer_off_y;		//    dto.

#ifndef _cursor_h
#	include "cursor.h"
#endif

// ------ PBallNorm ----------------------------------------------------------

int    PBallNorm::instance_count = 0;
int    PBallNorm::pwidth  = 0;
int    PBallNorm::pheight = 0;
Pixmap PBallNorm::pmap    = 0;
GC     PBallNorm::gc_bit  = 0;
double PBallNorm::next_frame = 0.0;
double PBallNorm::last_frame = 0.0;


PBallNorm::PBallNorm( PBallType type, char *disp_name ) :
	PBallTop( type )
{
//
// Remote Display öffnen
//
	rem_dpy = XOpenDisplay( disp_name );
	if (!rem_dpy) {
		fprintf( stderr, "ERROR: can't open display '%s'.\n", disp_name );
		exit(0);
	}
	scr=DefaultScreen(rem_dpy);
//
// Cursor überdefinieren
//
Cursor	cursor;
Pixmap	pixmap;
XColor	col1;
Window	grab_win;

	pixmap = XCreateBitmapFromData(rem_dpy,
					RootWindow( rem_dpy, scr ),
					cursor_bits, cursor_width, cursor_height );
	XParseColor(rem_dpy,DefaultColormap(rem_dpy,scr), "black", &col1 );
	cursor = XCreatePixmapCursor( rem_dpy, pixmap, pixmap, &col1, &col1,
							cursor_x_hot, cursor_y_hot );


	if (!strcmp(DisplayString(dpy),DisplayString(rem_dpy))) {
		warp_dpy = dpy;
		grab_win = (new_root)?new_root:win;
	}
	else {
		warp_dpy = rem_dpy;
		grab_win = RootWindow( rem_dpy, scr );
	}
	XGrabPointer( rem_dpy, grab_win,
			False, (unsigned)(ButtonPressMask|ButtonReleaseMask|PointerMotionMask),
			GrabModeAsync, GrabModeAsync, None, cursor, CurrentTime );

//
// Transformation festlegen
//
	w2n_x = DisplayWidth( rem_dpy,scr)/world_x;
	w2n_y = DisplayHeight(rem_dpy,scr)/world_y;
//
// Pointer auf Bildschirm initialisieren.
//
	x_old = -20;	// ausserhalb des Bildschirms (wegen erstem CopyArea)
	y_old = -20;

	XFreePixmap( rem_dpy, pixmap );
	XFreeCursor( rem_dpy, cursor );


	if (!instance_count++) {
		pwidth  = 30;
		pheight = 30;
		pmap    = XCreatePixmap(dpy,win,pwidth,pheight,1);
		gc_bit  = XCreateGC(dpy,pmap,0,0);
		XSetState(dpy,gc_bit,1,0,GXxor,1);
		next_frame=GetCurrentTime();
		last_frame=GetCurrentTime();
	}
}

PBallNorm::~PBallNorm() {
	if (!--instance_count) {
		XFreePixmap(dpy,pmap);
		XFreeGC(dpy,gc_bit);
	}
//
// Pointer vom Bildschirm löschen
//
	XCopyArea( dpy, pointer_pmap, win, gc_cursor, 0, 0, 16, 16,
					x_old-pointer_off_x, y_old-pointer_off_y );

	XSync( rem_dpy, 0 );
	XCloseDisplay( rem_dpy );
}

void PBallNorm::WaitForEvents() {
PBallTop *current;

	// if any balls are running or any of the pballs is not ReadyToSleep
	// we have to return directly
	if (!frames_per_sec) {
		if (g->running_balls)															return;
		for (current=pball_queue;current;current=current->next) {
			if (!(current->mode&ReadyToSleep))										return;
		}
	}

	// collected the opened displays into a fd-structure for the select
	// statement and wait a bit using a select call (if there aren't already
	// some events in the queue.
	struct timeval		timeout;
	if (!frames_per_sec) {
		timeout.tv_sec  = 1;
		timeout.tv_usec = 0;
	}
	else {
		double ctime = GetCurrentTime();
		if (current_time<last_frame || ctime>next_frame) {
			last_frame = ctime;
			next_frame = ctime + 1.0/frames_per_sec;
			return;
		}
		timeout.tv_sec  = 0;
		timeout.tv_usec = (unsigned)(1000000*(next_frame-ctime));
	}
	int	nfds;

#ifndef FD_ZERO
// if the macros aren't defined, we assume that this is a system
// using integers for the select-parameters
	int				readfds;
#	define FD_ZERO(n)	n=0
#	define FD_SET(b,n)	(n|(1<<b))
#	undef	 FDS_TYPE
#	define FDS_TYPE	(int*)
#else
	struct fd_set	readfds;
#endif

	FD_ZERO( &readfds );
	FD_SET(  ConnectionNumber(dpy), &readfds  );
	nfds = ConnectionNumber(dpy)+1;
	if (XEventsQueued(dpy,QueuedAfterFlush))										return;

	for (current=pball_queue;current;current=current->next) {
		FD_SET(ConnectionNumber(((PBallNorm*)current)->rem_dpy),&readfds);
		if (ConnectionNumber(((PBallNorm*)current)->rem_dpy)>=nfds) {
			nfds=ConnectionNumber(((PBallNorm*)current)->rem_dpy)+1;
		}
		if (XEventsQueued(((PBallNorm*)current)->rem_dpy,QueuedAfterFlush))return;
	}

// FDS_TYPE should have been set in global.h to the type, that is need
// in the select-call for the filedescriptors.
	if (!select(nfds,FDS_TYPE &readfds,0,0,&timeout)) {
		/* TIMEOUT */
		if (frames_per_sec) {
				last_frame = next_frame;
				next_frame = next_frame + 1.0/frames_per_sec;
		}
	}
}

void PBallNorm::Update() {
XEvent	event;

	while (XEventsQueued( rem_dpy, QueuedAfterFlush )) {
		XNextEvent( rem_dpy, &event );
		switch( event.xany.type ) {
		case MotionNotify:
			while( XCheckMaskEvent( rem_dpy, PointerMotionMask, &event ));
			PointerMoveTo(Vec2(Real(event.xmotion.x_root)/w2n_x,
												Real(event.xmotion.y_root)/w2n_y));
			break;
		case ButtonPress:
			Press( event.xbutton.button );
			break; // case ButtonPress

		case ButtonRelease:
			Release( event.xbutton.button );
		} // switch(event.type)
	}

	PBallTop::Update();
}


void PBallNorm::Warp( const Vec2 &dest ) {
XEvent	event;

	XWarpPointer( warp_dpy, None, RootWindow( rem_dpy, scr ),
			0,0,0,0 ,
			(int)(dest.X()*w2n_x),
			(int)(dest.Y()*w2n_y) );

	if (mode&UsingTool)	MoveAimingTool();

	XSync( warp_dpy, 0 );
	while( XCheckMaskEvent( rem_dpy, PointerMotionMask, &event ));
}


void PBallNorm::RedrawPointer() {
	XCopyArea( dpy, pointer_pmap, win, gc_cursor, 0, 0, 16, 16,
					x_old-pointer_off_x, y_old-pointer_off_y );
}

void PBallNorm::SetPointer( int x, int y ) {
	XCopyArea( dpy, pointer_pmap, win, gc_cursor, 0, 0, 16, 16,
					x_old-pointer_off_x, y_old-pointer_off_y );
	x_old = x;
	y_old = y;
//	RedrawPointer:
	XCopyArea( dpy, pointer_pmap, win, gc_cursor, 0, 0, 16, 16,
					x_old-pointer_off_x, y_old-pointer_off_y );
}


// ------------------------------------------------------------

#ifndef XPix
#	define XPix(x)   ((int)((x)*w2n))
#	define YPix(y)   ((int)((y)*w2n))
#	define Pix(r)    ((int)((r)*w2n))
#endif

void PBallNorm::DrawQueue() {
	if (valid_queue_position) {
		XPoint	p[3];

		p[0].x = XPix(q_end_s.X());
		p[0].y = YPix(q_end_s.Y());
		p[1].x = XPix(q_s1_s.X());
		p[1].y = YPix(q_s1_s.Y());
		p[2].x = XPix(q_s2_s.X());
		p[2].y = YPix(q_s2_s.Y());
		XFillPolygon(dpy,win,gc_current,p,3,0,0);
	}
}

void PBallNorm::StartQueue(const Vec2 &end, const Vec2 &s1, const Vec2 &s2) {
	q_end_s = end;
	q_s1_s  = s1;
	q_s2_s  = s2;
	valid_queue_position = 1;
	DrawQueue();
}

void PBallNorm::SetMinSize( int width, int height ) {
// ggf. vergroesserte Pixmap anlegen
	if (width>pwidth||height>pheight) {
		XFreePixmap(dpy,pmap);
		if (width>pwidth)			pwidth =width +10;
		if (height>pheight)		pheight=height+10;
		pmap = XCreatePixmap(dpy,win,pwidth+2,pheight+2,1);
	}
}

#define _TEST_BUG

void PBallNorm::MoveQueue(const Vec2 &end, const Vec2 &s1, const Vec2 &s2) {
XPoint	p[6];
unsigned	i;

#ifdef TEST_BUG
//
// There is an optimization (-O2) bug in gcc-2.7.0, so that q_end_s always
// seems be equal to end. I assume, that due to the optimization, the
// assignment q_end_s = end is done before the first assignment: 
//
	printf( "OLD POSITION: %f x %f\n", (float)q_end_s.X(), (float)q_end_s.Y() );
	printf( "NEW POSITION: %f x %f\n", (float)end.X(), (float)end.Y() );
#endif

	p[0].x = XPix(q_end_s.X());	// triangle-coordinates of old queue
	p[0].y = YPix(q_end_s.Y());
	p[1].x = XPix(q_s1_s.X());
	p[1].y = YPix(q_s1_s.Y());
	p[2].x = XPix(q_s2_s.X());
	p[2].y = YPix(q_s2_s.Y());

	q_end_s = end;						// store new position
	q_s1_s  = s1;
	q_s2_s  = s2;
	p[3].x = XPix(q_end_s.X());	// triangle-coordinates of new queue
	p[3].y = YPix(q_end_s.Y());
	p[4].x = XPix(q_s1_s.X());
	p[4].y = YPix(q_s1_s.Y());
	p[5].x = XPix(q_s2_s.X());
	p[5].y = YPix(q_s2_s.Y());

	valid_queue_position = 1;

int	min_x = p[0].x;
int	max_x = p[0].x;
int	min_y = p[0].y;
int	max_y = p[0].y;

#ifdef TEST_BUG
	printf( "NOT ASSIGNED: %f x %f\n", (float)q_end_s.X(), (float)q_end_s.Y() );
#endif

// Minimum suchen
	for (i=1;i<sizeof(p)/sizeof(XPoint);i++) {
		if (p[i].x < min_x)		min_x = p[i].x;
		if (p[i].x > max_x)		max_x = p[i].x;
		if (p[i].y < min_y)		min_y = p[i].y;
		if (p[i].y > max_y)		max_y = p[i].y;
	}
#ifdef TEST_BUG
	printf( "    ASSIGNED: %f x %f\n", (float)q_end_s.X(), (float)q_end_s.Y() );
#endif


// Punkte nun relativ zur Bitmap
	for (i=0;i<sizeof(p)/sizeof(XPoint);i++) {
		p[i].x -= min_x;
		p[i].y -= min_y;
	}

int width  = max_x - min_x;
int height = max_y - min_y;

	SetMinSize(width,height);

// Pixmap aufbauen und ins Fenster verknuepfen
	XSetFunction(dpy,gc_bit,GXclear);
	XFillRectangle(dpy,pmap,gc_bit,0,0,width,height);
	XSetFunction(dpy,gc_bit,GXxor);
	XFillPolygon(dpy,pmap,gc_bit,&p[0],3,0,0);
	XFillPolygon(dpy,pmap,gc_bit,&p[3],3,0,0);
	XCopyPlane(dpy,pmap,win,gc_current,0,0,width,height,min_x,min_y,1);
}

void PBallNorm::EndQueue() {
	DrawQueue();
	valid_queue_position = 0;
}

// ------ PBall -------------------------------------------------------------

PBall::PBall( PBallType type, char *disp_name ) :
	PBallNorm( type, disp_name )
{
	zoom  = 0;
	ozoom = 0;
	off_x = 0.0;
	off_y = 0.0;
	mid_x = DisplayWidth( rem_dpy,scr)/2;
	mid_y = DisplayHeight(rem_dpy,scr)/2;
	zfact = 32;
}

PBall::~PBall() {
}

void PBall::Update() {
XEvent	event;

	while (XEventsQueued( rem_dpy, QueuedAfterFlush )) {
		XNextEvent( rem_dpy, &event );
		switch( event.xany.type ) {
		case MotionNotify:
			while( XCheckMaskEvent( rem_dpy, PointerMotionMask, &event ));
			if (zoom) {
				off_x=(double)(event.xmotion.x_root-mid_x)/zfact;
				off_y=(double)(event.xmotion.y_root-mid_y)/zfact;
				PointerMoveTo(Vec2(Real(zoom_x+off_x)/w2n_x,
												Real(zoom_y+off_y)/w2n_y));
			}
			else {
				PointerMoveTo(Vec2(Real(event.xmotion.x_root+off_x)/w2n_x,
												Real(event.xmotion.y_root+off_y)/w2n_y));
			}
			break;
		case ButtonPress:
			if (!zoom) {
				if ( event.xbutton.button == Button2 ) {
					StartZooming( event.xbutton.x_root, event.xbutton.y_root );
				}
				else {
					Press( event.xbutton.button );
				}
			}
			break; // case ButtonPress

		case ButtonRelease:
			if ( event.xbutton.button == Button2 )		StopZooming();
			else											Release( event.xbutton.button );
		} // switch(event.type)
	}

	PBallTop::Update();
}

void PBall::StartZooming(int x,int y) {
	if (!zoom) {
		zoom_x = x;	zoom_y = y;
		zoom=1;
		XWarpPointer( warp_dpy, None, RootWindow( rem_dpy, scr ),
			0,0,0,0,(int)(mid_x+zfact*off_x),(int)(mid_y+zfact*off_y) );
		SetPointer( (int)(dest.X()*w2n), (int)(dest.Y()*w2n) );
		XSync( warp_dpy, 1 );
	}
}

void PBall::StopZooming() {
	if (zoom) {
		zoom_x += (int)floor(off_x);
		zoom_y += (int)floor(off_y);
		off_x    = off_x-floor(off_x);
		off_y    = off_y-floor(off_y);
		XWarpPointer( warp_dpy, None, RootWindow( rem_dpy, scr ),
			0,0,0,0,zoom_x,zoom_y );
		XSync( warp_dpy, 0 );
		zoom=0;
		SetPointer( (int)(dest.X()*w2n), (int)(dest.Y()*w2n) );
	}
}

void PBall::Warp( const Vec2 &dest ) {

	StopZooming();
	PBallNorm::Warp(dest);
}

//
// when cursor moves less than ISTEP pixels (in zoom mode) at an instant
// both grid-displays will be double-buffered in the same pixmap
//
#define	ISTEP	2

void PBall::SetPointer( int x, int y ) {
	SetMinSize(2*zfact+2+2*ISTEP,2*zfact+2+2*ISTEP);

int	oox=ISTEP;		// internal offset in help-pixmap
int	ooy=ISTEP;

	if (ozoom) {
							// prepare the grid of the old position (centered)
			XSetFunction(dpy,gc_bit,GXclear);
			XFillRectangle(dpy,pmap,gc_bit,0,0,2*zfact+2+2*ISTEP,2*zfact+2+2*ISTEP);
				/*** GRID DRAWER (identical code as in zoom) ***/
			XSetFunction(dpy,gc_bit,GXxor);
			XDrawRectangle(dpy,pmap,gc_bit,
					oox,ooy,2*zfact+1,2*zfact+1);
			XDrawLine(dpy,pmap,gc_bit,
					(int)(zfact-ooff_x)+oox,(int)(ooy),
					(int)(zfact-ooff_x)+oox,(int)(2*zfact+ooy) );
			XDrawLine(dpy,pmap,gc_bit,
					(int)(2*zfact-ooff_x)+oox,(int)(ooy),
					(int)(2*zfact-ooff_x)+oox,(int)(2*zfact+ooy) );
			XDrawLine(dpy,pmap,gc_bit,
					(int)(oox),(int)(zfact-ooff_y)+ooy,
					(int)(2*zfact+oox),(int)(zfact-ooff_y)+ooy );
			XDrawLine(dpy,pmap,gc_bit,
					(int)(oox),(int)(2*zfact-ooff_y)+ooy,
					(int)(2*zfact+oox),(int)(2*zfact-ooff_y)+ooy );
				/*** end GRID DRAWER ***/
	}
	oox = x-x_old+ISTEP;
	ooy = y-y_old+ISTEP;

	XCopyArea( dpy, pointer_pmap, win, gc_cursor, 0, 0, 16, 16,
					x_old-pointer_off_x, y_old-pointer_off_y );
	x_old = x;
	y_old = y;
	XCopyArea( dpy, pointer_pmap, win, gc_cursor, 0, 0, 16, 16,
					x_old-pointer_off_x, y_old-pointer_off_y );

	if (zoom) {
			ooff_x = (off_x-floor(off_x))*zfact;
			ooff_y = (off_y-floor(off_y))*zfact;

			if (ozoom&&(oox<0)||(oox>2*ISTEP)||(ooy<0)||(ooy>2*ISTEP)) {
				// flush prepared map
				XCopyPlane(dpy,pmap,win,gc_cursor,ISTEP,ISTEP,2*zfact+2,2*zfact+2,
					x_old-zfact-oox+ISTEP, y_old-zfact-ooy+ISTEP, 1 );
				ozoom=0;
			}
			if (!ozoom) {
				// reset map
				oox = ISTEP;
				ooy = ISTEP;
				XSetFunction(dpy,gc_bit,GXclear);
				XFillRectangle(dpy,pmap,gc_bit,0,0,2*zfact+2+2*ISTEP,2*zfact+2+2*ISTEP);
			}
			ozoom=1;

				/*** GRID DRAWER (identical code as in ozoom) ***/
			XSetFunction(dpy,gc_bit,GXxor);
			XDrawRectangle(dpy,pmap,gc_bit,
					oox,ooy,2*zfact+1,2*zfact+1);
			XDrawLine(dpy,pmap,gc_bit,
					(int)(zfact-ooff_x)+oox,(int)(ooy),
					(int)(zfact-ooff_x)+oox,(int)(2*zfact+ooy) );
			XDrawLine(dpy,pmap,gc_bit,
					(int)(2*zfact-ooff_x)+oox,(int)(ooy),
					(int)(2*zfact-ooff_x)+oox,(int)(2*zfact+ooy) );
			XDrawLine(dpy,pmap,gc_bit,
					(int)(oox),(int)(zfact-ooff_y)+ooy,
					(int)(2*zfact+oox),(int)(zfact-ooff_y)+ooy );
			XDrawLine(dpy,pmap,gc_bit,
					(int)(oox),(int)(2*zfact-ooff_y)+ooy,
					(int)(2*zfact+oox),(int)(2*zfact-ooff_y)+ooy );
				/*** end GRID DRAWER ***/

			// copy full map (2*zfact+2+2*ISTEP), since ozoom&zoom might be joined
			XCopyPlane(dpy,pmap,win,gc_cursor,0,0,2*zfact+2+2*ISTEP,2*zfact+2+2*ISTEP,
					x_old-zfact-oox, y_old-zfact-ooy, 1 );
	}
	else {
		if (ozoom) {
			// flush prepared map
			XCopyPlane(dpy,pmap,win,gc_cursor,ISTEP,ISTEP,2*zfact+2,2*zfact+2,
					x_old-zfact-oox+ISTEP, y_old-zfact-ooy+ISTEP, 1 );
		}
		ozoom=0;
	}
}
