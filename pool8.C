#ifndef _global_h
#	include "global.h"
#endif

#ifndef _pool8_h
#	include "pool8.h"
#endif
#ifndef _graph_h
#	include "graph.h"
#endif
#ifndef _mover_h
#	include "mover.h"
#endif
#ifndef _ball_h
#	include "ball.h"
#endif

Pool8::Pool8(double ft) :
	Pool( ft*2.54*12, ft*2.54*6 )
{
	SelectTable(-1);

	black_col	= AddBallColor( "black" );
	cueball=0;

	if (!deluxe) {
// Pool-Kugeln: "Television"
		full_col		= AddBallColor( "yellow" );
		half_col		= AddBallColor( "red" );
	}
	else {
// normale Pool-Kugeln
		ball_col[0] = AddBallColor( "gold1" );			// yellow
		ball_col[1] = AddBallColor( "blue" );			// blue
		ball_col[2] = AddBallColor( "red" );			// red
		ball_col[3] = AddBallColor( "DarkViolet" );	// dark violett
		ball_col[4] = AddBallColor( "orange" );		// orange
		ball_col[5] = AddBallColor( "MediumForestGreen" );	// dark green
		ball_col[6] = AddBallColor( "brown" );			// brown
	}

	mh = 0;
	balls_in_pocket = 0;
	for (int i=0;i<15;i++)	ball[i]=NULL;
}

Pool8::~Pool8() {
	if (cueball) {
		delete cueball;
		for (int i=0;i<15;i++)	if (ball[i])	delete ball[i];
		cueball=0;
	}
	if (mh)		delete mh;
}


void Pool8::Triangle( const Vec2 &vec )
{
static int t[15] = {		1,
						-1, -2,
					  2,  0,  3,
				  -3,  4, -4, -5,
				 5, -6,  6,  7, -7 };

Real	x = vec.X();
Real	y = vec.Y();
const int count = 5;
int		c=0;
double	cdist = sqrt( 3.*(GetNormalBallSize()+Offset)*(GetNormalBallSize()+Offset) );
Ball	*r;

//	x-=(cdist*(count-1)/2);
	for( int col=0; col<count; col++ ) {
		for( int row=0; row<=col; row++ ) {
			Vec2	pos(x+col*cdist,y+(row-col/2.0)*2*(GetNormalBallSize()+Offset));
			r=new Ball( pos, GetNormalBallSize() );
			if (t[c]>0) {
				if (!deluxe)
						r->state = new BallState( m, full_col, r->P() );
				else	r->state = new HalfBallState( m, ball_col[t[c]-1], r->P() );
				ball[t[c]-1]   = r;
				ball_p[t[c]-1] = pos;
			}
			else if (t[c]<0) {
				if (!deluxe)
						r->state = new BallState( m, half_col, r->P() );
				else	r->state = new HalfBallState( mh, ball_col[-t[c]-1], r->P() );
				ball[8-t[c]-1]   = r;
				ball_p[8-t[c]-1] = pos;
			}
			else {
				if (!deluxe)
						r->state = new BallState( m, black_col, r->P() );
				else	r->state = new HalfBallState( m, black_col, r->P() );
				ball[7]   = r;
				ball_p[7] = pos;
			}
			c++;
		}
	}
}


void Pool8::InitPlayground() {

	if (deluxe) {
		mh = HalfBallMover::Create( GetNormalBallSize(), (deluxe==3)?2:4 );
		mh->Init();
		m  = HalfBallMover::Create( GetNormalBallSize(), (deluxe==2)?3:((deluxe==3)?1:0) );
		m->Init();
		BallRadius = m->GetActRadius()-Offset;
	}
	Pool::InitPlayground();

	tridef  = Vec2(AreaOffX()+AreaWidth()*0.75, AreaOffY()+AreaHeight()/2.);
	Triangle(tridef);
	cuedef  = Vec2(AreaOffX()+AreaWidth()*0.125,AreaOffY()+AreaHeight()*0.375);
	cueball = new Ball( cuedef.X(), cuedef.Y(), RealZero, RealZero, BallRadius );
	if (!deluxe)
			cueball->state = new BallState( m, cue_col, cueball->P() );
	else	cueball->state = new HalfBallState( m, cue_col, cueball->P() );

#ifdef DEBUG
	if (deluxe) {
		if (debug&ShowTurns)		mh->CreateTurnWindow();
		if (debug&ShowRings)		mh->CreateRingWindow();
	}
#endif
}


