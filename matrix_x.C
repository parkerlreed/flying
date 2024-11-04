#include <stdio.h>
#include <stdlib.h>
#ifndef _global_h
#  include "global.h"
#endif
#include "vec2.h"
#include "vec3.h"
#include "matrix.h"

int main() {

Vec2		v21(1.2,3.4),v22(4.5,6.7),v23(8.9,0.0);

	v21 = 3.2*v22;

Vector	v( 3, 17.0, 17.0, 17.0 );

	v.show( "\n" );

Matrix	m( 3,3,	1.0, 0.5, 0.5, 1/3.0, 1.0, 1/3.0, 1/4.0, 1/4.0, 1.0 );
	m.show( "\n" );

//	m *= Real(4.0);
//	m.show( "\n" );

Vector	x = v/m;
	x.show( "\n" );


	(m*x).show("\n");	
	(x*m).show("\n");

	return 0;
};
