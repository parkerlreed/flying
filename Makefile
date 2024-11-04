### There is a special SOLARIS-switch (added by Blair Zajac)

### directory for writing temporary data and pixmap files

DATA_DIRECTORY = '"/tmp"'

### compiler section
### for debugging, there might be the additional flags -DDEBUG and -DSTATISTICS
### the -O2 option doesn't work correct on gcc-2.7.0
CXX			= gcc -Wall -fstrength-reduce -fpcc-struct-return
CXXFLAGS	   = -O1
#CXX			= CC
#CXXFLAGS	= -O

### parameter type of select-parameters (have to be casted on HP-UX)

FDS_TYPE		= ''
# FDS_TYPE		= '(int*)'

### Directories for include files of X11 and libraries

X_INCLUDE	= /usr/X11R6/include
# X_INCLUDE		= /usr/include/X11R5

XLIB_DIR		= /usr/X11R6/lib
# XLIB_DIR		= /usr/lib/X11R5

###################################################################

LDFLAGS		= -L$(XLIB_DIR)

.SUFFIXES: .C .o

.C.o:
	$(CXX) -I$(X_INCLUDE) -DDATA_DIRECTORY=$(DATA_DIRECTORY) -DFDS_TYPE=$(FDS_TYPE) $(CXXFLAGS) -c $<

LD	      = $(CXX)

LIBS	      = -lX11 -lm

OBJS	= arcs.o \
		ball.o \
		billard.o \
		cannon.o \
		carrom.o \
		curling.o \
		dynobj.o \
		game.o \
		global.o \
		goal.o \
		graph.o \
		hockey.o \
		keeper.o \
		logfile.o \
		main.o \
		mover.o \
		object.o \
		objtab.o \
		pball.o \
		pcon.o \
		pocket.o \
		pool.o \
		pool8.o \
		real.o \
		rgb.o \
		snooker.o \
		vec2.o \
		vec3.o \
		vector.o \
		wall.o \
		xbench.o \
		xsound.o

PROGRAM       = flying

all:		$(PROGRAM)

$(PROGRAM): $(OBJS)
		$(LD) $(LDFLAGS) $(OBJS) $(LIBS) -o $(PROGRAM)

