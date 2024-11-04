//
// XMover: Modul zur Optimierten Ball-Darstellung unter XWindows
// - flackerfreies Bewegen der Bälle durch Hilfspixmaps
// - Beleuchtungseffekt durch Raytracing-Bitmaps
// - Darstellung halber Kugeln durch vorberechnete Pixmaps
//

#ifndef _vec3_h
#	include "vec3.h"
#endif

#define	LEFT_RIGHT_OPT
#define	USE_STUFFER

/*----------------------------------------------------------------------------*/

PixmapCache::PixmapCache() {
	count = 0;
}

PixmapCache::~PixmapCache() {
}

Pixmap PixmapCache::Lock( int id_in, int size_in ) {
	for (int i=0;i<count;i++) {
		if (id_in==id[i]&&size_in==size[i]) {
			lockcount[i]++;
			return pmap[i];
		}
	}
	return 0;
}

void PixmapCache::Unlock( Display *dpy, Pixmap pix ) {
//
// free the given pixmap, if nobody else will use it.
//
	for (int i=0;i<count;i++) {
		if (pix==pmap[i]) {
			if (--lockcount[i]==0) {
				XFreePixmap(dpy,pix);
				count--;
				while( i<count ) {
					pmap[i]      = pmap[i+1];
					size[i]      = size[i+1];
					id[i]        = id[i+1];
					lockcount[i] = lockcount[i+1];
					i++;
				}
			}
			return;
		}
	}
}

void PixmapCache::InsertAndLock( Display * /*dpy*/, Pixmap pix, int id_in, int size_in ) {
	if (Lock(id_in,size_in)) {
		fprintf(stderr,"internal error 1 in PixmapCache::InsertAndLock\n" );
		return;
	}
	if (count>=PCACHE_MAX) {
		fprintf(stderr,"internal error 2 in PixmapCache::InsertAndLock\n" );
		return;
	}
	pmap[count]      = pix;
	size[count]      = size_in;
	id[count]        = id_in;
	lockcount[count] = 1;
	count++;
}

PixmapCache PixmapCache::pcache;

/*----------------------------------------------------------------------------*/

//
// Klasse zur Komprimierung der RingState-Felder
//
class Stuffer {
	public:
		Stuffer() : ws(0), wl(sizeof(RingState))			{}

		void Init( int ws_in, int wl_in ) {
				ws = ws_in;
				if (wl!=wl_in) {
					printf( "ERROR: Stuffer used with illegal input size !\n" );
					exit(0);
				}
		}
		void Shrink( RingState *addr, int len );
		void Expand( RingState *addr, int len );
		int  SSize( int len )
#ifdef USE_STUFFER
		{		return ((len*ws+7)/8);	}
#else
		{		return ((len*wl+7)/8);	}
#endif
		int  LSize( int len )
		{		return ((len*wl+7)/8);	}

	private:
		int	ws;	// Bits klein
		const int	wl;	// Bits gross
		static mtab[9];
};

int Stuffer::mtab[9] = { 0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff };

#ifdef USE_STUFFER
void Stuffer::Shrink( RingState *addr, int len ) {
RingState		*r_p;
RingState		r_val;
int				r_len;
unsigned char	*w_p;
unsigned char	w_val;
int	w_space;

	r_p     = addr;
	w_p     = (unsigned char*)addr;
	w_space = 8;
	w_val   = 0;
	while( len-- ) {
		r_val = *r_p++;
		r_len = ws;

		while (r_len>=w_space) {
			w_val  |= ((unsigned char)r_val)&mtab[w_space];
			*w_p++  = w_val;
			w_val   = 0;
			r_val >>= w_space;
			r_len  -= w_space;
			w_space = 8;
		}

		if (r_len) {
			w_val   |= (((unsigned char)r_val)&mtab[w_space])<<(w_space-r_len);
			w_space -= r_len;
		}
	}
	*w_p++ = w_val;
}

void Stuffer::Expand( RingState *addr, int len ) {
RingState	*w_p;						// Schreibzeiger - expandiertes Ende
RingState	w_val;
int	w_space;							// fehlende Bits zum Gesamtwort
unsigned char	*r_p;					// Lesezeiger - komprimiertes Ende
unsigned char	r_val;				// gueltige Bits
int	r_len;							// Zahl gueltige Bits an Lesezeigerposition

	r_p     = (unsigned char*)addr + len*ws/8;		// Adresse des letzten Byte
	r_len   = (len*ws)%8;			// gültige Bits im letzen Byte
	if (r_len)	r_val = *r_p;		// letzte Bits holen
	else {		r_val = *--r_p;
					r_len = 8;
	}
	w_p     = addr + len;

	while( len-- ) {
		w_space = ws;
		w_val   =  0;
		while( r_len<=w_space ) {
			w_val <<= r_len;
			w_val  |= r_val>>(8-r_len);
			w_space-= r_len;
			r_val   = *--r_p;			// ABR: liest 1 Byte ueber die Untergrenze
			r_len   = 8;
		}
		if (w_space) {
			w_val   <<= w_space;
			w_val    |= r_val&mtab[w_space];
			r_len    -= w_space;
		}
		*--w_p  = w_val;
	}
}

#else
void Stuffer::Shrink( RingState *, int ) {}
void Stuffer::Expand( RingState *, int ) {}
#endif

/*----------------------------------------------------------------------------*/

class Bitmap {
public:
	void Clear()	{	memset( bits, 0, bytes_per_line*height ); }

	void Init( int w, int h, char *bmap ) {
		width				= w;
		bytes_per_line = (width+7)/8;
		height 			= h;
		bits 				= bmap;
	}
	Bitmap( int w, int h, char *bmap )	{ Init(w,h,bmap); }
	Bitmap()										{}

	int GetPixel( int x, int y ) {
		if ((x<0)||(x>=width)||(y<0)||(y>=height))		return 0;
		else {
			unsigned char mask = 1<<(x&7);
			return (bits[bytes_per_line*y+(x>>3)]&mask)?1:0;
		}
	}
	void SetPixel( int x, int y ) {
		unsigned char mask = 1<<(x&7);
		bits[bytes_per_line*y+(x>>3)] |= mask;
	}
	void ClrPixel( int x, int y ) {
		unsigned char mask = 1<<(x&7);
		bits[bytes_per_line*y+(x>>3)] &= ~mask;
	}


