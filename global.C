#ifndef _global_h
#	include "global.h"
#endif

//
// Im Modul global.C werden sämtliche Spielparameter wie Größen,
// Kräfte ... festgehalten, damit sie an einer lokalen Stelle zugreifbar
// sind. Die zugeordneten Werte sind Voreinstellungen, die durch eine
// Konfigurationsdatei (wenn vorhanden) überschrieben werden können.
//

#ifndef _real_h
#	include "real.h"
#endif
#ifndef _cannon_h
#	include "cannon.h"
#endif
#ifndef _carrom_h
#	include "carrom.h"
#endif
#ifndef _game_h
#	include "game.h"
#endif
#ifndef _goal_h
#	include "goal.h"
#endif
#ifndef _hockey_h
#	include "hockey.h"
#endif
#ifndef _curling_h
#	include "curling.h"
#endif
#ifndef _pball_h
#	include "pball.h"
#endif
#ifndef _pool_h
#	include "pool.h"
#endif
#ifndef _pool8_h
#	include "pool8.h"
#endif
#ifndef _snooker_h
#	include "snooker.h"
#endif
#ifndef _pocket_h
#	include "pocket.h"
#endif


Real Billard::PresetA			= -25.0;
Real Billard::PresetHaft		= 5.0;
#ifndef __TURBOC__
Real Billard::SlowGranularity	= 0.25;
#else
Real Billard::SlowGranularity	= 0.5;
#endif
Real Billard::FrameOffset		= 20.0;
Real Billard::InnerCushion		=  4.0;
Real Billard::OuterCushion		=  8.0;



Real Cannon::BallRadius		=   6.15 / 2;
Real Cannon::TableWidth		= 224.0;
Real Cannon::TableHeight	= 112.0;


Real Carrom::Offset           = 0.01;
Real Carrom::PresetA				= -400.0;
Real Carrom::PresetHaft			= 5.0;
Real Carrom::SlowGranularity	= 0.125;
Real Carrom::FrameOffset		= 14.0;
Real Carrom::ToPocket			=  2.0;
Real Carrom::Cushion				=  6.0;
Real Carrom::TableWidth			= 73.0;
Real Carrom::TableHeight		= 73.0;
Real Carrom::PocketRadius		=  6.20 / 2;
Real Carrom::StoneRadius		=  3.30 / 2;
Real Carrom::StrikerRadius		=  4.35 / 2;
Real Carrom::StoneWeight		=  4.;		//  4 g
Real Carrom::StrikerWeight		= 10.;		// 10 g

Real Carrom::ChargeGranularity	= 0.05;	// seconds
Real Carrom::ChargeSpeed			= 400.;	// Charge per second
Real Carrom::MaxCharge				= 400.;
Real Carrom::ShootTime				= 0.1;	// seconds
Real Carrom::PocketFrame	= 2.0;
Real Carrom::PocketWidth	= 8.5;

Real CarromDemo::PresetA			= 0.0;
Real CarromDemo::SlowGranularity	= 1.0;



Real Game::PresetA			= -10.0;
Real Game::PresetHaft		= 10.0;
Real Game::SlowGranularity	= 0.1;
Real Game::NormalBallSize	= 1.2;

#ifndef __TURBOC__
Real Game::ChargeGranularity	= 0.01;	// seconds
Real Game::ChargeSpeed			= 250.;	// Charge per second
Real Game::MaxCharge				= 300.;
Real Game::ShootTime				= 0.1;	// seconds
#else
Real Game::ChargeGranularity	= 0.05;	// seconds
Real Game::ChargeSpeed			= 250.;	// Charge per second
Real Game::MaxCharge				= 300.;
Real Game::ShootTime				= 0.2;	// seconds
#endif


Real Hockey::PresetA				= -100.0;
Real Hockey::PresetHaft			= 5.;
Real Hockey::SlowGranularity	= 0.125;
Real Hockey::FrameOffset		=   5.0;
Real Hockey::TableWidth			=  100.0;
Real Hockey::TableHeight		=   70.0;
Real Hockey::GoalSize			=  20.0;
Real Hockey::DiscRadius			=   4.0;
Real Hockey::HandRadius			=   6.0;
Real Hockey::DiscWeight			=  4.;		//  4 g
Real Hockey::HandWeight			= 10.;		// 10 g
Real Hockey::GoalFrame			= 0.9;
Real Hockey::GoalHeight			= 10.0;
Real TestHockey::PresetA			= -400.0;
Real TestHockey::SlowGranularity	= 0.01;


