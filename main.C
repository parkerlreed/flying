#ifndef _global_h
#	include "global.h"
#endif


#ifndef _dynobj_h
#	include "dynobj.h"
#endif
#ifndef _object_h
#	include "object.h"
#endif

#ifndef _graph_h
#	include "graph.h"
#endif
#ifndef _mover_h
#	include "mover.h"
#endif

#ifndef _game_h
#	include "game.h"
#endif
#ifndef _snooker_h
#	include "snooker.h"
#endif
#ifndef _cannon_h
#	include "cannon.h"
#endif
#ifndef _pool8_h
#	include "pool8.h"
#endif
#ifndef _carrom_h
#	include "carrom.h"
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

//
// Stackgröße in Turbo-C
//
#ifdef __TURBOC__
// #	include <dos.h>
unsigned _stklen = 24000;
#endif


#ifdef DEBUG
#	ifdef __TURBOC__
		long debug = TCTrace;			// globale Variable
#	else
		long debug = UnixTrace;
#	endif
#endif

#ifdef __TURBOC__
void start_log( const char * /* name */ )	{}
void end_log() {}
#else
void start_log( const char *name );
void end_log();
void main_xbench( int argc, char **argv );
#endif

double current_time;

int	deluxe =   2;						// Luxus-Ausführung bei Pool-Billard
                                    //         half full
												// case 1:  4    0   Intro
												// case 2:  4    3   game
												// case 3:  2    1   old game
int	size				= 800;			// Default-Größe
int	sound_request	= 0;				// Wunsch nach Sound
int	light_flag		= 1;				// Light-Source
int	color_flag		= 0;				// Desired table color
int	nohint_flag		= 0;				// Dont draw arrows for hints
int	enhanced_mover = 2;
int	frames_per_sec = 0;				// limit frames per seconds
int	no_server_grabs= 0;				// never grab server
int	no_override    = 0;				// 'pseudo'-root as normal window
int	nine_ball      = 0;				// special parameter for 9ball pool-game

#ifdef STATISTICS
int	showmeter      = 0;
#endif

void usage() {
	printf("usage: flying <options>\n" );
	printf("options:\n" );
	printf("    select game:\n" );
	printf("       -snooker      Snooker      (xsnooker)\n" );
	printf("       -pool         Pool-Billard (xpool)\n" );
	printf("       -pool9        9-ball Pool\n" );
	printf("       -cannon       Karambolage  (xcannon)\n" );
	printf("       -carrom       Carrom       (xcarrom)\n" );
	printf("       -hockey       Air-Hockey   (xhockey)\n" );
	printf("       -curling      Curling      (xcurling)\n" );
//	printf("       -bowling      Bowling\n" );
//	printf("       -golf         Miniature-Golf\n" );
	printf("       -ft <n>       table size in feet [6...9]\n" );
	printf("       -deluxe       special version of pool (default)\n" );
	printf("       -tv           normal version of pool with TV-balls\n" );
//	printf("       -light <n>    0 - static, 1 moving light\n" );
	printf("       -[red|green|blue] select billard table color\n" );
	printf("       -nohints      turn off hint arrows\n" );
	printf("       -demo         demonstration (no pockets)\n" );
	printf("       -presets      show preset variables\n" );
#ifndef __TURBOC__
	printf("    X11 options:\n");
	printf("       -size <n>     window size\n" );
	printf("       -root         full screen (default)\n" );
	printf("       -no_override  don't let the pseudo-root be of OverrideRedirect\n" );
	printf("       -no_grabs     never grab the server\n" );
	printf("       -display      ouput-display (def: $DISPLAY)\n" );
	printf("       -in1          input-display (def: = output-display)\n" );
	printf("       -in2          alternate input-display\n" );
	printf("       -mode <n>     select update mode 1/2/3 with default 2\n" );
	printf("       -chkmode      run a benchmark with the 3 update modes\n" );
#endif
#if (SOUND_SUBSYSTEM)
	printf("       -sound        clicks when hitting balls\n" );
#endif
	printf("    for time measurements:\n");
	printf("       -frames <n>   limit update to <n> frames per sec (def: full speed)\n" );
	printf("       -time <n>     stop after n seconds\n");
	printf("       -maxspeed     jump from collision to collision (asynchronous)\n" );

#ifdef STATISTICS
	printf("    for debugging:\n");
	printf("       -meter        performance meter\n" );
#endif
#ifdef DEBUG
	printf("       -debug <n>    verbose messages at stdout\n" );
	printf("\ndebug-flags:\n");
	show_flags();
#endif


	printf("\nsmail (for gifts):\n");
	printf("        Helmut Hoenig\n");
	printf("        Hopfenstrasse 8a\n");
	printf("        65520 Bad Camberg\n");
	printf("        GERMANY\n");
	printf("\n");
	printf( "                By the way, I am collecting banknotes!!\n");
	printf( "    If you want to join into my collection, get any bill of your country,\n");
	printf( " sign it on the backside and send it to me so I will pin it on my world map.\n");
	printf( "             (Don't forget the exact location for the pin :-)\n");
	printf( "    Anyway, you can also send me a picture postcard or anything else ...\n\n" );
	exit(-1);
}

