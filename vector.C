#ifndef _global_h
#	include "global.h"
#endif

#include <stdarg.h>

#ifndef _vector_h
#	include "vector.h"
#endif

Vector VectorZero;
Real Vector::ErrorZero = RealZero;


Vector::Vector( int n, double d0, ... )
{
va_list	argptr;

	va_start(argptr,d0);
	dim = n;
	if (dim) {
		data = new Real [dim];
		if (n>0)	data[0] = d0;
		for (int i=1;i<n;i++) {
			data[i] = Real( va_arg(argptr,double) );
		}
	}
	va_end(argptr);
}

Vector::Vector( const Vector &v ) {
	dim  = v.dim;
	data = new Real[dim];
	for (int i=0;i<dim;i++)		data[i] = v.data[i];
}

void Vector::show(const char *str) const {
	putchar('(');
	for (int i=0;i<dim;i++) {
		if (i)	putchar(',');
		printf( "%g", (double)data[i] );
	}
	putchar(')');
	if (str)		printf(str);
}

int Vector::IsZero() const {
	for (int i=0;i<dim;i++) {
		if (data[i]!=RealZero)	return 0;
	}
	return -1;
}

void Vector::Resize(int n) {
	if (n>dim) {
	int i;
		Real	*new_data;

		new_data = new Real[n];                           // allocate new array
		for (i=0;i<dim;i++)		new_data[i] = data[i];	// copy old data
		for (   ;i<n;i++)			new_data[i] = RealZero;	// reset new data
		if (dim)		delete [] data;							// free old array
		data = new_data;
		dim  = n;
	}
}

Real &Vector::GetRef(int n) {
static Real	error=RealZero;

	if (n<0) {
		fprintf(stderr,"illegal index: GetRef(%d)\n",n);
		return error;				// return at least something to play with ...
	}
	if (n>=dim)	Resize(n+1);
	return data[n];
}

Real Vector::SqrNorm() const {
Real	erg = RealZero;

	for (int i=0;i<dim;i++)		erg += data[i]*data[i];
	return erg;
}


const Vector &Vector::operator=(const Vector &v) {
	if (dim<v.dim) {
		if (dim) delete [] data;
		data = new Real[v.dim];
	}
	dim = v.dim;
	if (!dim)	delete [] data;
	else			for (int i=0;i<dim;i++)		data[i] = v.data[i];
	return *this;
}

const Vector &Vector::operator<<=(Vector &v) {
	if (dim)		delete [] data;
	dim  = v.dim;
	data = v.data;
	v.dim  = 0;
	return *this;
}

Real operator*(const Vector& z1, const Vector& z2)
{
int	min_dim = (z1.dim>z2.dim)?z1.dim:z2.dim;
Real	erg = RealZero;

		for (int i=0;i<min_dim;i++)	erg += z1.data[i]*z2.data[i];
		return erg;
}

int operator==(const Vector& z1, const Vector& z2)
{
		if (z1.dim<z2.dim) {
			int i;
			for (i=0;i<z1.dim;i++)
				if (z1.data[i]!=z2.data[i])	return 0;
			for (   ;i<z2.dim;i++)
				if (z2.data[i]!=RealZero)		return 0;
			return -1;
		}
		else {
			int i;
			for (i=0;i<z2.dim;i++)
				if (z1.data[i]!=z2.data[i])	return 0;
			for (   ;i<z1.dim;i++)
				if (z1.data[i]!=RealZero)		return 0;
			return -1;
		}
}

const Vector& Vector::operator+=(const Vector& z2)
{
	if (z2.dim>dim)	Resize(z2.dim);
	for (int i=0;i<dim;i++)			data[i]+=z2.data[i];
	return *this;
}
const Vector& Vector::operator-=(const Vector& z2)
{
	if (z2.dim>dim)	Resize(z2.dim);
	for (int i=0;i<dim;i++)			data[i]-=z2.data[i];
	return *this;
}
const Vector& Vector::operator*=(const Real& r)
{
	for (int i=0;i<dim;i++)			data[i]*=r;
	return *this;
}
const Vector& Vector::operator/=(const Real& r)
{
	for (int i=0;i<dim;i++)			data[i]/=r;
	return *this;
}


#if (Vec2IsVector==0)

#ifndef _vec2_h
#	include "vec2.h"
#endif

Vector::Vector( const Vec2 &v ) {
	dim = 2;
	data = new Real [dim];
	data[0] = v.X();
	data[1] = v.Y();
}

#endif

#if (Vec3IsVector==0)

#ifndef _vec3_h
#	include "vec3.h"
#endif

Vector::Vector( const Vec3 &v ) {
	dim = 3;
	data = new Real [dim];
	data[0] = v.X();
	data[1] = v.Y();
	data[2] = v.Z();
}

#endif
