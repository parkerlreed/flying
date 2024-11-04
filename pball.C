#ifndef _global_h
#	include "global.h"
#endif

#ifndef _pball_h
#	include "pball.h"
#endif
#ifndef _ball_h
#	include "ball.h"
#endif
#ifndef _graph_h
#	include "graph.h"
#endif
#ifndef _game_h
#	include "game.h"
#endif

#define	SHOW_MODE		1
#ifndef STATISTICS
#	if (SHOW_MODE)
#		undef  SHOW_MODE
#		define SHOW_MODE	0
#	endif
#endif
#define	NEW_FACTOR		2.0

static const int OLD = 1;
static const int NEW = 0;

PBallTop *PBallTop::pball_queue=0l;
int PBallTop::id_count = 0;

PBallTop::PBallTop(PBallType type):
aim_hint(1), hint_valid(0)
{
	PBallTop::id = PBallTop::id_count++;
	next  		= pball_queue;
	pball_queue	= this;

	pball_type	= type;			/* 0-Queue 1-DiscSlider 2-DiscThrower */
	target_time	= NO_TARGET;

	cue = 0l;
	mode = Unlocked;
	default_cue = new Ball( 0.0, 0.0, 0.0, 0.0, 0.1 );

	tool_is_visible      = 0;
	valid_queue_position = 0;
}


PBallTop::~PBallTop() {
PBallTop	*last;
//
// Pointer aus Listen entfernen
//
	if (pball_queue==this)		pball_queue = next;
	else {
		last = pball_queue;
		while ( last->next && last->next != this )      last = last->next;
		last->next = next;
	}
//
// eigenes aufräumen
//
	if (default_cue)		delete default_cue;
}


void PBallTop::CueMoved() {
	if (mode&UsingTool)			MoveAimingTool();
}

void PBallTop::CueWasHit() {
//
// Der cueball war an einer Kollision beteiligt und veränderte seine Richtung
// Im PullMoving-Mode bedeutet dies, das die Maus an eine andere Stelle
// gewarpt werden muß, damit der cueball immer noch kontinuierlich in
// Mausrichtung gezogen wird.
//
	if (mode&PullMoving) {
#ifndef __TURBOC__
		if (target_time!=NO_TARGET) {
			dest = cue->P()+cue->V()*(target_time-current_time);
			Warp( dest );			// internen X-Pointer verschieben
			SetPointer( (int)(dest.X()*w2n), (int)(dest.Y()*w2n) );
										// Darstellung verschieben
		}
#endif
	}
}


void PBallTop::WarpRecalc() {
	if (dest!=cue->P()) {
		target_time = GetCurrentTime() + WarpTime;	// Offset bis zum Zielpunkt
		cue->ChgV(((dest-cue->P())/(target_time-current_time)));

#ifdef DEBUG
		if (debug&PointerMove) {
		printf( "PointerMove: P()=(%g,%g), V()=(%5.1f,%5.1f), dest=(%g, %g) (in %g secs)\n",
					(double)cue->PX(), (double)cue->PY(),
					(double)cue->VX(), (double)cue->VY(),
					(double)dest.X(), (double)dest.Y(),
					(double)(target_time-current_time) );
		}
#endif
	}
	else {
		target_time = NO_TARGET;
#ifdef DEBUG
		if (debug&PointerMove) {
			printf( "PointerMove: P()=(%g,%g) - destination reached\n",
						(double)cue->PX(), (double)cue->PY() );
		}
#endif
	}
}

void PBallTop::PointerMoveTo( const Vec2 &pointer_position ) {
	dest = pointer_position;
	SetPointer( (int)(dest.X()*w2n), (int)(dest.Y()*w2n) );
	if (mode&UsingTool)				MoveAimingTool();
	else if (mode&Moving)			cue->SetP(dest);
	else if (mode&PullMoving) {
		WarpRecalc();
	}
}


void PBallTop::ForAll( PBallFun fun ) {
	for (PBallTop *pball=pball_queue; pball; pball=pball->next)	(pball->*fun)();
}


