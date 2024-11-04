#ifndef _rgb_h
#define _rgb_h

class RGBColor {
	public:
		int SetColor( const char *name );

		RGBColor()							{ red=green=blue = 0; }
		RGBColor(int r, int g, int b)	{ red=r; green=g; blue=b; }
		RGBColor( const char *name )	{ SetColor(name); }

		~RGBColor()		{}

		int	red;
		int	green;
		int	blue;

	private:
		int SetInternalColor( const char *name );
};

#endif
