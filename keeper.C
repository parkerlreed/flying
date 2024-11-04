#ifndef _global_h
#	include "global.h"
#endif

#ifndef _ball_h
#	include "ball.h"
#endif
#ifndef _wall_h
#	include "wall.h"
#endif
#ifndef _keeper_h
#	include "keeper.h"
#endif
#ifndef _game_h
#	include "game.h"
#endif
#ifndef _graph_h
#	include "graph.h"
#endif

Keeper::Keeper(const Real &size_in, const Real &frame_in, ColorId frame_col_in, ColorId table_col_in) {
	size  = size_in;
	frame = frame_in;
	frame_col = frame_col_in;
	table_col = table_col_in;
	off_count = 0;
}

Keeper::~Keeper() {}

void Keeper::TakeOffBoard(Ball *b) {
	b->Draw();		// Ball an aktueller Position löschen
	b->GoUnlocked();
	g->InPocket(b);

	PlaceOffBoard(b);

	off_count++;
	b->Draw();		// Ball an neuer Position setzen
}

int Keeper::IsOffBoard(Ball * /*b*/) {
	return 0;
}

Ball *Keeper::GetBack(Ball *b) {
	return b;
}

void Keeper::GameWasReset() {
	off_count = 0;
}

///////////////////////////////////////////////////////////

extern double	world_x, world_y;

//
// Klasse LinePocket -  Objekte werden in Zeile am unteren Bildrand geworfen
//

LineKeeper::LineKeeper(const Real &height, const Real &frame, ColorId frame_col, ColorId table_col, int mode_in) : Keeper(height,frame,frame_col, table_col)
{
Real	ypos=(mode_in&UPPER_FLAG)?0.0:(double)(MaxY()-size);

	mode = mode_in;
	if (mode&CLOSE_LEFT) {
			wall1 = new Wall(frame,ypos,frame,ypos+size);
	}
	else	wall1 = 0;
	if (mode&CLOSE_RIGHT) {
			wall2 = new Wall(MaxX()-frame,ypos,MaxX()-frame,ypos+size);
	}
	else	wall2 = 0;
}

LineKeeper::~LineKeeper()
{
	if (wall1)	delete wall1;
	if (wall2)	delete wall2;
}

void LineKeeper::Draw() {

	if (mode&UPPER_FLAG) {
		SetBgColor(table_col);
		FillRectangle( 0.0, 0.0, MaxX(), size );
		SetBgColor(frame_col);
		FillRectangle( 0.0, 0.0,  MaxX(), frame );
		FillRectangle( 0.0, size-frame, MaxX(), frame );
		if (mode&CLOSE_LEFT)
				FillRectangle( 0.0, 0.0,  frame, size);
		if (mode&CLOSE_RIGHT)
				FillRectangle( MaxX()-frame, 0.0,  frame, size);
	}
	else {
		SetBgColor(table_col);
		FillRectangle( 0.0, MaxY()-size, MaxX(), size );
		SetBgColor(frame_col);
		FillRectangle( 0.0, MaxY()-size,  MaxX(), frame );
		FillRectangle( 0.0, MaxY()-frame, MaxX(), frame );
		if (mode&CLOSE_LEFT)
				FillRectangle( 0.0, MaxY()-size,  frame, size);
		if (mode&CLOSE_RIGHT)
				FillRectangle( MaxX()-frame, MaxY()-size,  frame, size);
	}
}

void LineKeeper::PlaceOffBoard( Ball *b ) {
Real	to;		// Endpunkt für aktuelle Kugel in der Pocketline
Real	from;		// Startpunkt für aktuelle Kugel in der Pocketline
Real	vx;		// Startgeschwindigkeit, damit Kugel in Punkt to liegenbleibt

	to		= Real((off_count*2+1))*g->GetNormalBallSize()+frame;
//	from	= world_x - Real(40-off_count*2)*g->GetNormalBallSize();
	from  = world_x - 2.0*g->GetNormalBallSize();
	vx		= sqrt( 2.*fabs((to-from)*b->a) );

	b->p = Vec2( from, world_y-size/2. );
	b->SetV( Vec2(-vx,RealZero) );
}

//
// Klasse StackKeeper -  Objekte werden in Zeile am rechten Bildrand geworfen
//

StackKeeper::StackKeeper(const Real &width, const Real &frame, ColorId frame_col, ColorId table_col,int mode_in) : Keeper(width,frame,frame_col,table_col)
{
Real	xpos=(mode_in&LEFT_FLAG)?0.0:(double)(MaxX()-size);

	mode = mode_in;
	if (mode&CLOSE_TOP) {
			wall1 = new Wall(xpos,frame,xpos+size,frame);
	}
	else	wall1 = 0;
	if (mode&CLOSE_BOT) {
			wall2 = new Wall(xpos,MaxY()-frame,xpos+size,MaxY()-frame);
	}
	else	wall2 = 0;
}

StackKeeper::~StackKeeper()
{
	if (wall1)		delete wall1;
	if (wall2)		delete wall2;
}

void StackKeeper::Draw() {
	if (mode&LEFT_FLAG) {
		SetBgColor(table_col);
		FillRectangle( 0.0, 0.0, size,  MaxY() );
		SetBgColor(frame_col);
		FillRectangle( 0.0, 0.0, frame, MaxY() );
		FillRectangle( size-frame,0.0, frame, MaxY() );
		if (mode&CLOSE_TOP)
				FillRectangle( 0.0, 0.0,  size, frame);
		if (mode&CLOSE_BOT)
				FillRectangle( 0.0, MaxY()-frame, size, frame);
	}
	else {
		SetBgColor(table_col);
		FillRectangle( MaxX()-size, 0.0, size,  MaxY() );
		SetBgColor(frame_col);
		FillRectangle( MaxX()-size, 0.0, frame, MaxY() );
		FillRectangle( MaxX()-frame,0.0, frame, MaxY() );
		if (mode&CLOSE_TOP)
				FillRectangle( MaxX()-size, 0.0,  size, frame);
		if (mode&CLOSE_BOT)
				FillRectangle( MaxX()-size, MaxY()-frame, size, frame);
	}
}

void StackKeeper::PlaceOffBoard( Ball *b ) {
Real	to;		// Endpunkt für aktuelle Kugel in der Pocketline
Real	from;		// Startpunkt für aktuelle Kugel in der Pocketline
Real	vy;		// Startgeschwindigkeit, damit Kugel in Punkt to liegenbleibt

	to		= world_y - (Real(off_count*2+1)*g->GetNormalBallSize()+frame);
	from	= Real(40-off_count*2)*g->GetNormalBallSize();
	vy		= sqrt( fabs((to-from)*b->a) );

	b->p = Vec2( world_x-size/2., from );
	b->SetV( Vec2(RealZero,vy) );
}