int main(int argc, char **argv)
{
char		*disp=0l;	// Ausgabe
char		*in1=0l;		// Eingabe1
char		*in2=0l;		// Eingabe2
double	stop_time=0.0;
long		all_cycles=0l;
int		max_speed	= 0;
double	ft = 0.0;

			size = ON_ROOT;
int			game_flag  = 0;
PBallType	game_pball = BillardQueue;
int		demo_flag  = 0;
double	demo_param = 0.0;
char		pressed_key=0;

	srand( (unsigned)GetCurrentTime() );
	load_konfi();

	if (!strcmp(argv[0],"xsnooker"))					game_flag = 1;
	else if (!strcmp(argv[0],"xpool"))				game_flag = 2;
	else if (!strcmp(argv[0],"xcannon"))			game_flag = 3;
	else if (!strcmp(argv[0],"xcarrom"))			game_flag = 4;
	else if (!strcmp(argv[0],"xhockey"))			game_flag = 5;
	else if (!strcmp(argv[0],"xcurling"))			game_flag = 6;
	else if (!strcmp(argv[0],"xbowling"))			game_flag = 7;
	else if (!strcmp(argv[0],"xgolf"))				game_flag = 8;

	for (int i=1; i<argc; i++ ) {
		if (!strcmp(argv[i],"-snooker"))				game_flag = 1;
		else if (!strcmp(argv[i],"-pool"))			game_flag = 2;
		else if (!strcmp(argv[i],"-pool9")) {		game_flag = 2; nine_ball = 1; }
		else if (!strcmp(argv[i],"-cannon"))		game_flag = 3;
		else if (!strcmp(argv[i],"-carrom"))		game_flag = 4;
		else if (!strcmp(argv[i],"-hockey"))		game_flag = 5;
		else if (!strcmp(argv[i],"-curling"))		game_flag = 6;
		else if (!strcmp(argv[i],"-bowling"))		game_flag = 7;
		else if (!strcmp(argv[i],"-golf"))			game_flag = 8;
		else if (!strcmp(argv[i],"-deluxe")) {
					deluxe    = (i<argc-1)?atoi(argv[i+1]):0;
					if (deluxe) { i++; } else { deluxe = 2; }
					game_flag = 2;
					if (!ft)		ft = 8.0;
		}
		else if (!strcmp(argv[i],"-tv")) {
					deluxe    = 0;
					game_flag = 2;
		}
		else if (!strcmp(argv[i],"-light"))		light_flag = atoi(argv[++i]);
		else if (!strcmp(argv[i],"-mode"))		enhanced_mover = atoi(argv[++i]);
#ifndef __TURBOC__
		else if (!strcmp(argv[i],"-chkmode"))	{ main_xbench(0,0L);	exit(0); }
#endif
		else if (!strcmp(argv[i],"-red"))		color_flag = 1;
		else if (!strcmp(argv[i],"-green"))		color_flag = 2;
		else if (!strcmp(argv[i],"-blue"))		color_flag = 3;
		else if (!strcmp(argv[i],"-nohints"))	nohint_flag = 1;
		else if (!strcmp(argv[i],"-demo"))			{
				demo_flag=1;
				if (i<argc-1)			demo_param = atof(argv[i+1]);	// read param
				else						demo_param = 0.0;					// no param
				if (demo_param>0.0) 	i++;									// skip arg
		}
#if (SOUND_SUBSYSTEM)
		else if (!strcmp(argv[i],"-sound"))			sound_request = 1;
#endif
		else if (!strcmp(argv[i],"-time"))			stop_time = atof(argv[++i]);
		else if (!strcmp(argv[i],"-size"))			size = atoi(argv[++i]);
		else if (!strcmp(argv[i],"-ft"))				ft = atof(argv[++i]);
		else if (!strcmp(argv[i],"-display"))		disp = argv[++i];
		else if (!strcmp(argv[i],"-in1"))			in1 = argv[++i];
		else if (!strcmp(argv[i],"-in2"))			in2 = argv[++i];
		else if (!strcmp(argv[i],"-root"))			size = ON_ROOT;
		else if (!strcmp(argv[i],"-no_override"))	no_override = 1;
		else if (!strcmp(argv[i],"-no_grabs"))		no_server_grabs = 1;
		else if (!strcmp(argv[i],"-frames"))		frames_per_sec=atoi(argv[++i]);
		else if (!strcmp(argv[i],"-maxspeed")) {
				if (!stop_time)		stop_time = 5.0;
				max_speed = 1;
				demo_flag = 1;
		}
		else if (!strcmp(argv[i],"-presets"))	{	show_defaults();	exit(0);	}

#ifdef STATISTICS
		else if (!strcmp(argv[i],"-meter"))			showmeter = 1;
#endif
#ifdef DEBUG
		else if (!strcmp(argv[i],"-debug"))			debug= atoi(argv[++i]);
		else if (!set_debug(argv[i]))					;
#endif
		else if (argv[i][0]=='-')						usage();
		else usage();
	}

#ifdef DEBUG
	if (debug&Intro) {
		debug |= __ShowAll | ForceCalc;
		size			= 600;
		deluxe		= 1;
		game_flag	=   2;
		ft				= 8.0;
	}
	if ((size==ON_ROOT)&&
		( debug&ShowColors||debug&ShowLight||debug&ShowRings||debug&ShowTurns
		||debug&GameState))
	{
		size			= 600;
	}
	if (debug&xdb) {
		size			= 600;
		in1			= "hgr49:0.0";
	}
#endif

	if (game_flag!=2)		deluxe=0;
	if (nine_ball)			deluxe=2;

do {
	PBall	*p1=0;
	PBall	*p2=0;
	InitColors();

#ifdef DEBUG
	if (debug&Motion) {
		ft = 8.0;
		game_flag = 2;
		deluxe    = 2;
		g=(Game*) new Pool8Test(ft,demo_param);
	}
	else
#endif
	switch(game_flag) {
	case 1:		if (demo_flag) {
						if (demo_param)	g=(Game*) new SnookerDemo(demo_param);
						else					g=(Game*) new SnookerDemo();
						break;
					}
					g = (Game*)new Snooker;
					start_log( "Snooker" );
					break;
	case 2:
					if (deluxe)				if (!ft)		ft = 8.0;
#ifdef DEBUG
					if ((ft<1||ft>9))		ft = 6.0 + (rand()%30) / 10;
#else
					if ((ft<6||ft>9))		ft = 6.0 + (rand()%30) / 10;
#endif
					if (demo_flag) {
						if (demo_param)	g=(Game*) new Pool8Demo(ft,demo_param);
						else					g=(Game*) new Pool8Demo(ft);
						break;
					}
					if (!nine_ball) {
						g = (Game*)new Pool8(ft);
						start_log( "8-Ball" );
					}
					else {
						g = (Game*)new Pool9(ft);
						start_log( "9-Ball" );
					}
					break;
	case 3:		g = (Game*)new Cannon;
					start_log( "Cannon" );
					break;
	case 4:		if (demo_flag) {
						if (demo_param)	g=(Game*) new CarromDemo(demo_param);
						else					g=(Game*) new CarromDemo();
						break;
					}
					g = (Game*)new Carrom;
					start_log( "Carrom" );
					break;
	case 5:		if (demo_flag) {
						if (demo_param)	g=(Game*) new TestHockey(demo_param);
						else					g=(Game*) new TestHockey();
						break;
					}
					g = (Game*)new Hockey;
					start_log( "Hockey" );
					break;
	case 6:		g = (Game*)new Curling;
					start_log( "Curling" );
					break;
	default:		if (demo_flag) {
						if (demo_param)	g=(Game*) new CarromDemo(demo_param);
						else					g=(Game*) new CarromDemo();
						break;
					}
					usage();						break;
	};

//
// Durch das Instanziieren der Game-Klasse entstanden Farbanfordungen
// welche nun befriedigt werden müssen, bevor sie in den InitPlayground
// Routinen verwendet werden können.
	AllocColors();
//
// Die für die einzelnen Spiele notwendigen Objekte werden nun angelegt.
	g->InitPlayground();
	MapMainWindow();

	switch(game_flag) {
	case 5:	game_pball = DiscSlider; 	break;
	case 6:	game_pball = DiscThrower;	break;
	default:	game_pball = BillardQueue;	break;
	}

	if (in1) 					p1 = new PBall( game_pball, in1 );
	if (in2)						p2 = new PBall( game_pball, in2 );
	if (!demo_flag&&!p1)		p1 = new PBall( game_pball, (disp)?disp:"" );


	g->DrawBackground();
	DynObj::DynInitAll();
#ifdef DEBUG
	if (debug&ObjectInfo)	Object::ForAll(&Object::Info);
#endif
	current_time = GetCurrentTime();
	DynObj::ForAllDyn(&DynObj::Reset);
	DynObj::ForAllDyn(&DynObj::Redraw);
	current_time = GetCurrentTime();
	DynObj::ForAllDyn(&DynObj::CollisionCalc);
	if (stop_time)		stop_time+=current_time;		// Jetzt geht's los ...


	while(1) {
		double	collision_step;
		collision_step = DynObj::CollisionAll();	// nächste Kollisionszeit

		double	current_step   = GetCurrentTime()-current_time;

		if ((!max_speed)&&collision_step>current_step) {
			// Zwischenschritt einlegen
#ifdef DEBUG
			if (debug&MoveAll) {
					printf("MOVE intermediate: +%f = %f (OFF: %f, Collide: %f)\n",
								current_step, current_time+current_step,
								0.0,
								(collision_step!=MAX_TIME)?collision_step:-1 );
			}
#endif
			current_time += current_step;
			DynObj::ForAllDyn(&DynObj::Move,current_step);
		}
		else {
			// Kollisionsschritt einlegen
#ifdef DEBUG
			if (debug&MoveAll) {
					printf("MOVE to collision: +%f = %f (OFF: %f)\n",
								collision_step, current_time+collision_step,
								current_step-collision_step );
			}
#endif
			current_time+=collision_step;
			DynObj::ForAllDyn(&DynObj::Move,collision_step);
			DynObj::Collide();							// Kollision ausführen
		}

#ifdef STATISTICS
{
static double next_rate		= 0.0;
static double cycle_start	= 0.0;
static int off					= 0;
static int cycle_count		= 0;

		double check_time = GetCurrentTime();
		cycle_count++;
		if (next_rate<check_time)
		{  char	buffer[30];
			double			time_step = check_time - cycle_start;

			sprintf( buffer, "Rate:%7.1f/s", cycle_count/time_step );
			showinfo( CycleRate, buffer );
			if (showmeter)	graphinfo((int)(cycle_count/time_step));

			cycle_count=0;
			sprintf( buffer, "Moves:%7.1f/s", BallMover::moves/time_step );
			showinfo( MoveRate, buffer );
			BallMover::moves=0;

			if (check_time-current_time>0.05) {
				sprintf( buffer, "Offset:%7.1f/s", check_time-current_time );
				showinfo(OffsetInfo,buffer);
				off=1;
			}
			else if (off) {
				showinfo(OffsetInfo,"");
				off=0;
			}
			cycle_start=check_time;
			next_rate  =check_time+0.50;
		}
}
#endif
		PBallTop::ForAll(&PBallTop::Update);
		all_cycles++;
		pressed_key = GetKey();
#ifdef DEBUG
		if (pressed_key=='0')	enhanced_mover=0;		
		if (pressed_key=='1')	enhanced_mover=1;		
		if (pressed_key=='2')	enhanced_mover=2;
#endif

		if (pressed_key==' ')	g->ResetGame();
		if (pressed_key=='q'||pressed_key=='\033'||pressed_key=='\n'||pressed_key=='r'||((stop_time)&&(GetCurrentTime()>stop_time)))		break;
	};

	UnlockGraphic();
	if (p2)		delete p2;
	if (p1)		delete p1;
	delete g;
	CloseGraphic();
	printf( "end after %ld cycles\n", all_cycles );
	Object::ForAll(&Object::Info);
}
while( pressed_key=='r' );

	end_log();
	return 0;
}

