#ifndef _graph_h
#	include "graph.h"
#endif
//
// Erweiterung der PBall-Klasse um X-Spezifika
//
class PBallNorm : public PBallTop {
	public:
		PBallNorm( PBallType mode=BillardQueue, const char *display=0l );
		~PBallNorm();
		static void WaitForEvents();

	protected:
		virtual void Update();

		virtual void Warp( const Vec2 &dest );
		virtual void RedrawPointer();						// Mausanzeige
		virtual void SetPointer( int x, int y );		// Mausanzeige

		static double	next_frame;
		static double	last_frame;
		static int		instance_count;
		static int		pwidth, pheight;
		static Pixmap	pmap;
		static GC		gc_bit;
		static void SetMinSize(int width, int height);

		virtual void StartQueue(const Vec2 &end, const Vec2 &s1, const Vec2 &s2);
		virtual void MoveQueue(const Vec2 &end, const Vec2 &s1, const Vec2 &s2);
		virtual void EndQueue();
		virtual void DrawQueue();

	protected:
		struct _XDisplay	*rem_dpy;
		struct _XDisplay	*warp_dpy;	// points to the original dpy for the case
												// the rem_dpy is at the same screen ...
												// This prevents from locks during server grabs
		int					scr;
		Real					w2n_x, w2n_y;
};

//
// as an enhancement to the first derived class PBallNorm, the following
// class adds interpixel-movement to the cursor, which means, that the
// queue can be fine-adjusted be pressing the middle pointer button
//

class PBall : public PBallNorm {
	public:
		PBall( PBallType mode=BillardQueue, const char *display=0L );
		virtual ~PBall();


	protected:
		virtual void Update();

		virtual void Warp( const Vec2 &dest );
		virtual void SetPointer( int x, int y );		// Mausanzeige

		void StartZooming(int x,int y);
		void StopZooming();

		Real					off_x, off_y;					// "interpixel"-offset
		int					zfact;							// zoom-factor
		int					zoom_x, zoom_y;				// startposition of zooming
		int					zoom;								// Flag, if in zoom-mode
		int					mid_x, mid_y;					// center position of screen
		int					ozoom;
		Real					ooff_x, ooff_y;				// last position
};

