#ifndef _billard_h
#define _billard_h

#ifndef _real_h
#	include "real.h"
#endif
#ifndef _game_h
#	include "game.h"
#endif

class ShadedBallMover;		// forward
class Wall;

class Billard : public Game {
	public:
		Billard(double wx, double wy);
		virtual ~Billard();

		virtual const Real & GetPresetA() const;
		virtual const Real & GetPresetHaft() const;
		virtual const Real & GetSlowGranularity() const;

		virtual const Real AreaOffX() const;
		virtual const Real AreaOffY() const;
		virtual const Real AreaWidth() const;
		virtual const Real AreaHeight() const;

		virtual const Real PAreaOffX() const;
		virtual const Real PAreaOffY() const;
		virtual const Real PAreaWidth() const;
		virtual const Real PAreaHeight() const;

		virtual const Real TAreaOffX() const;
		virtual const Real TAreaOffY() const;
		virtual const Real TAreaWidth() const;
		virtual const Real TAreaHeight() const;

		virtual void InitPlayground();
		virtual void DrawBackground() const;

		void InitTable();
		virtual int SelectTable( int col );

	public:
		static Real PresetA;
		static Real PresetHaft;
		static Real SlowGranularity;
		static Real FrameOffset;
		static Real InnerCushion;
		static Real OuterCushion;

	protected:
		void InitArea( double width, double height );
		Real area_off_x;
		Real area_off_y;
		Real area_width;
		Real area_height;

		ShadedBallMover	*m;

		ColorId		table_col;
		ColorId		outer_cushion_col;
		ColorId		inner_cushion_col;
		ColorId		marker_col;

		ColorId		table_line_col;
		ColorId		table_light_col;
		ColorId		table_dark_col;

		void	DrawMarker( const Real &midx, const Real &midy, const Real &ox, const Real &oy ) const;

	private:
		Wall			*w[4];

friend class Konfi;
};

#endif