	Pixmap Create() {
		return XCreatePixmapFromBitmapData(dpy,win,bits,width,height,1,0,1 );
	}
	void Print();

	int	width;
	int	height;
protected:
	int	bytes_per_line;
	char *bits;
};

class LocBitmap : public Bitmap {
public:
	void Init( int w, int h ) {
		Bitmap::Init(w,h,0);
		bits 				= new char[bytes_per_line*height];
		Clear();
	}
	void Init( int w, int h,  FILE *fp ) {
		Bitmap::Init(w,h,0);
		bits 				= new char[bytes_per_line*height];
		fread(bits,sizeof(char),bytes_per_line*height,fp);
	}
	void Write( FILE *fp ) {
		fwrite(bits,sizeof(char),bytes_per_line*height,fp);
	}
	LocBitmap( int w, int h )		{ Init(w,h); }

	LocBitmap()		{}
	~LocBitmap()		{ delete bits; }

	// change for a sun sparc-station runing 4.1.3 that thinks:
	// xmover.c:463: no non-hidden member function 'LocBitmap::create' defined
	Pixmap Create() { return Bitmap::Create(); }
};


void Bitmap::Print() {
	for(int y=0;y<height;y++) {
		for(int x=0;x<width;x++)	printf( "%d", GetPixel(x,y) );
		printf( "\n" );
	}
}

/*----------------------------------------------------------------------------*/

#ifdef STATISTICS
unsigned long	BallMover::moves = 0l;
#endif

BallMover::BallMover( const Real &r_in ) {
	r = (int)(r_in*w2n);	// Radius als Member festhalten
	d = 2*r;					// Bem: Durchmesser immer gerade

	bpix = help = 0;
}


BallMover::~BallMover() {
	if (help)	XFreePixmap( dpy, help );
	if (bpix)	XFreePixmap( dpy, bpix );
}


void BallMover::Init() {
LocBitmap	*bit;			// Kugelbitmap
int		x,y;
double	dr=r-0.5;		// tatsächlicher (Welt-)Radius

	bit = new LocBitmap( d, d );
	max_rad  = r-0.25+EPS;
	max_rad2 = max_rad * max_rad;
	for (y=0;y<d;y++) {
		for (x=0;x<d;x++) {
			double dx = x-dr;								// X-Abstand zur Kreismitte
			double dy = y-dr;								// Y-Abstand zur Kreismitte
			double dist2 = dx*dx + dy*dy;
			if (dist2<max_rad2) {
				bit->SetPixel(x,y);
			}
		}
	}

	bpix = bit->Create();

	d_help	= 2*d-1;
	help		= XCreatePixmap(dpy,win,d_help,d_help,DefaultDepth(dpy,0));

	delete bit;
}

void BallMover::DrawBallAt( int x, int y, int col_x )
{
#ifndef NODRAW
	XFillRectangle(dpy,help,gc_bclear,0,0,d,d);
	XCopyPlane(dpy,bpix,help,gc_ball[col_x],0,0,d,d,0,0,1);
	Pixmap	shadow = GetShadowMap(x,y);
	if (shadow)	XCopyPlane(dpy,shadow,help,gc_lay2,0,0,d,d,0,0,1);
	XCopyArea(dpy,help,win,gc_bxor,0,0,d,d,x-r, y-r );
#endif
}


void BallMover::MoveBallOnScreen( int oldx, int oldy, int newx, int newy, int col_x ) {

int	dx = newx-oldx;
int	dy = newy-oldy;
int absx = (dx>0)?dx:-dx;
int absy = (dy>0)?dy:-dy;

#ifndef NODRAW
	if ( (absx<d_help-d) && (absy<d_help-d) ) {
		Pixmap	shadow;

		int	width = d+absx;		// tatsächlicher Ausschnittgröße der Pixmap
		int	height= d+absy;
		int	ox = (dx>0)?0:absx;	// relativer Abstand alte Position
		int	oy = (dy>0)?0:absy;
		int	nx = (dx>0)?absx:0;	// relativer Abstand neue Position
		int	ny = (dy>0)?absy:0;

		XFillRectangle(dpy,help,gc_bclear,0,0,width,height);

		XCopyPlane(dpy,bpix,help,gc_ball[col_x],0,0,d,d,ox,oy,1);
		shadow = GetShadowMap(oldx,oldy);
		if (shadow)	XCopyPlane(dpy,shadow,help,gc_lay2,0,0,d,d,ox,oy,1);

		XCopyPlane(dpy,bpix,help,gc_ball[col_x],0,0,d,d,nx,ny,1);
		shadow = GetShadowMap(newx,newy);
		if (shadow)	XCopyPlane(dpy,shadow,help,gc_lay2,0,0,d,d,nx,ny,1);

		XCopyArea(dpy,help,win,gc_bxor,0,0,width,height,oldx-ox-r, oldy-oy-r );
	}
	else {
		DrawBallAt(oldx,oldy,col_x);
		DrawBallAt(newx,newy,col_x);
	}
#endif

#ifdef STATISTICS
	moves++;
#endif
}

Pixmap BallMover::GetShadowMap( int /*x*/, int /*y*/ )		{ return 0; }

// -------------------------------------------------------------------------

DiscMover::DiscMover( const Real &r ) :
	BallMover( r )
{
	lpix = 0;
}

DiscMover::~DiscMover()
{
	if (lpix)	XFreePixmap(dpy,lpix);
}

void DiscMover::Init() {
LocBitmap	*lbit;		// Kreisbitmap
int		x,y;
double	dr=r-0.5;		// tatsächlicher (Welt-)Radius

	BallMover::Init();

	lbit	= new LocBitmap( d, d );
double	shade_rad	= 1.2;			// Pixelbreite in WS-umgerechnet
double	inner1		= r*1/4;
double	inner2		= r*2/4;

	for (y=0;y<d;y++) {
		for (x=0;x<d;x++) {
			double dx = x-dr;								// X-Abstand zur Kreismitte
			double dy = y-dr;								// Y-Abstand zur Kreismitte
			double dist = sqrt(dx*dx + dy*dy);
			if (dist<max_rad) {
				if (  (dist>max_rad-shade_rad)
					|| (dist>inner1-shade_rad)&&(dist<inner1)
					|| (dist>inner2-shade_rad)&&(dist<inner2) ) {
					lbit->SetPixel(x,y);
				}
			}
		}
	}

	lpix = lbit->Create();
	delete lbit;
}

