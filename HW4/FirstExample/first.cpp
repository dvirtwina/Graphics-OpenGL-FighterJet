// NOTE: Bombs are releasesd by pressing 'q' and 'e' (approved by lecturer)

#define _CRT_SECURE_NO_WARNINGS

#include "GLUT.h"
#include <math.h>
#include <time.h>
#include <Windows.h>
#include <stdio.h>

const double PI = 3.14;
const int GSIZE = 200;
const int W = 600;
const int H = 600;
const double CANON_RANGE = 100.0;
const int FIRE_RATE = 30;
const int CANON_ACCURACY_RADIUS = 5;
const double GRAVITY = -0.2;
const int BOMB_HIT_RADIUS = 3;

double ground[GSIZE][GSIZE];


double offset = 0;
double eyex = 0/*5*/, eyey = 20, eyez = 0/*15*/;

double speed = 0;
double angular_speed = 0;
double sight_angle = PI+0.2; 
double dirx=0, dirz=-1;
bool stop = true;
// for airplane
double air_dir[3]; // x,y,z
double yaw=PI, pitch=0.0, roll=0;
double air_speed=0, air_angular_speed=0;
double airpl[3] = { 0,15,0 }; // airplane location
// for bombs
bool lBombStart = false;
bool rBombStart = false;
bool leftBombReleased = false;
bool rightBombReleased = false;
double lBombLoc[3] = { airpl[0],airpl[1], airpl[2] };
double rBombLoc[3] = { airpl[0],airpl[1], airpl[2] };
double lBombInitSpeed;
double rBombInitSpeed;
double lBombInitDir[3];
double rBombInitDir[3];
double lt = 0;
double rt = 0;
bool lBombDetonation = false;
bool rBombDetonation = false;
double lDetonationLoc[3];
double rDetonationLoc[3];
int lDetoPhaze = 0;
int rDetoPhaze = 0;

//for canon
int canonx, canonz;
int canonHitX, canonHitZ;
int blastPhaze;
double canonAirplaneDist;

bool endGame = false;

///TO BE DELETED:
double tempx, tempy, tempz;
///////

void UpdateTerrain1();
void UpdateTerrain2();
void UpdateTerrain3();
void Smooth();


bool generateCanonCoordinates()
{
	canonx = rand() % (GSIZE - 20); // -20 so the canon won't be at the edge of the screen
	canonz = rand() % (GSIZE - 20); // -20 so the canon won't be at the edge of the screen

	if (ground[canonz][canonx] > 0)
		return true;
	else 
		return false;
}

void init()
{
	int i, j;
	glClearColor(0.7, 0.8, 1, 0);

	glEnable(GL_DEPTH_TEST);

	srand(time(0)); // start random generator from each time new number 


	for (i = 1; i < 2000; i++)
	{
		UpdateTerrain2();
		UpdateTerrain2();
		UpdateTerrain3();
	}
	Smooth();
	Smooth();

	while (!generateCanonCoordinates());

}

void SetColor(double h)
{
	// sand
	if (fabs(h) < 0.2)
		glColor3d(0.8, 0.8, 0.7);
	// grass-stones
	else if (fabs(h) < 4)
		glColor3d(fabs(h)/10,0.3+(4-fabs(h))/8,fabs(h)/25);
	// snow
	else
		glColor3d(fabs(h)/8, fabs(h)/8,fabs(h)/6);
}

void DrawFloor()
{
	int i,j;

	for (i = 0; i <GSIZE-1; i++)
		for(j=0;j<GSIZE-1;j++)
	{
			glBegin(GL_POLYGON);
			SetColor(ground[i][j]);
			glVertex3d(j-GSIZE/2, ground[i][j], i - GSIZE / 2);
			SetColor(ground[i][j+1]);
			glVertex3d(j+1 - GSIZE / 2, ground[i][j+1], i - GSIZE / 2);
			SetColor(ground[i+1][j + 1]);
			glVertex3d(j+1 - GSIZE / 2, ground[i+1][j+1], i+1 - GSIZE / 2);
			SetColor(ground[i+1][j]);
			glVertex3d(j - GSIZE / 2, ground[i + 1][j ], i + 1 - GSIZE / 2);
		glEnd();
	}

	// water

	glEnable(GL_BLEND); // add transparency
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4d(0, 0.4, 0.6,0.7);
	glBegin(GL_POLYGON);
		glVertex3d(-GSIZE / 2, 0, -GSIZE / 2);
		glVertex3d(-GSIZE / 2, 0, GSIZE / 2);
		glVertex3d(GSIZE / 2, 0, GSIZE / 2);
		glVertex3d(GSIZE / 2, 0, -GSIZE / 2);
	glEnd();
	glDisable(GL_BLEND); // switch off transparency

}

void UpdateTerrain1()
{
	int i, j;
	for (i = 0; i < GSIZE; i++)
		for (j = 0; j < GSIZE; j++)
			ground[i][j] += (((rand() % 1001)-500) / 5000.0 ) ;
}

void UpdateTerrain2()
{
	int i, j;
	int x1, y1, x2, y2;
	double a, b,delta = 0.05;

	x1 = rand() % GSIZE;
	x2 = rand() % GSIZE;
	y1 = rand() % GSIZE;
	y2 = rand() % GSIZE;

	if (rand() % 2 == 0)
		delta = -delta;


	if (x1 != x2)
	{
		a = ((double)y2 - y1) / (x2 - x1);
		b = y1 - a*x1;

		for (i = 0; i < GSIZE; i++)
			for (j = 0; j < GSIZE; j++)
				if (i < a*j + b)
					ground[i][j] += delta;
				else 
					ground[i][j] -= delta;
	}

}

void UpdateTerrain3()
{
	int x, y, max_pts = 2000,counter;
	double delta = 0.02;

	if (rand() % 2 == 0)
		delta = -delta;


	x = rand() % GSIZE;
	y = rand() % GSIZE;

	for (counter = 0; counter < max_pts; counter++)
	{
		ground[y][x] += delta;
		switch (rand() % 4)
		{
		case 0:
			x++;
			break;

		case 1:
			x--;
			break;
		case 2:
			y++;
			break;
			y--;
		case 3:
			break;
		}// switch
		x += GSIZE;
		x %= GSIZE;
		y += GSIZE;
		y %= GSIZE;

	}
}

void generateHitCoord()
{
	int sign = rand()%2;

	if (sign == 0)
	{
		canonHitX = airpl[0] + (rand()%CANON_ACCURACY_RADIUS);
	}
	else
	{
		canonHitX = airpl[0] - (rand()%CANON_ACCURACY_RADIUS);
	}

	sign = rand() % 2;

	if (sign == 0)
	{
		canonHitZ = airpl[2] + (rand()%CANON_ACCURACY_RADIUS);
	}
	else
	{
		canonHitZ = airpl[2] - (rand()%CANON_ACCURACY_RADIUS);
	}
}

void flattenCanonGround()
{
	for(int i = canonz-3; i<=canonz+3; i++)
		for(int j = canonx-3; j<=canonx+3; j++)
			ground[i][j] = ground[canonz][canonx];
}

void Smooth()
{
	double tmp[GSIZE][GSIZE];
	int i, j;

	for (i = 1; i < GSIZE - 1; i++)
		for (j = 1; j < GSIZE - 1; j++)
			tmp[i][j] = (ground[i - 1][j - 1] + 2*ground[i - 1][j] + ground[i - 1][j + 1] +
				2*ground[i][j - 1] + 4*ground[i][j] + 2*ground[i][j + 1] +
				ground[i + 1][j - 1] + 2*ground[i + 1][j] + ground[i + 1][j + 1] ) / 16.0;

	for (i = 1; i < GSIZE - 1; i++)
		for (j = 1; j < GSIZE - 1; j++)
			ground[i][j] = tmp[i][j];

}

