#ifndef _global_h
#	include "global.h"
#endif

#include <stdarg.h>

#ifndef _matrix_h
#	include "matrix.h"
#endif

Matrix MatrixZero;

Matrix::Matrix( int n, int m, ... )
{
va_list	argptr;

	va_start(argptr,m);
	dim = n;
	if (dim) {
		data = new Vector [n];
		for (int i=0;i<n;i++) {
			data[i].Resize(m);
			for (int j=0;j<m;j++) {
				data[i][j] = Real( va_arg(argptr,double) );
			}
		}
	}
	va_end(argptr);
}

Matrix::Matrix( const Matrix &m ) {
	dim  = m.dim;
	data = new Vector[dim];
	for (int i=0;i<dim;i++)		data[i] = m.data[i];
}

void Matrix::show(const char *str) const {
	putchar( '[' );
	for (int i=0;i<dim;i++) {
		if (i)	printf( ", " );
		data[i].show();
	}
	putchar(']');
	if (str)		printf(str);
}

int Matrix::Cols() const {
int	max=0;

	for (int i=0;i<dim;i++)		if (data[i].dim>max)		max=data[i].dim;
	return max;
}

int Matrix::IsZero() const {
	for (int i=0;i<dim;i++) {
		if (data[i]!=VectorZero)	return 0;
	}
	return -1;
}

void Matrix::Resize(int n) {
	if (n>dim) {
		Vector	*new_data;

		new_data = new Vector[n];                          // allocate new array
		for (int i=0;i<dim;i++)	new_data[i] = data[i];		// copy old data
//		for (			;i<n;i++)	new_data[i] = VectorZero;	// reset new data
		delete [] data;												// free old array
		data = new_data;
		dim  = n;
	}
}
void Matrix::Resize(int n,int m) {
	Resize(n);
	for (int i=0;i<dim;i++)		data[i].Resize(m);
}


Vector &Matrix::GetRef(int n) {
static Vector error=VectorZero;

	if (n<0) {
		fprintf(stderr,"illegal index: Matrix::GetRef(%d)\n",n);
		return error;				// return at least something to play with ...
	}
	if (n>=dim)	Resize(n+1);
	return data[n];
}


const Matrix &Matrix::operator=(const Matrix &m) {
	if (dim<m.dim) {
		if (dim) delete [] data;
		data = new Vector[m.dim];
	}
	dim = m.dim;
	for (int i=0;i<dim;i++)		data[i] = m.data[i];
	return *this;
}

int operator==(const Matrix& z1, const Matrix& z2)
{
		if (z1.dim<z2.dim) {
			for (int i=0;i<z1.dim;i++)
				if (z1.data[i]!=z2.data[i])	return 0;
			for (       ;i<z2.dim;i++)
				if (z2.data[i]!=VectorZero)	return 0;
			return -1;
		}
		else {
			for (int i=0;i<z2.dim;i++)
				if (z1.data[i]!=z2.data[i])	return 0;
			for (       ;i<z1.dim;i++)
				if (z1.data[i]!=VectorZero)	return 0;
			return -1;
		}
}

const Matrix& Matrix::operator+=(const Matrix& z2)
{
	if (z2.dim>dim)	Resize(z2.dim);
	for (int i=0;i<dim;i++)			data[i]+=z2.data[i];
	return *this;
}
const Matrix& Matrix::operator-=(const Matrix& z2)
{
	if (z2.dim>dim)	Resize(z2.dim);
	for (int i=0;i<dim;i++)			data[i]-=z2.data[i];
	return *this;
}
const Matrix& Matrix::operator*=(const Real& r)
{
	for (int i=0;i<dim;i++)			data[i]*=r;
	return *this;
}
const Matrix& Matrix::operator/=(const Real& r)
{
	for (int i=0;i<dim;i++)			data[i]/=r;
	return *this;
}

Vector operator*(const Vector& v, const Matrix& m) {
Vector	erg;

	erg.Resize(m.Cols());
	for (int i=0;i<erg.dim;i++) {
		Real	sum = RealZero;
		for (int j=0;j<v.dim;j++)		sum += v(j)*m(j)(i);
		erg[i] = sum;
	}
	return erg;
}

