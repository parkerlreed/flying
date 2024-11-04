#ifndef _global_h
#	include "global.h"
#endif

#ifndef _ball_h
#	include "ball.h"			// Klassen-Info
#endif
#ifndef _game_h
#	include "game.h"			// Info-Weitergabe bei Kollisionen/Start/Stop
#endif
#ifndef _mover_h
#	include "mover.h"			// Optimierer für bewegte Bälle
#endif
#ifndef _pball_h
#	include "pball.h"			// als Callback bei Bewegungen
#endif

#ifdef DEBUG
#	ifndef _graph_h
#		include "graph.h"	// Test auf Bereichsüberschreitungen
#	endif
#endif

//
// ============================================================================
// class Ball: fliegende Kugel
// ============================================================================
//

Ball::Ball( double x, double y, double vx, double vy,double r_in,double m_in) :
StaticBall(x,y,r_in), run_flag(0), a(g->GetPresetA()),
#if (SIM_SLOW)
next_slowstep(SUPPRESS_SLOWSTEP),
#endif
pball(0), m(m_in)
{
	type = BallObj;
	SetV(Vec2(vx, vy));
	dyn_id = DynObj::id;				// aktuelle DynID ins Objekt kopieren
	state=0;
}

Ball::Ball( double x, double y, double vx, double vy, double r_in ) :
StaticBall(x,y,r_in), run_flag(0), a(g->GetPresetA()),
#if (SIM_SLOW)
next_slowstep(SUPPRESS_SLOWSTEP),
#endif
pball(0), m((R()*R()*R())/8.)
{
	type = BallObj;
	SetV(Vec2(vx, vy));
	dyn_id = DynObj::id;				// aktuelle DynID ins Objekt kopieren
	state=0;
}

Ball::Ball( double x, double y, double vx, double vy ) :
StaticBall(x,y,g->GetNormalBallSize()),
run_flag(0),
a(g->GetPresetA()),
#if (SIM_SLOW)
next_slowstep(SUPPRESS_SLOWSTEP),
#endif
pball(0),
m((R()*R()*R())/8.)
{
	type = BallObj;
	SetV(Vec2(vx, vy));
	dyn_id = DynObj::id;				// aktuelle DynID ins Objekt kopieren
	state=0;
}

Ball::Ball( const Vec2 &v_in, double r_in ) :
StaticBall(v_in,r_in), run_flag(0), a(g->GetPresetA()),
#if (SIM_SLOW)
next_slowstep(SUPPRESS_SLOWSTEP),
#endif
pball(0),
m((R()*R()*R())/8.)
{
	type = BallObj;
	SetV(Vec2Zero);
	dyn_id = DynObj::id;				// aktuelle DynID ins Objekt kopieren
	state=0;
}

Ball::Ball( const Vec2 &v_in, double r_in, double m_in ) :
StaticBall(v_in,r_in), run_flag(0), a(g->GetPresetA()),
#if (SIM_SLOW)
next_slowstep(SUPPRESS_SLOWSTEP),
#endif
pball(0),
m(m_in)
{
	type = BallObj;
	SetV(Vec2Zero);
	dyn_id = DynObj::id;				// aktuelle DynID ins Objekt kopieren
	state=0;
}

Ball::~Ball() {
	if (state)		delete state;
}

//
// Reset wird direkt vor dem Start der Animation aufgerufen, um die SlowStep-
// Berechnung zu initialisieren.
//
void Ball::Reset() {
#if (SIM_SLOW)
	last_slowstep = current_time;
	slow_granoff  = g->GetSlowGranularity()*(double)((rand()%10000)/10000);
	if (V().IsZero())		next_slowstep=0.0;
	else						next_slowstep=current_time+slow_granoff;
#endif
	if (!state)		state = new BallStateTop;
}

void Ball::Redraw() {
	state->Redraw();
}

void Ball::SetV( const Vec2 &v_in )	{
	v = v_in;
#if (SIM_SLOW)
	if (next_slowstep==SUPPRESS_SLOWSTEP)		return;
#endif
	if (IsRunning()) {
		if (v.IsZero()) {
			run_flag = 0;
			g->StopBall(this);
		}
	}
	else {
		if (!v.IsZero()) {
			run_flag = 1;
			g->StartBall(this);
		}
	}
				
#if (SIM_SLOW)
	SlowStep();
#endif
}


Vec2 Ball::Dir() const {
	if (IsRunning())		return V().Norm1();
	else						return Vec2Zero;
}

//
// Setzten der neuen Position mit Aktualisierung auf Bildschirm
//
void Ball::SetP( const Vec2 &new_p ) {
	state->MoveTo( new_p );
	p=new_p;
}

