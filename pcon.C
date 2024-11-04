#ifndef _global_h
#	include "global.h"
#endif

#ifndef _pcon_h
#	include "pcon.h"
#endif
#ifndef _pocket_h
#	include "pocket.h"
#endif
#ifndef _wall_h
#	include "wall.h"
#endif
#ifndef _graph_h
#	include "graph.h"
#endif


PocketConnector::~PocketConnector() {
	if (w1)	delete w1;
	if (b1)	delete b1;
	if (w2)	delete w2;
	if (b2)	delete b2;
	if (w3)	delete w3;
}

//
// RoundedSegment berechnet eine Wand und den nächsten Bogen eines zigzag's.
// Der Startpunkt ist dabei Eingabe-Parameter für den ersten Punkt und
// gleichzeitig Ausgabe-Parameter für den genauen Endpunkt des Bogens.
//
// e1, e2	- Punkt 1&2
// st			- Punkt 3 ein/aus
// r			- Radius der Rundung
// w			- erzeugtes Wandobjekt
// a			- erzeugtes Bogenobjekt
//
void PocketConnector::RoundedSegment( const Vec2 &e1, const Vec2 &e2,
		Vec2 *st,	const Real &r,
		Wall **w, StaticArc **a )
{
const Real off=RealZero;
Vec2	para;								// Hilfsvektoren
Vec2	d1 = (e1-(*st)).Norm1();	// Vektor parallel zur Start-Wand;
Vec2	d2 = (e2-e1).Norm1();		// Vektor parallel zur Zielwand

Vec2	o1 = d1.TurnLeft() * r;		// Abstandsvektor 1
Vec2	o2;								// Abstandsvektor 2

Real		off_r;							// tatsächlicher Radius (nach Verschiebung)

			o1.Split( d2, &para );	  	// Anteil in Richtung d2

			if ( (para.X()*d2.X()<RealZero) || (para.Y()*d2.Y()<RealZero) ) {
				// nächster Bogen nach rechts ...
						o1   *= -1.0;	// entgegen, dann umkehren
						o2    = d2.TurnRight() * r;
						off_r = r-off;
			}
			else {
				// nächster Bogen nach links ...
						o2    = d2.TurnLeft() * r;
						off_r = r+off;
			}

Real l;		// Faktor für: p = e1 + l*d2 - o2;

		if (Vec2::Solve( e1+o2, d2, (*st)+o1, d1, &l ))		return;

		if (l<RealZero) {
			printf( "ZigZag: Ecke: (%f,%f), Radius: %f zu gross (<%f)",
							(double)e1.X(), (double)e1.Y(), (double)r, (double)(l+r) );
		}

Vec2 p  = (*st)+o1 + l*d1;				// Mitte des Kreises
Vec2 p1 = p-o1;								// Endpunkt erstes Segment
Vec2 p2 = p-o2;								// Startpunkt zweites Segment
double a1, a2;									// Winkel zu den beiden Segmentpunkten
double ang;										// Winkel-Differenz

Vec2	from = (*st)-d1.TurnLeft()*off;
Vec2	to   = p1-d1.TurnLeft()*off;
		*w = new Wall( from.X(), from.Y(), to.X(), to.Y() );

		if (off_r>RealZero) {
			a1 = p.AngleDeg( p-o1 );
			a2 = p.AngleDeg( p-o2 );
			ang = a2-a1;
			if (ang<0)	ang+=360.;
			if (ang>180)	*a = new OuterArc( p.X(), p.Y(), off_r, a2, 360-ang );
			else				*a = new OuterArc( p.X(), p.Y(), off_r, a1, ang );
		}
		else					*a = new StaticBall( p.X(), p.Y(), 0.0 );

		*st = p2;								// Startpunkt zurueckgeben
}

//
// Analog zur pocket_connect-Routine aus creator.C wurde die Init-Methode
// des PocketConnector's aufgebaut. Allerdings werden keine gerundeten
// Ecken erzeugt.
//
//			  start                          end
//          /.\                            /
//		s_ang/   \                          /
//			 /     \                        /
//		  p1---> --\----------------------/----  p2
//			|  dir   \angle               /
//		off|			 \                  /
//			v	 		   ----------------
//				 		 egde1           edge2
//

#define	TANGENTIAL


int PocketConnector::Init( const Pocket &p1, const Real &angle1,
							 const Pocket &p2, const Real &angle2,
							 const Real &cushion, const Real &rad )
{
Real		ls;                                 // Ergebnis des Gleichungssystems
Vec2	dir	=(p2.P()-p1.P()).Norm1();		// Richtungsvektor
Vec2	off	= dir.TurnLeft()*cushion;		// Abstand der Wand

Vec2	s_ang = dir.TurnAngleDeg(angle1);	// Richtungsvektor zum Startpunkt

#ifdef TANGENTIAL
Vec2	start = p1.P()+s_ang*p1.R();			// Startpunkt
#else
Vec2	start = p1.P() + dir*p1.R();
#endif

		if (Vec2::Solve( start, s_ang.TurnRight(), p1.P()+off, dir, &ls ))	return 1;
Vec2	edge1 = p1.P()+off+dir*ls;


Vec2	s_end = Vec2(-dir).TurnAngleDeg(-angle2);// Richtungsvektor zum Endpunkt

#ifdef TANGENTIAL
Vec2	end   = p2.P()+s_end*p2.R();			// Endpunkt
#else
Vec2	end	= p2.P() - dir*p2.R();
#endif

		if (Vec2::Solve( end, s_end.TurnLeft(), p2.P()+off, dir, &ls ))	return 1;
Vec2	edge2 = p2.P()+off+dir*ls;

	RoundedSegment( edge1, edge2, &start, rad, &w1, &b1 );
	RoundedSegment( edge2,   end, &start, rad, &w2, &b2 );
	w3 = new Wall( start, end );

	return 0;
}