Pixmap DiscMover::GetShadowMap( int /*x*/, int /*y*/ )		{ return lpix; }

// -------------------------------------------------------------------------

ShadedBallMover::ShadedBallMover( const Real &r ) :
	BallMover( r )
{
	lpix=0;
}

ShadedBallMover::~ShadedBallMover()
{
	if (lpix) {
		for (int i=0;i<lpixs_all;i++) {
			XFreePixmap( dpy, lpix[i] );
		}
		delete [] lpix;
		lpix = 0;
	}
}

#if (0)
const	double circ_x	=  -50.0;
const	double circ_y	=  -50.0;
const	double circ_r1 =  150.0;
const	double circ_r2 =  300.0;
#endif


void ShadedBallMover::Init() {
int			x,y,i;

LocBitmap	*lbit;	// Feld der Hilfbitmaps

double	dr=r-0.5;		// tatsächlicher (Welt-)Radius

int		ncircs;			// number of lamps
double	circ_z;			// hight of lamps
struct lightdata {
	double	x,y;			// position of lamps
	double	r1, r2;		// size (inner/outer) of lamps
}			circ[3];

	BallMover::Init();

	if (light_flag) {
		lpixs_x = 24;     							// Zahl der Bitmaps in X-Richtung
		lpixs_y = 12;									// Zahl der Bitmaps in Y-Richtung
	}
	else {
		lpixs_x = 0;
		lpixs_y = 0;
	}
	lpixs_all = lpixs_x * lpixs_y;			// Gesamtzahl
	distx = max_x / lpixs_x;					// Weltkoordinatenraster X
	disty = max_y / lpixs_y;					// Weltkoordinatenraster Y
	lbit = new LocBitmap[lpixs_all];			// Anlegen der Bitmaps
	for (i=0;i<lpixs_all;i++) 	lbit[i].Init(d,d);

	if (light_flag<=1) {
		ncircs = 1;								// single light, far far away
		circ_z = 2000.0;
		circ[0].x= -50.0; circ[0].y= -50.0; circ[0].r1= 150.0; circ[0].r2 = 300.0;
	}
	else {
		ncircs = 3;
		circ_z = MaxX() * 2.0 / 3.0;		// einfach 'mal so angenommen
		for (int l=0;l<ncircs;l++) {
			circ[l].x  = MaxX() * (double)(l+1) / 4.0;
			circ[l].y  = MaxY() /  2.0;
			circ[l].r1 = MaxX() / 16.0;
			circ[l].r2 = MaxX() /  8.0;
		}
	}


	DBG1( UnixTrace, "Starting image processing with ball size %d pixel:\n", d );
	DBG1( UnixTrace, "BitmapData: >%d bytes\n", (d+7)/8*d*lpixs_all );

	for (y=0;y<d;y++) {							// Für alle Punkte der Bitmaps
		for (x=0;x<d;x++) {
			double dx = x-dr;								// X-Abstand zur Kreismitte
			double dy = y-dr;								// Y-Abstand zur Kreismitte
			double dist2 = dx*dx + dy*dy;
			if (dist2<max_rad2) {

				double dz = dr*dr-dist2;				// quadratischer Z-Abstand
				if (dz>=0) {
					double f = circ_z/sqrt(dz);		// Multiplikator zur Decke
					dx *= f;									//	XPos an Decke
					dy *= f;									// YPos an Decke

					for (int px=0;px<lpixs_x;px++) {	// für alle Bitmaps
						for (int py=0;py<lpixs_y;py++) {
							int	lflag=0;		// 0-dark, 1-medium, 2-light
							for (int l=0;l<ncircs;l++) {
							// Weltposition relativ zu Kreislicht
								double wx = (px*distx+(distx>>1)+dx)/w2n-circ[l].x;
								double wy = (py*disty+(disty>>1)+dy)/w2n-circ[l].y;
								double d = wx*wx+wy*wy;
								if (d<circ[l].r2*circ[l].r2) {
									if (d<circ[l].r1*circ[l].r1) {
										// inner circle
											lflag=2; break;
									}
									else	lflag=1;
								}
							}
							if ( (lflag==2) || (lflag&&((x^y)&1)) ) {
								lbit[px+py*lpixs_x].SetPixel(x,y);
							}
						}
					}
				}
			}
		}
	}

	lpix = new Pixmap[lpixs_all];
	for (i=0;i<lpixs_all;i++) {
		lpix[i] = lbit[i].Create();
	}
	delete [] lbit;
}


Pixmap ShadedBallMover::GetShadowMap( int x, int y ) {
int	xp = x/distx;
	if (xp<0)			xp=0;
	if (xp>=lpixs_x)	xp=lpixs_x-1;
int	yp = y/disty;
	if (yp<0)			yp=0;
	if (yp>=lpixs_y)	yp=lpixs_y-1;

	return lpix[xp+yp*lpixs_x];
}


void ShadedBallMover::CreateLightWindow() {
Window	help;
const int o=2;

	help=CreateWindow( "Beleuchtung", lpixs_x*(d+o)+1 , lpixs_y*(d+o)+1 );
	for (int px=0;px<lpixs_x;px++) {
		for (int py=0;py<lpixs_y;py++) {
			XCopyPlane(dpy,bpix,help,gc_ball[2],0,0,d,d,o/2+px*(d+o),o/2+py*(d+o),1);
			XCopyPlane(dpy,lpix[px+py*lpixs_x],help,gc_lay2,0,0,d,d,o/2+px*(d+o),o/2+py*(d+o),1);

		}
	}
}

// -------------------------------------------------------------------------

const int HalfBallMover::o=4;

HalfBallMover::HalfBallMover( const Real &r, int mode_in ) :
	ShadedBallMover( r )
{
	rpix	= 0;		// Ring-Pixmap-Feld
	nbpix = 0;		// negative Pixmap zum Debuggen
	tw		= 0;
	mode	= mode_in;

	right = left = up = down = 0;
}