void DrawCube()
{
	// top face
	glBegin(GL_POLYGON);
	//glColor3d(1, 0, 0); // red
		glVertex3d(1, 1, 1);
	//glColor3d(1, 1, 0); // yellow
		glVertex3d(1, 1, -1);
	//glColor3d(0,1, 0); // green
		glVertex3d(-1, 1, -1);
	//glColor3d(1, 1, 1); // white
		glVertex3d(-1, 1, 1);
	glEnd();
	// bottom face
	//glBegin(GL_POLYGON);
	//glColor3d(1, 0, 1); // magenta
		glVertex3d(1, -1, 1);
	//glColor3d(0, 0, 0); // black
		glVertex3d(1, -1, -1);
	//glColor3d(0, 1, 1); // cyan
		glVertex3d(-1, -1, -1);
	//glColor3d(0, 0, 1); // blue
		glVertex3d(-1, -1, 1);
	//glEnd();
	// left
	glBegin(GL_POLYGON);
		//glColor3d(1, 1, 1); // white
		glVertex3d(-1, 1, 1);
		//glColor3d(0, 1, 0); // green
		glVertex3d(-1, 1, -1);
		//glColor3d(0, 1, 1); // cyan
		glVertex3d(-1, -1, -1);
		//glColor3d(0, 0, 1); // blue
		glVertex3d(-1, -1, 1);
	glEnd();
//right
	glBegin(GL_POLYGON);
	//glColor3d(1, 0, 0); // red
	glVertex3d(1, 1, 1);
	//glColor3d(1, 1, 0); // yellow
	glVertex3d(1, 1, -1);
	//glColor3d(0, 0, 0); // black
	glVertex3d(1, -1, -1);
	//glColor3d(1, 0, 1); // magenta
	glVertex3d(1, -1, 1);
	glEnd();
// front
	glBegin(GL_POLYGON);
		//glColor3d(1, 1, 1); // white
		glVertex3d(-1, 1, 1);
		//glColor3d(1, 0, 0); // red
		//glVertex3d(1, 1, 1);
		glColor3d(1, 0, 1); // magenta
		//glVertex3d(1, -1, 1);
		glColor3d(0, 0, 1); // blue
		//glVertex3d(-1, -1, 1);
	glEnd();
	// rear
	glBegin(GL_POLYGON);
	//glColor3d(1, 1, 0); // yellow
	glVertex3d(1, 1, -1);
	//glColor3d(0, 1, 0); // green
	glVertex3d(-1, 1, -1);
	//glColor3d(0, 1, 1); // cyan
	glVertex3d(-1, -1, -1);
	//glColor3d(0, 0, 0); // black
	glVertex3d(1, -1, -1);
	glEnd();

}

void DrawWireCube()
{
	// top face
	glBegin(GL_POLYGON);
	glColor3d(1, 0, 0); // red
	glVertex3d(1, 1, 1);
	glColor3d(1, 1, 0); // yellow
	glVertex3d(1, 1, -1);
	glColor3d(0, 1, 0); // green
	glVertex3d(-1, 1, -1);
	glColor3d(1, 1, 1); // white
	glVertex3d(-1, 1, 1);
	glEnd();
	// bottom face
	glBegin(GL_POLYGON);
	glColor3d(1, 0, 1); // magenta
	glVertex3d(1, -1, 1);
	glColor3d(0, 0, 0); // black
	glVertex3d(1, -1, -1);
	glColor3d(0, 1, 1); // cyan
	glVertex3d(-1, -1, -1);
	glColor3d(0, 0, 1); // blue
	glVertex3d(-1, -1, 1);
	glEnd();
	// left
	glBegin(GL_LINE_LOOP);
	glColor3d(1, 1, 1); // white
	glVertex3d(-1, 1, 1);
	glColor3d(0, 1, 0); // green
	glVertex3d(-1, 1, -1);
	glColor3d(0, 1, 1); // cyan
	glVertex3d(-1, -1, -1);
	glColor3d(0, 0, 1); // blue
	glVertex3d(-1, -1, 1);
	glEnd();
	//right
	glBegin(GL_LINE_LOOP);
	glColor3d(1, 0, 0); // red
	glVertex3d(1, 1, 1);
	glColor3d(1, 1, 0); // yellow
	glVertex3d(1, 1, -1);
	glColor3d(0, 0, 0); // black
	glVertex3d(1, -1, -1);
	glColor3d(1, 0, 1); // magenta
	glVertex3d(1, -1, 1);
	glEnd();
	// front
	glBegin(GL_LINE_LOOP);
	glColor3d(1, 1, 1); // white
	glVertex3d(-1, 1, 1);
	glColor3d(1, 0, 0); // red
	glVertex3d(1, 1, 1);
	glColor3d(1, 0, 1); // magenta
	glVertex3d(1, -1, 1);
	glColor3d(0, 0, 1); // blue
	glVertex3d(-1, -1, 1);
	glEnd();
	// rear
	glBegin(GL_LINE_LOOP);
	glColor3d(1, 1, 0); // yellow
	glVertex3d(1, 1, -1);
	glColor3d(0, 1, 0); // green
	glVertex3d(-1, 1, -1);
	glColor3d(0, 1, 1); // cyan
	glVertex3d(-1, -1, -1);
	glColor3d(0, 0, 0); // black
	glVertex3d(1, -1, -1);
	glEnd();

}

void DrawCilynder(int n)
{
	double alpha, teta = 2 * PI / n;

	for (alpha = 0; alpha < 2 * PI; alpha += teta)
	{
		glBegin(GL_POLYGON);
		glColor3d(fabs(sin(alpha)), fabs(sin(alpha)), 1 - 0.2*fabs(sin(alpha)));
			glVertex3d(sin(alpha), 1, cos(alpha)); // 1
		glColor3d(fabs(sin(alpha + teta)), fabs(sin(alpha + teta)), 1 - 0.2*fabs(sin(alpha + teta)));
			glVertex3d(sin(alpha+teta), 1, cos(alpha + teta)); // 2
		glColor3d(fabs(sin(alpha)), fabs(sin(alpha)), 1 - 0.2* fabs(sin(alpha)));
			glVertex3d(sin(alpha + teta), 0, cos(alpha + teta)); // 3
			glVertex3d(sin(alpha), 0, cos(alpha )); // 4
		glEnd();
	}
}

//color = 1 -> default color 
//color = 2 -> custom color 
void DrawCilynder1(int n,double topr, double bottomr, int color)
{
	double alpha, teta = 2 * PI / n;

	switch (color)
	{
	case 1:
		for (alpha = PI; alpha < 3 * PI; alpha += teta)
		{
			glBegin(GL_POLYGON);// GL_LINE_LOOP);//GL_POLYGON);
			glColor3d(fabs(sin(alpha)), fabs(sin(alpha)), 1 - 0.2*fabs(sin(alpha)));
			glVertex3d(topr*sin(alpha), 1, topr*cos(alpha)); // 1
			glColor3d(fabs(sin(alpha + teta)), fabs(sin(alpha + teta)), 1 - 0.2*fabs(sin(alpha + teta)));
			glVertex3d(topr*sin(alpha + teta), 1, topr*cos(alpha + teta)); // 2
			glVertex3d(bottomr*sin(alpha + teta), 0, bottomr*cos(alpha + teta)); // 3
			glColor3d(fabs(sin(alpha)), fabs(sin(alpha)), 1 - 0.2* fabs(sin(alpha)));
			glVertex3d(bottomr*sin(alpha), 0, bottomr*cos(alpha)); // 4
			glEnd();
		}
		break;
	case 2:
		for (alpha = PI; alpha < 3 * PI; alpha += teta)
		{
			glBegin(GL_POLYGON);// GL_LINE_LOOP);//GL_POLYGON);
			glVertex3d(topr*sin(alpha), 1, topr*cos(alpha)); // 1
			glVertex3d(topr*sin(alpha + teta), 1, topr*cos(alpha + teta)); // 2
			glVertex3d(bottomr*sin(alpha + teta), 0, bottomr*cos(alpha + teta)); // 3
			glVertex3d(bottomr*sin(alpha), 0, bottomr*cos(alpha)); // 4
			glEnd();
		}
		break;
	}
}

void DrawConus(int n)
{
	double alpha, teta = 2 * PI / n;

	for (alpha = 0; alpha < 2 * PI; alpha += teta)
	{
		glBegin(GL_POLYGON);
		glColor3d(1,1,1);
		glVertex3d(0, 1, 0); // 1
		glColor3d(alpha / (2 * PI), fabs(cos(alpha)) / 2, fabs(sin(alpha)));
		glVertex3d(sin(alpha + teta), 0, cos(alpha + teta)); // 3
		glVertex3d(sin(alpha), 0, cos(alpha)); // 4
		glEnd();
	}
}