Real Curling::PresetA				= -50.0;
Real Curling::PresetHaft			= 5.;
Real Curling::SlowGranularity	= 0.125;
Real Curling::FrameOffset		=   0.0;
Real Curling::TableWidth			=  100.0;
Real Curling::TableHeight		=   40.0;
Real Curling::GoalSize			=  20.0;
Real Curling::Offset					=  0.5;
Real Curling::CurlRadius			=  2.0;
Real Curling::CurlWeight			=  4.;		//  4 g
Real Curling::KeeperFrame			= 0.9;
Real Curling::KeeperHeight			= 6.0;


Real Pool::Offset       = 0.05;
Real Pool::EdgeRadius	= 0.3;
Real Pool::EdgeAngle		= 50.;
Real Pool::MidAngle		= 30.;
Real Pool::TableWidth	= 200.0;
Real Pool::TableHeight	= 100.0;
Real Pool::PocketRadius	=   8.00 / 2.;
Real Pool::BallRadius	=   5.72 / 2.;
Real Pool::PocketFrame  = 2.0;
Real Pool::PocketHeight   = 10.0;

Real Pool8Demo::PresetA			= 0.0;
Real Pool8Demo::SlowGranularity	= 1.0;

Real PBallTop::WarpTime				= 0.02;	// Zeit zum Sprung auf Cursorposition
Real PBallTop::LockedQueueDelay	= 0.25;	// Nachlauf des gelock'ten Queue

Real Snooker::TableWidth	= 350.0;
Real Snooker::TableHeight	= 175.0;
Real Snooker::BallRadius	=   5.24 / 2;

Real SnookerDemo::PresetA			= 0.0;
Real SnookerDemo::SlowGranularity	= 1.0;


/* ========================================================================== */
/* ====                DO NOT EDIT BEYOND THIS LINE                      ==== */
/* ========================================================================== */

typedef struct {
	const char	*name;
	Real			*val_addr;
} KonfiParam;