//
// Die Move-Routine verschiebt den Ball gemäß seiner Geschwindigkeit und
// der verschrittenen Zeit und führt die Aktualisierung auf dem Bildschirm aus.
//
void Ball::Move( Real t ) {
//
// Positionsbestimmung
//
Vec2 new_p=p+V()*t;

	SetP( new_p );
	if (pball)		pball->CueMoved();

//
// Gegebenenfalls Test auf Bildschirmüberschreitung
//
#ifdef DEBUG
	if (debug&CheckBoundary) {
		if ((PX()-R()<0.0)||(PX()+R()>Real(max_x))||
			 (PY()-R()<0.0)||(PY()+R()>Real(max_y))) {
			if ((PX()-R()<-EPS)||(PX()+R()-Real(max_x)>EPS)||
				 (PY()-R()<-EPS)||(PY()+R()-Real(max_y)>EPS)) {
				printf( "*** BOUNDARY crossed by: " );
				this->Info();
			}
#	if (0)
			else {
				printf( "    BOUNDARY touched by: " );
				this->Info();
			}
#	endif
		}
	}
#endif

}

void Ball::Info() {
	printf( "%02d: Ball:       %08lx: P()=(%4.1f,%4.1f), R()=%3.1f, V()=(%5.1f,%5.1f)\n",
				Object::id, (unsigned long)this,
				(double)PX(), (double)PY(), (double)R(),
				(double)VX(), (double)VY() );
}

void Ball::TellPressed() {
Ball	*ball;
Real	dist;

	for (ball=(Ball*)DynObj::dyn_queue; ball; ball=(Ball*)ball->DynObj::next ) {
		if (ball!=this) {
			dist = (ball->P()-P()).Norm();		// absoluter Wert egal
			if (dist<ball->R()+R()+0.02) {
				g->PressedBall(ball);
			}
		}
	}
}

#if (!TIME_CACHE)
//
// NextCollision
// - Alle Objekte werden befragt, wann sie vom angegebenen Ball getroffen werden
// - Es würde reichen, die Liste ab dem aktuellen Ball zu testen, da die vorher
//   eingereihten Objekte bereits einen Test gegen den aktuellen Ball gemacht
//   haben.
//
Real Ball::NextCollision() {
Object	*obj;
Real	next_time;

//
// alle Objekte
//
	for (obj=stat_queue; obj; obj=obj->Object::next ) {
		next_time = obj->HitFromBall(this)+current_time;
		if ( next_time < min_time ) {
				min_time   = next_time;
				hit_ball   = this;
				hit_object = obj;
		}
	}
	return min_time;
}
#else

//
// NextCollision
// - Die NextCollision-Version bei eingeschaltetem TimeCache muß nur noch
//   seine Werte mit den bisherigen Bestwerten vergleichen.
//
Real Ball::NextCollision() {
	if (collision_time<min_time) {
		min_time   = collision_time;
		hit_ball   = this;
		hit_object = collision_object;
	}
	return min_time;
}

//
// CollisionCalc berechnet den ZeitenCache neu, indem er jedes Objekt
// mit der HitFromBall-Routine nach der nächsten Kollision befragt. Die
// Zeiten der dynamischen Objekte sind dabei prinzipiell doppelt gespeichert,
// da sie jeweils in den Cache's beider betroffener Objekte vorhanden sind.
// Um sie nicht mehrfach zu berechnen, wird die Zeit den anderen beteiligten
// Objekten mit Hilf der HitYouAt-Routine mitgeteilt.
//
void Ball::CollisionCalc() {
int i;

	collision_time	 	= MAX_TIME;
	collision_object	= 0;

#ifdef DEBUG
	if (debug&CollCalc) {
		printf( "CollCalc: " ); Info();
	}
#endif

	obj_list.StartRecalc();
	for (i=0;i<col_objects;i++) {
		if (obj_list[i]->dyn_id<=dyn_id) {
			Real htime = obj_list[i]->HitFromBall(this);
			if (htime<MAX_TIME)		htime+=current_time;

			if (htime<collision_time) {
				collision_time		= htime;
				collision_object	= obj_list[i];
				obj_list.ToFront(i);
			}
			else {
				obj_list.ToBack(i);
			}
			obj_list.SetTime(i,htime);
		}
		else {
		//	Das Objekt darf nicht auf DynObj gecastet werden.
			Real htime = obj_list[i]->HitFromBall(this);
			if (htime<MAX_TIME)		htime+=current_time;
			((Ball*)obj_list[i])->HitYouAt( this, htime );
			obj_list.SetTime(i,htime);
		}	
	}
#ifdef DEBUG
	if (debug&CollCalc) {
		if (collision_time<MAX_TIME) {
				printf( "  Target: " );	collision_object->Info();
		}
		else	printf( "  Target: None\n" );
	}
#endif
	obj_list.StopRecalc();
}


