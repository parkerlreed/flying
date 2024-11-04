#ifndef _global_h
#	include "global.h"
#endif
#ifndef _logfile_h
#	include "logfile.h"
#endif
#ifndef USE_EXTERNAL_STANDARD_INCLUDES
#	include <stdio.h>
#endif

#include <errno.h>
#include <unistd.h>

static char		message[200];
static time_t	start_time;

void end_log() {
FILE		*fp1,*fp2;
char		buffer[200];
time_t	end_time;
// char		new_message[200];
double	dt;
int		hh, mm;

	time(&end_time);
	dt = end_time - start_time;
	mm = (int)(dt/60);
	hh = mm/60;
	mm = mm%60;
 
	if ( rename( LOG_FILE, LOG_FILE".BAK" ) ) {
		// perror( LOG_FILE );
	}
	fp1 = fopen( LOG_FILE, "w" );
	if (!fp1)	return;

	fp2 = fopen( LOG_FILE".BAK", "r" );
	if (fp2) {
		while ( fgets( buffer, sizeof(buffer), fp2 ) ) {
			if ( !strcmp(buffer, message) ) {
				message[strlen(message)-1]='\0';
				if (dt/60>1)
					fprintf( fp1, "%s (%d:%02d)\n", message, hh, mm );
			}
			else fputs( buffer, fp1 );
		}
		fclose(fp2);
	}
	unlink( LOG_FILE".BAK" );

	fclose( fp1 );
}

void start_log( const char *game ) {
FILE	*fp1,*fp2;
char	buffer[200];
const char	*user_string = getenv("USER");
	if (!user_string || *user_string=='\0')		user_string = "unknown";
const char	*dpy_string  = getenv( "DISPLAY" );
	if (!dpy_string || *dpy_string=='\0')	      dpy_string = "localhost:0.0";

	printf( "flying:\n" );
	printf( "\t%s", flying_descriptor_string+4 );
	printf( "\t%s", flying_version_string+4 );

	time(&start_time);
	strftime( buffer, sizeof(buffer), "%d-%m-%y %H:%M:%S", localtime(&start_time) );
	sprintf( message, "%-8s%s %s: %s started by ", game, buffer,
			dpy_string,revision );
	if ( !strcmp( user_string, "emshh" ) )		return;

	strcat( message, user_string );
	strcat( message, "\n" );

	if ( rename( LOG_FILE, LOG_FILE".BAK" ) ) {
		// perror( LOG_FILE );
	}
	fp1 = fopen( LOG_FILE, "w" );
	if (!fp1)	return;
	fputs( message, fp1 );

	fp2 = fopen( LOG_FILE".BAK", "r" );
	if (fp2) {
		while ( fgets( buffer, sizeof(buffer), fp2 ) ) {
			fputs( buffer, fp1 );
		}
		fclose(fp2);
	}
	unlink( LOG_FILE".BAK" );

	fclose( fp1 );

	// only for ANSI C, therefor commented out
	// atexit( end_log );
}