void DrawSphere(int n, int slices)
{
	double beta, delta = PI / slices;
	int i;

	for (beta = -PI / 2,i=1; beta <= PI / 2; beta += delta,i++)
	{

		glPushMatrix();
//			glRotated(i*offset*0.5, 0, 1, 0);
			glTranslated(0, sin(beta), 0);
			glScaled(1,sin(beta+delta) -sin(beta), 1);
			DrawCilynder1(n, cos(beta + delta), cos(beta), 1);
		glPopMatrix();
	}
}

void DrawBlast(int n, int slices, int blastPhaze)
{
	double beta, delta = PI / slices;
	int i;

	switch(blastPhaze)
	{
		//canon blast sequence
	case 1:
		glColor4d(1, 0.784, 0 , 0.8);
		break;
	case 2:
		glColor4d(1, 1, 0.078, 0.6);
		break;
	case 3:
		glColor4d(1, 1, 0.588, 0.4);
		break;

		//canon hit blast sequence
	case 4:
		glColor4d(0, 0, 0, 0.8);
		break;
	case 5:
		glColor4d(0.2, 0.2, 0.2, 0.6);
		break;
	case 6:
		glColor4d(0.4, 0.4, 0.4, 0.4);
		break;

		//bomb blast sequence
	case 7:
		glColor4d(1, 0.4, 0, 0.8);
		break;
	case 8:
		glColor4d(1, 0.6, 0, 0.6);
		break;
	case 9:
		glColor4d(1, 0.8, 0, 0.4);
		break;
	}
	glEnable(GL_BLEND); // add transparency
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (beta = -PI / 2, i = 1; beta <= PI / 2; beta += delta, i++)
	{

		glPushMatrix();
		//			glRotated(i*offset*0.5, 0, 1, 0);
		glTranslated(0, sin(beta), 0);
		glScaled(1, sin(beta + delta) - sin(beta), 1);
		DrawCilynder1(n, cos(beta + delta), cos(beta), 2);
		glPopMatrix();
	}

	glDisable(GL_BLEND);
}

// shape = 1 -> circle
// shape = 2 -> polygon
void drawCircle(int shape)
{
	double x, y;
	double alpha, teta = PI / 90;

	//glColor3d(0.5, 0.5, 0.5);
	glPushMatrix();
	switch (shape)
	{
	case 1:
		glBegin(GL_LINE_STRIP);
		break;
	case 2:
		glBegin(GL_POLYGON);
		break;
	}
	for (alpha = 0; alpha <= 2 * PI; alpha += teta)
	{
		x = cos(alpha);
		y = sin(alpha);
		glVertex2d(x, y);
	}
	glEnd();

	glPopMatrix();
}

void DrawClosedCylinder(int n)
{
	glPushMatrix();
	DrawCilynder(n);
	glPopMatrix();

	glColor3d(0.8, 0.8, 0.8);
	glPushMatrix();
	glRotated(90, 1, 0, 0);
	drawCircle(2);
	glPopMatrix();

	glColor3d(0.8, 0.8, 0.8);
	glPushMatrix();
	glRotated(90, 1, 0, 0);
	glTranslated(0, 0, -1);
	drawCircle(2);
	glPopMatrix();
}

void DrawAirplane()
{

	// head
	glPushMatrix();
		glRotated(90, 0, 0, 1);
		glTranslated(0, 2, 0);
		glScaled(1, 5, 1);
		DrawCilynder1(20, 0, 1, 1);
	glPopMatrix();
	// middle part
	glPushMatrix();
		glRotated(90, 0, 0, 1);
		glTranslated(0, -1, 0);
		glScaled(1, 3, 1);
		DrawCilynder1(20, 1, 1.2, 1);
	glPopMatrix();
	// rear part
	glPushMatrix();
	glRotated(90, 0, 0, 1);
	glTranslated(0, -8, 0);
	glScaled(1, 7, 1);
	DrawCilynder1(20, 1.2, 0.5, 1);
	glPopMatrix();

	// cockpit
	glPushMatrix();
	glRotated(90, 0, 0, 1);
	glTranslated(0.4, 0.5, 0);
	glScaled(1, 3, 1);
	glRotated(180, 0, 1, 0);
	DrawSphere(20, 10);
	glPopMatrix();

	// wings
	glColor3d(0.8, 0.8, 0.8);
	glBegin(GL_POLYGON);
		glVertex3d(-4, 0, 0);
		glVertex3d(5, 0, 5.2);
		glVertex3d(6, 0, 5.2);
		glVertex3d(3, 0, 0);
	glEnd();
	glBegin(GL_POLYGON);
		glVertex3d(-4, 0, 0);
		glVertex3d(5, 0, -5.2);
		glVertex3d(6, 0, -5.2);
		glVertex3d(3, 0, 0);
	glEnd();

	// rear wings
	glBegin(GL_POLYGON);
		glVertex3d(3, 0, 0);
		glVertex3d(7, 0, 2);
		glVertex3d(8, 0, 2);
		glVertex3d(7, 0, 0);
	glEnd();
	glBegin(GL_POLYGON);
		glVertex3d(3, 0, 0);
		glVertex3d(7, 0, -2);
		glVertex3d(8, 0, -2);
		glVertex3d(7, 0, 0);
	glEnd();
	// tail
	glBegin(GL_POLYGON);
		glVertex3d(1, 0, 0);
		glColor3d(0.3, 0.5, 1);
		glVertex3d(8, 3, 0);
		glVertex3d(9, 3, 0);
		glColor3d(0.8, 0.8, 0.8);
		glVertex3d(8, 0, 0);
	glEnd();
	// left bomb
	glPushMatrix();
		glRotated(90,0, 0, 1);
		glTranslated(-2.66, -2, 3);// down/up,	bock/front, right/left
		if (leftBombReleased)
		{
			glTranslated(lBombLoc[1], lBombLoc[0], lBombLoc[2]);
		}
		glScaled(0.75, 0.75, 0.75);

		glPushMatrix();
			glTranslated(3, 1.5, 0);
			glScaled(0.5, 1.5, 0.5);
			DrawCilynder1(100, 0, 1, 1);
		glPopMatrix();

		glPushMatrix();
			glTranslated(3, 0, 0);
			glScaled(0.5, 1.5, 0.5);
			DrawCilynder(100);
		glPopMatrix();

		glPushMatrix();
			glTranslated(3, 0, 0);
			glScaled(0.5, -1.5, 0.5);
			DrawCilynder1(100, 0, 1, 1);
		glPopMatrix();

	glPopMatrix();

	// right bomb
	glPushMatrix();
	glRotated(90, 0, 0, 1);
	glTranslated(-2.66, -2, -3);// down/up,	bock/front, right/left
	if (rightBombReleased)
	{
		glTranslated(rBombLoc[1], rBombLoc[0], rBombLoc[2]);
	}
	glScaled(0.75, 0.75, 0.75);

		glPushMatrix();
			glTranslated(3, 1.5, 0);
			glScaled(0.5, 1.5, 0.5);
			DrawCilynder1(100, 0, 1, 1);
		glPopMatrix();

		glPushMatrix();
			glTranslated(3, 0, 0);
			glScaled(0.5, 1.5, 0.5);
			DrawCilynder(100);
		glPopMatrix();

		glPushMatrix();
			glTranslated(3, 0, 0);
			glScaled(0.5, -1.5, 0.5);
			DrawCilynder1(100, 0, 1, 1);
		glPopMatrix();

	glPopMatrix();

}