//
// HitYouAt sagt einem Ball, daß der Kollisionzeit von dem übergebenen Objekt
// neu bestimmt wurde und daher in den Cache übernommen werden kann. Falls
// das Objekt bisher das 'Target' der nächsten Kollision war, muß das Target
// neu bestimmt werden.
//
void Ball::HitYouAt( Object *obj, Real htime ) {
	obj_list.SetTimeDirect(obj->id,htime);
	if (htime<collision_time) {
		collision_time		= htime;
		collision_object	= obj;
#ifdef DEBUG
		if (debug&CollCalc) {
			printf( "... CollCalc Hit: " );	Info();
			printf( "...   New Target: " );	collision_object->Info();
		}
#endif
	}
	else if (collision_object==obj) {
		collision_time		= MAX_TIME;
		collision_object	= 0L;
		for (int i=0;i<col_objects;i++) {
			if (obj_list[i]->dyn_id<=dyn_id) {
#if (ABORT_CALC)
				if (obj_list.GetTime(i)==NOT_REACHABLE) {
					Real htime = obj_list[i]->HitFromBall(this);
					if (htime<MAX_TIME)		htime+=current_time;
#ifdef DEBUG
					if (debug&AbortReCalc) {
						if (htime==NOT_REACHABLE) {
							printf( "... CollCalc %d: Recalc for %d. (TOO FAR)\n",
															Object::id, obj_list[i]->id );
						}
						else {
							printf( "... CollCalc %d: Recalc for %d. (enhancement)\n",
															Object::id, obj_list[i]->id );
						}
					}
#endif
					obj_list.SetTime(i,htime);
				}
#endif
				if (obj_list.GetTime(i)<collision_time) {
					collision_time		= obj_list.GetTime(i);
					collision_object	= obj_list[i];
				}
			}
		}
#ifdef DEBUG
		if (debug&CollCalc) {
			printf( "... CollCalc Hit: " );	Info();
			if (collision_time<MAX_TIME) {
					printf( "... Reset Target: " );	collision_object->Info();
			}
			else	printf( "... Reset Target: None\n" );
		}
#endif
	}
}

#endif


//
// HitFromBall berechnet die Zeit bis zum Zusammenstoß mit der übergebenen
// Kugel. Als Sonderbehandlung liefert er die Zeit bis zum Stillstand, bzw.
// bis zum nächsten SlowStep, wenn er sich selbst als Parameter hat.
//
Real Ball::HitFromBall( Ball *b ) {
	if (IsIdle()||b->IsIdle())		return MAX_TIME;

	if (this==b) {
#if (SIM_SLOW)
				if (next_slowstep)	return next_slowstep-current_time;
#endif
			// Kugel steht bereits (oder wird nicht gebremst)
				return NO_HIT;
	}
	else {
#				ifdef DEBUG
					if (debug&CheckBoundary) {
						Real dist = (P()-b->P()).Norm();
						if ( dist<R()+b->R() ) {
							if (dist-R()-b->R()<-2*EPS) {
								printf( "overlap in distance %g:\n", dist );
								printf( "         " ); Info();
								printf( "         " ); b->Info();
								CloseGraphic();
								exit(0);
							}
						}
					}
#				endif
				return OuterHitFromBall( b, b->V()-v );

	}
}


#if (SIM_SLOW)

//
// Der Slowstep wird bei der entsprechenden Option eingefuegt, um die Kugel
// gemäß der für sie gesetzten negativ-Beschleunigung zu bremsen. Das geschieht
// in bestimmten Intervallen, der Maximalgröße 'SlowGranularity'. Damit die
// Zeiten des Time-Cache sich nicht alle gleichzeitig verändern. Bei jeder
// Kollision findet die Aktualisierung der Zeit explizit statt, da in diesem
// Fall sowieso der Zeitenspeicher ungültig wird.
//
void Ball::SlowStep() {

	if (IsRunning()) {
		if (next_slowstep) {
		Real	len = Len();
		Real	new_len = len + a*(current_time-last_slowstep);
				if (new_len<RealZero)			new_len=RealZero;
				v *= new_len/len;
				last_slowstep=current_time;
				next_slowstep=current_time+g->GetSlowGranularity();
		}
		else {
				last_slowstep=current_time;
				next_slowstep=current_time+slow_granoff;
		}
	}
	else {
		if (next_slowstep) {
			next_slowstep = RealZero;
		}
	}
}
#endif


