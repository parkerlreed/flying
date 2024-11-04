#ifndef _mover_h
#define _mover_h

//
// Im Model 'mover' wird die Klasse BallMover definiert, die allerdings
// in Wirklichkeit aus den Modulen 'xmover' bzw. 'dosmover' übernommen wird.
// Die darin definierten Klassen müssen daher folgenden Schnittstelle haben
//
// 	BallMover( const Real &r );	// Konstruktor mit Radius
//		~BallMover();						// Destruktor
//
//		void Init();						// Pixmaps anlegen
//		Real GetActRadius();				// aus Pixelgröße zurückgerechneter Radius
//
//		void DrawBallAt( int x, int y, ColorId col );
//												// Ball setzen (und löschen)
//		void MoveBallOnScreen( int oldx, int oldy, int newx, int newy, ColorId col );
//												// Ball bewegen von ... nach ...
//
// Die Funktionen sind virtuell sein, um Ableitungen zuzulassen !!
// Folgende Ableitungen existieren:
//		DiscMover		 - Scheibe (für Carrom)
//		ShadedBallMover - Ball mit Beleuchtungseffekt (auf XWindows)
//
//	Die Initialisierung des BallMovers geschieht aus der Game-Klasse heraus.
//	Die Bewegung der Objekte geschieht aus der BallState-Klasse heraus, welche
//	auch in diesem Modul definiert wird. Sie ist allerdings unabhängig von
//	der Hardware.
//


#ifndef _real_h
#	include "real.h"
#endif

#ifndef _graph_h
#	include "graph.h"
#endif



#ifndef __TURBOC__
#	ifndef _xmover_h
#		include "xmover.h"
#	endif
#else
#	ifndef _dosmover_h
#		include "dosmover.h"
#	endif
#endif

class BallStateTop {
	public:
		BallStateTop();
		virtual ~BallStateTop();

		virtual void Redraw();
		virtual void Show();
		virtual void MoveTo( const Vec2 &new_pos );
		virtual void Hide();
};

class BallState : public BallStateTop {
	public:
		BallState( BallMover *m, ColorId col, const Vec2 &pos );
		virtual ~BallState();

		virtual void Redraw();
		virtual void Show();
		virtual void MoveTo( const Vec2 &new_pos );
		virtual void Hide();

	protected:
		int				vis;
		int				x,y;		// old position
		BallMover		*m;		// Pointer to Mover
		ColorId			col;
		int				col_x;	// Index in Ball-Farbenliste

friend class BallMover;
};


class HalfBallState : public BallState {
	public:
		HalfBallState( BallMover *m, ColorId col, const Vec2 &pos );
		virtual ~HalfBallState();

		virtual void Redraw();
		virtual void Show();
		virtual void MoveTo( const Vec2 &new_pos );
		virtual void Hide();

	protected:
		HalfBallMover	*mh;		// Kopie von m - zum leichteren Zugriff
		RingState		st;		// Pol der Kugel

friend class HalfBallMover;
#ifdef DEBUG
	friend class TestField;
#endif
};


#endif