HalfBallMover* HalfBallMover::Create( const Real &r, int mode_in ) {
	switch (enhanced_mover) {
	case 2:		return new HiEnhancedHalfBallMover(r,mode_in);
	case 1:		return new LoEnhancedHalfBallMover(r,mode_in);
	default:		return new HalfBallMover(r,mode_in);
	}
}

HalfBallMover::~HalfBallMover()
{
	if (rpix) {
		for (int i=0;i<rpixs_all;i++) {
			XFreePixmap( dpy, rpix[i] );
		}
		delete [] rpix;
		rpix = 0;
	}
	if (nbpix)		XFreePixmap( dpy, nbpix );

	if (right)		delete [] right;
	if (left)		delete [] left;
	if (up)			delete [] up;
	if (down)		delete [] down;
}

void HalfBallMover::Init() {
int			x,y,i,l,b;

LocBitmap	*rbit;	// Feld der Ring-Bitmaps
Vec3			*pv;		// Vec2 zum Pol der Kugel der entsprechenden Bitmap

	ShadedBallMover::Init();

//
// Zahl der Pixmap's, Größe der Felder bestimmen
//
	sym = (mode>1)?2.0:1.0;
	//sym = 1.0;

	if (mode) {
		rpixs_l   = (int)(3.14*d/sym);   // Zahl der Bitmaps in Längen-Richtung
		rpixs_b   = (int)(3.14*d/2.0);	// Zahl der Bitmaps in Breiten-Richtung
		mult	= 6;								// Internes Raster
	}
	else {
		rpixs_l = rpixs_b = 1;
		mult = 1;
	}
	mult2 = mult/2;
	rpixs_all = rpixs_l * rpixs_b;		// Gesamtzahl

	vecs_l    = rpixs_l * mult;			// Hilfsraster feiner als Bitmap-Raster
	vecs_b    = rpixs_b * mult;

	vecs_all  = vecs_l * vecs_b;			// Gesamtgröße des Rasters
	for (i=0;i<(int)sizeof(RingState);i++)	vecs_all/=256;
	if (vecs_all>0) {
		printf( "too many RingStates (%d) -> switch type to unsigned long (in xmover.h)\n",
					vecs_l * vecs_b );
		exit(0);
	}
	vecs_all  = vecs_l * vecs_b;			// Gesamtgröße des Rasters

char	fname[80];
	sprintf(fname,"%s/fly%d-%d.dta",DATA_DIRECTORY,mode,d);
FILE	*fp;

#ifdef DEBUG
	fp = (debug&ForceCalc)?0:fopen( fname, "r" );
#else
	fp = fopen( fname, "r" );
#endif

	rbit = new LocBitmap[rpixs_all];		// Anlegen der Bitmaps
	for (l=0;l<rpixs_l;l++) {				// für alle Bitmaps
		for (b=0;b<rpixs_b;b++) {
			if (!fp)	rbit[PixIndex(l,b)].Init(d,d);
			else		rbit[PixIndex(l,b)].Init(d,d,fp);
		}
	}

	pv   = new Vec3[d*d];
	for (y=0;y<d;y++) {
		for (x=0;x<d;x++) {
			double dx = x-r+0.5;								// X-Abstand zur Kreismitte
			double dy = y-r+0.5;								// Y-Abstand zur Kreismitte
			double dist2 = dx*dx + dy*dy;					// Dist. zur sichtbaren Mitte
			if (dist2<max_rad2) {
		//		double dzr = sqrt(dr*dr-dy*dy);		// Kugel-Radius auf Höhe z
		//		double dz  = sqrt(dzr*dzr-dx*dx);	// fehlende Tiefenkoordinate
				double dz  = sqrt(r*r-dist2);
				pv[x+y*d] = Vec3( dx, dz, dy );
			}
			else {
				pv[x+y*d] = Vec3( 0.0, 0.0, 0.0 );
			}
		}
	}

#ifdef DEBUG
double	start_time = GetCurrentTime();
#endif

	if (!fp) {
		DBG3( UnixTrace, "Rings need %dx%d=%d Bitmaps\n",rpixs_l,rpixs_b,rpixs_all);
		DBG1( UnixTrace, "BitmapData:   >%7d Bytes\n", (d+7)/8*d*rpixs_all );
		DBG1( UnixTrace, "+ temporary:  >%7d Bytes\n",
					 d*d*(sizeof(Vec3)+sizeof(Bitmap)+(d*(d+7)/8) ) );

//
// Bitmaps ausrechen
//
		printf( "image processing: 00%%" ); fflush(stdout);

		for (l=0;l<rpixs_l;l++) {	// für alle Bitmaps
			printf( "\b\b\b%02d%%", (int)100*l/rpixs_l ); fflush(stdout);
			for (b=0;b<rpixs_b;b++) {
				Vec3	v = Vec3( lToDeg(l*mult+mult2), bToDeg(b*mult+mult2) )*Real(r);
				switch(mode) {
				case 1:	for (y=0;y<d;y++) {		// Volle Kugeln
								for (x=0;x<d;x++) {
									if (!pv[x+y*d].IsZero()) {
										Real	ang = v.AngleRadTo( pv[x+y*d] );
										if (ang>0.35)	rbit[PixIndex(l,b)].SetPixel(x,y);
									}
								}
							}
							break;
				case 2:	for (y=0;y<d;y++) {		// Halbe Kugeln
								for (x=0;x<d;x++) {
									if (!pv[x+y*d].IsZero()) {
										Real	ang = v.AngleRadTo( pv[x+y*d] );
										if (ang>1.0708 && ang<2.0708)
											rbit[PixIndex(l,b)].SetPixel(x,y);
									}
								}
							}
							break;
				case 3:	for (y=0;y<d;y++) {		// Volle Kugeln 2
								for (x=0;x<d;x++) {
									if (!pv[x+y*d].IsZero()) {
										Real	ang = v.AngleRadTo( pv[x+y*d] );
										if (ang>0.35)	rbit[PixIndex(l,b)].SetPixel(x,y);
									}
								}
							}
							break;
				case 4:	for (y=0;y<d;y++) {		// Halbe Kugeln 2
								for (x=0;x<d;x++) {
									if (!pv[x+y*d].IsZero()) {
										Real	ang = v.AngleRadTo( pv[x+y*d] );
										if ((ang>0.35&&ang<1.27)||(ang>1.87&&ang<2.79))
											rbit[PixIndex(l,b)].SetPixel(x,y);
									}
								}
							}
							break;
				default:	for (y=0;y<d;y++) {
								for (x=0;x<d;x++) {
									if (!pv[x+y*d].IsZero()) {
										rbit[PixIndex(l,b)].SetPixel(x,y);
									}
								}
							}
							break;
				}
			}
		}

		printf( "\b\b\bdone\n" );
	}



//
// RingState-Felder anfordern und initialisieren
// - zu jeder Pixmap existiert dabei in ein Eintrage mit einer
//   Folgepixmap in Feldern zu jeder Himmelsrichtung.
//
	DBG4( UnixTrace, "RingStateData:>%7d Bytes (for %dx%d=%d States)\n",
				 4*vecs_all*sizeof(RingState), vecs_l, vecs_b, vecs_all );

	right	= new RingState[vecs_all];
	left	= new RingState[vecs_all];
	up		= new RingState[vecs_all];
	down	= new RingState[vecs_all];

//
// Stuffer anlegen und initialisieren
//
Stuffer	stuff;
	for (i=0,l=vecs_all;l;l>>=1)	i++;		// Bits in vecs_all zaehlen
	stuff.Init(i,sizeof(RingState));

	if (fp) {
		printf( "reading %s\n", fname );

#ifndef LEFT_RIGHT_OPT
		fread( right,1,stuff.SSize(vecs_all), fp );
		stuff.Expand( right, vecs_all );
		fread( left, 1,stuff.SSize(vecs_all), fp );
		stuff.Expand( left, vecs_all );
#endif
		fread( up,   1,stuff.SSize(vecs_all), fp );
		stuff.Expand( up, vecs_all );
		fread( down, 1,stuff.SSize(vecs_all), fp );
		stuff.Expand( down, vecs_all );
	}
	else {
Real	b_ang = (double)mult*M_PI/vecs_b;	// Rotationswinkel pro Pixel

		printf( "state processing: 00%%" ); fflush(stdout);

		for (l=0;l<vecs_l;l++) {
			printf( "\b\b\b%02d%%", (int)100*l/vecs_l ); fflush(stdout);

			for (int b=0;b<vecs_b;b++) {
				RingState	st = AngVec2St(l,b);				// aktueller Index
				RingState	nst;									// auszurechnender State
				Real	newl, newb;

				Vec3	v( lToDeg(l), bToDeg(b) );

				v.XTurnAngleRad(b_ang).GetPolarRad(&newl,&newb);
				nst = AngRad2St( newl, newb );
				if (st==nst)	DBG2( Loops, "Loop-Warning Up:    %03d/%03d\n",l,b );
				up[st] = nst;


				v.XTurnAngleRad(-b_ang).GetPolarRad(&newl,&newb);
				nst = AngRad2St( newl, newb );
				if (st==nst)	DBG2( Loops, "Loop-Warning Down:  %03d/%03d\n",l,b );
				down[st] = nst;

#ifndef LEFT_RIGHT_OPT
				v.ZTurnAngleRad(-b_ang).GetPolarRad(&newl,&newb);
				nst = AngRad2St( newl, newb );
				if (st==nst)	DBG2( Loops, "Loop-Warning Right: %03d/%03d\n",l,b );
				right[st] = nst;

				v.ZTurnAngleRad(b_ang).GetPolarRad(&newl,&newb);
				nst = AngRad2St( newl, newb );
				if (st==nst)	DBG2( Loops, "Loop-Warning Left:  %03d/%03d\n",l,b );
				left[st] = nst;
#endif
			}
		}

		printf( "\b\b\bdone\n" );

		DBG1( UnixTrace, "Time: %g secs.\n", GetCurrentTime()-start_time );
	}
	
#ifdef LEFT_RIGHT_OPT
	for (l=0;l<vecs_l;l++) {
		for (int b=0;b<vecs_b;b++) {
			RingState	st = AngVec2St(l,b);				// aktueller Index
			right[st] = AngVec2StBnd((l+(vecs_l*(int)sym)-mult)%(vecs_l*(int)sym),b);
			left[st]  = AngVec2StBnd((l+mult)%(vecs_l*(int)sym),b);
		}
	}
#endif

#ifdef DEBUG
	if (debug&ShowRings) {
		//
		// Eine spezielle Pixmap wird erzeugt, die eine Negation der Balldarstellung
		// ist, um in dem Trace-Window einzelne Pixmap's herzuheben.
		//
		nbpix	= XCreatePixmap(dpy,bpix,d+o,d+o,1);
		GC	gc = XCreateGC( dpy, nbpix, 0, 0l );
		XSetFunction(dpy,gc,GXset);
		XFillRectangle(dpy,nbpix,gc,0,0,d+o,d+o);			// alle Pixmap setzen
		XSetFunction(dpy,gc,GXxor);
		XCopyArea(dpy,bpix,nbpix,gc,0,0,d,d,o/2,o/2);	// Ball löschen
		XFreeGC(dpy,gc);
	}
#endif

	if (fp) {
		fclose(fp);
		fp = 0;
	}
	else {
		fp = fopen( fname, "w" );
	}
//
// Bitmaps in Pixmaps wandeln und interne Strukturen freigeben
//
	rpix = new Pixmap[rpixs_all];
	for (i=0;i<rpixs_all;i++) {
		rpix[i] = rbit[i].Create();
		if (fp) rbit[i].Write( fp );
	}
	delete [] rbit;
	delete [] pv;

	if (fp) {
		printf( "writing %s\n", fname );

#ifndef LEFT_RIGHT_OPT
		stuff.Shrink( right, vecs_all );
		fwrite( right,1,stuff.SSize(vecs_all), fp );
		stuff.Expand( right, vecs_all );
		stuff.Shrink( left, vecs_all );
		fwrite( left, 1,stuff.SSize(vecs_all), fp );
		stuff.Expand( left, vecs_all );
#endif
		stuff.Shrink( up, vecs_all );
		fwrite( up,   1,stuff.SSize(vecs_all), fp );
		stuff.Expand( up, vecs_all );
		stuff.Shrink( down, vecs_all );
		fwrite( down, 1,stuff.SSize(vecs_all), fp );
		stuff.Expand( down, vecs_all );
		fclose(fp);
	}
}