void drawCanon()
{
	//glColor3d(0.3, 0.3, 0.3);
	glPushMatrix(); // canon
//		glRotated(90-((atan(fabs((/*eyey*/airpl[1]-ground[canonz][canonx])/(/*eyex*/airpl[0]-(canonx-(GSIZE/2))))))*180/PI), 0, 0, 1); // EYE PARAMETERS SHOULD BE REPLACED WITH AIRPL[]
		glScaled(0.3, 4, 0.3);
		DrawCilynder(50);
	glPopMatrix();

		glPushMatrix();// base of canon
			glColor3d(0.5, 0.5, 0.5);
			glRotated(90, 1, 0, 0);
			glScaled(2.5, 2.5, 1);
			drawCircle(2);
		glPopMatrix();

		glPushMatrix(); // stage
		DrawClosedCylinder(50);
		glPopMatrix();
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, W, H);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity(); // here starts  the perspective projection

	glFrustum(-1, 1, -1, 1, 0.8, 500);
	//        eye coordinates, poi, up
	gluLookAt(eyex, eyey, eyez, 
		eyex+dirx, eyey-0.3, eyez+dirz, 
		0, 1, 0);

	DrawFloor();

	// show airplane
	glPushMatrix();
	
	/////////////////////////////////////////////////////////////////INTERNAL USE: SELF ROTATION
	//tempx = airpl[0];
	//tempy = airpl[1];
	//tempz = airpl[2];
	//glTranslated(-tempx, -tempy, -tempz);
	//glRotated(offset, 0, 1, 0);
	//glTranslated(tempx, tempy+10, tempz);
	///////////////////////////////////// WHEN DELETING THIS SECTION, COMMENT IN THE BELOW TRANSFORMATIONS

		glTranslated(airpl[0], /*15 +*/ airpl[1], airpl[2]);
		glRotated(yaw * 180 / PI, 0, 1, 0);
		glScaled(.5, .5, .5);
		glRotated(90, 0, 1, 0);
		glRotated(roll*1000, 1, 0, 0);
		glRotated(-pitch * 180 / PI, 0, 0, 1);
		DrawAirplane();
	glPopMatrix();

	// show canon
	glPushMatrix();
		glTranslated(canonx - GSIZE/2, ground[canonz][canonx]+0.01,canonz - GSIZE / 2);
		//glRotated(offset, 0, 1, 0);
		drawCanon();
	glPopMatrix();
	flattenCanonGround();

	// canon blast
	switch (blastPhaze)
	{
		// 3 cases for canon blast
	case 1:
			glPushMatrix();
			glTranslated(canonx - GSIZE / 2, ground[canonz][canonx] + 4, canonz - GSIZE / 2);
			glScaled(0.5, 0.5, 0.5);
			DrawBlast(50, 50, 1);
			glPopMatrix();
			break;
		
	case 2:
		glPushMatrix();
		glTranslated(canonx - GSIZE / 2, ground[canonz][canonx] + 4, canonz - GSIZE / 2);
		glScaled(1, 1, 1);
		DrawBlast(50, 50, 2);
		glPopMatrix();
		break;

	case 3:
		glPushMatrix();
		glTranslated(canonx - GSIZE / 2, ground[canonz][canonx] + 4, canonz - GSIZE / 2);
		glScaled(2, 2, 2);
		DrawBlast(50, 50, 3);
		glPopMatrix();
		break;

		// 3 cases for conon hit blast
	case 4:
		glPushMatrix();
		glTranslated(canonHitX/* - GSIZE / 2*/, airpl[1], canonHitZ/* - GSIZE / 2*/);
		glScaled(0.5, 0.5, 0.5);
		DrawBlast(50, 50, 4);
		glPopMatrix();
		break;

	case 5:
		glPushMatrix();
		glTranslated(canonHitX/* - GSIZE / 2*/, airpl[1], canonHitZ/* - GSIZE / 2*/);
		glScaled(1, 1, 1);
		DrawBlast(50, 50, 5);
		glPopMatrix();
		break;

	case 6:
		glPushMatrix();
		glTranslated(canonHitX/* - GSIZE / 2*/, airpl[1], canonHitZ/* - GSIZE / 2*/);
		glScaled(2, 2, 2);
		DrawBlast(50, 50, 6);
		glPopMatrix();
		break;

	default:
		break;
	}

	//bomb blast
	switch (lDetoPhaze)
	{
	case 1:
		glPushMatrix();
		glTranslated(lDetonationLoc[0], lDetonationLoc[(int)ground[0][2]], lDetonationLoc[2]);
		glScaled(1,1,1);
		DrawBlast(50, 50, 7);
		glPopMatrix();
		break;
	case 2:
		glPushMatrix();
		glTranslated(lDetonationLoc[0], lDetonationLoc[(int)ground[0][2]], lDetonationLoc[2]);
		glScaled(2,2,2);
		DrawBlast(50, 50, 8);
		glPopMatrix();
		break;
	case 3:
		glPushMatrix();
		glTranslated(lDetonationLoc[0], lDetonationLoc[(int)ground[0][2]], lDetonationLoc[2]);
		glScaled(4,4,4);
		DrawBlast(50, 50, 9);
		glPopMatrix();
		break;
	}

	switch (rDetoPhaze)
	{
	case 1:
		glPushMatrix();
		glTranslated(rDetonationLoc[0], rDetonationLoc[(int)ground[0][2]], rDetonationLoc[2]);
		glScaled(1, 1, 1);
		DrawBlast(50, 50, 7);
		glPopMatrix();
		break;
	case 2:
		glPushMatrix();
		glTranslated(rDetonationLoc[0], rDetonationLoc[(int)ground[0][2]], rDetonationLoc[2]);
		glScaled(2, 2, 2);
		DrawBlast(50, 50, 8);
		glPopMatrix();
		break;
	case 3:
		glPushMatrix();
		glTranslated(rDetonationLoc[0], rDetonationLoc[(int)ground[0][2]], rDetonationLoc[2]);
		glScaled(4, 4, 4);
		DrawBlast(50, 50, 9);
		glPopMatrix();
		break;
	}

	glViewport(0, 0, 100, 100); //  pitch control  **********************

	glDisable(GL_DEPTH_TEST);  // disable testing what is near and what is far
							   // From this point the later drawing will occlude the previous drawing

	glMatrixMode(GL_PROJECTION); // changes 3D to 2D
	glLoadIdentity(); // here starts  the perspective projection
	glOrtho(-1, 1, -1, 1, -1, 1); // 2D
	glMatrixMode(GL_MODELVIEW); // 
	glLoadIdentity(); // here starts  the orthogonal projection

	glEnable(GL_BLEND); // add transparency
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4d(0.7, 0.4, 0.4, 0.6);
	glBegin(GL_POLYGON);
	glVertex2d(-1, -1);
	glVertex2d(-1, 1);
	glVertex2d(1, 1);
	glVertex2d(1, -1);
	glEnd();
	glDisable(GL_BLEND); // turn off transparency

	glColor3d(0, 0, 0);
	glBegin(GL_LINES);
	glVertex2d(0, -1);
	glVertex2d(0, 1);
	glEnd();

	// handle
	glColor3d(0.8, 0.8, 0.8);

	glPushMatrix();
	glTranslated(0, pitch, 0);
	glBegin(GL_POLYGON);
	glVertex2d(-0.2, -0.1);
	glVertex2d(-0.2, 0.1);
	glVertex2d(0.2, 0.1);
	glVertex2d(0.2, -0.1);
	glEnd();
	glPopMatrix();

	glEnable(GL_DEPTH_TEST);

	//glViewport(0,0,W,H);
	//drawCanon();

	glutSwapBuffers();// show what was drawn in "frame buffer"
}

