#ifndef _stdinc_h
#define _stdinc_h

#define	USE_EXTERNAL_STANDARD_INCLUDES
#define	_USE_OWN_STANDARD_INCLUDES


#ifdef USE_EXTERNAL_STANDARD_INCLUDES
// ===========================================================================
//
// The main include-files are included here and therfore in every other file.
//
// ===========================================================================
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#ifndef M_PI_2
#  define M_PI_2  (M_PI/2.0)
#endif
#ifndef __TURBOC__
#	include <X11/Xos.h>
	/* define X_GETTIMEOFDAY macro, a portable gettimeofday() */
	/* copied from Xos.h for pre-X11R6 releases               */
#	if defined(SVR4) || defined(VMS) || defined(WIN32)
#		define X_GETTIMEOFDAY(t) gettimeofday(t)
#	else
#		define X_GETTIMEOFDAY(t) gettimeofday(t, (struct timezone*)0)
#	endif

#else
#	include <string.h>
#	include <time.h>
#endif
#include <ctype.h>

#endif


#ifdef USE_OWN_STANDARD_INCLUDES
// ===========================================================================
//
// just for information, the library functions and their usage is
// stated below. Actually it was meant to use these lines instead of
// the actual includes on very slow machines, but this is very
// unportable, therefor it is not advised to use this switch
//
// ===========================================================================

#	ifndef  _TIME_T
#		define _TIME_T
		typedef long time_t;
#	endif
#	ifndef _SIZE_T
#		define _SIZE_T
		typedef unsigned int size_t;
#	endif

# ifndef _STRUCT_TIMEVAL
#  define _STRUCT_TIMEVAL
   /* Structure returned by gettimeofday(2) system call and others */
     struct timeval {
	  unsigned long	tv_sec;		/* seconds */
	  long		tv_usec;	/* and microseconds */
     };
# endif /* _STRUCT_TIMEVAL */

   struct timezone {
	int	tz_minuteswest;	/* minutes west of Greenwich */
	int	tz_dsttime;	/* type of dst correction */
   };

extern "C" {

// -----------
// from math.h
// -----------
// acos        : vec3.C
// atan        : vec2.C vec3.C
// cos         : vec2.C vec3.C
// fabs        : hockey.C keeper.C matrix.C
// sin         : vec2.C vec3.C
// sqrt        : vec2.h vec3.h vector.h
// fmod        : real.C
	double acos(double x);
	double atan(double x);
	double cos(double x);
	double fabs(double x);
	double sin(double x);
	double sqrt(double x);
	double fmod(double x, double y);
#  define M_PI_2  1.57079632679489661923

// -------------
// from stdlib.h
// -------------
// atof        : global.C main.C
// atoi        : main.C
// exit        : [ball.C] main.C xgraph.C xmover.C xpball.C
// rand        : ball.C main.C xgraph.C
// srand       : main.C
	double atof(const char *str);
	int atoi(const char *str);
	void exit(int status);
	int rand(void);
	void srand(unsigned int seed);

// -----------
// from time.h
// -----------
// difftime    : logfile.C
// gettimeofday: xgraph.C
// time        : logfile.C
	double difftime(time_t time1, time_t time0);
	int gettimeofday(
		struct timeval *tp,
		struct timezone *tzp
	);
	time_t time(time_t *tloc);

// ------------
// from stdio.h
// ------------
// fclose      : global.C rgb.C
// fflush      : xmover.C
// fgets       : global.C logfile.C rgb.C
// fopen       : global.C logfile.C rgb.C
// fprintf     : graph.C matrix.C rgb.C vector.C xpball.C
// fread       : xmover.C
// fwrite      : xmover.C
// printf      : arcs.C ball.C [carrom.C] [curling.C] [dynobj.C] game.C
//               global.C goal.C [graph.C] [hockey.C] main.C matrix.C object.C
//               [pball.C] pcon.C pocket.C vector.C wall.C xmover.C
// putchar     : matrix.C vector.C
// rename      : logfile.C
// sprintf     : [game.C] global.C main.C pball.C
// sscanf      : rgb.C
   typedef struct {
	int		 __cnt;
	unsigned char	*__ptr;
	unsigned char	*__base;
	unsigned short	 __flag;
	unsigned char 	 __fileL;		/* low byte of file desc */
	unsigned char 	 __fileH;		/* high byte of file desc */
   } FILE;
	extern FILE __iob[];
#  define   stdout   (&__iob[1])
#  define   stderr   (&__iob[2])

	int fclose(FILE *stream);
	int fflush(FILE *stream);
	char *fgets(char *s, int n, FILE *stream);
	FILE *fopen(const char *pathname, const char *type);
	int fprintf(FILE *stream, const char *format, /* [arg,] */ ...);
	size_t fread(void *ptr, size_t size, size_t nitems, FILE *stream);
	size_t fwrite(const void *ptr, size_t size, size_t nitems, FILE *stream);
	int printf(const char *format, /* [arg,] */ ...);
	int putchar(int c);
	int rename(const char *source, const char *target);
	int sprintf(char *s, const char *format, /* [arg,] */ ...);
	int sscanf(const char *s, const char *format, /* [pointer,] */ ...);


// ------------
// from ctype.h
// ------------
// isspace     : global.C
// isupper     : xgraph.C
// tolower     : xgraph.C
	int isspace(int c);
	int isupper(int c);
	int tolower(int c);



// -------------
// from string.h
// -------------
// memset      : xmover.C
// strcmp      : [global.C] graph.C logfile.C main.C rgb.C
// strlen      : global.C logfile.C rgb.C xgraph.C
// strncmp     : rgb.C
// strstr      : global.C
	void *memset(void *s, int c, size_t n);
	int strcmp(const char *s1, const char *s2);
	size_t strlen(const char *s);
	int strncmp(const char *s1, const char *s2, size_t n);
	char *strstr(const char *s1, const char *s2);

// ------------
// from errno.h
// ------------
// perror      : logfile.C
//
// local included

// -------------
// from stdarg.h
// -------------
// va_end      : matrix.C vector.C
// va_start    : matrix.C vector.C
//
// local included

};

#endif


#endif