void PBallTop::Press(int button) {
	switch(button) {
	case 1:
		switch(pball_type) {
		case BillardQueue:
			switch(mode) {
			case Aiming:				StartCharging(OLD);	break;
			case Charging:				StartShooting(NEW);	break;
			case Unlocked:				StartAiming();			break;
			default:						break;
			}
			break;
		case DiscSlider:
		case DiscThrower:
			if (mode!=PullMoving)	StartPullMoving();
			break;
		}
		break;

#if (0)
	case 2:		// Middle Button
		if (PBallTop::id==0) {
			CloseGraphic();
			exit(0);
		}
		break;
#endif

	case 3:		// Right Button
		switch(mode) {
		case Unlocked:
#if (1)
											StartPullMoving();
#else
		// PullMoving is not yet debugging in the DOS-version
											StartMoving();
#endif
											break;
		case Aiming:					StartCharging(NEW);	break;
		default:
			break;
		}
		break;	// case 3 - Right Button
	}
}


void PBallTop::Release(int button) {
	switch(button) {
	case 1:		// Left Button
		switch(mode) {
		case LockedQueue:				GoUnlocked();				break;
		case OldCharging: 			StartShooting(OLD);		break;
		case Shooting:					RestartCharging();		break;
		case PullMoving:				if (pball_type==DiscThrower)	StopMoving();
											break;
		default:							break;
		}
		break;

	case 3:		// Right Button
		switch(mode) {
		case PullMoving:				if (pball_type==DiscSlider)	break;
		case Moving:					StopMoving();						break;
		case Charging:
		case OldCharging:
		case Shooting:
		case OldShooting:
											GoUnlocked();			break;
		default:							break;
		}
		break;
	}
}


void PBallTop::Update() {
	switch(mode) {
		case OldCharging:
		case Charging:
		case OldShooting:
		case Shooting:
			charge += charge_increment*(current_time-last_chargestep);
			last_chargestep=current_time;
			next_chargestep=last_chargestep+g->GetChargeGranularity();

#ifndef __TURBOC__
#ifdef STATISTICS
{			char	buffer[30];
			sprintf( buffer, "Charge: %5.1f%%", (double)(charge/g->GetMaxCharge()*100.0) );
			showinfo(PointerInfo,buffer);
}
#endif
#endif
			MoveAimingTool();		// Ausgabe des Ladezustands
			if (mode==OldCharging) {
				if (charge>g->GetMaxCharge())	{
						charge = charge / 6.0;	// Überlauf -> Kraft reduzieren
						MoveAimingTool();			// Ausgabe des Ladezustands
						StartShooting(OLD);
				}
			}
			else {
				if (charge<RealZero) {
						Shoot();
				}
				else if (charge>g->GetMaxCharge())	charge=charge/2.0;
			}
			break;

		case PullMoving:
			if (target_time<current_time) {
				WarpRecalc();
			}
			break;

		case LockedQueue:
			if (lock_time<current_time) {
				GoUnlocked();
			}
			break;
		default:	
			break;
	}
}

void PBallTop::Redraw() {
	tool_is_visible      = 0;
	valid_queue_position = 0;
	if (mode&UsingTool)			DrawAimingTool();
	RedrawPointer();
}


#if (0)
void PBallTop::SetIndirect() {
	mode=Indirect;
#if (0)
	Draw();
	m = cue->R()*R()*R()/8;
	Draw();
#endif
#if (SHOW_MODE)
	showinfo(PointerInfo,"Indirect       ");
#endif
}

void PBallTop::SetDirect() {
	mode=Direct;
	target_time=NO_TARGET;
	Draw();
	m = 1;
	Draw();
	PointerMoveTo( dest );
	Warp( dest );
#if (SHOW_MODE)
	showinfo(PointerInfo,"Direct         ");
#endif
}
#endif