void Pool8::DrawBackground() const {
	Pool::DrawBackground();
	SetBgColor(table_line_col);
	DrawLine( AreaOffX()+AreaWidth()*0.25, AreaOffY(),
							AreaOffX()+AreaWidth()*0.25, AreaOffY()+AreaHeight() );
	FillCircle( AreaOffX()+AreaWidth()*0.25, AreaOffY()+AreaHeight()*0.50, 1.0 );
	FillCircle( AreaOffX()+AreaWidth()*0.75, AreaOffY()+AreaHeight()*0.50, 1.0 );
}

void Pool8::ResetGame() {
	for (int i=0;i<15;i++)
		if (ball[i])				ball[i]->SetPV( ball_p[i] );
	balls_in_pocket = 0;
	Pool::ResetGame();
}

////////////////////////////////////////////////////////////////////////////

void Pool8::InPocket( Ball *b ) {
	for (int i=0;i<15;i++) {
		if (b==ball[i]) {
			balls_in_pocket  |= (1<<i);
			return;
		}
	}
	Pool::InPocket(b);
}

int Pool8::IsSelectable(Ball *b) {
	for (int i=0;i<15;i++) {
		if (b==ball[i]) {
			if (balls_in_pocket & (1<<i)) {
				Vec2	newpos;
				b->FitsNextTo(tridef,Vec2(1.0,RealZero),&newpos);
				b->SetP(newpos);					// closer to right wall

				b->ChgV(Vec2Zero);
				balls_in_pocket &= ~(1<<i);	// it's back in the game

				return 0;
			}
			else {
				return 1;
			}
		}
	}

	return Pool::IsSelectable(b);
}

// -------------------------------------------------------------------------

Pool9::~Pool9() {
}

void Pool9::Triangle( const Vec2 &vec )
{
static int t[9] = {		1,
						 2,  3,
					  4, -1,  5,
				       6,  7,
				         0 };

Real	x = vec.X();
Real	y = vec.Y();
const int count = 5;
int		c=0;
double	cdist = sqrt( 3.*(GetNormalBallSize()+Offset)*(GetNormalBallSize()+Offset) );
Ball	*r;

//	x-=(cdist*(count-1)/2);
	for( int col=0; col<count; col++ ) {
		int per_col = 2-abs(2-col);
		for( int row=0; row<=per_col; row++ ) {
			Vec2 pos( x+col*cdist, y+(row-per_col/2.0)*2*(GetNormalBallSize()+Offset) );
			r=new Ball( pos, GetNormalBallSize() );
			if (t[c]>0) {
				if (!deluxe)
						r->state = new BallState( m, full_col, r->P() );
				else	r->state = new HalfBallState( m, ball_col[t[c]-1], r->P() );
				ball[t[c]-1]   = r;
				ball_p[t[c]-1] = pos;
			}
			else if (t[c]<0) {
				if (!deluxe)
						r->state = new BallState( m, half_col, r->P() );
				else	r->state = new HalfBallState( mh, ball_col[-t[c]-1], r->P() );
				ball[8-t[c]-1]   = r;
				ball_p[8-t[c]-1] = pos;
			}
			else {
				if (!deluxe)
						r->state = new BallState( m, black_col, r->P() );
				else	r->state = new HalfBallState( m, black_col, r->P() );
				ball[7]   = r;
				ball_p[7] = pos;
			}
			c++;
		}
	}
}

// -------------------------------------------------------------------------

Pool8Demo::~Pool8Demo() {}

const Real &Pool8Demo::GetPresetA() const			{ return PresetA; }
const Real &Pool8Demo::GetSlowGranularity() const	{ return SlowGranularity; }

void Pool8Demo::InitPlayground() {

	if (deluxe) {
		mh = HalfBallMover::Create( GetNormalBallSize(), (deluxe==3)?2:4 );
		mh->Init();
		m  = HalfBallMover::Create( GetNormalBallSize(), (deluxe==2)?3:((deluxe==3)?1:0) );
		m->Init();
		BallRadius = m->GetActRadius()-Offset;
	}
	Billard::InitPlayground();
	BallRadius = m->GetActRadius();
	Billard::InitTable();

	Triangle(   Vec2(AreaOffX()+AreaWidth()*0.75, AreaOffY()+AreaHeight()/2.) );
	cueball = new Ball( AreaOffX()+AreaWidth()*0.125, AreaOffY()+AreaHeight()*0.375,
					shot_speed, shot_speed, BallRadius );
	if (!deluxe)
			cueball->state = new BallState( m, cue_col, cueball->P() );
	else	cueball->state = new HalfBallState( m, cue_col, cueball->P() );
}

