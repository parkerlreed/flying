#ifndef _global_h
#	include "global.h"
#endif
#ifndef _rgb_h
#	include "rgb.h"
#endif

#define	RGB_FILE		"rgb.txt"

int RGBColor::SetColor( const char *name ) {
FILE	*fp;
char	buffer[80];
char	*bp;
int	name_len = strlen(name);

	if (SetInternalColor(name))		return 0;

	fp = fopen( RGB_FILE, "r" );
	if (!fp) {
		fp = fopen( "\\borlandc\\bgi\\" RGB_FILE, "r" );
		if (!fp) {
			fp = fopen( "\\" RGB_FILE, "r" );
		}
	}
	if (!fp) {
		fprintf( stderr, "missing colorfile '%s'\n", RGB_FILE );
		return -1;
	}

	while( (bp=fgets( buffer, sizeof(buffer), fp))!=0 ) {
		if ( (buffer[24+name_len]=='\n')
			&& (!strncmp( buffer+24, name, name_len )) )	break;	// gesucher Name ?
	}
	fclose(fp);

	if (bp) {
		// BINGO
		if (sscanf(buffer, "%d %d %d", &red, &green, &blue)==3)	return 0;
		fprintf( stderr, "bad syntax for colorname '%s'\n", name );
		fprintf( stderr, "%s\n", buffer );
		return -1;
	}

	fprintf( stderr, "colorname '%s' not found.\n", name );
	return -1;
}


typedef struct {
	const char		*name;
	unsigned short	r,g,b;
} RGBValue;

static RGBValue coltab[] = {
{ "Brown", 165, 42, 42 },
{ "DarkGoldenrod", 184, 134, 11 },
{ "DarkGoldenrod1", 255, 185, 15 },
{ "DarkGreen", 0, 86, 45 },
{ "DarkViolet", 148, 0, 211 },
{ "DeepSkyBlue", 0, 191, 255 },
{ "HotPink3", 205, 96, 144 },
{ "LightSalmon", 255, 160, 122 },
{ "LightSalmon2", 238, 149, 114 },
{ "LightSteelBlue", 124, 152, 211 },
{ "LightSteelBlue3", 162, 181, 205 },
{ "LightSteelBlue4", 110, 123, 139 },
{ "LimeGreen", 0, 175, 20 },
{ "Maroon", 143, 0, 82 },
{ "MediumAquamarine", 0, 147, 143 },
{ "MediumForestGreen", 50, 129, 75 },
{ "MidnightBlue", 47, 47, 100 },
{ "Navy", 35, 35, 117 },
{ "PaleTurquoise", 175, 238, 238 },
{ "RosyBrown", 188, 143, 143 },
{ "SeaGreen", 82, 149, 132 },
{ "SteelBlue", 84, 112, 170 },
{ "Tan", 222, 184, 135 },
{ "White", 255, 255, 255 },
{ "bisque", 255, 228, 196 },
{ "black", 0, 0, 0 },
{ "blue", 0, 0, 255 },
{ "brown", 165, 42, 42 },
{ "brown4", 139, 35, 35 },
{ "coral2", 238, 106, 80 },
{ "coral3", 205, 91, 69 },
{ "coral4", 139, 62, 47 },
{ "cyan", 0, 255, 255 },
{ "gold1", 255, 215, 0 },
{ "green4", 0, 139, 0 },
{ "grey", 126, 126, 126 },
{ "ivory", 255, 255, 240 },
{ "orange", 255, 135, 0 },
{ "red", 255, 0, 0 },
{ "red3", 205, 0, 0 },
{ "red4", 139, 0, 0 },
{ "white", 255, 255, 255 },
{ "white", 245, 245, 245 },
{ "yellow", 255, 255, 0 },
{ "yellow1", 255, 255, 0 },
{ 0, 0, 0, 0 }			// end coltab
};

int RGBColor::SetInternalColor( const char *flag_name ) {
	if (!strncmp(flag_name,"rgb:",4)||!strncmp(flag_name,"rgb:",4)) {
		if (sscanf(flag_name+4,"%x/%x/%x",&red,&green,&blue)==3) {
			switch (strlen(flag_name)) {
#if (0)
			case 9:						/* 6+3*1 */
				red   |= (red<<4);
				green |= (green<<4);
				blue  |= (blue<<4);
			case 12:						/* 6+3*2 */
				red   |= (red<<8);
				green |= (green<<8);
				blue  |= (blue<<8);
			case 18:						/* 6+3*4 */
#else
	// The class was actually meant for DOS-purposes and doesn't store more
	// than the 8-Bit resolution of the rgb-values.
			case 9:						/* 6+3*1 */
				red   |= (red<<4);
				green |= (green<<4);
				blue  |= (blue<<4);
				break;
			case 18:						/* 6+3*4 */
				red   >>= 8;						
				green >>= 8;
				blue  >>= 8;
				break;
			case 12:						/* 6+3*2 */
#endif
				return 1;
			}
		}
		printf( "can't convert color '%s'\n", flag_name );
		return 0;
	}
	for (unsigned i=0;i<sizeof(coltab)/sizeof(RGBValue)-1;i++) {
		if (!strcmp(flag_name,coltab[i].name)) {
			red   = coltab[i].r;
			green = coltab[i].g;
			blue  = coltab[i].b;
			return 1;
		}
	}
	return 0;
}
