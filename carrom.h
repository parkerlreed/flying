#ifndef _carrom_h
#define _carrom_h

#ifndef _real_h
#	include "real.h"
#endif
#ifndef _vec2_h
#	include "vec2.h"
#endif
#ifndef _game_h
#	include "game.h"
#endif

class StaticArc;
class StaticBall;
class Wall;
class Ball;
class BallMover;		// forward

class Carrom : public Game {
	public:
		static Real TableWidth;
		static Real TableHeight;

		Carrom(double wx=TableWidth, double wy=TableHeight);
		virtual ~Carrom();

		virtual const Real & GetPresetA() const;
		virtual const Real & GetPresetHaft() const;
		virtual const Real & GetSlowGranularity() const;

		virtual const Real & GetNormalBallSize() const;

		virtual void InitPlayground();
		virtual void DrawBackground() const;

		virtual const Real AreaOffX() const;
		virtual const Real AreaOffY() const;
		virtual const Real AreaWidth() const;
		virtual const Real AreaHeight() const;

		virtual const Real TAreaOffX() const;
		virtual const Real TAreaOffY() const;
		virtual const Real TAreaWidth() const;
		virtual const Real TAreaHeight() const;

		virtual const Real& GetChargeGranularity()	const;
		virtual const Real& GetChargeSpeed()			const;
		virtual const Real& GetMaxCharge()				const;
		virtual const Real& GetShootTime()				const;

		virtual void AllBallsStopped();
		virtual void InPocket( Ball *b );
	private:
				  void ResetToCenter(Ball *b);
	public:
		virtual int  IsSelectable(Ball *b);

	public:
		static Real Offset;

		static Real PresetA;
		static Real PresetHaft;
		static Real SlowGranularity;

		static Real FrameOffset;
		static Real ToPocket;
		static Real Cushion;
		static Real	PocketRadius;
		static Real StoneRadius;
		static Real StrikerRadius;
		static Real StoneWeight;
		static Real StrikerWeight;

		static Real PocketFrame;
		static Real PocketWidth;

		static Real	ChargeGranularity;	// Länge eines Ladeschritts
		static Real ChargeSpeed;			// Einheiten pro Sekunde (Aufladen)
		static Real	MaxCharge;				// maximale Ladung
		static Real ShootTime;				// Zeit der Entladeanimation

	protected:
		void InitArea( double width, double height );
		void Setup(double px, double py);			// Steine verteilen

		Real area_off_x;
		Real area_off_y;
		Real area_width;
		Real area_height;

		BallMover	*mstone;
		BallMover	*mstriker;

		ColorId		table_col;

		ColorId		table_red;
		ColorId		table_green;
		ColorId		table_black;

		ColorId		pocket_col;

	protected:
		StaticArc	*p[4];			// Taschenobjekte
		StaticBall	*e[4][2];		// Ecken am Taschenrand
		Wall			*w[4];			// Wände zwischen Ecken am Taschenrand

		Ball			*white[9];		// Spielsteine
		Ball			*black[9];
		Ball			*queen;
		Ball			*striker;

		int			striker_in_pocket;
		int			queen_in_pocket;
		int			whites_in_pocket;
		int			blacks_in_pocket;

		Vec2		mid;				// Mittelpunktvektor
		Vec2		base[4][2];		// Begrenzung der Grundlinien

		ColorId		white_col;
		ColorId		black_col;
		ColorId		striker_col;
		ColorId		queen_col;


	private:
		Carrom(const Carrom &)	{}	// to overcome a BUG in 2.6.0 of g++
		Carrom &operator=( const Carrom& /*obj*/ )	{ return *this; }
};

class CarromDemo : public Carrom {
	public:
		CarromDemo( double s=40.0 ) : shot_speed(s)	{}
		virtual ~CarromDemo();

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