void HalfBallMover::CreateRingWindow() {
	tw=CreateWindow( "Ringverteilung", rpixs_l*(d+o)+1 , rpixs_b*(d+o)+1 );
	for (int l=0;l<rpixs_l;l++) {
		for (int b=0;b<rpixs_b;b++) {
			XCopyPlane(dpy,bpix,tw,gc_ballwhite,0,0,d,d,o/2+l*(d+o),o/2+b*(d+o),1);
			XCopyPlane(dpy,lpix[(l*lpixs_x/rpixs_l)+(b*lpixs_y/rpixs_b)*lpixs_x],tw,gc_lay2,0,0,d,d,o/2+l*(d+o),o/2+b*(d+o),1);
			XCopyPlane(dpy,rpix[PixIndex(rpixs_l-1-l,b)],tw,gc_ball[7],0,0,d,d,o/2+l*(d+o),o/2+b*(d+o),1);
		}
	}
#ifdef DEBUG
	if (debug&xwd) {
		char command[200];
		XSync(dpy,0);
		sprintf( command, "/usr/bin/X11/xwd -id %ld -frame > XWD/rings", tw );
		system(command);
	}
#endif
}


#define _INFO

void HalfBallMover::CreateTurnWindow() {
Window	win;

	win=CreateWindow( "Senkrecht-Rotation", rpixs_b*(d+o)+1,2*rpixs_b*(d+o)+1 );
	for (int b=0;b<rpixs_b;b++) {
#ifdef INFO
		printf( "Row %d: angle: %f\n", b, (double)bToDeg(b*mult+mult2) );
#endif
		RingState	st = AngPix2St(0,b);
		for (int y=0;y<2*rpixs_b;y++) {
#ifdef INFO
		int lp,bp;
		int lv,bv;
	//
	// Ausgabe der Tabellenerte
	//
		St2AngVec(st,&lv,&bv);
		St2AngPix(st,&lp,&bp);
		printf( "  Line %2d: Vec2: %03d/%03d, Pixmap: %02d/%02d",
			y, lv, bv, lp, bp );

	//
	// Berechnung der direkten Werte zum Vergleich
	//
		Real	b_ang=(double)mult*M_PI/vecs_b;   // Rotationswinkel pro Pixel
		Vec3	v( lToDeg(mult2), bToDeg(b*mult+mult2) );
		Real	newl, newb;
		v.XTurnAngleRad((y*b_ang)).GetPolarRad(&newl,&newb);
		RingState nst = AngRad2St( newl, newb );
	// Ausgabe
		St2AngVec(nst,&lv,&bv);
		St2AngPix(nst,&lp,&bp);
		printf( " <=> Vec2: %03d/%03d, Pixmap: %02d/%02d\n",
			lv, bv, lp, bp );

		if (((b^y)&1)) {
			XCopyPlane(dpy,bpix,win,gc_ballwhite,0,0,d,d,o/2+b*(d+o),o/2+y*(d+o),1);
			XCopyPlane(dpy,lpix[(b*lpixs_x/rpixs_b)+(y/2*lpixs_y/rpixs_b)*lpixs_x],win,gc_lay2,0,0,d,d,o/2+b*(d+o),o/2+y*(d+o),1);
			XCopyPlane(dpy,rpix[PixIndex(nst)],win,gc_ball[7],0,0,d,d,o/2+b*(d+o),o/2+y*(d+o),1);
		}
		else
#endif
		{
			XCopyPlane(dpy,bpix,win,gc_ballwhite,0,0,d,d,o/2+b*(d+o),o/2+y*(d+o),1);
			XCopyPlane(dpy,lpix[(b*lpixs_x/rpixs_b)+(y/2*lpixs_y/rpixs_b)*lpixs_x],win,gc_lay2,0,0,d,d,o/2+b*(d+o),o/2+y*(d+o),1);
			XCopyPlane(dpy,rpix[PixIndex(st)],win,gc_ball[8],0,0,d,d,o/2+b*(d+o),o/2+y*(d+o),1);
		}
			st = Turn(st,0,1);
		}
	}
#ifdef DEBUG
	if (debug&xwd) {
		char command[200];
		XSync(dpy,0);
		sprintf( command, "/usr/bin/X11/xwd -id %ld -frame > XWD/turns", win );
		system(command);
	}
#endif
}



