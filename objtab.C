#ifndef _global_h
#	include "global.h"
#endif

#ifndef _objtab_h
#	include "objtab.h"
#endif


void ObjTab::Init(int size_val) {
int i;

	size = size_val;
	obj  = new Object*[size];
	time = new Real[size];
	for (i=0;i<size;i++) {
		obj[i]=0L;
		time[i]=MAX_TIME;
	}
#if (!DIRECT_ACCESS)
	ind  = new unsigned[size];
	new_ind = new unsigned[size];
	for (i=0;i<size;i++) {
		ind[i]=i;
	}
#endif
}

ObjTab::~ObjTab() {
	delete [] obj;
	delete [] time;
#if (!DIRECT_ACCESS)
	delete [size] ind;
	delete [size] new_ind;
#endif

}