//
// CollideWithBall führt die Collision aus, die gerade zwischen dem Ball-Objekt
// und dem übergebenen Ball stattfindet. Als Sonderbehandlung wird der SlowStep
// eingeleitet, bzw. der Ball wird gestoppt, falls das Objekt selbst übergeben
// wird.
//
void Ball::CollideWithBall( Ball *b ) {
Vec2	x1,y1,x2,y2;

	if (this==b) {
#if (SIM_SLOW)
		SetV(v);					// ruft indirekt den SlowStep auf
#else
		SetV(Vec2Zero);	// Stillstand erreicht
#endif
		return;
	}

// Aufsplittung der Vektoren in Anteile parallel und orthogonal zur
// Verbindungslinie der beiden Kugeln.

	V().Split( P()-b->P(), &x1, &y1 );
	b->V().Split( P()-b->P(), &x2, &y2 );

	Vec2	help = M()*x1;
				help+= b->M()*x2;
	Vec2	diff= x1-x2;
	Vec2	x1n = (help - b->M()*diff ) / (M()+b->M());
				x1n += y1;
	Vec2	x2n = (help +    M()*diff ) / (M()+b->M());
				x2n += y2;

//
// wird bei stehenden Kugeln die Haftreibung ueberwunden ?
//
	if (!IsRunning()) {
#ifdef DEBUG
	if (debug&StickLevel) {
			printf( "%g < %g ?\n", (double)x1n.SqrNorm(),
									(double)(g->GetPresetHaft()*g->GetPresetHaft()) );
	}
#endif
		if (x1n.SqrNorm()<g->GetPresetHaft()*g->GetPresetHaft()) {
			x1n = Vec2Zero;
			g->TouchedBall(this);
		}
	}
	if (!b->IsRunning()) {
#ifdef DEBUG
	if (debug&StickLevel) {
			printf( "%g < %g ?\n", (double)x2n.SqrNorm(),
									(double)(g->GetPresetHaft()*g->GetPresetHaft()) );
	}
#endif
		if (x2n.SqrNorm()<g->GetPresetHaft()*g->GetPresetHaft()) {
			x2n = Vec2Zero;
			g->TouchedBall(b);
		}
	}
//
// demnaechst fliegende Kugel zuerst bearbeiten
//
	if (!x1n.IsZero()) {
		SetV(x1n);
		b->SetV(x2n);
	}
	else {
		b->SetV(x2n);
		SetV(x1n);
	}

	ClickBall();
	WasHit(b);
}

//
// WasHit ist eine überlagerbare Funktion, die die Berechnung des Caches
// anstößt, wenn der Ball durch ein fremdes Objekt (Wand oder anderer Ball)
// getroffen wurde.
//
void Ball::WasHit(Object *) {
#if (TIME_CACHE)
	CollisionCalc();
#endif
	if (pball)		pball->CueWasHit();
}


int Ball::Lock( PBallTop *pball_in ) {
	if (!pball) {
		pball = pball_in;
		return 0;
	}
	else {
		return 1;
	}
}

Real Ball::FindClosest( const Ball *myself, const Vec2 &pos, Ball **best ) {
Ball	*ball;
Real	dist;
Real	min_dist=RealZero;

	*best=0L;
	for (ball=(Ball*)DynObj::dyn_queue; ball; ball=(Ball*)ball->DynObj::next ) {
		dist = (ball->P()-pos).SqrNorm();		// absoluter Wert egal
		if ((ball!=myself)&&(!*best||dist<min_dist)) {
			*best		= ball;								// nächste Kugel
			min_dist = dist;								// bisheriger Minimalabstand
		}
	}

	return sqrt(min_dist);
}

int Ball::FitsAt(const Vec2 &pos) {
Ball *best;
Real dist;

	dist = FindClosest(this,pos,&best);
	return (dist<(2.0*R()+EPS))?0:1;
}

int Ball::FitsNextTo(const Vec2 &pos, const Vec2 &delta, Vec2 *newpos ) {

	*newpos = pos;
	do {
		if (FitsAt(*newpos))		return 1;
		*newpos += delta;
	}
	while(1);		// the check against the table boundary is missing
}


int Ball::Unlock( PBallTop *q ) {
	if (pball==q) {
		pball = 0;
		return 0;
	}
	else {
		return 1;
	}
}

void Ball::GoUnlocked() {
	if (pball)	pball->StopMoving();
}

void Ball::Draw() {
	state->Show();
}
