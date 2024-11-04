#ifndef _pool8_h
#define _pool8_h

#ifndef _pool_h
#	include "pool.h"
#endif

class HalfBallMover;

class Pool8 : public Pool {
	public:
		Pool8(double ft);						// Parameter: Tischgröße in Fuß
		virtual ~Pool8();

		void InitTable();
		virtual void InitPlayground();
		virtual void DrawBackground() const;
		virtual void ResetGame();

		virtual void InPocket( Ball *b );
		virtual int  IsSelectable(Ball *b);

	protected:
		virtual void Triangle( const Vec2 &v );
		ColorId	black_col;
		ColorId	full_col;		// für Normal-Mode
		ColorId	half_col;		// für Normal-Mode
		ColorId	ball_col[7];	// für Deluxe-Mode

		HalfBallMover	*mh;

		Ball	*ball[15];		// 0-6 full // 7 black // 8-15 half
		Vec2	ball_p[15];		// default positions for these balls
		int	balls_in_pocket;
};

class Pool9 : public Pool8 {
	public:
		Pool9( double ft ) : Pool8(ft)	{}
		virtual ~Pool9();

	protected:
		virtual void Triangle( const Vec2 &v );
};

class Pool8Demo : public Pool8 {
	public:
		Pool8Demo( double ft, double s=100.0 ) : Pool8(ft), shot_speed(s)	{}
		virtual ~Pool8Demo();

		virtual const Real &GetPresetA() const;
		virtual const Real &GetSlowGranularity() const;

		virtual void InitPlayground();
		virtual void DrawBackground() const;

	protected:
		Real			shot_speed;

	public:
		static Real	PresetA;
		static Real	SlowGranularity;
};

#ifdef DEBUG
class Pool8Test : public Pool8Demo {
	public:
		Pool8Test( double ft, double s=100.0 ) : Pool8Demo(ft,s)		{}
		virtual ~Pool8Test();
		virtual void InitPlayground();
		virtual void DrawBackground() const;

	private:
		class TestField	*field_queue;

friend class TestField;
};
#endif

#endif
