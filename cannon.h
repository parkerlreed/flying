#ifndef _cannon_h
#define _cannon_h

#ifndef _real_h
#	include "real.h"
#endif
#ifndef _billard_h
#	include "billard.h"
#endif

class Wall;
class Ball;

class Cannon : public Billard {
	public:
		static Real TableWidth;
		static Real TableHeight;
		static Real BallRadius;

	public:
		Cannon(double dx=TableWidth, double dy=TableHeight);
		virtual ~Cannon();

		virtual const Real &GetNormalBallSize() const;

		virtual void InitPlayground();
		virtual void DrawBackground() const;
		virtual void ResetGame();

	protected:
		ColorId	red_col;
		ColorId	white_col;
		ColorId	yellow_col;

	private:
		Ball *w1, *w2;
		Ball *r;

		Vec2	w1_p, w2_p, r_p;			// Starting-Positions
};

#endif
