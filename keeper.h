#ifndef _keeper_h
#define _keeper_h

#ifndef _real_h
#	include "real.h"
#endif

class Ball;
class Wall;


class Keeper {
	public:
		Keeper(const Real &size_int, const Real &frame_in, ColorId frame_col, ColorId table_col);
		virtual ~Keeper();

		virtual void Draw() = 0;
		virtual void GameWasReset();

		void TakeOffBoard(Ball *b);

		static int IsOffBoard(Ball *b);
		static Ball *GetBack(Ball *b);

	protected:
		virtual void PlaceOffBoard(Ball *b) = 0;

		int		off_count;
		ColorId	frame_col;
		ColorId	table_col;
		Real		size;			// Hoehe oder Breite
		Real		frame;		// Breite des Rahmens
};


#define	UPPER_FLAG		0x01
#define	CLOSE_LEFT		0x02
#define	CLOSE_RIGHT		0x04

class LineKeeper : public Keeper {
	public:
		LineKeeper(const Real &height, const Real &frame, ColorId frame_col, ColorId table_col=0, int mode=CLOSE_LEFT);
		virtual ~LineKeeper();
		virtual void Draw();

	protected:
		virtual void PlaceOffBoard(Ball *b);
		int mode;

		Wall	*wall1,*wall2;
};


#define	LEFT_FLAG		0x01
#define	CLOSE_TOP		0x02
#define	CLOSE_BOT		0x04

class StackKeeper : public Keeper {
	public:
		StackKeeper(const Real &width, const Real &frame, ColorId frame_col, ColorId table_col=0, int mode=CLOSE_BOT);
		virtual ~StackKeeper();
		virtual void Draw();

	protected:
		virtual void PlaceOffBoard(Ball *b);
		int mode;

		Wall	*wall1, *wall2;
};

#endif
