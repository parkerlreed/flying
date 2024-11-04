#ifndef _game_h
#define _game_h

#ifndef _vec2_h
#	include "vec2.h"
#endif

#ifndef _keeper_h
#	include "keeper.h"		// for inlines
#endif

class Ball;
class XWall;
class YWall;

class Game {
	public:
		Game(double wx=100., double wy=80.);
		virtual ~Game();

		virtual const Real & GetPresetA()				const;
		virtual const Real & GetPresetHaft()			const;
		virtual const Real & GetSlowGranularity()		const;
		virtual const Real & GetNormalBallSize() 		const;

		virtual const Real& GetChargeGranularity()	const;
		virtual const Real& GetChargeSpeed()			const;
		virtual const Real& GetMaxCharge()				const;
		virtual const Real& GetShootTime()				const;

		virtual const Real AreaOffX() const   = 0;
		virtual const Real AreaOffY() const   = 0;
		virtual const Real AreaWidth() const  = 0;
		virtual const Real AreaHeight() const = 0;
		Vec2 Edge( int i ) const;
		Vec2 Mid( int i ) const;

		virtual void InitPlayground();
		virtual void DrawBackground() const;

		void ExposeRedraw() const;
		void ResizeRedraw() const;

		void ShowInfo( char *str ) const;
		void ShowInfoIf( char *str ) const {
#			ifdef DEBUG
				if (debug&GameState)
#			endif
						ShowInfo(str);
		}

		virtual void ResetGame();

		virtual void ShootBall( Ball *b );
		virtual void PressedBall( Ball *b );
		virtual void TouchedBall( Ball *b );
		virtual void HitWall( Ball *b );
		virtual void StartBall( Ball *b );
		virtual void StopBall( Ball *b );
		virtual void InPocket( Ball *b );
		virtual void AllBallsStopped();

		virtual int  IsSelectable( Ball *B );

	// Durchreichen der Keeper-Funktionalitaet
		void TakeOffBoard(Ball *b)		{ keeper->TakeOffBoard(b); }

	protected:
		ColorId	floor;

		Keeper	*keeper;

	private:
		int	running_balls;
friend class PBallNorm;

	public:
		static Real PresetA;
		static Real PresetHaft;
		static Real SlowGranularity;
		static Real NormalBallSize;

		static Real	ChargeGranularity;	// Länge eines Ladeschritts
		static Real ChargeSpeed;			// Einheiten pro Sekunde (Aufladen)
		static Real	MaxCharge;				// maximale Ladung
		static Real ShootTime;				// Zeit der Entladeanimation


		XWall	*x1, *x2;
		YWall	*y1, *y2;

#ifdef __TURBOC__
		void *buffer;
#endif
};

extern Game	*g;

#endif
