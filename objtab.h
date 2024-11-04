#ifndef _objtab_h
#define _objtab_h

#ifndef _real_h
#	include "real.h"
#endif
#ifndef _object_h
#	include "object.h"
#endif

#define DIRECT_ACCESS 	1


class ObjTab {
public:
	ObjTab()	: size(0) { };
	~ObjTab();

	void Init(int size);
	int GetSize()				{ return size; }

	Object *&operator[](unsigned i);

	Real &GetTime(unsigned i) const;
	void SetTime(unsigned i, const Real &time_val);
	void SetTimeDirect(unsigned i, const Real &time_val);

	void StartRecalc();
	void ToFront( int i );
	void ToBack( int i );
	void StopRecalc();

private:
	int		size;				// Größe der Felder
	Object	**obj;			// Feld mit den Objektzeigern
	Real		*time;			// Feld mit den dazu gehörenden Zeiten

#if (!DIRECT_ACCESS)
	unsigned	*ind;				// Indexfeld bei normalen Zugriffen
	unsigned	*new_ind;		// Feld zu Erstellung der neuen Index-Liste
	unsigned	front, back;	// Indizes innerhalb der Index-Liste
#endif
};

#if (DIRECT_ACCESS)
inline Object *&ObjTab::operator[](unsigned i)  {	return obj[i];	}

inline Real &ObjTab::GetTime(unsigned i) const
											{	return time[i];	}
inline void ObjTab::SetTime(unsigned i, const Real &time_val)
											{	time[i] = time_val; }
inline void ObjTab::SetTimeDirect(unsigned i, const Real &time_val)
											{	time[i] = time_val; }

inline void ObjTab::StartRecalc()		{ }
inline void ObjTab::ToFront( int )		{ }
inline void ObjTab::ToBack( int )		{ }
inline void ObjTab::StopRecalc()			{ }
#else
inline Object *&ObjTab::operator[](unsigned i)
											{	return obj[ind[i]];	}

inline Real &ObjTab::GetTime(unsigned i) const
											{	return time[ind[i]];	}
inline void ObjTab::SetTime(unsigned i, const Real &time_val)
											{	time[ind[i]] = time_val; }
inline void ObjTab::SetTimeDirect(unsigned i, const Real &time_val)
											{	time[i] = time_val; }

inline void ObjTab::StartRecalc()		{ front=size; back=0; }
inline void ObjTab::ToFront( int i )	{ new_ind[--front]=ind[i]; }
inline void ObjTab::ToBack( int i )		{ new_ind[back++]=ind[i]; }
inline void ObjTab::StopRecalc() {
		memcpy( &ind[0], &new_ind[front], (size-front)*sizeof(unsigned) );
		memcpy( &ind[size-front], &new_ind[0],  back*sizeof(unsigned) );
}
#endif


#endif