static KonfiParam konfi[] = {
{ "Billard::FrameOffset",  &Billard::FrameOffset },
{ "Billard::InnerCushion",  &Billard::InnerCushion },
{ "Billard::OuterCushion",  &Billard::OuterCushion },
{ "Billard::PresetA",  &Billard::PresetA },
{ "Billard::PresetHaft",  &Billard::PresetHaft },
{ "Billard::SlowGranularity",  &Billard::SlowGranularity },
{ "Cannon::BallRadius",  &Cannon::BallRadius },
{ "Cannon::TableHeight",  &Cannon::TableHeight },
{ "Cannon::TableWidth",  &Cannon::TableWidth },
{ "Carrom::ChargeGranularity",  &Carrom::ChargeGranularity },
{ "Carrom::ChargeSpeed",  &Carrom::ChargeSpeed },
{ "Carrom::Cushion",  &Carrom::Cushion },
{ "Carrom::FrameOffset",  &Carrom::FrameOffset },
{ "Carrom::MaxCharge",  &Carrom::MaxCharge },
{ "Carrom::Offset",  &Carrom::Offset },
{ "Carrom::PocketFrame",  &Carrom::PocketFrame },
{ "Carrom::PocketRadius",  &Carrom::PocketRadius },
{ "Carrom::PocketWidth",  &Carrom::PocketWidth },
{ "Carrom::PresetA",  &Carrom::PresetA },
{ "Carrom::PresetHaft",  &Carrom::PresetHaft },
{ "Carrom::ShootTime",  &Carrom::ShootTime },
{ "Carrom::SlowGranularity",  &Carrom::SlowGranularity },
{ "Carrom::StoneRadius",  &Carrom::StoneRadius },
{ "Carrom::StoneWeight",  &Carrom::StoneWeight },
{ "Carrom::StrikerRadius",  &Carrom::StrikerRadius },
{ "Carrom::StrikerWeight",  &Carrom::StrikerWeight },
{ "Carrom::TableHeight",  &Carrom::TableHeight },
{ "Carrom::TableWidth",  &Carrom::TableWidth },
{ "Carrom::ToPocket",  &Carrom::ToPocket },
{ "CarromDemo::PresetA",  &CarromDemo::PresetA },
{ "CarromDemo::SlowGranularity",  &CarromDemo::SlowGranularity },
{ "Curling::CurlRadius",  &Curling::CurlRadius },
{ "Curling::CurlWeight",  &Curling::CurlWeight },
{ "Curling::FrameOffset",  &Curling::FrameOffset },
{ "Curling::GoalSize",  &Curling::GoalSize },
{ "Curling::KeeperFrame",  &Curling::KeeperFrame },
{ "Curling::KeeperHeight",  &Curling::KeeperHeight },
{ "Curling::Offset",  &Curling::Offset },
{ "Curling::PresetA",  &Curling::PresetA },
{ "Curling::PresetHaft",  &Curling::PresetHaft },
{ "Curling::SlowGranularity",  &Curling::SlowGranularity },
{ "Curling::TableHeight",  &Curling::TableHeight },
{ "Curling::TableWidth",  &Curling::TableWidth },
{ "Game::ChargeGranularity",  &Game::ChargeGranularity },
{ "Game::ChargeSpeed",  &Game::ChargeSpeed },
{ "Game::MaxCharge",  &Game::MaxCharge },
{ "Game::NormalBallSize",  &Game::NormalBallSize },
{ "Game::PresetA",  &Game::PresetA },
{ "Game::PresetHaft",  &Game::PresetHaft },
{ "Game::ShootTime",  &Game::ShootTime },
{ "Game::SlowGranularity",  &Game::SlowGranularity },
{ "Hockey::DiscRadius",  &Hockey::DiscRadius },
{ "Hockey::DiscWeight",  &Hockey::DiscWeight },
{ "Hockey::FrameOffset",  &Hockey::FrameOffset },
{ "Hockey::GoalFrame",  &Hockey::GoalFrame },
{ "Hockey::GoalHeight",  &Hockey::GoalHeight },
{ "Hockey::GoalSize",  &Hockey::GoalSize },
{ "Hockey::HandRadius",  &Hockey::HandRadius },
{ "Hockey::HandWeight",  &Hockey::HandWeight },
{ "Hockey::PresetA",  &Hockey::PresetA },
{ "Hockey::PresetHaft",  &Hockey::PresetHaft },
{ "Hockey::SlowGranularity",  &Hockey::SlowGranularity },
{ "Hockey::TableHeight",  &Hockey::TableHeight },
{ "Hockey::TableWidth",  &Hockey::TableWidth },
{ "PBallTop::LockedQueueDelay",  &PBallTop::LockedQueueDelay },
{ "PBallTop::WarpTime",  &PBallTop::WarpTime },
{ "Pool8Demo::PresetA",  &Pool8Demo::PresetA },
{ "Pool8Demo::SlowGranularity",  &Pool8Demo::SlowGranularity },
{ "Pool::BallRadius",  &Pool::BallRadius },
{ "Pool::EdgeAngle",  &Pool::EdgeAngle },
{ "Pool::EdgeRadius",  &Pool::EdgeRadius },
{ "Pool::MidAngle",  &Pool::MidAngle },
{ "Pool::Offset",  &Pool::Offset },
{ "Pool::PocketFrame",  &Pool::PocketFrame },
{ "Pool::PocketHeight",  &Pool::PocketHeight },
{ "Pool::PocketRadius",  &Pool::PocketRadius },
{ "Pool::TableHeight",  &Pool::TableHeight },
{ "Pool::TableWidth",  &Pool::TableWidth },
{ "Snooker::BallRadius",  &Snooker::BallRadius },
{ "Snooker::TableHeight",  &Snooker::TableHeight },
{ "Snooker::TableWidth",  &Snooker::TableWidth },
{ "SnookerDemo::PresetA",  &SnookerDemo::PresetA },
{ "SnookerDemo::SlowGranularity",  &SnookerDemo::SlowGranularity },
{ "TestHockey::PresetA",  &TestHockey::PresetA },
{ "TestHockey::SlowGranularity",  &TestHockey::SlowGranularity },
{ 0, 0 }			// end konfi
};

