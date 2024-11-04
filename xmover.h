#ifndef _xmover_h
#define _xmover_h


// tiny caching class so that multiply classes that need the
// same help-pixmaps can share them

#define	PCACHE_MAX	10
#define	ENHANCED_LBPIX_ID	1

class PixmapCache {
	public:
		PixmapCache();
		~PixmapCache();

		void Unlock(Display *dpy, Pixmap pix);
		Pixmap Lock( int id_in, int size_in );
		void InsertAndLock( Display *dpy, Pixmap pix, int id_in, int size_in );

	static PixmapCache pcache;

	private:
		int		count;
		Pixmap	pmap[PCACHE_MAX];
		int		size[PCACHE_MAX];
		int		id[PCACHE_MAX];
		int		lockcount[PCACHE_MAX];
};

#define	MOVER_MODE	2

class BallMover {
	public:
		BallMover( const Real &r );
		virtual ~BallMover();

		Real GetActRadius()		{ return d/w2n/2; }

		virtual void Init();
		virtual void DrawBallAt( int x, int y, int col_x );
		virtual void MoveBallOnScreen( int oldx, int oldy, int newx, int newy, int col_x );
		virtual Pixmap	GetShadowMap( int x, int y );

#ifdef STATISTICS
		static unsigned long	moves;
#endif

	protected:
		int	r;				// Radius (in Pixeln)
		int	d;				// Durchmesser (gleich 2mal Radius)

		double	max_rad;	// maximaler Radius, damit Pixel zur Kugel gehört
		double	max_rad2;// Quadrat des maximalen Radius
		Pixmap	bpix;		// Bitmap der Balldarstellung
		int		d_help;	// Größe der Hintergrund-Pixmap (mind. (2*d-1))
		Pixmap	help;		// Hintergrund-Pixmap


friend class BallState;
};


class DiscMover : public BallMover {
	public:
		DiscMover( const Real &r );
		virtual ~DiscMover();

		virtual void Init();
		virtual Pixmap	GetShadowMap( int x, int y );

	protected:
		Pixmap	lpix;	// Bitmap des Highlights
};


class ShadedBallMover : public BallMover {
	public:
		ShadedBallMover( const Real &r );
		virtual ~ShadedBallMover();

		virtual void Init();

		void CreateLightWindow();
		virtual Pixmap	GetShadowMap( int x, int y );
		void WC2Index( int x, int y, int *xp, int *yp ) {
			*xp = x/distx;
   		if (*xp<0)         *xp=0;
   		if (*xp>=lpixs_x)  *xp=lpixs_x-1;
			*yp = y/disty;
   		if (*yp<0)         *yp=0;
   		if (*yp>=lpixs_y)  *yp=lpixs_y-1;
		}

	protected:
		Pixmap	*lpix;	// Bitmap-Feld der Highlights

		int	distx, disty;			// Rastergröße für Bereich der Highlights
		int	lpixs_x, lpixs_y, lpixs_all;		// Zahl der Highlights
};

#if (0)
//
// ------------------------------------------------------------------------
//                      mult=6
	Szenario:  rpixs_l=18  => vecs_l=108

	Polarkoordinaten     0    10    20    30                 ^
								|     |     |     |          ltoRad | | Radtol
																	  btoRad | | Radtob
	Vektor-Ids           0     6     12    18                  v
						|.....|.....|.....|.....|.....|           <----St2AngVec
																					/-St2AngPix
	Pixmaps-Ids       |  0  |  1  |  2  |  3  |              <-/

	Polarkoordinaten	  170   180   190	             170   180   190
								|     |     |		 +mult2    |     |     |
	Vektor-Ids     	  102   108   114           102   108   114
						|.....|.....|.....|.....|  =>   |.....|.....|.....|
	Pixmaps-Ids       |  17 |  0  |  1  |          |  17 |  0  |  1  |
//
// ------------------------------------------------------------------------
//
#endif

typedef unsigned int RingState;


class HalfBallMover : public ShadedBallMover {
	public:
		HalfBallMover( const Real &r, int mode_in=1 );
		virtual ~HalfBallMover();
		static HalfBallMover *Create( const Real &r, int mode_in=1 );

		virtual void Init();

		void CreateRingWindow();
		void ShowDebugRing( RingState st, int col_x );
		void CreateTurnWindow();

