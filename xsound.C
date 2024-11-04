#ifndef _global_h
#	include "global.h"
#endif

// to get: FIONREAD
#include <sys/ioctl.h>
// to get: unlink()
// #include <unistd.h>

#include <X11/Xlib.h>

#ifdef SOLARIS
#	include <sys/filio.h>
#endif

#define	QUIT	"quit"

#define	DBG	0

static XKeyboardState	old_keyboard_values;
static Display	*xsound_dpy;
static pid_t pid;
static int pipefd[2];
static int sound_on=0;
static void local_usleep( long time )
{
struct timeval  timeout;
int    nfound;

   timeout.tv_sec  = (long)0;
   timeout.tv_usec = (long)time;

   nfound=select(0,0,0,0,&timeout);
}

static void sound_loop( char *disp_name ) {
XKeyboardControl	values;
char		buffer[80];
int		valid;
int		pitch, percent,duration;
int		pri, old_pri;
int		len;

	xsound_dpy = XOpenDisplay(disp_name);
	if (!xsound_dpy) {
		fprintf( stderr, "can't open display\n" );
		exit(-1);
	}
	XGetKeyboardControl(xsound_dpy,&old_keyboard_values);

	valid=0;
	do {
		old_pri = -1;

		len=read(0,buffer+valid,sizeof(buffer)-valid);
		if (len<=0)		goto exit_sound;
#if (DBG)
		printf( "* at least %d bytes\n", len);
		fflush(stdout);
#endif

		do {
			valid += len;
			char	*lf = strchr(buffer,'\n');

			while (lf) {
				*lf = '\0';
#if (DBG)
				printf( "* %s\n", buffer );
				fflush(stdout);
#endif
				if ( !strcmp(buffer,QUIT) )		goto exit_sound;
				if ( sscanf(buffer,"%d %d %d %d",&pitch,&percent,&duration,&pri)
										==4 ) {
					if (pri>old_pri) {
						values.bell_pitch=pitch;
						values.bell_percent=percent;
						values.bell_duration=duration;
						old_pri=pri;
					}
				}
				valid-=(lf-buffer+1);
#if (0)
	//
	// If you got trouble with memmove, than you've got to change this switch
	//
				memmove(buffer,lf+1,valid);
#else
				for (int i=0;i<valid;i++)	buffer[i]=lf[i+1];
#endif

				lf = strchr(buffer,'\n');
			}

			ioctl(0,FIONREAD,&len);
			if (len)	len=read(0,buffer+valid,sizeof(buffer)-valid);
		} while(len>0);
	
#if (DBG)	
		printf( "* ------------------------\n" );
		fflush(stdout);
#endif
		XChangeKeyboardControl(xsound_dpy,KBBellPercent|KBBellPitch|KBBellDuration,&values);
		XBell(xsound_dpy,values.bell_percent);
		XFlush(xsound_dpy);
		local_usleep(200000);
	}
	while(1);

exit_sound:

#if (0)
	values.bell_pitch    = old_keyboard_values.bell_pitch;
	values.bell_percent  = old_keyboard_values.bell_percent;
	values.bell_duration = old_keyboard_values.bell_duration;
#else
	values.bell_pitch    = 400;
	values.bell_percent  =  50;
	values.bell_duration = 100;
#endif
	XChangeKeyboardControl(xsound_dpy,KBBellPercent|KBBellPitch|KBBellDuration,&values);

	XCloseDisplay( xsound_dpy );
	printf( "* subprocess xsound termiated\n" );
	exit(0);
}



void init_sound( char *disp_name ) {

	pipe (pipefd);

	pid=fork();
	if (pid == (pid_t)0) {
		printf( "* subprocess xsound started\n" );
		close(0);					// stdin schlieﬂen		(Sohn)
		dup (pipefd[0]);
		close (pipefd[1]);
		sound_loop(disp_name);
	}
	else {
		close (pipefd[0]);		//								(Vater)
		sound_on = 1;
	}
}

void end_sound() {
	if (!sound_on)		return;
	write( pipefd[1], QUIT "\n", strlen(QUIT)+1 );
	close( pipefd[1] );
}


void do_sound( int pitch, int percent, int duration, int pri ) {
char buffer[80];

	if (!sound_on)		return;
	sprintf( buffer, "%d %d %d %d\n", pitch, percent, duration, pri );
	write( pipefd[1],buffer,strlen(buffer));
}