void displayTop()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, W, H);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity(); // here starts  the perspective projection

	glFrustum(-1, 1, -1, 1, 0.8, 500);
	//        eye coordinates, poi, up
	gluLookAt(0, 85, 0,
		1, 0, 0,
		0, 1, 0);

	DrawFloor();

	// show airplane
	glPushMatrix();
	glTranslated(airpl[0], /*15+*/airpl[1], airpl[2]);
	glRotated(yaw * 180 / PI, 0, 1, 0);
	glScaled(.5, .5, .5);
	glRotated(90, 0, 1, 0);
	glRotated(roll * 1000, 1, 0, 0);
	glRotated(-pitch * 180 / PI, 0, 0, 1);
	DrawAirplane();
	glPopMatrix();

	// show canon
	glPushMatrix();
	glTranslated(canonx - GSIZE / 2, ground[canonz][canonx] + 0.01, canonz - GSIZE / 2);
	//glRotated(offset, 0, 1, 0);
	drawCanon();
	glPopMatrix();
	flattenCanonGround();

	// canon blast
	switch (blastPhaze)
	{
		// 3 cases for canon blast
	case 1:
		glPushMatrix();
		glTranslated(canonx - GSIZE / 2, ground[canonz][canonx] + 4, canonz - GSIZE / 2);
		glScaled(0.5, 0.5, 0.5);
		DrawBlast(50, 50, 1);
		glPopMatrix();
		break;

	case 2:
		glPushMatrix();
		glTranslated(canonx - GSIZE / 2, ground[canonz][canonx] + 4, canonz - GSIZE / 2);
		glScaled(1, 1, 1);
		DrawBlast(50, 50, 2);
		glPopMatrix();
		break;

	case 3:
		glPushMatrix();
		glTranslated(canonx - GSIZE / 2, ground[canonz][canonx] + 4, canonz - GSIZE / 2);
		glScaled(2, 2, 2);
		DrawBlast(50, 50, 3);
		glPopMatrix();
		break;

		// 3 cases for conon hit blast
	case 4:
		glPushMatrix();
		glTranslated(canonHitX/* - GSIZE / 2*/, airpl[1], canonHitZ/* - GSIZE / 2*/);
		glScaled(0.5, 0.5, 0.5);
		DrawBlast(50, 50, 4);
		glPopMatrix();
		break;

	case 5:
		glPushMatrix();
		glTranslated(canonHitX/* - GSIZE / 2*/, airpl[1], canonHitZ/* - GSIZE / 2*/);
		glScaled(1, 1, 1);
		DrawBlast(50, 50, 5);
		glPopMatrix();
		break;

	case 6:
		glPushMatrix();
		glTranslated(canonHitX/* - GSIZE / 2*/, airpl[1], canonHitZ/* - GSIZE / 2*/);
		glScaled(2, 2, 2);
		DrawBlast(50, 50, 6);
		glPopMatrix();
		break;

	default:
		break;
	}

	//bomb blast
	switch (lDetoPhaze)
	{
	case 1:
		glPushMatrix();
		glTranslated(lDetonationLoc[0], lDetonationLoc[(int)ground[0][2]], lDetonationLoc[2]);
		glScaled(1, 1, 1);
		DrawBlast(50, 50, 7);
		glPopMatrix();
		break;
	case 2:
		glPushMatrix();
		glTranslated(lDetonationLoc[0], lDetonationLoc[(int)ground[0][2]], lDetonationLoc[2]);
		glScaled(2, 2, 2);
		DrawBlast(50, 50, 8);
		glPopMatrix();
		break;
	case 3:
		glPushMatrix();
		glTranslated(lDetonationLoc[0], lDetonationLoc[(int)ground[0][2]], lDetonationLoc[2]);
		glScaled(4, 4, 4);
		DrawBlast(50, 50, 9);
		glPopMatrix();
		break;
	}

	switch (rDetoPhaze)
	{
	case 1:
		glPushMatrix();
		glTranslated(rDetonationLoc[0], rDetonationLoc[(int)ground[0][2]], rDetonationLoc[2]);
		glScaled(1, 1, 1);
		DrawBlast(50, 50, 7);
		glPopMatrix();
		break;
	case 2:
		glPushMatrix();
		glTranslated(rDetonationLoc[0], rDetonationLoc[(int)ground[0][2]], rDetonationLoc[2]);
		glScaled(2, 2, 2);
		DrawBlast(50, 50, 8);
		glPopMatrix();
		break;
	case 3:
		glPushMatrix();
		glTranslated(rDetonationLoc[0], rDetonationLoc[(int)ground[0][2]], rDetonationLoc[2]);
		glScaled(4, 4, 4);
		DrawBlast(50, 50, 9);
		glPopMatrix();
		break;
	}

	glViewport(0, 0, 100, 100); //  pitch control  **********************

	glDisable(GL_DEPTH_TEST);  // disable testing what is near and what is far
							   // From this point the later drawing will occlude the previous drawing

	glMatrixMode(GL_PROJECTION); // changes 3D to 2D
	glLoadIdentity(); // here starts  the perspective projection
	glOrtho(-1, 1, -1, 1, -1, 1); // 2D
	glMatrixMode(GL_MODELVIEW); // 
	glLoadIdentity(); // here starts  the orthogonal projection

	glEnable(GL_BLEND); // add transparency
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4d(0.7, 0.4, 0.4, 0.6);
	glBegin(GL_POLYGON);
	glVertex2d(-1, -1);
	glVertex2d(-1, 1);
	glVertex2d(1, 1);
	glVertex2d(1, -1);
	glEnd();
	glDisable(GL_BLEND); // turn off transparency

	glColor3d(0, 0, 0);
	glBegin(GL_LINES);
	glVertex2d(0, -1);
	glVertex2d(0, 1);
	glEnd();

	// handle
	glColor3d(0.8, 0.8, 0.8);

	glPushMatrix();
	glTranslated(0, pitch, 0);
	glBegin(GL_POLYGON);
	glVertex2d(-0.2, -0.1);
	glVertex2d(-0.2, 0.1);
	glVertex2d(0.2, 0.1);
	glVertex2d(0.2, -0.1);
	glEnd();
	glPopMatrix();

	glEnable(GL_DEPTH_TEST);

	glutSwapBuffers();// show what was drawn in "frame buffer"
}

void displayCockpit()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, W, H ); 

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity(); // here starts  the perspective projection

	glFrustum(-1, 1, -1, 1, 1.8, 500);
	//        eye coordinates, poi, up
	gluLookAt(airpl[0], airpl[1]+/*16*/1, airpl[2],
		airpl[0]+air_dir[0], airpl[1]+air_dir[1]+/*15*/.5, airpl[2]+air_dir[2],
		0, 1, 0);

	DrawFloor();

	// show airplane
	glPushMatrix();
	glTranslated(airpl[0], /*15+*/airpl[1], airpl[2]);
	glRotated(yaw * 180 / PI, 0, 1, 0);
	glScaled(.5, .5, .5);
	glRotated(90, 0, 1, 0);
	glRotated(roll * 1000, 1, 0, 0);
	glRotated(-pitch * 180 / PI, 0, 0, 1);
	DrawAirplane();
	glPopMatrix();

	// show canon
	glPushMatrix();
	glTranslated(canonx - GSIZE / 2, ground[canonz][canonx] + 0.01, canonz - GSIZE / 2);
	//glRotated(offset, 0, 1, 0);
	drawCanon();
	glPopMatrix();
	flattenCanonGround();

	// canon blast
	switch (blastPhaze)
	{
		// 3 cases for canon blast
	case 1:
		glPushMatrix();
		glTranslated(canonx - GSIZE / 2, ground[canonz][canonx] + 4, canonz - GSIZE / 2);
		glScaled(0.5, 0.5, 0.5);
		DrawBlast(50, 50, 1);
		glPopMatrix();
		break;

	case 2:
		glPushMatrix();
		glTranslated(canonx - GSIZE / 2, ground[canonz][canonx] + 4, canonz - GSIZE / 2);
		glScaled(1, 1, 1);
		DrawBlast(50, 50, 2);
		glPopMatrix();
		break;

	case 3:
		glPushMatrix();
		glTranslated(canonx - GSIZE / 2, ground[canonz][canonx] + 4, canonz - GSIZE / 2);
		glScaled(2, 2, 2);
		DrawBlast(50, 50, 3);
		glPopMatrix();
		break;

		// 3 cases for conon hit blast
	case 4:
		glPushMatrix();
		glTranslated(canonHitX/* - GSIZE / 2*/, airpl[1], canonHitZ/* - GSIZE / 2*/);
		glScaled(0.5, 0.5, 0.5);
		DrawBlast(50, 50, 4);
		glPopMatrix();
		break;

	case 5:
		glPushMatrix();
		glTranslated(canonHitX/* - GSIZE / 2*/, airpl[1], canonHitZ/* - GSIZE / 2*/);
		glScaled(1, 1, 1);
		DrawBlast(50, 50, 5);
		glPopMatrix();
		break;

	case 6:
		glPushMatrix();
		glTranslated(canonHitX/* - GSIZE / 2*/, airpl[1], canonHitZ/* - GSIZE / 2*/);
		glScaled(2, 2, 2);
		DrawBlast(50, 50, 6);
		glPopMatrix();
		break;

	default:
		break;
	}

	//bomb blast
	switch (lDetoPhaze)
	{
	case 1:
		glPushMatrix();
		glTranslated(lDetonationLoc[0], lDetonationLoc[(int)ground[0][2]], lDetonationLoc[2]);
		glScaled(1, 1, 1);
		DrawBlast(50, 50, 7);
		glPopMatrix();
		break;
	case 2:
		glPushMatrix();
		glTranslated(lDetonationLoc[0], lDetonationLoc[(int)ground[0][2]], lDetonationLoc[2]);
		glScaled(2, 2, 2);
		DrawBlast(50, 50, 8);
		glPopMatrix();
		break;
	case 3:
		glPushMatrix();
		glTranslated(lDetonationLoc[0], lDetonationLoc[(int)ground[0][2]], lDetonationLoc[2]);
		glScaled(4, 4, 4);
		DrawBlast(50, 50, 9);
		glPopMatrix();
		break;
	}

	switch (rDetoPhaze)
	{
	case 1:
		glPushMatrix();
		glTranslated(rDetonationLoc[0], rDetonationLoc[(int)ground[0][2]], rDetonationLoc[2]);
		glScaled(1, 1, 1);
		DrawBlast(50, 50, 7);
		glPopMatrix();
		break;
	case 2:
		glPushMatrix();
		glTranslated(rDetonationLoc[0], rDetonationLoc[(int)ground[0][2]], rDetonationLoc[2]);
		glScaled(2, 2, 2);
		DrawBlast(50, 50, 8);
		glPopMatrix();
		break;
	case 3:
		glPushMatrix();
		glTranslated(rDetonationLoc[0], rDetonationLoc[(int)ground[0][2]], rDetonationLoc[2]);
		glScaled(4, 4, 4);
		DrawBlast(50, 50, 9);
		glPopMatrix();
		break;
	}

	glViewport(0, 0, 100, 100); //  pitch control  **********************
 
	glDisable(GL_DEPTH_TEST);  // disable testing what is near and what is far
	// From this point the later drawing will occlude the previous drawing

	glMatrixMode(GL_PROJECTION); // changes 3D to 2D
	glLoadIdentity(); // here starts  the perspective projection
	glOrtho(-1, 1, -1, 1, -1, 1); // 2D
	glMatrixMode(GL_MODELVIEW); // 
	glLoadIdentity(); // here starts  the orthogonal projection

	glEnable(GL_BLEND); // add transparency
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4d(0.7, 0.4, 0.4,0.6);
	glBegin(GL_POLYGON);
	glVertex2d(-1, -1);
	glVertex2d(-1, 1);
	glVertex2d(1, 1);
	glVertex2d(1, -1);
	glEnd();
	glDisable(GL_BLEND); // turn off transparency

	glColor3d(0,0,0);
	glBegin(GL_LINES);
	glVertex2d(0, -1);
	glVertex2d(0, 1);
	glEnd();

	// handle
	glColor3d(0.8, 0.8, 0.8);

	glPushMatrix();
	glTranslated(0, pitch, 0);
		glBegin(GL_POLYGON);
		glVertex2d(-0.2, -0.1);
		glVertex2d(-0.2, 0.1);
		glVertex2d(0.2, 0.1);
		glVertex2d(0.2, -0.1);
		glEnd();
	glPopMatrix();

	glEnable(GL_DEPTH_TEST);

	glutSwapBuffers();// show what was drawn in "frame buffer"
}