		Real	lToRad( int l )		{return (l-mult2)*M_PI*2.0/sym/(double)vecs_l;}
		Real	bToRad( int b )		{return (b-mult2)*M_PI/(double)vecs_b;}
		Real	lToDeg( int l )		{return (l-mult2)*360.0/sym/(double)vecs_l;}
		Real	bToDeg( int b )		{return (b-mult2)*180.0/(double)vecs_b;}
		int	RadTol( const Real &l )	{return (rtoi(l*(double)vecs_l/M_PI/2.0*sym)+mult2)%(vecs_l*(int)sym);}
		int	RadTob( const Real &b )	{return (rtoi(b*(double)vecs_b/M_PI)+mult2)%vecs_b;}
		int	DegTol( const Real &l )	{return (rtoi(l*(double)vecs_l/360.0*sym)+mult2)%(vecs_l*(int)sym);}
		int	DegTob( const Real &b )	{return (rtoi(b*(double)vecs_b/180.0)+mult2)%vecs_b;}

#if (0)
		Real	lVecToDeg( const Real &l )	{	return l*360.0/sym/rpixs_l/mult;	}
		Real	bVecToDeg( const Real &b )	{	return b*180.0/rpixs_b/mult;	}

		RingState GetStateForDeg( Real l, Real b ) {
		void StateToAngleDeg( RingState st, Real *l, Real *b ) {
			*l = (st/rpixs_b)*360.0/rpixs_l;
			*b = (st%rpixs_b)*180.0/rpixs_b;
		}
#endif

		RingState AngVec2St( int l, int b ) { return (l*vecs_b+b)%vecs_all;	}
		RingState AngVec2StBnd( int l, int b ) {
				if ( sym>1.0 && l>=vecs_l ) { l-=vecs_l; b=(vecs_b-b+mult)%vecs_b; }
				return AngVec2St( l, b );
		}
			
		RingState AngPix2St( int l, int b ) { return AngVec2St(l*mult+mult2,b*mult+mult2); }
		RingState AngRad2St( const Real &l, const Real &b ) {
				return AngVec2StBnd(RadTol(l),RadTob(b));
		}

		void St2AngVec( RingState st, int *l, int *b ) {
			*l = ((int)st)/vecs_b;
			*b = ((int)st)%vecs_b;
		}
		void St2AngPix( RingState st, int *l, int *b ) {
			St2AngVec(st,l,b);
		/*	*l += mult2; */	*l /= mult;
		/*	*b += mult2; */	*b /= mult;
		}
		int PixIndex( int l, int b )	{				/* l,b in PixSystem */
			return (l*rpixs_b+b)%rpixs_all;
		}
		int PixIndex( RingState st ) {
			int	l,b;
			St2AngPix( st, &l, &b );
			return l*rpixs_b+b;
		}

		RingState Turn( RingState ost, int dx, int dy );


		virtual void RollBallAt( int x, int y, RingState st, int col_x );
		virtual void RollBallOnScreen( int oldx, int oldy, RingState ost,
									  int newx, int newy, RingState *nst, int col_x );


	protected:
		Pixmap		*rpix;		// Bitmap-Feld der Ringe

		RingState	*right;		// Felder zur Verkettung der RingState's
		RingState	*left;
		RingState	*up;
		RingState	*down;

		Real	sym;		// bei Symetrie nicht alle muessen nicht fuer alle
							// Winkel die Kugeln berechnet werden.
		int	mult;		// Zahl der gespeicherten Zwischenschritte
		int	mult2;	// Zwischenschritte / 2
		int	vecs_l, vecs_b, vecs_all;		// Zahl der Hilfs-Vektoren
		int	rpixs_l, rpixs_b, rpixs_all;	// Zahl der Ring-Pixmaps

		int	mode;		// Halbkugel / Vollkugel mit Punkt

		static const int o;		// Offset im Trace-Fenster
		Window		tw;			// Trace-Fenster
		Pixmap		nbpix;

friend class HalfBallState;
#ifdef DEBUG
	friend class TestField;
#endif
};


class LoEnhancedHalfBallMover : public HalfBallMover {
	public:
		LoEnhancedHalfBallMover( const Real &r, int mode_in=1 );
		virtual ~LoEnhancedHalfBallMover();

		virtual void Init();

		virtual void RollBallAt( int x, int y, RingState st, int col_x );
		virtual void RollBallOnScreen( int oldx, int oldy, RingState ost,
									  int newx, int newy, RingState *nst, int col_x );

	protected:
		Pixmap	lbpix;
		int		lb_dist;		// sizeof of helpmaps (mind. 3*d-2 = (d-1)+d+(d-1)
		int		boffset;		// offset of real contents (d-1)
};

class HiEnhancedHalfBallMover : public LoEnhancedHalfBallMover {
	public:
		HiEnhancedHalfBallMover( const Real &r, int mode_in=1 );
		virtual ~HiEnhancedHalfBallMover();

		virtual void RollBallOnScreen( int oldx, int oldy, RingState ost,
									  int newx, int newy, RingState *nst, int col_x );
};

#endif