Vector operator*(const Matrix& m, const Vector& v) {
Vector	erg;

		erg.Resize(m.dim);
		for (int i=0;i<m.dim;i++)			erg[i] = m(i)*v;
		return erg;
}

const Matrix &Matrix::operator<<=(Matrix &m2) {
	if (dim)		delete [] data;
	dim  = m2.dim;
	data = m2.data;
	m2.dim  = 0;
	return *this;
}

const Matrix &Matrix::operator*=(const Matrix &m2) {
Matrix help;

	help <<= *this;

int	cols = m2.Cols();

	Resize(help.dim,cols);
	for (int i=0;i<dim;i++) {
		for (int j=0;j<cols;j++) {
			Real	sum = RealZero;
			for (int k=0;k<m2.dim;k++)		sum += help(i)(k)*m2(k)(j);
			(*this)[i][j] = sum;
		}
	}
	return *this;
}

Matrix operator*(const Matrix& m1, const Matrix& m2) {
Matrix erg;
int	cols = m2.Cols();

	erg.Resize(m1.dim,cols);
	for (int i=0;i<erg.dim;i++) {
		for (int j=0;j<cols;j++) {
			Real	sum = RealZero;
			for (int k=0;k<m2.dim;k++)		sum += m1(i)(k)*m2(k)(j);
			erg[i][j] = sum;
		}
	}
	return erg;
}


Vector operator/(const Vector &b, const Matrix &a) {
Matrix	a_tmp = a;
Vector	b_tmp = b;
Vector	x;

	if (system_calc(a_tmp,&x,b_tmp))		return VectorZero;
	else											return x;
}

int system_calc(Matrix &a, Vector *x, Vector &b) {
int		n;				// Dimension
int		*v;			// Vertauschungsvektor
int		i,j,k,help;

// Überprüfung der Vektorgrößen

	n = b.dim;
	if (n!=a.Rows()||n!=a.Cols()) {
		fprintf(stderr,"Ax=b: Fehler in Dimension A[%d][%d], b[%d]\n", a.Rows(), a.Cols(), n );
		return -1;
	}

// Initialisierung des Vertauschungsvektors

	v = new int[n];
	for (i=0;i<n;i++)		v[i]=i;


	for (j=0;j<n-1;j++) {
		Real	pvt = RealZero;
		int	j2 = j;

	// skalierte Pivotsuche

		for (i=j;i<n;i++) {
			Real	a2;
			Real	d = RealZero;

		// Zeilenmaximum für Skalierung bestimmen

			for (k=j;k<n;k++) {
				if (Real(fabs(a(v[i])(k)))>d)		d=fabs(a(v[i])(k));
			}
			if (d<EPS) {
				fprintf(stderr,"Ax=b: Matrix singulär\n");
				delete v;
				return -2;
			}

		// skaliertes Pivotelement berechnen und mit bisher größtem vergleichen

			a2 = a(v[i])(j)/d;
			if (Real(fabs(a2))>pvt) {
				pvt = fabs(a2);
				j2  = i;				// Pivotzeile merken
			}
		}

	// Pivotisierung und fiktiever Zeilentausch

		help  = v[j];
		v[j]  = v[j2];
		v[j2] = help;
		if (fabs(a(v[j])(j))<EPS) {
			fprintf(stderr,"Ax=b: Matrix singulär\n" );
			delete v;
			return -3;
		}

	// v[j] ist Pivotzeile

		for (i=j+1;i<n;i++) {
			Real	q = a(v[i])(j)/a(v[j])(j);
			for (k=j+1;k<n;k++) {
				a[v[i]][k] -= q*a(v[j])(k);
			}
			b[v[i]] -= q*b(v[j]);
		}
	}

	x->Resize(n);
	for (i=n-1;i>=0;i--) {
		(*x)[v[i]] = b(v[i]);
		for (k=i+1;k<n;k++) {
			(*x)[v[i]] -= a(v[i])(k) * (*x)(v[k]);
		}
		(*x)[v[i]] /= a(v[i])(i);
	}

	delete v;
	return 0;
}

