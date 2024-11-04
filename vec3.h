#ifndef _vec3_h
#define _vec3_h

#ifndef _real_h
#	include "real.h"
#endif

//
// -------------------------------------------------------------------------
// class Vec3 : 3-dimensionale Vektorklasse
// -------------------------------------------------------------------------
//
/*

						Z
					   |   Y
					   |  /
					   | /
					   |/
					---|------------X
					  /|

 */

#if (Vec3IsVector)

#ifndef _vector_h
#	include "vector.h"
#endif

class Vec3 : public Vector {
	public:

		const Real &X() const 		{ return data[0]; }
		const Real &Y() const 		{ return data[1]; }
		const Real &Z() const 		{ return data[2]; }

		Vec3( const Real &l, const Real &b );	// Polarkoordinaten-Konstruktor (Norm)
		Vec3( double x, double y, double z ) : Vector( 3, x, y, z ) {}
		Vec3( const Vector &v ) : Vector(v)		{ Resize(3); }
		Vec3()											{}

		const Vec3& operator=(const Vector &v)
			{ Vector::operator=(v); return *this; }

		static Real AngleRad( const Real &x, const Real &y );

		void GetPolarRad( Real *l, Real *b ) const ;
		void GetPolarRad( Real *l, Real *b, Real *r );
		void GetPolarDeg( Real *l, Real *b ) const {
			GetPolarRad(l,b);
			*l *= Real(180/M_PI);
			*b *= Real(180/M_PI);
		}
		void GetPolarDeg( Real *l, Real *b, Real *r ) {
			GetPolarRad(l,b,r);
			*l *= Real(180/M_PI);
			*b *= Real(180/M_PI);
		}

		Vec3 XTurnAngleRad( const Real &angle ) const;
		Vec3 XTurnAngleDeg( const Real &angle ) const
			{ return XTurnAngleRad(angle/Real(180/M_PI)); }
		Vec3 YTurnAngleRad( const Real &angle ) const;
		Vec3 YTurnAngleDeg( const Real &angle ) const
			{ return YTurnAngleRad(angle/Real(180/M_PI)); }
		Vec3 ZTurnAngleRad( const Real &angle ) const;
		Vec3 ZTurnAngleDeg( const Real &angle ) const
			{ return ZTurnAngleRad(angle/Real(180/M_PI)); }


		Real AngleRadTo( Vec3 &d );
		Real AngleDegTo( Vec3 &d )		{ return AngleRadTo(d)*Real(180.0/M_PI); }
};

#else

class Vec3 {
	private:
		Real	x_val;
		Real	y_val;
		Real	z_val;
//	protected:
		Real	norm;		// Cache zur optimierten Berechnen von Norm() wird der Wert
	public:

		const Real &X() const 		{ return x_val; }
		const Real &Y() const 		{ return y_val; }
		const Real &Z() const 		{ return z_val; }

		int	IsZero() const			{ return X()==0.0&&Y()==0.0&&Z()==0.0; }

		Vec3( const Real &l, const Real &b );	// Polarkoordinaten-Konstruktor (Norm)
		Vec3( const Real &x, const Real &y, const Real &z, Real n=0.0 )
											{ x_val=x; y_val=y; z_val=z; norm=n; }
		Vec3( const Vec3 &v )		{ x_val=v.X(); y_val=v.Y(); z_val=v.Z();
											  norm=v.norm; }
		Vec3()							{ }

		static Real AngleRad( const Real &x, const Real &y );

		void GetPolarRad( Real *l, Real *b ) const ;
		void GetPolarRad( Real *l, Real *b, Real *r );
		void GetPolarDeg( Real *l, Real *b ) const {
			GetPolarRad(l,b);
			*l *= Real(180/M_PI);
			*b *= Real(180/M_PI);
		}
		void GetPolarDeg( Real *l, Real *b, Real *r ) {
			GetPolarRad(l,b,r);
			*l *= Real(180/M_PI);
			*b *= Real(180/M_PI);
		}

		Vec3 XTurnAngleRad( const Real &angle ) const;
		Vec3 XTurnAngleDeg( const Real &angle ) const
			{ return XTurnAngleRad(angle/Real(180/M_PI)); }
		Vec3 YTurnAngleRad( const Real &angle ) const;
		Vec3 YTurnAngleDeg( const Real &angle ) const
			{ return YTurnAngleRad(angle/Real(180/M_PI)); }
		Vec3 ZTurnAngleRad( const Real &angle ) const;
		Vec3 ZTurnAngleDeg( const Real &angle ) const
			{ return ZTurnAngleRad(angle/Real(180/M_PI)); }

