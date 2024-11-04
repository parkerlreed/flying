#ifndef _global_h
#	include "global.h"
#endif

#ifndef _vec3_h
#	include "vec3.h"
#endif

Vec3 Vec3Zero( RealZero, RealZero, RealZero );

#if (Vec3IsVector)
Vec3::Vec3( const Real &l, const Real &b ) : Vector(3,0.0,0.0,0.0) {
Real	hr = sin(b/180.0*M_PI);		// Radius am 'Breitengrad' b;

	data[0] = hr*cos(l/180.0*M_PI);
	data[1] = hr*sin(l/180.0*M_PI);
	data[2] = cos(b/180.0*M_PI);			// Höhe, bestimmt durch Breitengrad
}
#else
Vec3::Vec3( const Real &l, const Real &b ) {
Real	hr = sin(b/180.0*M_PI);		// Radius am 'Breitengrad' b;

	x_val = hr*cos(l/180.0*M_PI);
	y_val = hr*sin(l/180.0*M_PI);
	z_val = cos(b/180.0*M_PI);			// Höhe, bestimmt durch Breitengrad

	norm  = RealZero;
}
#endif

//
// Berechnung des Winkels zwischen 2 Vec2en
//
Real Vec3::AngleRadTo( Vec3 &d )
{
Real	cos_phi = (*this)*d/(Norm()*d.Norm());

		return acos( cos_phi );
}


Real Vec3::AngleRad( const Real &dx, const Real &dy )
{
Real	erg;

#if (0)
static FunTab	*TAN=0;
	if (!TAN)			TAN = new FunTab( tan, -M_PI_2, M_PI_2, 64 );
#endif

	if (dx)		erg = atan( dy/dx );
	else					erg = (dy>=RealZero)?M_PI_2:3*M_PI_2;
	if (dx<RealZero)	erg+= M_PI;
	if (erg<RealZero)	erg+= 2*M_PI;
	return erg;
}


void Vec3::GetPolarRad( Real *l, Real *b ) const
{
	*l = AngleRad( X(), Y() );
	*b = AngleRad( Z(), sqrt( X()*X() + Y()*Y() ) );
}
void Vec3::GetPolarRad( Real *l, Real *b, Real *r )
{
	*r = Norm();
	GetPolarRad(l,b);
}



Vec3 Vec3::XTurnAngleRad( const Real &angle ) const
{
	if (Y()!=RealZero||Z()!=RealZero) {
			Real	len = sqrt( Y()*Y() + Z()*Z() );
			Real	ang = AngleRad(Y(),Z()) + angle;
#if (Vec3IsVector)
			return Vec3( X(), len*cos(ang), len*sin(ang) );
#else
			return Vec3( X(), len*cos(ang), len*sin(ang), norm );
#endif
	}
	else	return *this;
}
Vec3 Vec3::YTurnAngleRad( const Real &angle ) const
{
	if (X()!=RealZero||Z()!=RealZero) {
			Real	len = sqrt( X()*X() + Z()*Z() );
			Real	ang = AngleRad(X(),Z()) + angle;
#if (Vec3IsVector)
			return Vec3( len*cos(ang), Y(), len*sin(ang) );
#else
			return Vec3( len*cos(ang), Y(), len*sin(ang), norm );
#endif
	}
	else	return *this;
}
Vec3 Vec3::ZTurnAngleRad( const Real &angle ) const
{
	if (X()!=RealZero||Y()!=RealZero) {
			Real	len = sqrt( X()*X() + Y()*Y() );
			Real	ang = AngleRad(X(),Y()) + angle;
#if (Vec3IsVector)
			return Vec3( len*cos(ang), len*sin(ang), Z() );
#else
			return Vec3( len*cos(ang), len*sin(ang), Z(), norm );
#endif
	}
	else	return *this;
}
