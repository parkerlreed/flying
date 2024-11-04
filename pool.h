#ifndef _pool_h
#define _pool_h

#ifndef _real_h
#	include "real.h"
#endif
#ifndef _billard_h
#	include "billard.h"
#endif

class Pocket;
class PocketConnector;

class Pool : public Billard {
	public:
		static Real TableWidth;
		static Real TableHeight;

	public:
		Pool(double dx=TableWidth, double dy=TableHeight);
		virtual ~Pool();

		void InitArea( double wx, double wy );
		void InitTable( double Rad = 0.0 );
		virtual void InitPlayground();
		virtual void DrawBackground() const;

		virtual int SelectTable( int col );

		virtual const Real & GetNormalBallSize() 		const;

		virtual void ResetGame();
		virtual void InPocket( Ball *b );
		virtual void AllBallsStopped();
		virtual int  IsSelectable(Ball *b);

	protected:
		Pocket				*p[6];
		Ball					*cueball;
		Vec2					cuedef;				// Default for cueball
		Vec2					tridef;				// Triangle-Position

		int	cue_in_pocket;

		void Triangle( double x, double y );
						 Real po;					// Offset von KeeperLine

		ColorId		cue_col;
		ColorId		pocket_col;

	private:
		static const int  nPockets;

		PocketConnector	*pcon;


	public:
		static Real Offset;
		static Real PocketRadius;
		static Real BallRadius;
		static Real EdgeRadius;
		static Real EdgeAngle;
		static Real MidAngle;
		static Real	PocketHeight;
		static Real PocketFrame;
};

#endif