void Pool8Demo::DrawBackground() const {
	Billard::DrawBackground();
	SetBgColor(table_line_col);
	DrawLine( AreaOffX()+AreaWidth()*0.25, AreaOffY(),
							AreaOffX()+AreaWidth()*0.25, AreaOffY()+AreaHeight() );
	FillCircle( AreaOffX()+AreaWidth()*0.25, AreaOffY()+AreaHeight()*0.50, 1.0 );
	FillCircle( AreaOffX()+AreaWidth()*0.75, AreaOffY()+AreaHeight()*0.50, 1.0 );
}

// -------------------------------------------------------------------------

#ifdef DEBUG

#ifndef _wall_h
#	include "wall.h"
#endif


class TestField {
	public:
		TestField( const Vec2 &p1, const Vec2 &p2, const Real &len,
							Pool8Test *d );
		~TestField();

		void Draw() const;
		void Insert( TestField *n )	{ n->next = next; next = n; }

	private:
		Vec2		e[4];
		Wall			*w[4];
		int			nballs;
		Ball			**b;
		TestField	*next;
};

TestField::TestField( const Vec2 &p1, const Vec2 &p2, const Real &len,
		Pool8Test *d ) {
Vec2	dist = (p2-p1);
Vec2	dir_x = dist.TurnRight().Norm1();
Vec2	dir_y = dist.Norm1();

const double SPEED = 20.0;

	next = 0l;
	e[0] = p1;
	e[1] = p1+dir_x*len;
	e[2] = p2+dir_x*len;
	e[3] = p2;
	for (int i=0;i<4;i++)	w[i] = new Wall( e[i], e[(i+1)%4] );

	nballs = (int)(dist.Norm()/Pool8Test::BallRadius/3.);
	b = new Ball*[nballs];
	for (i=0;i<nballs;i++) {
		Vec2 pos	= e[0]
						+ 3.*Pool8Test::BallRadius*dir_x
						+ Real((i+1)*3)*Pool8Test::BallRadius * dir_y;
		b[i] = new Ball( pos.X()+((rand()%150)/100.0), pos.Y(),
						dir_x.X()*SPEED, dir_x.Y()*SPEED,
						Pool8Test::BallRadius );
		b[i]->state = new HalfBallState( d->mh, d->ball_col[i%7], b[i]->P() );
		((HalfBallState*)b[i]->state)->st = d->mh->AngVec2St(0,i*d->mh->vecs_b/nballs);

	}
}

TestField::~TestField() {
	for (int i=0;i<nballs;i++)		delete b[i];
	delete b;
	for (i=0;i<4;i++)	delete w[i];
	if (next)		delete next;
}

void TestField::Draw() const {
	for (int i=0;i<4;i++)	DrawLine( e[i], e[(i+1)%4] );
	if (next)		next->Draw();
}


Pool8Test::~Pool8Test()
{
	delete field_queue;
}

void Pool8Test::InitPlayground() {

	if (deluxe) {
		mh = HalfBallMover::Create( GetNormalBallSize(), 2 );
		mh->Init();
		BallRadius = mh->GetActRadius()-Offset;
	}
	Billard::InitPlayground();
	BallRadius = m->GetActRadius();
	Billard::InitTable();

	Vec2	dir = (Mid(2)-Mid(0))*3.0/4.0;
	if (shot_speed>=RealZero&&shot_speed<=90.)
			dir=dir.TurnAngleDeg(-shot_speed);

#if (1)
	field_queue = new TestField( Mid(0), Mid(0)+dir, AreaWidth()/4., this );
#else
	field_queue = new TestField( Edge(0), Edge(3), AreaWidth(), this );
	field_queue->Insert( new TestField( Mid(2), Mid(1), AreaWidth()/5, this ) );
#endif
}

void Pool8Test::DrawBackground() const {
	Pool8Demo::DrawBackground();
	SetBgColor(marker_col);
	field_queue->Draw();
}
#endif
