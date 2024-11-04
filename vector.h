#ifndef _vector_h
#define _vector_h

#ifndef _real_h
#	include "real.h"
#endif

//
// -------------------------------------------------------------------------
// class Vector : Vektorklasse für beliebig dimensionierte Vektoren
// -------------------------------------------------------------------------
//

class Vec2;
class Vec3;

class Vector {
	protected:
		short	dim;
		Real	*data;
		static Real	ErrorZero;

	public:
		Vector()									{ dim=0; }
		Vector( int n, double d0, ... );
		Vector( const Vector &v );
#if (Vec2IsVector==0)
		Vector( const Vec2 &v );
#endif
#if (Vec3IsVector==0)
		Vector( const Vec3 &v );
#endif
		~Vector()								{ if (dim)	delete [] data; }

	void show(const char *str=0) const;

	protected:
		void Resize(int i);
		Real &GetRef(int i);
	public:
		const Real &operator()(int i) const
				{ return (i>=0&&i<dim)?data[i]:ErrorZero; }
		Real &operator[](int i)
				{ return (i>=0&&i<dim)?data[i]:GetRef(i); }

		int	IsZero() const;

		Real SqrNorm() const;
		Real Norm() const;
		Vector Norm1() const;


	const Vector& operator=(const Vector &v);		// Copy
	const Vector& operator<<=(Vector &v);			// Move

	inline Vector operator+(const Vector&) const;
	inline Vector operator-(const Vector&) const;

	friend Real operator*(const Vector&, const Vector&);
	inline Vector operator*(const Real&) const;
	friend inline Vector operator*(const Real&, const Vector&);
	inline Vector operator/(const Real&) const;

	friend int operator==(const Vector&, const Vector&);
	friend inline int operator!=(const Vector&, const Vector&);
	const Vector& operator+=(const Vector&);
	const Vector& operator-=(const Vector&);
	const Vector& operator*=(const Real&);
	const Vector& operator/=(const Real&);
	inline Vector operator+() const;
	inline Vector operator-() const;


friend class Matrix;
	friend Vector operator*(const class Matrix& m, const Vector& v);
	friend Vector operator*(const Vector& v, const class Matrix& m);
	friend int system_calc(class Matrix &a, Vector *x, Vector &b);
};

inline Vector Vector::operator+() const
{
	return *this;
}
inline Vector Vector::operator-() const
{
Vector	v(*this);
	return v*=Real(-1.0);
}


// Definitions of non-member binary operator functions

inline Vector Vector::operator+(const Vector& z2) const
{
	if (dim>z2.dim) {
		Vector	v(*this);
					return v+=z2;
	}
	else {
		Vector	v(z2);
					return v+=*this;
	}
}
inline Vector Vector::operator-(const Vector& z2) const
{
Vector	v(*this);
			return v-=z2;
}

inline Vector Vector::operator*(const Real& r) const
{
Vector	erg(*this);
			erg *= r;
			return erg;
}

inline Vector Vector::operator/(const Real& r) const
{
Vector	erg(*this);
			erg /= r;
			return erg;
}


inline Vector operator*(const Real& val, const Vector& z2)
{
		return z2*val;		// kommutativ
}

inline int operator!=(const Vector& z1, const Vector& z2)
{
		return !(z1==z2);
}

inline Real Vector::Norm() const					{ return sqrt(SqrNorm()); }
inline Vector Vector::Norm1() const				{ return *this / Norm(); }

extern Vector VectorZero;

#endif
