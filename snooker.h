#ifndef _snooker_h
#define _snooker_h

#ifndef _vec2_h
#	include "vec2.h"
#endif
#ifndef _pool_h
#	include "pool.h"
#endif

class Snooker : public Pool {
	public:
		static Real TableWidth;
		static Real TableHeight;
		static Real BallRadius;

		Snooker(double dx=TableWidth, double dy=TableHeight);
		virtual ~Snooker();

		virtual void InitPlayground();
		virtual void DrawBackground() const;
		virtual void ResetGame();
		virtual const Real & GetNormalBallSize() const;

		ColorId		red_col;
		ColorId		cols[6];

		virtual void InPocket( Ball *b );
		virtual void AllBallsStopped();
		virtual int  IsSelectable( Ball *b );

	protected:

		void SetupBalls();
		void Triangle( double x, double y );
		void Triangle( const Vec2 &v )	{ Triangle(v.X(),v.Y()); }

		Ball	*red[15];
		Ball	*colored[6];

		Vec2	reddefs[15];	// Positionen der roten Kugeln
		Vec2	defs[6];			// Default-Positionen der Farben

		int	reds_in_pocket;
		int	color_in_pocket;
};

class SnookerDemo : public Snooker {
	public:
		SnookerDemo( double s=100.0 ) : shot_speed(s)	{}
		virtual ~SnookerDemo();

		virtual const Real &GetPresetA() const;
		virtual const Real &GetSlowGranularity() const;

		virtual void InitPlayground();
		virtual void DrawBackground() const;

	private:
		Real			shot_speed;

	public:
		static Real	PresetA;
		static Real	SlowGranularity;
};

#endif
