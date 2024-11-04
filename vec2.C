#ifndef _global_h
#	include "global.h"
#endif

#ifndef _vec2_h
#	include "vec2.h"
#endif

Vec2 Vec2Zero( RealZero, RealZero );

//
// Aufsplittung des Vektors in 2 Vektoren parallel und vertikal zum
// Richtungsvektor d. Dazu wird folgendes Gleichungssytem gelöst:
//
//	   vx        dx        -dy
//	  (  ) =	l *(  ) + u *(   )
// 	vy        dy         dx
//
//     V()  =   x     +   y
//
void Vec2::Split( const Vec2 &d, Vec2 *vx, Vec2 *vy ) const
{
Real	l,u;

	if (d.Y()!=RealZero) {
		l  = (   Y() + (   X() * d.X() / d.Y() ) )
			/ ( d.Y() + ( d.X() * d.X() / d.Y() ) );
		u  = ( l * d.X() - X() ) / d.Y();
		*vx = Vec2( l*  d.X() , l*d.Y() );
		*vy = Vec2( u*(-d.Y()), u*d.X() );
	}
	else {
		if (d.X()!=RealZero) {
			*vx = Vec2( X(), RealZero );		// parallel zur X-Achse
			*vy = Vec2(RealZero, Y() );
		}
		else {
			*vx = *this;					// keine Richtung -> gesamter Vektor ist x
			*vy = Vec2Zero;
		}
	}
}

//
// Analog zur kompletten Aufsplittung wird in der folgenden
// Version von split nur der parallele Anteil zurückgeliefert.
//
void Vec2::Split( const Vec2 &d, Vec2 *vx ) const
{
Real	l;

	if (d.Y()!=RealZero) {
		l  = (   Y() + (   X() * d.X() / d.Y() ) )
			/ ( d.Y() + ( d.X() * d.X() / d.Y() ) );
		*vx = Vec2( l*  d.X() , l*d.Y() );
	}
	else {
		if (d.X()!=RealZero) {
			*vx = Vec2( X(), RealZero );		// parallel zur X-Achse
		}
		else {
			*vx = *this;					// keine Richtung -> gesamter Vektor ist x
		}
	}
}


//
// Berechnung des Winkels, den der angegebene Punkt zur aktuellen Korrdinate
// hat. Ergebnis liegt zwischen 0 und 2*M_PI
//
Real Vec2::AngleRadial( const Vec2 &d ) const
{
Real	erg;

	if (d.X()!=X())	erg = atan( (Y()-d.Y())/(d.X()-X()) );
	else					erg = (d.Y()<Y())?M_PI_2:3*M_PI_2;		// Fehler behoben ???
	if (d.X()<X())		erg+= M_PI;
	if (erg<RealZero)	erg+= 2*M_PI;
	return erg;
}

Vec2 Vec2::TurnAngleRad( const Real &angle ) const
{
	if (!IsZero()) {
			Real	len = Norm();
			Real	ang = Vec2Zero.AngleRadial(*this) + angle;

			return Vec2( len*cos(ang), -len*sin(ang) );
	}
	else	return *this;
}

//
// Lösung des Gleichungssystems:	p1+t1*d1 = p2+t2*d2
// nach den beiden "Zeiten" t1 und t2
//
int Vec2::Solve(	const Vec2 &p1, const Vec2 &d1,
							const Vec2 &p2, const Vec2 &d2, Real *t1 )
{
		if (d1.X()!=RealZero) {
			Real div = d2.Y()-d2.X()/d1.X()*d1.Y();
			if (div==RealZero)		{ *t1=RealZero; return 1; }		// parallel
			*t1 = ( p1.Y()-p2.Y()+
					(p2.X()-p1.X())/d1.X()*d1.Y() )
					/ div;
		}
		else {
			Real div = d2.X()	/* -d2.Y()/d1.Y()*d1.X() */;
			if (div==RealZero)		{ *t1=RealZero; return 1; }		// parallel
			*t1 = ( p1.X()-p2.X()
					/* + (p2.Y()-p1.Y())/d1.Y()*d1.X() */ )
					/ div;
		}
		return 0;											// Ergebnis ok.
}

#if (0)
int Vec2::Project( const Vec2 &p1, const Vec2 &d1,
							const Vec2 &p2, Real *t1 )
{
	return Solve(p1,d1,p2,d1.TurnLeft(),t1);
}
#endif