void displayCombined()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//               x,  y ,    width,   height
	glViewport(0, H / 2, W / 2, H / 2); // regular

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity(); // here starts  the perspective projection

	glFrustum(-1, 1, -1, 1, 0.8, 500);
	//        eye coordinates, poi, up
	gluLookAt(eyex, eyey, eyez,
		eyex + dirx, eyey - 0.3, eyez + dirz,
		0, 1, 0);

	DrawFloor();

	// show airplane
	glPushMatrix();
	glTranslated(airpl[0], /*15*/airpl[1], airpl[2]);
	glRotated(yaw * 180 / PI, 0, 1, 0);
	glScaled(.5, .5, .5);
	glRotated(90, 0, 1, 0);
	glRotated(roll * 1000, 1, 0, 0);
	DrawAirplane();
	glPopMatrix();

	// show canon
	glPushMatrix();
	glTranslated(canonx - GSIZE / 2, ground[canonz][canonx] + 0.01, canonz - GSIZE / 2);
	//glRotated(offset, 0, 1, 0);
	drawCanon();
	glPopMatrix();
	flattenCanonGround();

	// canon blast
	switch (blastPhaze)
	{
		// 3 cases for canon blast
	case 1:
		glPushMatrix();
		glTranslated(canonx - GSIZE / 2, ground[canonz][canonx] + 4, canonz - GSIZE / 2);
		glScaled(0.5, 0.5, 0.5);
		DrawBlast(50, 50, 1);
		glPopMatrix();
		break;

	case 2:
		glPushMatrix();
		glTranslated(canonx - GSIZE / 2, ground[canonz][canonx] + 4, canonz - GSIZE / 2);
		glScaled(1, 1, 1);
		DrawBlast(50, 50, 2);
		glPopMatrix();
		break;

	case 3:
		glPushMatrix();
		glTranslated(canonx - GSIZE / 2, ground[canonz][canonx] + 4, canonz - GSIZE / 2);
		glScaled(2, 2, 2);
		DrawBlast(50, 50, 3);
		glPopMatrix();
		break;

		// 3 cases for conon hit blast
	case 4:
		glPushMatrix();
		glTranslated(canonHitX/* - GSIZE / 2*/, airpl[1], canonHitZ/* - GSIZE / 2*/);
		glScaled(0.5, 0.5, 0.5);
		DrawBlast(50, 50, 4);
		glPopMatrix();
		break;

	case 5:
		glPushMatrix();
		glTranslated(canonHitX/* - GSIZE / 2*/, airpl[1], canonHitZ/* - GSIZE / 2*/);
		glScaled(1, 1, 1);
		DrawBlast(50, 50, 5);
		glPopMatrix();
		break;

	case 6:
		glPushMatrix();
		glTranslated(canonHitX/* - GSIZE / 2*/, airpl[1], canonHitZ/* - GSIZE / 2*/);
		glScaled(2, 2, 2);
		DrawBlast(50, 50, 6);
		glPopMatrix();
		break;

	default:
		break;
	}

	//bomb blast
	switch (lDetoPhaze)
	{
	case 1:
		glPushMatrix();
		glTranslated(lDetonationLoc[0], lDetonationLoc[(int)ground[0][2]], lDetonationLoc[2]);
		glScaled(1, 1, 1);
		DrawBlast(50, 50, 7);
		glPopMatrix();
		break;
	case 2:
		glPushMatrix();
		glTranslated(lDetonationLoc[0], lDetonationLoc[(int)ground[0][2]], lDetonationLoc[2]);
		glScaled(2, 2, 2);
		DrawBlast(50, 50, 8);
		glPopMatrix();
		break;
	case 3:
		glPushMatrix();
		glTranslated(lDetonationLoc[0], lDetonationLoc[(int)ground[0][2]], lDetonationLoc[2]);
		glScaled(4, 4, 4);
		DrawBlast(50, 50, 9);
		glPopMatrix();
		break;
	}

	switch (rDetoPhaze)
	{
	case 1:
		glPushMatrix();
		glTranslated(rDetonationLoc[0], rDetonationLoc[(int)ground[0][2]], rDetonationLoc[2]);
		glScaled(1, 1, 1);
		DrawBlast(50, 50, 7);
		glPopMatrix();
		break;
	case 2:
		glPushMatrix();
		glTranslated(rDetonationLoc[0], rDetonationLoc[(int)ground[0][2]], rDetonationLoc[2]);
		glScaled(2, 2, 2);
		DrawBlast(50, 50, 8);
		glPopMatrix();
		break;
	case 3:
		glPushMatrix();
		glTranslated(rDetonationLoc[0], rDetonationLoc[(int)ground[0][2]], rDetonationLoc[2]);
		glScaled(4, 4, 4);
		DrawBlast(50, 50, 9);
		glPopMatrix();
		break;
	}

	//               x,  y ,    width,   height
	glViewport(W/2, H / 2, W / 2, H / 2);  // top ************************************

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity(); // here starts  the perspective projection

	glFrustum(-1, 1, -1, 1, 0.8, 500);
	//        eye coordinates, poi, up
	gluLookAt(0, 85, 0,
		1, 0, 0,
		0, 1, 0);

	DrawFloor();

	// show airplane
	glPushMatrix();
	glTranslated(airpl[0], /*15*/airpl[1], airpl[2]);
	glRotated(yaw * 180 / PI, 0, 1, 0);
	glScaled(.5, .5, .5);
	glRotated(90, 0, 1, 0);
	glRotated(roll * 1000, 1, 0, 0);
	DrawAirplane();
	glPopMatrix();

	// show canon
	glPushMatrix();
	glTranslated(canonx - GSIZE / 2, ground[canonz][canonx] + 0.01, canonz - GSIZE / 2);
	//glRotated(offset, 0, 1, 0);
	drawCanon();
	glPopMatrix();
	flattenCanonGround();

	// canon blast
	switch (blastPhaze)
	{
		// 3 cases for canon blast
	case 1:
		glPushMatrix();
		glTranslated(canonx - GSIZE / 2, ground[canonz][canonx] + 4, canonz - GSIZE / 2);
		glScaled(0.5, 0.5, 0.5);
		DrawBlast(50, 50, 1);
		glPopMatrix();
		break;

	case 2:
		glPushMatrix();
		glTranslated(canonx - GSIZE / 2, ground[canonz][canonx] + 4, canonz - GSIZE / 2);
		glScaled(1, 1, 1);
		DrawBlast(50, 50, 2);
		glPopMatrix();
		break;

	case 3:
		glPushMatrix();
		glTranslated(canonx - GSIZE / 2, ground[canonz][canonx] + 4, canonz - GSIZE / 2);
		glScaled(2, 2, 2);
		DrawBlast(50, 50, 3);
		glPopMatrix();
		break;

		// 3 cases for conon hit blast
	case 4:
		glPushMatrix();
		glTranslated(canonHitX/* - GSIZE / 2*/, airpl[1], canonHitZ/* - GSIZE / 2*/);
		glScaled(0.5, 0.5, 0.5);
		DrawBlast(50, 50, 4);
		glPopMatrix();
		break;

	case 5:
		glPushMatrix();
		glTranslated(canonHitX/* - GSIZE / 2*/, airpl[1], canonHitZ/* - GSIZE / 2*/);
		glScaled(1, 1, 1);
		DrawBlast(50, 50, 5);
		glPopMatrix();
		break;

	case 6:
		glPushMatrix();
		glTranslated(canonHitX/* - GSIZE / 2*/, airpl[1], canonHitZ/* - GSIZE / 2*/);
		glScaled(2, 2, 2);
		DrawBlast(50, 50, 6);
		glPopMatrix();
		break;

	default:
		break;
	}

	//bomb blast
	switch (lDetoPhaze)
	{
	case 1:
		glPushMatrix();
		glTranslated(lDetonationLoc[0], lDetonationLoc[(int)ground[0][2]], lDetonationLoc[2]);
		glScaled(1, 1, 1);
		DrawBlast(50, 50, 7);
		glPopMatrix();
		break;
	case 2:
		glPushMatrix();
		glTranslated(lDetonationLoc[0], lDetonationLoc[(int)ground[0][2]], lDetonationLoc[2]);
		glScaled(2, 2, 2);
		DrawBlast(50, 50, 8);
		glPopMatrix();
		break;
	case 3:
		glPushMatrix();
		glTranslated(lDetonationLoc[0], lDetonationLoc[(int)ground[0][2]], lDetonationLoc[2]);
		glScaled(4, 4, 4);
		DrawBlast(50, 50, 9);
		glPopMatrix();
		break;
	}

	switch (rDetoPhaze)
	{
	case 1:
		glPushMatrix();
		glTranslated(rDetonationLoc[0], rDetonationLoc[(int)ground[0][2]], rDetonationLoc[2]);
		glScaled(1, 1, 1);
		DrawBlast(50, 50, 7);
		glPopMatrix();
		break;
	case 2:
		glPushMatrix();
		glTranslated(rDetonationLoc[0], rDetonationLoc[(int)ground[0][2]], rDetonationLoc[2]);
		glScaled(2, 2, 2);
		DrawBlast(50, 50, 8);
		glPopMatrix();
		break;
	case 3:
		glPushMatrix();
		glTranslated(rDetonationLoc[0], rDetonationLoc[(int)ground[0][2]], rDetonationLoc[2]);
		glScaled(4, 4, 4);
		DrawBlast(50, 50, 9);
		glPopMatrix();
		break;
	}

	//               x,  y ,    width,   height
	glViewport(0, 0, W, H / 2);  // cockpit ************************************


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity(); // here starts  the perspective projection

	glFrustum(-1, 1, -1, 1, 1.8, 500);
	//        eye coordinates, poi, up
	gluLookAt(airpl[0], airpl[1] + /*16*/1, airpl[2],
		airpl[0] + air_dir[0], airpl[1] + air_dir[1] + /*15*/.5, airpl[2] + air_dir[2],
		0, 1, 0);

	DrawFloor();

	// show airplane
	glPushMatrix();
	glTranslated(airpl[0], /*15*/airpl[1], airpl[2]);
	glRotated(yaw * 180 / PI, 0, 1, 0);
	glScaled(.5, .5, .5);
	glRotated(90, 0, 1, 0);
	glRotated(roll * 1000, 1, 0, 0);
	DrawAirplane();
	glPopMatrix();

	// show canon
	glPushMatrix();
	glTranslated(canonx - GSIZE / 2, ground[canonz][canonx] + 0.01, canonz - GSIZE / 2);
	//glRotated(offset, 0, 1, 0);
	drawCanon();
	glPopMatrix();
	flattenCanonGround();

	// canon blast
	switch (blastPhaze)
	{
		// 3 cases for canon blast
	case 1:
		glPushMatrix();
		glTranslated(canonx - GSIZE / 2, ground[canonz][canonx] + 4, canonz - GSIZE / 2);
		glScaled(0.5, 0.5, 0.5);
		DrawBlast(50, 50, 1);
		glPopMatrix();
		break;

	case 2:
		glPushMatrix();
		glTranslated(canonx - GSIZE / 2, ground[canonz][canonx] + 4, canonz - GSIZE / 2);
		glScaled(1, 1, 1);
		DrawBlast(50, 50, 2);
		glPopMatrix();
		break;

	case 3:
		glPushMatrix();
		glTranslated(canonx - GSIZE / 2, ground[canonz][canonx] + 4, canonz - GSIZE / 2);
		glScaled(2, 2, 2);
		DrawBlast(50, 50, 3);
		glPopMatrix();
		break;

		// 3 cases for conon hit blast
	case 4:
		glPushMatrix();
		glTranslated(canonHitX/* - GSIZE / 2*/, airpl[1], canonHitZ/* - GSIZE / 2*/);
		glScaled(0.5, 0.5, 0.5);
		DrawBlast(50, 50, 4);
		glPopMatrix();
		break;

	case 5:
		glPushMatrix();
		glTranslated(canonHitX/* - GSIZE / 2*/, airpl[1], canonHitZ/* - GSIZE / 2*/);
		glScaled(1, 1, 1);
		DrawBlast(50, 50, 5);
		glPopMatrix();
		break;

	case 6:
		glPushMatrix();
		glTranslated(canonHitX/* - GSIZE / 2*/, airpl[1], canonHitZ/* - GSIZE / 2*/);
		glScaled(2, 2, 2);
		DrawBlast(50, 50, 6);
		glPopMatrix();
		break;

	default:
		break;
	}

	//bomb blast
	switch (lDetoPhaze)
	{
	case 1:
		glPushMatrix();
		glTranslated(lDetonationLoc[0], lDetonationLoc[(int)ground[0][2]], lDetonationLoc[2]);
		glScaled(1, 1, 1);
		DrawBlast(50, 50, 7);
		glPopMatrix();
		break;
	case 2:
		glPushMatrix();
		glTranslated(lDetonationLoc[0], lDetonationLoc[(int)ground[0][2]], lDetonationLoc[2]);
		glScaled(2, 2, 2);
		DrawBlast(50, 50, 8);
		glPopMatrix();
		break;
	case 3:
		glPushMatrix();
		glTranslated(lDetonationLoc[0], lDetonationLoc[(int)ground[0][2]], lDetonationLoc[2]);
		glScaled(4, 4, 4);
		DrawBlast(50, 50, 9);
		glPopMatrix();
		break;
	}

	switch (rDetoPhaze)
	{
	case 1:
		glPushMatrix();
		glTranslated(rDetonationLoc[0], rDetonationLoc[(int)ground[0][2]], rDetonationLoc[2]);
		glScaled(1, 1, 1);
		DrawBlast(50, 50, 7);
		glPopMatrix();
		break;
	case 2:
		glPushMatrix();
		glTranslated(rDetonationLoc[0], rDetonationLoc[(int)ground[0][2]], rDetonationLoc[2]);
		glScaled(2, 2, 2);
		DrawBlast(50, 50, 8);
		glPopMatrix();
		break;
	case 3:
		glPushMatrix();
		glTranslated(rDetonationLoc[0], rDetonationLoc[(int)ground[0][2]], rDetonationLoc[2]);
		glScaled(4, 4, 4);
		DrawBlast(50, 50, 9);
		glPopMatrix();
		break;
	}


	glutSwapBuffers();// show what was drawn in "frame buffer"
}