void show_defaults() {
	for (unsigned i=0;i<sizeof(konfi)/sizeof(KonfiParam)-1U;i++) {
		printf( "%s = %g\n", konfi[i].name, (double)*konfi[i].val_addr );
	}
}

void load_konfi() {
FILE		*fp;
char		buffer[80];

	fp = fopen( PRESET_FILE, "r" );
	if (!fp) {
		sprintf(buffer,"%s/%s",DATA_DIRECTORY,PRESET_FILE);
		fp = fopen( buffer, "r" );
		if (!fp)		return;
	}

	while( fgets( buffer, sizeof(buffer), fp )) {
		if (strstr(buffer,"::")) {
			char *ptr = buffer;
			while( (ptr)&&!isspace(*ptr)&&*ptr!='=' )		ptr++;
			if (!ptr)			break;
			*ptr++ = '\0';
			while( (ptr)&&isspace(*ptr)||*ptr=='=' )   	ptr++;
			if (!ptr)			break;

			int i;
			for (i=sizeof(konfi)/sizeof(KonfiParam)-2;i>=0;i--) {
				if (!strcmp(buffer,konfi[i].name)) {
					(*konfi[i].val_addr) = Real(atof(ptr));
					break;
				}
			}
			if (i<0) {
				printf( "%s: unknown parameter in %s", PRESET_FILE, buffer );
			}
		}
	}
	fclose(fp);
}

#ifdef DEBUG

/* ========================================================================== */

typedef struct {
	const char		*name;
	unsigned long	mask;
} DebugFlag;

static DebugFlag dbg[] = {
{ "AbortCalc",  AbortCalc },
{ "AbortReCalc",  AbortReCalc },
{ "AfterCollision",  AfterCollision },
{ "BMover",  BMover },
{ "BState",  BState },
{ "BeforCollision",  BeforCollision },
{ "CheckBoundary",  CheckBoundary },
{ "CollCalc",  CollCalc },
{ "ForceCalc",  ForceCalc },
{ "GameState",  GameState },
{ "Intro",  Intro },
{ "Loops",  Loops },
{ "Motion",  Motion },
{ "MoveAll",  MoveAll },
{ "ObjectInfo",  ObjectInfo },
{ "PBallHit",  PBallHit },
{ "PointerMove",  PointerMove },
{ "ShowColors",  ShowColors },
{ "ShowLight",  ShowLight },
{ "ShowRings",  ShowRings },
{ "ShowTurns",  ShowTurns },
{ "StickLevel",  StickLevel },
{ "Sync",  Sync },
{ "TCTrace",  TCTrace },
{ "UnixTrace",  UnixTrace },
{ "XWallHit",  XWallHit },
{ "YWallHit",  YWallHit },
{ "__Moves",  __Moves },
{ "__Rings",  __Rings },
{ "__ShowAll",  __ShowAll },
{ "xdb",  xdb },
{ "xwd",  xwd },
{ 0, 0 }		// end dbg
};

void show_flags() {
int	width = 0;

	for (int i=0;i<sizeof(dbg)/sizeof(DebugFlag)-1;i++) {
		if (strlen(dbg[i].name)>width)		width = strlen(dbg[i].name);
	}
	width++;
	int cols = 79/width;
	int rows = (sizeof(dbg)/sizeof(DebugFlag)-1)/cols+1;
	for (int r=0;r<rows;r++) {
		for (int c=0;c<cols;c++) {
			int entry = rows*c+r;
			if (entry<sizeof(dbg)/sizeof(DebugFlag)-1)
								printf( "%-*s", width, dbg[entry].name );
		}
		printf( "\n" );
	}
}

int set_debug( const char *flag_name ) {
	for (int i=0;i<sizeof(dbg)/sizeof(DebugFlag)-1;i++) {
		if (!strcmp(flag_name,dbg[i].name)) {
			debug |= dbg[i].mask;
			return 0;
		}
	}
	return 1;
}


#endif
