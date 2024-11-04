#ifndef _global_h
#	include "global.h"
#endif

#ifndef _object_h
#	include "object.h"
#endif
#ifndef _ball_h
#	include "ball.h"
#endif
#ifndef _graph_h
#	include "graph.h"
#endif

//
// Statik der Object-Klasse
//
Object	*Object::stat_queue = 0L;		// globale Liste der statischen Obje
int		Object::id_count = 0;			// aktuelle Objekt-Id
Real		Object::min_time = MAX_TIME;	// Initialisierung



Object::Object() {
	type   = UnknownObj;
	id		 = id_count++;
	dyn_id = -1;
	next	 = stat_queue;		// alle Objekte zunaechst in statische Liste
	stat_queue = this;
}

Object::~Object() {
Object	*last;

	if ( stat_queue == this )		stat_queue = next;
	else {
		last = stat_queue;		// Element aus Statik-Liste entfernen
		while ( last->next && last->next != this )		last = last->next;
		last->next = next;		// Element ausketten
	}
Object	*obj;
	for (obj=stat_queue; obj; obj=obj->next ) {
		if (obj->id>id)		obj->id--;	// Object-Ids neu nummerieren
	}
	id_count--;
}

void Object::ForAll( ObjFun fun ) {
Object *obj;

	for (obj=stat_queue; obj; obj=obj->next ) 	(obj->*fun)();
}


Real Object::Collision()		{ return NO_HIT;	}

void Object::WasHit(Object*)	{ }

void Object::Info() {
	printf( "%02d: Static:     %08lx\n", id, (unsigned long)this );
}

void Object::Draw() {
#if (0)
	switch(type) {
	case StaticArcObj:
		StaticArc	*p = this;
		DrawArc( p->P(), p->r, p->start, p->angle );
		break;
	case StaticBallObj:
		StaticBall	*p = this
		DrawCircle( p->P(), p->R() );
		break;
	case WallObj:
		Wall			*p = this;
		DrawLine( p->p1, p->p2 );
		break;
	case PocketObj:
		Pocket		*p = this;
#ifndef __TURBOC__
		XSetLineAttributes(dpy,*ga,0,LineOnOffDash,CapRound,JoinRound);
#endif
		DrawCircle( p->P(), p->R() );
#ifndef __TURBOC__
		XSetLineAttributes(dpy,*ga,0,LineSolid,CapRound,JoinRound);
#endif
		break;
	case UnknownObj:
		break;
	}
#endif
}