void idle()
{
	if (endGame)
	{
		exit(0);
	}

	offset += 0.3;

	if (!stop)
	{
		UpdateTerrain2();
		UpdateTerrain2();
		UpdateTerrain3();
	}

	// calculate dirx and dirz
	sight_angle += angular_speed;
	dirx = sin(sight_angle);
	dirz = cos(sight_angle);
	eyex += speed*dirx;
	eyez += speed*dirz;

	// airplane
	yaw += air_angular_speed;
	air_dir[0] = sin(yaw);
	air_dir[1] = sin(pitch);
	air_dir[2] = cos(yaw);
	airpl[0] += air_speed*air_dir[0];
	airpl[1] += air_speed*air_dir[1];
	airpl[2] += air_speed*air_dir[2];

	// canon 
	canonAirplaneDist = sqrt(pow((double)eyex - (canonx - (GSIZE / 2)), 2.0) + pow((double)eyez - (canonz - (GSIZE / 2)), 2.0) + pow((double)eyey - ground[canonz][canonx], 2.0));
	if (blastPhaze == 6)
	{
		blastPhaze = 0;

		if (canonHitX == (int)airpl[0] && canonHitZ == (int)airpl[2]) // airplane is hit by the cannon
		{
			exit(0);
		}
	}
	if (canonAirplaneDist <= CANON_RANGE)
	{
		if ((int)offset % FIRE_RATE == 0 || blastPhaze > 0)
		{
			blastPhaze++;
		}

		if (blastPhaze == 4)
		{
			generateHitCoord();
		}
	}
	else
		blastPhaze = 0;

	// bomb dropping
	if (lBombStart)
	{
		if (!leftBombReleased)
			{
				lBombInitDir[0] = air_dir[0];
				lBombInitDir[1] = air_dir[1];
				lBombInitDir[2] = air_dir[2];
				lBombInitSpeed = air_speed;
				lBombLoc[0] = air_speed*air_dir[0];
				lBombLoc[1] = air_speed*air_dir[1];
				lBombLoc[2] = air_speed*air_dir[2];
			}
		else // bomb is already in midair
		{
			lt += 0.085;
			lBombLoc[0] = lBombInitSpeed*lBombInitDir[0];
			lBombLoc[1] = lBombInitSpeed*lBombInitDir[1] + GRAVITY*lt*lt;
			lBombLoc[2] = lBombInitSpeed*lBombInitDir[2];
		}
		leftBombReleased = true;
	}

	if (rBombStart)
	{
		if (!rightBombReleased)
		{
			rBombInitDir[0] = air_dir[0];
			rBombInitDir[1] = air_dir[1];
			rBombInitDir[2] = air_dir[2];
			rBombInitSpeed = air_speed;
			rBombLoc[0] = air_speed*air_dir[0];
			rBombLoc[1] = air_speed*air_dir[1];
			rBombLoc[2] = air_speed*air_dir[2];
		}
		else // bomb is already in midair
		{
			rt += 0.085;
			rBombLoc[0] = rBombInitSpeed*rBombInitDir[0];
			rBombLoc[1] = rBombInitSpeed*rBombInitDir[1] + GRAVITY*rt*rt;
			rBombLoc[2] = rBombInitSpeed*rBombInitDir[2];
		}
		rightBombReleased = true;
	}

	// bomb detonation
	if ((int)ground[(int)lBombLoc[0]][(int)lBombLoc[2]] >= (int)lBombLoc[1])
	{
		lBombDetonation = true;
		lDetonationLoc[0] = lBombLoc[0];
		lDetonationLoc[1] = lBombLoc[1];
		lDetonationLoc[2] = lBombLoc[2];
	}
	if (lBombDetonation)
	{
		lDetoPhaze++;
		if (lDetoPhaze == 4)
		{
			lBombDetonation = false;

			if (lDetonationLoc[0] <= (canonx + BOMB_HIT_RADIUS) && lDetonationLoc[0] >= (canonx - BOMB_HIT_RADIUS) &&
				lDetonationLoc[2] <= (canonz + BOMB_HIT_RADIUS) && lDetonationLoc[2] >= (canonz - BOMB_HIT_RADIUS))
			{
				exit(0);
			}
		}
	}

	if ((int)ground[(int)rBombLoc[0]][(int)rBombLoc[2]] >= (int)rBombLoc[1])
	{
		rBombDetonation = true;
		rDetonationLoc[0] = rBombLoc[0];
		rDetonationLoc[1] = rBombLoc[1];
		rDetonationLoc[2] = rBombLoc[2];
	}
	if (rBombDetonation)
	{
		rDetoPhaze++;
		if (rDetoPhaze == 4)
		{
			rBombDetonation = false;

			if (rDetonationLoc[0] <= (canonx + BOMB_HIT_RADIUS) && rDetonationLoc[0] >= (canonx - BOMB_HIT_RADIUS) &&
				rDetonationLoc[2] <= (canonz + BOMB_HIT_RADIUS) && rDetonationLoc[2] >= (canonz - BOMB_HIT_RADIUS))
			{
				exit(0);
			}
		}
	}



	glutPostRedisplay();// calls indirectly to display
}