Ball *PBallTop::FindCueBall() {
Ball	*best;
Real	min_dist=Ball::FindClosest(cue,dest,&best);

	if (min_dist<2.*best->R()) {
		if (g->IsSelectable(best))		return best;
	}
	return 0;
}


void PBallTop::StartAiming( Ball *cueball ) {
	if (cueball)					cue = cueball;
	else								cue = FindCueBall();		// naechste Kugel suche
	if (cue&&cue->Lock(this))	cue=0;						// Lock erlaubt
	if (!cue)						return;

	mode    = Aiming;
	aim_hint = 1;
	charge  = 0.0;
	StartAimingTool();
#if (SHOW_MODE)
	showinfo(PointerInfo,"Aiming         ");
#endif
}

void PBallTop::StartCharging( int old ) {
	mode = (old)?OldCharging:Charging;
	charge = 0.0;
	if (old)	 charge_increment = g->GetChargeSpeed();
	else		 charge_increment = g->GetChargeSpeed()/NEW_FACTOR;
	last_chargestep  = current_time;
	next_chargestep  = last_chargestep + g->GetChargeGranularity();
#if (SHOW_MODE)
	showinfo(PointerInfo,"Charging       ");
#endif
}

void PBallTop::RestartCharging() {
	mode = Charging;
	charge_increment = g->GetChargeSpeed();
}

void PBallTop::StartShooting( int old ) {
	save_charge = charge;
	if (old)		charge_increment = -(charge/g->GetShootTime());
	else			charge_increment = -(charge/g->GetShootTime()/NEW_FACTOR);
	mode = (old)?OldShooting:Shooting;
}

void PBallTop::Shoot() {
Vec2	dir = dest-cue->P();

	if (!dir.IsZero()) {
		g->ShootBall( cue );				// neuen Schuß anzeigen
		cue->TellPressed();				// Pressed-Kugel dem Game-Objekt mitteilen
		cue->ChgV(dir.Norm1()*save_charge);
	}

	charge   = RealZero;					// Damit der Queue sichtbar
	EndAimingTool();						// Löschen (inkl. Vorausberechnung)

	default_cue->m = cue->m*4.;
	ReleaseBall();							// Verbindung zur Kugel abbrechen
	cue = default_cue;
	cue->idle = 0;
	cue->SetP( q_end );					// Hilfsball an Queue-Spitze
	cue->ChgV( Vec2Zero );	// ... ohne Bewegung
	cue->Lock(this);						// ... Verfolgung einschalten

	aim_hint = 0;
	mode = LockedQueue;
	StartAimingTool();

	lock_time = current_time + LockedQueueDelay;		// nachgelocken

#if (SHOW_MODE)
	showinfo(PointerInfo, "LockedQueue    " );
#endif
}


void PBallTop::StartPullMoving( Ball *cueball ) {
	if (cueball)					cue = cueball;
	else								cue = FindCueBall();
	if (cue&&cue->Lock(this))	cue=0;						// Lock erlaubt
	if (!cue)			return;

	mode = PullMoving;
#if (SHOW_MODE)
	showinfo(PointerInfo,"PullMoving     ");
#endif
}


void PBallTop::StartMoving( Ball *cueball ) {
	if (cueball)		cue = cueball;
	else					cue = FindCueBall();
	if (!cue)			return;

	mode = Moving;
	cue->idle = 1;		// Ball vom Tisch
	cue->SetP(dest);
	cue->ChgV(Vec2Zero);
#if (SHOW_MODE)
	showinfo(PointerInfo,"Moving         ");
#endif
}


void PBallTop::StopMoving() {
	cue->idle = 0;								// Ball wieder auf dem Tisch
	if (pball_type!=DiscThrower)
					cue->ChgV(Vec2Zero);		// Ball gegebenenfalls stoppen
	GoUnlocked();
}

void PBallTop::ReleaseBall() {
	if (cue) {
		cue->Unlock(this);
		if (cue==default_cue) {
			default_cue->idle = 1;
			default_cue->SetP( Vec2Zero );
			default_cue->ChgV( Vec2Zero );
		}
		cue=0l;
	}
}