#if (1)

#define	LINE(DY,YDEC,YADDOP,YTAB,DX,XLESS,XTAB) \
	int	dy_c = DY; \
	int	c    = 0; \
	while(DY) { \
		st = YTAB [st]; \
		DY YDEC; \
		c -= DX;	\
		if (c XLESS 0) {	\
			c YADDOP dy_c;	\
			st = XTAB [st];	\
		};	\
	}


RingState HalfBallMover::Turn( RingState st, int dx, int dy ) {

	if (dx>0) {
		if (dy>0) {
			if (dy>dx)	{ LINE(dy,--,+=,up,   dx,<,right);	}
			else			{ LINE(dx,--,+=,right,dy,<,up);		}
		}
		else {				// dy negativ => alle DY-Parameter umkehren
			if (-dy>dx)	{ LINE(dy,++,-=,down, dx,<,right);	}
			else			{ LINE(dx,--,+=,right,dy,>,down);	}
		}
	}
	else {				// dx negativ => alle DX-Parameter umkehren
		if (dy>0) {
			if (dy>-dx)	{ LINE(dy,--,+=,up,   dx,>,left);	}
			else			{ LINE(dx,++,-=,left ,dy,<,up);		}
		}
		else {				// dy negativ => alle DY-Parameter umkehren
			if (-dy>-dx){ LINE(dy,++,-=,down, dx,>,left);	}
			else			{ LINE(dx,++,-=,left ,dy,>,down);	}
		}
	}

	return st;
}

