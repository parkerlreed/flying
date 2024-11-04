#ifndef _global_h
#	include "global.h"
#endif

#ifndef _game_h
#	include "game.h"
#endif
#ifndef _wall_h
#	include "wall.h"
#endif
#ifndef _ball_h
#	include "ball.h"
#endif
#ifndef _graph_h
#	include "graph.h"
#endif
#ifndef _keeper_h
#	include "keeper.h"
#endif

#ifdef DEBUG
#	define	SHOW_INFO
#endif


Game	*g=0L;	// globaler Zeiger auf das Spiel-Objekt


Game::Game(double wx, double wy) {
	x1=x2=0;
	y1=y2=0;
	keeper = 0;

	InitGraphic(wx,wy);
	floor = 0;
#ifdef TEST_STATIC_COLORS
	AddStatColor( "Red" );
	AddStatColor( "Magenta" );
	AddStatColor( "Blue" );
	AddStatColor( "Cyan" );
	AddStatColor( "Green" );
	AddStatColor( "Yellow" );
	AddStatColor( "Red1" );
	AddStatColor( "Magenta1" );
	AddStatColor( "Blue1" );
	AddStatColor( "Cyan1" );
	AddStatColor( "Green1" );
	AddStatColor( "Yellow1" );
	AddStatColor( "Red2" );
	AddStatColor( "Magenta2" );
	AddStatColor( "Blue2" );
	AddStatColor( "Cyan2" );
	AddStatColor( "Green2" );
	AddStatColor( "Yellow2" );
#endif
	SetCursorColor( "PaleTurquoise" );

#ifdef __TURBOC__

unsigned size;

	size   = imagesize( getmaxx()/2, 8, getmaxx(), oy );
	buffer = malloc(size);
	if (!buffer)	exit(-1);
#endif
	running_balls=0;
}

Game::~Game() {
	if (keeper)		delete keeper;
	if (x1)			delete x1;
	if (y1)			delete y1;
	if (x2)			delete x2;
	if (y2)			delete y2;
}

const Real & Game::GetPresetA() const				{ return PresetA; }
const Real & Game::GetPresetHaft() const			{ return PresetHaft; }
const Real & Game::GetSlowGranularity() const	{ return SlowGranularity; }
const Real & Game::GetNormalBallSize() const		{ return NormalBallSize; }

const Real & Game::GetChargeGranularity() const	{ return ChargeGranularity; }
const Real & Game::GetChargeSpeed() const			{ return ChargeSpeed; }
const Real & Game::GetMaxCharge() const			{ return MaxCharge; }
const Real & Game::GetShootTime() const			{ return ShootTime; }

Vec2 Game::Edge(int i) const {
	switch(i&0x3) {
		case 0:	return Vec2(AreaOffX(),					AreaOffY() );
		case 1:	return Vec2(AreaOffX()+AreaWidth(),	AreaOffY() );
		case 2:	return Vec2(AreaOffX()+AreaWidth(),	AreaOffY()+AreaHeight() );
		default:	return Vec2(AreaOffX(),					AreaOffY()+AreaHeight() );
	}
}
Vec2 Game::Mid(int i) const {
	switch(i&0x3) {
		case 0:	return Vec2(AreaOffX()+AreaWidth()/2.0,	AreaOffY() );
		case 1:	return Vec2(AreaOffX()+AreaWidth(),		AreaOffY()+AreaHeight()/2.0 );
		case 2:	return Vec2(AreaOffX()+AreaWidth()/2.0,	AreaOffY()+AreaHeight() );
		default:	return Vec2(AreaOffX(),						AreaOffY()+AreaHeight()/2.0 );
	}
}

void Game::InitPlayground() {
	y1 = new YWall( 0.0, MaxX(), 0.0 );
	x1 = new XWall( MaxX(), 0.0, MaxY() );
	y2 = new YWall( 0.0, MaxX(), MaxY() );
	x2 = new XWall( 0.0, 0.0, MaxY() );
//	x2 = new XWall( 2.0, 0, MaxY() );
}

void Game::DrawBackground() const {
#ifdef __TURBOC__
	setfillstyle(SOLID_FILL,(int)GetAllocatedPixel("black"));
	bar(0,0,getmaxx(),getmaxy());
#endif
	SetBgColor( floor );
	FillRectangle( 0.0, 0.0, MaxX(), MaxY() );

	if (keeper)		keeper->Draw();
}

void Game::ExposeRedraw() const {
	DrawBackground();
	DynObj::ForAllDyn(&DynObj::Redraw);
}

void Game::ResizeRedraw() const {
}

void Game::ShowInfo( char *str ) const {
#ifndef __TURBOC__
	printf( "%s\n", str );
#else

	getimage( getmaxx()/2, 8, getmaxx(), oy, buffer );
	putimage( getmaxx()/2, 0, buffer, COPY_PUT );
	setfillstyle( SOLID_FILL, getbkcolor() );
	bar(getmaxx()/2,oy-8,getmaxx(),oy-1);
	outtextxy( getmaxx()/2, oy-8, str );
#endif
}

int Game::IsSelectable(Ball * /* b */) {
	return 1;
}

void Game::ResetGame() {
	if (keeper)		keeper->GameWasReset();
}

#ifndef SHOW_INFO

void Game::ShootBall( Ball * )		{}
void Game::PressedBall( Ball * )		{}
void Game::TouchedBall( Ball * )		{}
void Game::HitWall( Ball * )			{}
void Game::StartBall( Ball * )		{ running_balls++; }
void Game::StopBall( Ball * ) {
	running_balls--;
	if (!running_balls)	AllBallsStopped();
}
void Game::InPocket( Ball * )			{ ClickPocket(); }
void Game::AllBallsStopped()			{}

#else

void Game::ShootBall( Ball *b ) {
char	buffer[30];

	sprintf( buffer, "### ShootBall(%d)", b->Object::id );
	ShowInfoIf(buffer);
}

void Game::PressedBall(Ball *b) {
char	buffer[30];

	sprintf( buffer, "... PressedBall(%d)", b->Object::id );
	ShowInfoIf(buffer);
}

void Game::TouchedBall(Ball *b) {
char	buffer[30];

	sprintf( buffer, "... TouchedBall(%d)", b->Object::id );
	ShowInfoIf(buffer);
}

void Game::HitWall(Ball *b) {
char	buffer[30];

	sprintf( buffer, "... HitWall(%d)", b->Object::id );
	ShowInfoIf(buffer);
}

void Game::StartBall( Ball *b ) {
char	buffer[30];

	sprintf( buffer, "... StartBall(%d)", b->Object::id );
	ShowInfoIf(buffer);
	running_balls++;
}

void Game::StopBall( Ball *b ) {
char	buffer[30];

	sprintf( buffer, "... StopBall(%d)", b->Object::id );
	ShowInfoIf(buffer);
	running_balls--;
	if (!running_balls)	AllBallsStopped();
}

void Game::InPocket( Ball *b ) {
char	buffer[30];

	sprintf( buffer, "### InPocket(%d)", b->Object::id );
	ShowInfoIf(buffer);
	ClickPocket();
}

void Game::AllBallsStopped() {
	ShowInfoIf("### AllBallsStopped");
}

#endif