void PBallTop::GoUnlocked() {
	if (mode&UsingTool) {
		EndAimingTool();		// Löschen des Aiming-Tools
		ReleaseBall();
	}
	else if (mode==Moving) {
		Ball	*nextb;
		Real	dist = Ball::FindClosest( cue, dest, &nextb );

		if (nextb&&dist<nextb->R()+cue->R()) {
			Vec2 dir = (nextb->P()-cue->P()).Norm1();
			nextb->ChgV( dir*(nextb->R()+cue->R()-dist)*4.0 );
			cue->ChgV( -dir*(nextb->R()+cue->R()-dist)*4.0 );
		}
		cue=0l;
	}
	else if (mode==PullMoving) {
		target_time = NO_TARGET;
		ReleaseBall();
	}

	mode = Unlocked;
#if (SHOW_MODE)
	showinfo(PointerInfo,"Unlocked       ");
#endif
}


void PBallTop::PreCalc() {
Vec2	dir = dest-cue->P();

	ncalc_pos=0;
	balls_dir=Vec2Zero;

	if (!dir.IsZero()) {
		Vec2	p_save = cue->P();
		Vec2	v_save = cue->v;
		Real		slowstep_save = cue->next_slowstep;
		Real		next_time;
		Real		min_time;
		Object	*hit_object;

		cue->v = dir.Norm1();
		Object *obj;

		for (ncalc_pos=0;ncalc_pos<MAX_PREPOS;) {
			min_time   = MAX_TIME;
			hit_object = 0;
			for (obj=Object::stat_queue; obj; obj=obj->Object::next ) {
				if (obj!=cue) {
					next_time = obj->HitFromBall(cue);
					if ( next_time < min_time ) {
							min_time   = next_time;
							hit_object = obj;
					}
				}
			}
			if (min_time<MAX_TIME) {
				cue->p = cue->P() + min_time*cue->V();
			}
			else {
				break;
			}

			calc_pos[ncalc_pos++] = cue->P();
			if (hit_object->dyn_id!=-1) {
				if (hit_object->dyn_id>=0) {
					Vec2 e = cue->V().Norm1();
					e.Split(((Ball*)hit_object)->P()-cue->P(),&balls_dir,&my_dir);
				}
				break;
			}
			cue->next_slowstep = SUPPRESS_SLOWSTEP;
			hit_object->CollideWithBall(cue);
		}

		cue->next_slowstep = slowstep_save;
		cue->v = v_save;
		cue->p = p_save;
	}
}




#define	CHARGE_LEN	20.0
#define	QUEUE_LEN	50.0

void PBallTop::DrawQueue() {
	if (valid_queue_position) {
#ifdef __TURBOC__
		DrawLine(q_s1_s,q_end_s);
		DrawLine(q_s2_s,q_end_s);
#else
		FillPoly( 3, &q_end_s, &q_s1_s, &q_s2_s );
#endif
	}
}

void PBallTop::StartQueue(const Vec2 &end, const Vec2 &s1, const Vec2 &s2) {
	q_end_s = end;
	q_s1_s  = s1;
	q_s2_s  = s2;
	valid_queue_position = 1;
	DrawQueue();
}

void PBallTop::MoveQueue(const Vec2 &end, const Vec2 &s1, const Vec2 &s2) {
	DrawQueue();
	q_end_s = end;
	q_s1_s  = s1;
	q_s2_s  = s2;
	valid_queue_position = 1;
	DrawQueue();
}

void PBallTop::EndQueue() {
	DrawQueue();
	valid_queue_position = 0;
}

static void DrawArrow( const Vec2 &from, const Vec2 &dist ) {
Vec2	to = from+8.0*dist*4.0;
Vec2	pa = dist.TurnAngleDeg( 150.)*4.0;
Vec2	pb = dist.TurnAngleDeg(-150.)*4.0;

	DrawLine(from,to);
	DrawLine(to,to+pa);
	DrawLine(to,to+pb);
}


