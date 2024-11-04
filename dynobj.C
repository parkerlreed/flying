#ifndef _global_h
#	include "global.h"
#endif

#ifndef _dynobj_h
#	include "dynobj.h"
#endif
#ifndef _ball_h
#	include "ball.h"
#endif

//
// Statik der DynObj-Klasse
//
Ball		*DynObj::hit_ball=0L;		// an Kollision beteiligter Ball
Object 	*DynObj::hit_object=0L;		// weiteres an Kollision beteiligtes Objekt
DynObj 	*DynObj::dyn_queue = 0L;	// globale Liste der dynamischen Objekte
int 		DynObj::id_count = 0;		// aktuelle Objekt-Id

DynObj::DynObj() {
	id = id_count++;				// muss von der Unterklasse gelesen werden
	next = dyn_queue;		// alle Objekte in dynamische Liste ketten
	dyn_queue = this;
	idle=0;
}

DynObj::~DynObj() {
DynObj	*last;

	if ( dyn_queue == this )		dyn_queue = next;
	else {
		last = dyn_queue;			// Element in Dynamik-Liste suchen ...
		while ( last->next && last->next != this )	last = last->next;
		last->next = next;		// Element ausketten
	}
}
void DynObj::ForAllDyn( DynObjFun fun ) {
		for (DynObj *obj=dyn_queue; obj; obj=obj->next )	(obj->*fun)();
}
void DynObj::ForAllDyn( DynObjFunD fun, Real param ) {
   	for (DynObj *obj=dyn_queue; obj; obj=obj->next )	(obj->*fun)(param);
}

//
// CollisionAll
// -  Der naechste Collisionszeitpunkt soll bestimmt werden.
// -  Dazu wird jedes dynamische Objekt nach der naechsten Kollision befragt.
// -  Die Zeit wird ermittelt, indem sich das das Objekt an alle anderen Objekte
//    wendet und danach befragt, wann sie getroffen werden.
//
Real DynObj::CollisionAll() {

	Object::min_time = MAX_TIME;
	for (DynObj *obj=dyn_queue; obj; obj=obj->next )
		obj->NextCollision();
	return (Object::min_time<MAX_TIME)?Object::min_time-current_time:Real(MAX_TIME);
}

void DynObj::DynInitAll() {	ForAllDyn(&DynObj::DynInit);	}

void DynObj::DynEndAll() {
}

void DynObj::DynInit() {
#if (TIME_CACHE)
	col_objects = Object::id_count;
	obj_list.Init(col_objects);
	for (Object *obj=Object::stat_queue; obj; obj=obj->next )
			obj_list[obj->id] = obj;
#endif
}

//
// Collide
// Die Kollision an dem durch HitFromBallAll gefundenen Objekt findet statt.
//
void DynObj::Collide() {
	if (Object::min_time<MAX_TIME) {
#ifdef DEBUG
		if (debug&BeforCollision) {
		  	printf( "BEFOR: %9.5f, Collision %9.5f\n", current_time,
													(double)Object::min_time );
		 	hit_object->Info();
			hit_ball->Info();
		}
#endif
		hit_object->CollideWithBall( hit_ball );
		hit_ball->WasHit(hit_object);
#ifdef DEBUG
		if (debug&AfterCollision) {
		  	if (debug&BeforCollision)	 printf( "AFTER:\n" );
			else		printf( "AFTER: %9.5f, Collision %9.5f\n", current_time,
													(double)Object::min_time );
		   hit_object->Info();
		   hit_ball->Info();
		}
#endif
	}
}


void DynObj::Move( Real )		{ }
void DynObj::Reset()				{ }
void DynObj::Redraw()			{ }
