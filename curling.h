#ifndef _curling_h
#define _curling_h

#ifndef _vec2_h
#	include "vec2.h"
#endif
#ifndef _game_h
#	include "game.h"
#endif

#define	TEAMS		2
#define	CURLS		4

class BallMover;		// forward
class Goal;
class StaticBall;
class Wall;

class Curling : public Game {
	public:
		static Real	TableWidth;
		static Real TableHeight;

		Curling(double wx=TableWidth, double wy=TableHeight);
		virtual ~Curling();

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
		virtual void ResetGame();

		void InitArea( double wx, double wy );
		void InitTable();

		static Real PresetA;
		static Real PresetHaft;
		static Real SlowGranularity;

		static Real FrameOffset;
		static Real GoalSize;

		static Real Offset;
		static Real CurlRadius;
		static Real CurlWeight;

		static Real	KeeperFrame;	
		static Real KeeperHeight;

	protected:
		Real area_off_x;
		Real area_off_y;
		Real area_width;
		Real area_height;

		BallMover	*mcurl;

		ColorId		table_col;
		ColorId		black_bg_col;
		ColorId		red_bg_col;
		ColorId		blue_bg_col;

		ColorId		table_dark_col;
		ColorId		curl_col[TEAMS];

	protected:
		Vec2		mid;
		Vec2		edge[4];
		Goal		*goal;
		Wall		*w[3];

		Ball		*curl[TEAMS][CURLS];
		Vec2		curl_p[TEAMS][CURLS];

	private:
		Curling( const Curling& /*obj*/)						{}
		Curling &operator=( const Curling& /*obj*/ )		{ return *this; }
};

#endif