void PBallTop::DrawAimingTool(int charge_only) {
#ifdef __TURBOC__
#else
	gc_current = gc_cursor;
#endif
	Vec2 dir= old_dest - aim_p;

	if (!dir.IsZero()) {
		Real		len		= dir.Norm();
		Vec2	dir_norm	= dir / len;		// <=> Norm1()
	//	Vec2	mid		= aim_p+dir/2.0;
		Vec2	off = dir_norm*QUEUE_LEN/50.0;
		Vec2	off1=off.TurnLeft();
		Vec2	off2=off.TurnRight();

		if (len<6.*aim_r)	hint_valid=0;

		if ((!aim_hint||!hint_valid)&&(aim_mode!=LockedQueue)) {
			DrawLine(aim_p,old_dest);
		}

		if (aim_mode==LockedQueue||((aim_mode&ChargeOrShoot)&&(aim_charge>=0.0))) {
						q_end		= aim_p
						- dir_norm*(aim_r+aim_charge/g->GetMaxCharge()*CHARGE_LEN);
			Vec2	q_start	= q_end - QUEUE_LEN*dir_norm;
			Vec2	qs1 = q_start+off1;
			Vec2	qs2 = q_start+off2;
			if (!tool_is_visible) {
				if (valid_queue_position)	MoveQueue(q_end,qs1,qs2);
				else								StartQueue(q_end,qs1,qs2);
			}
		}

		if (!charge_only && hint_valid) {
			int	i;
#ifndef __TURBOC__
			XSetLineAttributes(dpy,gc_cursor,0,LineOnOffDash,CapRound,JoinRound);
#else
			setlinestyle(DASHED_LINE,0,1);
#endif
			DrawLine(aim_p,calc_pos[0]);
			for (i=1;i<ncalc_pos;i++) {
				DrawLine(calc_pos[i-1],calc_pos[i]);
			}
#ifndef __TURBOC__
			DrawCircle( calc_pos[i-1], aim_r );
			XSetLineAttributes(dpy,gc_cursor,0,LineSolid,CapRound,JoinRound);
			if ((!nohint_flag)&&(ncalc_pos<2)&&!balls_dir.IsZero()) {
#else
			setlinestyle(SOLID_LINE,0,1);
			if ((!nohint_flag)&&(ncalc_pos<3)&&!balls_dir.IsZero()) {
#endif
				DrawArrow( calc_pos[ncalc_pos-1], balls_dir );
				DrawArrow( calc_pos[ncalc_pos-1], my_dir );
			}
		}
	}
	tool_is_visible ^= 1;	// toggle state
}


void PBallTop::SaveAimingState() {
	aim_p			= cue->P();	// Werte retten
	aim_r			= cue->R();
	old_dest		= dest;
	aim_charge	= charge;
	int_charge	= (int)(charge/2.);
	aim_mode    = mode;
}


void PBallTop::StartAimingTool() {
	if (aim_hint) {
		PreCalc();
		hint_valid=1;
	}
	else {
		hint_valid=0;
	}
	SaveAimingState();
	DrawAimingTool();
}

void PBallTop::MoveAimingTool() {
int	same_pos = (mode==aim_mode)&&((aim_p==cue->P())&&(dest==old_dest));

	if (same_pos&&((int)(charge/2.)==int_charge))
		return;				// keine Änderung

	DrawAimingTool(same_pos);		// Löschen der alten Zeichnung
	if (aim_hint) {
			if (!same_pos||!hint_valid)	PreCalc();
			hint_valid=1;
	}
	else	hint_valid=0;
	SaveAimingState();
	DrawAimingTool(same_pos);		// Neuzeichnen
}

void PBallTop::EndAimingTool() {
	DrawAimingTool();
	EndQueue();
}


#ifndef __TURBOC__
#	include "xpball.C"
#else
#	include "dospball.C"
#endif
