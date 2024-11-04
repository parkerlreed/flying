
#ifndef _global_h
#	include "global.h"
#endif
#ifndef _real_h
#	include "real.h"
#endif

#ifndef RealZero
const Real RealZero = 0.0;
#endif


FunTab::FunTab( double (*fkt) (double), double from_in, double to_in, int step_in ) {

	from		= from_in;
	to   		= to_in;
	step		= step_in;
	interval = (to-from)/Real((double)step);

	val = new Real [step+1];

	for (int i=0;i<=step;i++) {
		val[i] = fkt( from+interval*(double)i );
	}
}


FunTab::~FunTab() {
	if (val)		delete [] val;
}

const Real &FunTab::GetVal( const Real &in ) const  {
	double	m = fmod( in, 2.0*M_PI );
	while( m<0 )	m+= M_PI*2.0;
	int ind = (int)((m-from)/interval+0.5);
	return val[ind];
}

Real FunTab::GetRezVal( const Real &in ) const {
int	hi = step;
int	lo = 0;

int	mid = (hi+lo)/2;

	while( mid!=lo ) {
		if (val[mid]>in)	hi=mid;
		else					lo=mid;
		mid = (hi+lo)/2;
	}
	return (double)mid*interval+from;
}
