#ifndef _hockey_h
#define _hockey_h

#ifndef _vec2_h
#	include "vec2.h"
#endif
#ifndef _game_h
#	include "game.h"
#endif

class BallMover;		// forward
class Goal;
class StaticBall;
class Wall;

class Hockey : public Game {
	public:
		static Real	TableWidth;
		static Real TableHeight;

		Hockey(double wx=TableWidth, double wy=TableHeight);
		virtual ~Hockey();

		virtual const Real & GetPresetA() const;
		virtual const Real & GetPresetHaft() const;
		virtual const Real & GetSlowGranularity() const;

		virtual const Real & GetNormalBallSize() const;

		virtual const Real AreaOffX() const;
		virtual const Real AreaOffY() const;
		virtual const Real AreaWidth() const;
		virtual const Real AreaHeight() const;

		virtual void InitPlayground();
		virtual void DrawBackground() const;

		void InitArea( double wx, double wy );
		void InitTable();

		virtual void InPocket( Ball *b );
		virtual void StopBall( Ball *b );

		static Real PresetA;
		static Real PresetHaft;
		static Real SlowGranularity;

		static Real FrameOffset;
		static Real GoalSize;

		static Real DiscRadius;
		static Real HandRadius;
		static Real DiscWeight;
		static Real HandWeight;

		static Real	GoalFrame;
		static Real GoalHeight;

	protected:
		Real area_off_x;
		Real area_off_y;
		Real area_width;
		Real area_height;

		BallMover	*mdisc;
		BallMover	*mhand;

		ColorId		table_col;
		ColorId		table_dark_col;
		ColorId		red_bg_col;
		ColorId		blue_bg_col;

		ColorId		disc_col;
		ColorId		hand_col;

	protected:
		Vec2		mid;
		Vec2		edge[4];
		Goal			*goal[2];
		StaticBall	*post[2][2];
		Wall			*w[2][3];

		int			disc_in_goal;
		Ball			*disc;
		Ball			*hand1, *hand2;
};

class TestHockey : public Hockey {
	public:
		TestHockey(double speed=100.0);
		virtual ~TestHockey();

		virtual const Real &GetPresetA() const;
		virtual const Real &GetSlowGranularity() const;

		virtual void InitPlayground();

		static Real	PresetA;
		static Real	SlowGranularity;

	private:
		Real	shot_speed;

		Wall	*w[4];
};

#endif