#else
RingState HalfBallMover::Turn( RingState st, int dx, int dy ) {
	while( dx && dy ) {
		if (dx>0)				{	dx--; st = right[st];	}
		else if (dx<0)			{	dx++; st = left[st]; }
		if (dy>0)				{	dy--;	st = up[st];	}
		else if (dy<0)			{	dy++;	st = down[st];	}
	}
	if (dx>0)				{ while(dx--)		st = right[st]; }
	else if (dx<0)			{ while(dx++)		st = left[st]; }
	if (dy>0)				{ while(dy--)		st = up[st]; }
	else if (dy<0)			{ while(dy++)		st = down[st]; }
	return st;
}
#endif


#ifdef DEBUG
void HalfBallMover::ShowDebugRing(RingState st, int col_x) {
	int	l,b;
	St2AngPix(st,&l,&b);
	XCopyPlane(dpy,nbpix,tw,gc_ballwhite,  0,0,d+o,d+o,(rpixs_l-1-l)*(d+o),b*(d+o),1);
	XCopyPlane(dpy,nbpix,tw,gc_ball[col_x],0,0,d+o,d+o,(rpixs_l-1-l)*(d+o),b*(d+o),1);
}
#endif

void HalfBallMover::RollBallAt( int x, int y, RingState st, int col_x )
{
#ifndef NODRAW
	XFillRectangle(dpy,help,gc_bclear,0,0,d,d);
	XCopyPlane(dpy,bpix,help,gc_ballwhite,0,0,d,d,0,0,1);
	XCopyPlane(dpy,GetShadowMap(x,y),help,gc_lay2,0,0,d,d,0,0,1);
	XCopyPlane(dpy,rpix[PixIndex(st)],help,gc_ball[col_x],0,0,d,d,0,0,1);
	XCopyArea(dpy,help,win,gc_bxor,0,0,d,d,x-r, y-r );

#ifdef DEBUG
	if ((tw)&&(debug&ShowRings))		ShowDebugRing(st,col_x);
#endif
#endif
}


void HalfBallMover::RollBallOnScreen( int oldx, int oldy, RingState ost,
				int newx, int newy, RingState *nst, int col_x ) {

int	dx = newx-oldx;
int	dy = newy-oldy;

		*nst = Turn( ost, dx, dy );

int absx = (dx>0)?dx:-dx;
int absy = (dy>0)?dy:-dy;

#ifndef NODRAW
	if ( (absx<d_help-d) && (absy<d_help-d) ) {
		int	width = d+absx;		// tatsächlicher Ausschnittgröße der Pixmap
		int	height= d+absy;
		int	ox = (dx>0)?0:absx;	// relativer Abstand alte Position
		int	oy = (dy>0)?0:absy;
		int	nx = (dx>0)?absx:0;	// relativer Abstand neue Position
		int	ny = (dy>0)?absy:0;

		XFillRectangle(dpy,help,gc_bclear,0,0,width,height);

		XCopyPlane(dpy,bpix,help,gc_ballwhite,0,0,d,d,ox,oy,1);
		XCopyPlane(dpy,GetShadowMap(oldx,oldy),help,gc_lay2,0,0,d,d,ox,oy,1);
		XCopyPlane(dpy,rpix[PixIndex(ost)],help,gc_ball[col_x],0,0,d,d,ox,oy,1);

		XCopyPlane(dpy,bpix,help,gc_ballwhite,0,0,d,d,nx,ny,1);
		XCopyPlane(dpy,GetShadowMap(newx,newy),help,gc_lay2,0,0,d,d,nx,ny,1);
		XCopyPlane(dpy,rpix[PixIndex(*nst)],help,gc_ball[col_x],0,0,d,d,nx,ny,1);

		XCopyArea(dpy,help,win,gc_bxor,0,0,width,height,oldx-ox-r, oldy-oy-r );

#ifdef DEBUG
		if ((tw)&&(debug&ShowRings)) {
			ShowDebugRing(ost,col_x);
			ShowDebugRing(*nst,col_x);
		}
#endif

	}
	else {
		RollBallAt(oldx,oldy,ost,col_x);
		RollBallAt(newx,newy,*nst,col_x);
	}
#endif

#ifdef STATISTICS
	moves++;
#endif
}


// ---------------------------------------------------------------------------
//                          LoEnhancedHalfBallMover
// ---------------------------------------------------------------------------


LoEnhancedHalfBallMover::LoEnhancedHalfBallMover( const Real &r, int mode_in )
: HalfBallMover( r, mode_in ) {
	lbpix = 0;
	boffset = 0;
	lb_dist = boffset+d;
}

LoEnhancedHalfBallMover::~LoEnhancedHalfBallMover() {
	if (lbpix) {
		PixmapCache::pcache.Unlock( dpy, lbpix );
		lbpix = 0;
	}
}

void LoEnhancedHalfBallMover::Init() {
	HalfBallMover::Init();

// create initialisation pixmap for all possible shades, which
// can directly be used as presets in the RollBall() functions
// A large Pixmap is created that contains all neccessary background-presets
// in a certain distance, so that a direct copy can be done, which presets
// the whole background help-pixmap.

	lbpix = PixmapCache::pcache.Lock( d, ENHANCED_LBPIX_ID );
	if (!lbpix ) {

// create map and clear it

		DBG1( UnixTrace, "EnhancedData: >%d bytes\n",
					(lb_dist*lpixs_x+d-1)*(lb_dist*lpixs_y+d-1) );
		lbpix = XCreatePixmap(dpy,win,
					lb_dist*lpixs_x+d-1,lb_dist*lpixs_y+d-1,DefaultDepth(dpy,0));
		XFillRectangle(dpy,lbpix,gc_bclear, 0,0,
					lb_dist*lpixs_x+d-1,lb_dist*lpixs_y+d-1);

// copy the balls & shades into the map

		for (int x=0;x<lpixs_x;x++) {
			for (int y=0;y<lpixs_y;y++) {
				XCopyPlane(dpy,bpix,   lbpix,gc_ballwhite,
										0,0,d,d,boffset+x*lb_dist,boffset+y*lb_dist,1);
				XCopyPlane(dpy,lpix[x+y*lpixs_x],lbpix,gc_lay2,    
										0,0,d,d,boffset+x*lb_dist,boffset+y*lb_dist,1);
			}
		}
		PixmapCache::pcache.InsertAndLock(dpy,lbpix,d,ENHANCED_LBPIX_ID);
	}
}


