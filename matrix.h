#ifndef _matrix_h
#define _matrix_h

#ifndef _vector_h
#	include "vector.h"
#endif

//
// -------------------------------------------------------------------------
// class Matrix : Matrixklasse als Liste gleichdimensionierter Vektoren
// -------------------------------------------------------------------------
//


class Matrix {
	protected:
		short		dim;
		Vector	*data;

	public:
		Matrix()									{ dim=0; }
		Matrix( int n, int m, ... );
		Matrix( const Matrix &m );
		~Matrix()								{ if (dim)	delete [] data; }

		void show(const char *str=0) const;

		int Rows() const						{ return dim; }
		int Cols() const;

	protected:
		void Resize(int i);
		void Resize(int i, int j);
		Vector &GetRef(int i);
	public:
		const Vector &operator()(int i) const
				{ return (i>=0&&i<dim)?data[i]:VectorZero; }
		Vector &operator[](int i)
				{ return (i>=0&&i<dim)?data[i]:GetRef(i); }

		int	IsZero() const;


	const Matrix& operator=(const Matrix &v);		// Copy
	const Matrix& operator<<=(Matrix &v);			// Move

	Matrix Matrix::operator+(const Matrix&) const;
	Matrix Matrix::operator-(const Matrix&) const;

	Matrix Matrix::operator*(const Real&) const;
	friend Matrix operator*(const Real&, const Matrix&);
	Matrix Matrix::operator/(const Real&) const;

	friend int operator==(const Matrix&, const Matrix&);
	friend int operator!=(const Matrix&, const Matrix&);
	const Matrix& operator+=(const Matrix&);
	const Matrix& operator-=(const Matrix&);
	const Matrix& operator*=(const Real&);
	const Matrix& operator/=(const Real&);
	Matrix Matrix::operator+() const;
	Matrix Matrix::operator-() const;

	friend Vector operator*(const Vector&, const Matrix&);
	friend Vector operator*(const Matrix&, const Vector&);
	const Matrix &operator*=(const Matrix&);
	friend Matrix operator*(const Matrix&, const Matrix&);

	friend Vector operator/(const Vector &b, const Matrix &a);
	friend int system_calc(Matrix &a, Vector *x, Vector &b);
};

inline Matrix Matrix::operator+() const
{
	return *this;
}
inline Matrix Matrix::operator-() const
{
Matrix	v(*this);
	return v*=Real(-1.0);
}


// Definitions of non-member binary operator functions

inline Matrix Matrix::operator+(const Matrix& z2) const
{
	if (dim>z2.dim) {
		Matrix	v(*this);
					return v+=z2;
	}
	else {
		Matrix	v(z2);
					return v+=*this;
	}
}
inline Matrix Matrix::operator-(const Matrix& z2) const
{
Matrix	v(*this);
			return v-=z2;
}

inline Matrix Matrix::operator*(const Real& r) const
{
Matrix	erg(*this);
			erg *= r;
			return erg;
}

inline Matrix Matrix::operator/(const Real& r) const
{
Matrix	erg(*this);
			erg /= r;
			return erg;
}


inline Matrix operator*(const Real& val, const Matrix& z2)
{
		return z2*val;		// kommutativ
}

inline int operator!=(const Matrix& z1, const Matrix& z2)
{
		return !(z1==z2);
}


extern Matrix MatrixZero;

#endif