void SpecialKey(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		angular_speed += 0.001;
		break;
	case GLUT_KEY_RIGHT:
		angular_speed -= 0.001;
		break;
	case GLUT_KEY_UP:
		speed += 0.01;
		break;
	case GLUT_KEY_DOWN:
		speed -= 0.01;
		break;
	case GLUT_KEY_PAGE_UP:
		eyey += 0.1;
		break;
	case GLUT_KEY_PAGE_DOWN:
		eyey -= 0.1;
		break;
	}
}
void mouse(int button, int state, int x, int y)
{
//	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
//		stop = !stop;
//	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
//		Smooth();

}

void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'a':
		air_angular_speed += 0.001;
		roll += 0.01;
		break;
	case 'w':
		air_speed += 0.01;
		break;
	case 's':
		air_speed -= 0.01;
		break;
	case 'd':
		air_angular_speed -= 0.001;
		roll -= 0.01;
		break;

	case 'q':
		lBombStart = true;
		break;
	case 'e':
		rBombStart = true;
		break;
	}
}

void menu(int option)
{
	switch (option)
	{
	case 1: // regular view
		glutDisplayFunc(display);
		break;
	case 2: // top view
		glutDisplayFunc(displayTop);
		break;
	case 3: // cockpit
		glutDisplayFunc(displayCockpit);
		break;
	case 4: // combined view
		glutDisplayFunc(displayCombined);
		break;
	}
}

void DragMouse(int x, int y)
{
	int yy = H - y;
	int startx = 100 * (-0.2 + 1) / 2;
	int stopx = 100 * (0.2 + 1) / 2;
	int starty = 100 * (pitch-0.1 + 1) / 2;
	int stopy = 100 * (pitch+0.1 + 1) / 2;

	// change pitch if we caught the handle close enough to its center
	if (x > startx && x<stopx && yy>starty && yy < stopy && yy<98 && yy>2)
		pitch = (yy / 100.0)*2 - 1;

}

void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(W, H);
	glutInitWindowPosition/*(800, 200);*/(200, 100);
	glutCreateWindow("3D Example");

	glutDisplayFunc(display); // refresh function
	glutIdleFunc(idle);
	glutSpecialFunc(SpecialKey);

	glutMouseFunc(mouse);
	glutKeyboardFunc(Keyboard);
	glutMotionFunc(DragMouse);

	// menu
	glutCreateMenu(menu);
	glutAddMenuEntry("Regular view", 1);
	glutAddMenuEntry("Top view", 2);
	glutAddMenuEntry("Cockpit view", 3);
	glutAddMenuEntry("Combined view", 4);

	glutAttachMenu(GLUT_RIGHT_BUTTON);

	init();

	glutMainLoop();
}