void LoEnhancedHalfBallMover::RollBallAt( int x, int y, RingState st, int col_x )
{
#ifdef DEBUG
	if (enhanced_mover<1) {
			HalfBallMover::RollBallAt(x,y,st,col_x);
			return;
	}
#endif

#ifndef NODRAW
int	xp,yp;
	WC2Index(x,y,&xp,&yp);
	XCopyArea(dpy,lbpix,help,gc_default,boffset+xp*lb_dist,boffset+yp*lb_dist,d,d,0,0);
	XCopyPlane(dpy,rpix[PixIndex(st)],help,gc_ball[col_x],0,0,d,d,0,0,1);
	XCopyArea(dpy,help,win,gc_bxor,0,0,d,d,x-r, y-r );

#ifdef DEBUG
	if ((tw)&&(debug&ShowRings))		ShowDebugRing(st,col_x);
#endif
#endif
}


void LoEnhancedHalfBallMover::RollBallOnScreen( int oldx, int oldy, RingState ost,
				int newx, int newy, RingState *nst, int col_x ) {
#ifdef DEBUG
	if (enhanced_mover<1) {
			HalfBallMover::RollBallOnScreen(oldx,oldy,ost,newx,newy,nst,col_x);
			return;
	}
#endif

int	dx = newx-oldx;
int	dy = newy-oldy;

		*nst = Turn( ost, dx, dy );

int absx = (dx>0)?dx:-dx;
int absy = (dy>0)?dy:-dy;

#ifndef NODRAW
	if ( (absx<d_help-d) && (absy<d_help-d) ) {
		int	width = d+absx;		// tatsächlicher Ausschnittgröße der Pixmap
		int	height= d+absy;
		int	ox = (dx>0)?0:absx;	// relativer Abstand alte Position
		int	oy = (dy>0)?0:absy;
		int	nx = (dx>0)?absx:0;	// relativer Abstand neue Position
		int	ny = (dy>0)?absy:0;

		int	xp,yp;
		WC2Index(oldx,oldy,&xp,&yp);
		XFillRectangle(dpy,help,gc_bclear,0,0,width,height);

		XCopyArea(dpy,lbpix,help,gc_bxor,
					boffset-ox+xp*lb_dist,boffset-oy+yp*lb_dist,d_help,d_help,0,0);
		XCopyPlane(dpy,rpix[PixIndex(ost)],help,gc_ball[col_x],0,0,d,d,ox,oy,1);

		WC2Index(newx,newy,&xp,&yp);
		XCopyArea(dpy,lbpix,help,gc_bxor,
					boffset+xp*lb_dist,boffset+yp*lb_dist,d,d,nx,ny);
		XCopyPlane(dpy,rpix[PixIndex(*nst)],help,gc_ball[col_x],0,0,d,d,nx,ny,1);

		XCopyArea(dpy,help,win,gc_bxor,0,0,width,height,oldx-ox-r, oldy-oy-r );

#ifdef DEBUG
		if ((tw)&&(debug&ShowRings)) {
			ShowDebugRing(ost,col_x);
			ShowDebugRing(*nst,col_x);
		}
#endif
	}
	else {
		RollBallAt(oldx,oldy,ost,col_x);
		RollBallAt(newx,newy,*nst,col_x);
	}
#endif

#ifdef STATISTICS
	moves++;
#endif
}

// ---------------------------------------------------------------------------
//                          HiEnhancedHalfBallMover
// ---------------------------------------------------------------------------


HiEnhancedHalfBallMover::HiEnhancedHalfBallMover( const Real &r, int mode_in )
: LoEnhancedHalfBallMover( r, mode_in ) {
	boffset = d-1;
	lb_dist = 2*d-1;
}

HiEnhancedHalfBallMover::~HiEnhancedHalfBallMover() {
}

void HiEnhancedHalfBallMover::RollBallOnScreen( int oldx, int oldy, RingState ost,
				int newx, int newy, RingState *nst, int col_x ) {
#ifdef DEBUG
	if (enhanced_mover<2) {
			LoEnhancedHalfBallMover::RollBallOnScreen(oldx,oldy,ost,newx,newy,nst,col_x);
			return;
	}
#endif

int	dx = newx-oldx;
int	dy = newy-oldy;

		*nst = Turn( ost, dx, dy );

int absx = (dx>0)?dx:-dx;
int absy = (dy>0)?dy:-dy;

#ifndef NODRAW
	if ( (absx<d_help-d) && (absy<d_help-d) ) {
		int	width = d+absx;		// tatsächlicher Ausschnittgröße der Pixmap
		int	height= d+absy;
		int	ox = (dx>0)?0:absx;	// relativer Abstand alte Position
		int	oy = (dy>0)?0:absy;
		int	nx = (dx>0)?absx:0;	// relativer Abstand neue Position
		int	ny = (dy>0)?absy:0;

		int	xp,yp;
		WC2Index(oldx,oldy,&xp,&yp);
		XCopyArea(dpy,lbpix,help,gc_default,
					boffset-ox+xp*lb_dist,boffset-oy+yp*lb_dist,d_help,d_help,0,0);
		XCopyPlane(dpy,rpix[PixIndex(ost)],help,gc_ball[col_x],0,0,d,d,ox,oy,1);

		WC2Index(newx,newy,&xp,&yp);
		XCopyArea(dpy,lbpix,help,gc_bxor,
					boffset+xp*lb_dist,boffset+yp*lb_dist,d,d,nx,ny);
		XCopyPlane(dpy,rpix[PixIndex(*nst)],help,gc_ball[col_x],0,0,d,d,nx,ny,1);

		XCopyArea(dpy,help,win,gc_bxor,0,0,width,height,oldx-ox-r, oldy-oy-r );

#ifdef DEBUG
		if ((tw)&&(debug&ShowRings)) {
			ShowDebugRing(ost,col_x);
			ShowDebugRing(*nst,col_x);
		}
#endif
	}
	else {
		RollBallAt(oldx,oldy,ost,col_x);
		RollBallAt(newx,newy,*nst,col_x);
	}
#endif

#ifdef STATISTICS
	moves++;
#endif
}