clean:
		-rm -f core *.o $(PROGRAM)
		-rm -f fly.dta/*.*

### dependencies ###

arcs.o: global.h   \
	  arcs.h object.h real.h \
	vec2.h vector.h ball.h dynobj.h objtab.h game.h keeper.h
ball.o: global.h   \
	  ball.h arcs.h object.h \
	real.h vec2.h vector.h dynobj.h objtab.h game.h keeper.h mover.h \
	graph.h xgraph.h  \
	xsound.h    \
	xmover.h  pball.h xpball.h 
billard.o: global.h   \
	 billard.h real.h game.h vec2.h \
	 vector.h keeper.h wall.h object.h graph.h \
	xgraph.h   \
	 xsound.h  \
	  mover.h xmover.h
cannon.o: global.h   \
	 cannon.h real.h billard.h game.h vec2.h \
	 vector.h keeper.h ball.h arcs.h object.h dynobj.h \
	objtab.h wall.h graph.h xgraph.h  \
	xsound.h    \
	mover.h xmover.h 
carrom.o: global.h   \
	 carrom.h real.h vec2.h  \
	vector.h game.h keeper.h wall.h object.h graph.h xgraph.h \
	 xsound.h  \
	  mover.h xmover.h \
	 arcs.h pocket.h ball.h dynobj.h objtab.h
curling.o: global.h   \
	 curling.h vec2.h  real.h \
	vector.h game.h keeper.h arcs.h object.h ball.h dynobj.h objtab.h \
	goal.h wall.h graph.h xgraph.h  \
	xsound.h    \
	mover.h xmover.h 
dynobj.o: global.h   \
	 dynobj.h real.h objtab.h object.h ball.h arcs.h \
	vec2.h  vector.h
game.o: global.h   \
	 game.h vec2.h  real.h \
	vector.h keeper.h wall.h object.h ball.h arcs.h dynobj.h objtab.h \
	graph.h xgraph.h  \
	xsound.h   
global.o: global.h   \
	  real.h cannon.h billard.h \
	game.h vec2.h  vector.h keeper.h carrom.h goal.h \
	wall.h object.h hockey.h curling.h pball.h xpball.h  pool.h \
	pool8.h snooker.h pocket.h ball.h arcs.h dynobj.h objtab.h
goal.o: global.h   \
	 goal.h wall.h object.h real.h vec2.h \
	 vector.h ball.h arcs.h dynobj.h objtab.h game.h \
	keeper.h
graph.o: global.h   \
	 object.h real.h graph.h vec2.h \
	 vector.h xgraph.h  \
	xsound.h    \
	pball.h xpball.h  xgraph.C pointer.h stipple.h game.h \
	keeper.h \
	rgb.h
hockey.o: global.h   \
	 hockey.h vec2.h  real.h \
	vector.h game.h keeper.h arcs.h object.h ball.h dynobj.h objtab.h \
	goal.h wall.h graph.h xgraph.h  \
	xsound.h    \
	mover.h xmover.h 
keeper.o: global.h   \
	 ball.h arcs.h object.h real.h vec2.h \
	 vector.h dynobj.h objtab.h wall.h keeper.h game.h \
	graph.h xgraph.h  \
	xsound.h   
logfile.o:   \
	  global.h \
	logfile.h
main.o: global.h   \
	  dynobj.h real.h objtab.h \
	object.h graph.h vec2.h vector.h xgraph.h \
	 xsound.h  \
	  mover.h xmover.h \
	 game.h keeper.h snooker.h pool.h billard.h cannon.h \
	pool8.h carrom.h hockey.h curling.h pball.h xpball.h 
mover.o: global.h   \
	 mover.h real.h graph.h object.h vec2.h \
	 vector.h xgraph.h  \
	xsound.h    \
	xmover.h  xmover.C vec3.h
object.o: global.h   \
	 object.h real.h ball.h arcs.h vec2.h \
	 vector.h dynobj.h objtab.h graph.h xgraph.h \
	 xsound.h
objtab.o: global.h   \
	 objtab.h real.h object.h
pball.o: global.h   \
	 pball.h vec2.h  real.h \
	vector.h xpball.h  ball.h arcs.h object.h dynobj.h objtab.h \
	graph.h xgraph.h  \
	xsound.h    \
	game.h keeper.h xpball.C cursor.h
pcon.o: global.h   \
	 pcon.h real.h pocket.h ball.h arcs.h object.h \
	vec2.h  vector.h dynobj.h objtab.h keeper.h wall.h \
	graph.h xgraph.h  \
	xsound.h   
pocket.o: global.h   \
	 pocket.h ball.h arcs.h object.h real.h vec2.h \
	 vector.h dynobj.h objtab.h keeper.h game.h
pool.o: global.h   \
	 pool.h real.h billard.h game.h vec2.h \
	 vector.h keeper.h pcon.h pocket.h ball.h arcs.h \
	object.h dynobj.h objtab.h wall.h graph.h xgraph.h \
	 xsound.h  \
	  mover.h xmover.h
pool8.o: global.h   \
	 pool8.h pool.h real.h billard.h game.h vec2.h \
	 vector.h keeper.h graph.h object.h xgraph.h \
	 xsound.h  \
	  mover.h xmover.h \
	 ball.h arcs.h dynobj.h objtab.h wall.h
real.o: global.h   \
	 real.h 
rgb.o: rgb.h 
snooker.o: global.h   \
	 snooker.h vec2.h  real.h \
	vector.h pool.h billard.h game.h keeper.h pocket.h ball.h arcs.h \
	object.h dynobj.h objtab.h graph.h xgraph.h \
	 xsound.h  \
	  mover.h xmover.h
vec2.o: global.h   \
	 vec2.h  real.h vector.h
vec3.o: global.h   \
	 vec3.h  real.h vector.h
vector.o: global.h   \
	 vector.h  real.h \
	  vec2.h vec3.h
wall.o: global.h   \
	 wall.h object.h real.h vec2.h \
	 vector.h ball.h arcs.h dynobj.h objtab.h game.h \
	keeper.h