		Real SqrNorm() const;
		Real Norm();
		Vec3 Norm1();

		Real AngleRadTo( Vec3 &d );
		Real AngleDegTo( Vec3 &d )		{ return AngleRadTo(d)*Real(180.0/M_PI); }

	inline const Vec3& operator=(const Vec3 &v);

	// Binary Operator Functions

	inline Vec3 operator+(const Vec3&) const;
	inline Vec3 operator-(const Vec3&) const;

#ifndef __TURBOC__
	friend inline Real operator*(const Vec3&, const Vec3&);
	friend inline Vec3 operator*(const Real&, const Vec3&);
	friend inline int operator==(const Vec3&, const Vec3&);
	friend inline int operator!=(const Vec3&, const Vec3&);
#else
	friend Real operator*(const Vec3&, const Vec3&);
	friend Vec3 operator*(const Real&, const Vec3&);
	friend int operator==(const Vec3&, const Vec3&);
	friend int operator!=(const Vec3&, const Vec3&);
#endif
	inline Vec3 operator*(const Real&) const;
	inline Vec3 operator/(const Real&) const;

	inline const Vec3& operator+=(const Vec3&);
	inline const Vec3& operator-=(const Vec3&);
	inline const Vec3& operator*=(const Real&);
	inline const Vec3& operator/=(const Real&);
	inline Vec3 operator+() const;
	inline Vec3 operator-() const;

};

inline const Vec3& Vec3::operator=(const Vec3 &v) {
	x_val = v.x_val;
	y_val = v.y_val;
	z_val = v.z_val;
	norm  = 0.0;
	return *this;
}

inline Vec3 Vec3::operator+() const
{
	return *this;
}

inline Vec3 Vec3::operator-() const
{
	return Vec3(-x_val, -y_val, -z_val, norm);
}


// Definitions of compound-assignment operator member functions

inline const Vec3& Vec3::operator+=(const Vec3& z2)
{
	x_val += z2.x_val;
	y_val += z2.y_val;
	z_val += z2.z_val;
	norm   = 0.0;
	return *this;
}

inline const Vec3& Vec3::operator-=(const Vec3& z2)
{
	x_val -= z2.x_val;
	y_val -= z2.y_val;
	z_val -= z2.z_val;
	norm   = 0.0;
	return *this;
}

inline const Vec3& Vec3::operator*=(const Real& val)
{
	x_val *= val;
	y_val *= val;
	z_val *= val;
	norm  *= val;
	return *this;
}

inline const Vec3& Vec3::operator/=(const Real& val)
{
	x_val /= val;
	y_val /= val;
	z_val /= val;
	norm  /= val;
	return *this;
}


// Definitions of non-member binary operator functions

inline Vec3 Vec3::operator+(const Vec3& z2) const
{
		  return Vec3(x_val + z2.x_val, y_val + z2.y_val, z_val + z2.z_val);
}
inline Vec3 Vec3::operator-(const Vec3& z2) const
{
		  return Vec3(x_val - z2.x_val, y_val - z2.y_val, z_val - z2.z_val);
}


inline Real operator*(const Vec3& z1, const Vec3& z2)
{
		  return z1.x_val*z2.x_val + z1.y_val*z2.y_val + z1.z_val*z2.z_val;
}
inline Vec3 Vec3::operator*(const Real& val2) const
{
		  return Vec3(x_val*val2, y_val*val2, z_val*val2, norm*val2);
}
inline Vec3 operator*(const Real& val, const Vec3& z2)
{
		  return Vec3(z2.x_val*val, z2.y_val*val, z2.z_val*val);
}

inline Vec3 Vec3::operator/(const Real& val) const
{
		  return Vec3(x_val/val, y_val/val, z_val/val, norm/val);
}

inline int operator==(const Vec3& z1, const Vec3& z2)
{
		  return z1.x_val == z2.x_val && z1.y_val == z2.y_val && z1.z_val == z2.z_val;
}

inline int operator!=(const Vec3& z1, const Vec3& z2)
{
		  return z1.x_val != z2.x_val || z1.y_val != z2.y_val || z1.z_val != z2.z_val;
}

inline Real Vec3::SqrNorm() const	{ return X()*X()+Y()*Y()+Z()*Z(); }
inline Real Vec3::Norm() {
	if (norm==0.0)	norm=sqrt(SqrNorm());
	return norm;
}
inline Vec3 Vec3::Norm1() { return *this / Norm(); }

#endif

extern Vec3 Vec3Zero;

#endif
