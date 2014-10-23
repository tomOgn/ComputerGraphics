#include <cmath>
#include <iostream>
#include <stdio.h>     
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "GL\glut.h"
#include "types.h"
using namespace std;

static const GLfloat PI = 3.14159265358979f;
static const int FlowerSpeed[3][5] =
{ 
	{ 5, 10, 0, 5, 6 }, 
	{ 2, 6, 0, 2, 3 }, 
	{ 1, 3, 0, 1, 1 }
};
static Color BrownWindow(0.54, 0.27, 0.07);
static Color GreenFlower(0.2, 0.6, 0.2);
static Color GreenGrass(0.2, 0.5, 0.0);
static Color BlueFlower(0.2, 0.4, 0.8);
static Color BlueSky(0.5, 0.78, 1.0);
static Color ColorCar(1.0, 0.0, 0.0);
static Color ColorGrass(0.2, 0.5, 0.0);
static Color ColorMountains(0.6, 0.6, 0.6);
static Color ColorRoof(0.8, 0.1, 0.1);
static Color ColorWindow(0.28, 0.82, 0.8);
static Color GrayMoon(0.78, 0.78, 0.78);
static Color GrayMountains(0.6, 0.6, 0.6);
static Color Black(0.0, 0.0, 0.0);
static Color OrangeWall(0.8, 0.5, 0.2);
static Color PurpleBall(0.6, 0.2, 0.8);
static Color RedFlower(0.6, 0.1, 0.1);
static Color RedRoof(0.8, 0.1, 0.1);
static Color RedCar(1.0, 0.0, 0.0);
static Color SilverWheel(0.75, 0.75, 0.75);
static Color YellowSun(0.9, 0.8, 0.0);
static Color YellowStar(1.0, 1.0, 0.4);
static Color YellowWindow(1.0, 1.0, 0.0);
static Color WhiteSnow(0.93, 0.91, 0.91);
static Color * EclipseColors[] =
{ 
	&BrownWindow, &GreenFlower, &ColorGrass, &BlueFlower,
	&BlueSky, &GrayMoon, &ColorMountains, &OrangeWall,
	&ColorCar, &RedFlower, &ColorRoof, &SilverWheel,
	&YellowSun, &YellowStar
};
static Color * SnowColors[] =
{
	&ColorGrass, &ColorMountains
};
static const int
	SunnyDay = 0,
	RainyNight = 1,
	StarryNight = 2,
	Eclipse = 3,
	SnowyNight = 4,
	DropsCount = 40;
static unsigned int
	Time = SunnyDay, 
	Milliseconds = 0,
	WindowWidth = 1024,
	WindowHeight = 800,
	RainbowStart = 0,
	RainbowEnd = 0,
	SpokesRotation = 0;
static bool EclipseEnding = false;
static GLfloat
	FlowerRotation = 0.0,
	EclipseX = 0.0,
	CarX = 0,
	SnowX,
	SnowY;
Point RainDrop[DropsCount], SnowDrop[DropsCount];
Snowflake Flake;

static inline float Min(GLfloat a, GLfloat b)
{
	return a > b ? b : a;
}

/* Draw a line. */
static inline void DrawLine(GLfloat length)
{
	glBegin(GL_LINES);
	glVertex2f(0.0, 0.0);
	glVertex2f(0.0, WindowHeight * length);
	glEnd();
	glTranslatef(0.0, WindowHeight * length, 0.0);
}

/* Draw a rough circle. */
static inline void DrawRoughCircle(GLfloat x, GLfloat y, GLfloat radius, Color color)
{
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(color.R, color.G, color.B);
	for (GLfloat i = 0; i < 360; i += 5)
		glVertex2f(x + radius * cos(i), y + radius * sin(i));
	glEnd();
}

/* Draw a smooth circle. */
static inline void DrawSmoothCircle(GLfloat x, GLfloat y, GLfloat radius, Color color)
{
	glBegin(GL_POLYGON);
	glColor3f(color.R, color.G, color.B);
	for (GLfloat i = 0; i <= 2 * PI; i += 0.05)
		glVertex2f(x + radius * cos(i), y + radius * sin(i));
	glEnd();
}

/* Split the window into two regions (sky and ground). */
static void DrawGroundSky()
{
	GLfloat y = WindowHeight * 0.1;

	glBegin(GL_QUADS);
	// Ground
	glColor3f(ColorGrass.R, ColorGrass.G, ColorGrass.B);
	glVertex2f(0.0, 0.0);
	glVertex2f(WindowWidth, 0.0);
	glVertex2f(WindowWidth, y);
	glVertex2f(0.0, y);

	// Sky
	if (Time == SunnyDay || Time == Eclipse)
		glColor3f(BlueSky.R, BlueSky.G, BlueSky.B);
	else
		glColor3f(Black.R, Black.G, Black.B);
	glVertex2f(0.0, y);
	glVertex2f(WindowWidth, y);
	glVertex2f(WindowWidth, WindowHeight);
	glVertex2f(0.0, WindowHeight);
	glEnd();
}

/* Draw sinusoidal mountains. */
static void DrawMountains()
{
	GLfloat y1, y2, y3;

	y1 = WindowHeight * 0.1;
	y2 = WindowHeight * 0.6;
	y3 = WindowHeight * 0.07;

	glBegin(GL_QUADS);
	glColor3f(ColorMountains.R, ColorMountains.G, ColorMountains.B);
	for (GLfloat i = 0, j = 0; i <= WindowWidth; i += 5, ++j)
	{
		glVertex2f(i, y1);
		glVertex2f(i, y2 + y3 * sin(0.1 * j));
		glVertex2f(i + 5.0, y2 + y3 * sin(0.1 * (j + 1)));
		glVertex2f(i + 5.0, y1);
	}
	glEnd();

	/* In case of snow, draw the part of mountains not covered by it */
	if (Time == SnowyNight)
	{
		glBegin(GL_QUADS);
		glColor3f(GrayMountains.R, GrayMountains.G, GrayMountains.B);
		glVertex2f(0, y1);
		glVertex2f(0, WindowHeight * 0.52);
		glVertex2f(WindowWidth, WindowHeight * 0.52);
		glVertex2f(WindowWidth, y1);
		glEnd();
	}
}

/* Draw a sun. */
static void DrawSun()
{
	GLfloat x, y, radius;

	x = WindowWidth * 0.9;
	y = WindowHeight * 0.9;
	radius = WindowWidth * 0.05;

	DrawRoughCircle(x, y, radius, YellowSun);
}

/* Draw a solar eclipse. */
static void DrawEclipse()
{
	GLfloat x, y, radius;

	x = EclipseX;
	y = WindowHeight * 0.9;
	radius = WindowWidth * 0.05;

	DrawSmoothCircle(x, y, radius, BlueSky);
}

/* Draw a rainbow. */
static void DrawRainbow()
{
	GLfloat y, width, height;

	y = WindowHeight * 0.8;
	width = WindowWidth * 0.002;
	height = WindowHeight * 0.01;

	if (RainbowEnd >= WindowWidth)
		RainbowStart += width;
	else
		RainbowEnd += width;

	if (RainbowStart >= RainbowEnd)
	{
		RainbowStart = 0;
		RainbowEnd = width;
	}

	for (GLfloat x = 0; x < RainbowEnd; x += width)
	{
		if (x < RainbowStart)
		{
			glBegin(GL_QUAD_STRIP);
			glColor3f(BlueSky.R, BlueSky.G, BlueSky.B);
			glVertex2f(x, y - height * 5);
			glVertex2f(x + width, y - height * 5);
			glEnd();
		}
		else
		{
			glBegin(GL_QUAD_STRIP);
			glColor3f(1, 0, 0);
			glVertex2f(x, y);
			glVertex2f(x + width, y);

			glColor3f(1, 0.6, 0);
			glVertex2f(x, y - height);
			glVertex2f(x + width, y - height);

			glColor3f(1, 1, 0);
			glVertex2f(x, y - height * 2);
			glVertex2f(x + width, y - height * 2);

			glColor3f(0.2, 1, 0);
			glVertex2f(x, y - height * 3);
			glVertex2f(x + width, y - height * 3);

			glColor3f(0, 0.6, 1);
			glVertex2f(x, y - height * 4);
			glVertex2f(x + width, y - height * 4);

			glColor3f(0.4, 0.2, 1);
			glVertex2f(x, y - height * 5);
			glVertex2f(x + width, y - height * 5);
			glEnd();
		}
	}
}

/* Draw a moon. */
static void DrawMoon()
{
	GLfloat x, y, radius;

	x = WindowWidth * 0.9;
	y = WindowHeight * 0.9;
	radius = WindowWidth * 0.05;

	// Draw a full moon
	DrawSmoothCircle(x, y, radius, GrayMoon);

	// Obscure part of it
	x += WindowWidth * 0.032;
	DrawSmoothCircle(x, y, radius, Black);
}

/* Draw a star. */
static void DrawStar(GLfloat x, GLfloat y)
{
	GLfloat distance = 0.0005;

	glPushMatrix();
	glTranslatef(WindowWidth * x, WindowHeight * y, 0);
	glColor3f(YellowStar.R, YellowStar.G, YellowStar.B);
	glPushMatrix();
	for (int segment = 0; segment < 40; segment++)
	{
		glTranslatef(0, WindowHeight * distance, 0);
		glRotatef(45, 0, 0, 1);
		distance += 0.0005;
		glPushMatrix();
		glRotatef(90, 0, 0, 1);
		DrawLine(distance);
		glPushMatrix();
		glRotatef(180, 0, 0, 1);
		DrawLine(distance);
		glPopMatrix();
		glPopMatrix();
	}
	glPopMatrix();
	glPopMatrix();
}

/* Draw a set of stars. */
static void DrawStars()
{
	DrawStar(0.05, 0.91);
	DrawStar(0.15, 0.75);
	DrawStar(0.23, 0.8);
	DrawStar(0.25, 0.87);
	DrawStar(0.33, 0.9);
	DrawStar(0.37, 0.81);
	DrawStar(0.43, 0.9);
	DrawStar(0.59, 0.81);
	DrawStar(0.64, 0.96);
	DrawStar(0.67, 0.73);
	DrawStar(0.75, 0.88);
	DrawStar(0.9, 0.77);
	DrawStar(0.83, 0.73);
}

/* Draw an house with a window. */
static void DrawHouse()
{
	GLfloat x0, x1, x2, x3, y0, y1, y2, y3, width, height, heightWall;
	
	x0 = 0.65;
	y0 = 0.1;
	width = 0.25;
	height = 0.3;
	heightWall = height * 0.6;

	// Wall
	x1 = WindowWidth * x0;
	x2 = WindowWidth * (x0 + width);
	y1 = WindowHeight * y0;
	y2 = WindowHeight * (y0 + heightWall);

	glBegin(GL_QUADS);
	glColor3f(OrangeWall.R, OrangeWall.G, OrangeWall.B);
	glVertex2f(x1, y1);
	glVertex2f(x2, y1);
	glVertex2f(x2, y2);
	glVertex2f(x1, y2);
	glEnd();

	// Roof
	x3 = WindowWidth * (x0 + width / 2);
	y1 = WindowHeight * (y0 + heightWall);
	y2 = WindowHeight * (y0 + height);

	glBegin(GL_TRIANGLES);
	glColor3f(ColorRoof.R, ColorRoof.G, ColorRoof.B);
	glVertex2f(x1, y1);
	glVertex2f(x2, y1);
	glVertex2f(x3, y2);
	glEnd();

	// Window
	x1 = WindowWidth * (x0 + width / 2 - width / 6);
	x2 = WindowWidth * (x0 + width / 2 + width / 6);
	y1 = WindowHeight * (y0 + heightWall / 2 + heightWall / 4);
	y2 = WindowHeight * (y0 + heightWall / 2 - heightWall / 4);

	glBegin(GL_POLYGON);
	glColor3f(ColorWindow.R, ColorWindow.G, ColorWindow.B);
	glVertex2f(x1, y1);
	glVertex2f(x2, y1);
	glVertex2f(x2, y2);
	glVertex2f(x1, y2);
	glEnd();
	glLineWidth(2);

	x1 = WindowWidth * (x0 + width / 2 - width / 6);
	x2 = WindowWidth * (x0 + width / 2 + width / 6);
	x3 = WindowWidth * (x0 + width / 2);
	y1 = WindowHeight * (y0 + heightWall / 2 + heightWall / 4);
	y2 = WindowHeight * (y0 + heightWall / 2 - heightWall / 4);
	y3 = WindowHeight * (y0 + heightWall / 2);

	glBegin(GL_LINES);
	glColor3f(BrownWindow.R, BrownWindow.G, BrownWindow.B);
	glVertex2f(x3, y1);
	glVertex2f(x3, y2);
	glVertex2f(x1, y3);
	glVertex2f(x2, y3);
	glEnd();
}

/* Draw a green rotating spiral. */
static void DrawGreenSpiral(GLfloat x, GLfloat y)
{
	GLfloat length = 0.01;

	glPushMatrix();
	glTranslatef(WindowWidth * x, WindowHeight * y, 0);
	glRotatef(FlowerRotation * FlowerSpeed[1][Time], 0, 0, 1);
	glColor3f(GreenFlower.R, GreenFlower.G, GreenFlower.B);
	for (int segment = 0; segment < 70; segment++)
	{
		DrawLine(length);
		glRotatef(45, 0, 0, 1);
		length += 0.001;
		glPushMatrix();
		glRotatef(60, 0, 0, 1);
		DrawLine(length);
		glPopMatrix();
	}
	glPopMatrix();
}

/* Draw a blue rotating spiral. */
void DrawBlueSpiral(GLfloat x, GLfloat y)
{
	GLfloat length = 0.001;

	glTranslatef(WindowWidth * x, WindowHeight * y, 0);
	glRotatef(FlowerRotation * FlowerSpeed[2][Time], 0, 0, 1);
	glColor3f(BlueFlower.R, BlueFlower.G, BlueFlower.B);
	glPushMatrix();
	for (int segment = 0; segment < 100; segment++)
	{
		glTranslatef(0, WindowHeight * length, 0);
		glRotatef(45, 0, 0, 1);
		length += 0.001;
		glPushMatrix();
		glRotatef(210, 0, 0, 1);
		DrawLine(length);
		glPushMatrix();
		glRotatef(180, 0, 0, 1);
		DrawLine(length);
		glPopMatrix();
		glPopMatrix();
	}
	glPopMatrix();
}

/* Draw a red rotating spiral. */
static void DrawRedSpiral(GLfloat x, GLfloat y)
{
	GLfloat length = 0.01;

	glPushMatrix();
	glTranslatef(WindowWidth * x, WindowHeight * y, 0);
	glRotatef(FlowerRotation * FlowerSpeed[0][Time], 0, 0, 1);
	glColor3f(RedFlower.R, RedFlower.G, RedFlower.B);
	for (int segment = 0; segment < 30; segment++)
	{
		glRotatef(90, 0, 0, 1);
		DrawLine(length);
		length += 0.003;
	}
	glPopMatrix();
}

/* Draw a colored thick line. */
static void DrawColoredLine(GLfloat x, GLfloat y, GLfloat width, GLfloat height, Color color)
{
	GLfloat x1, x2, y1, y2;

	x1 = WindowWidth * x;
	x2 = WindowWidth * (x + width);
	y1 = WindowHeight * y;
	y2 = WindowHeight * (y + height);

	glBegin(GL_POLYGON);
	glColor3f(color.R, color.G, color.B);
	glVertex2f(x1, y2);
	glVertex2f(x2, y2);
	glVertex2f(x2, y1);
	glVertex2f(x1, y1);
	glEnd();
}

/* Draw three spiral flowers. */
static void DrawSpiralFlowers()
{
	// Draw a red spiral flower
	DrawColoredLine(0.1, 0.1, 0.004, 0.2, RedFlower);
	DrawRedSpiral(0.1, 0.3);

	// Draw a green spiral flower
	DrawColoredLine(0.3, 0.1, 0.006, 0.3, GreenFlower);
	DrawGreenSpiral(0.3, 0.4);

	// Draw a blue spiral flower
	DrawColoredLine(0.5, 0.1, 0.008, 0.5, BlueFlower);
	DrawBlueSpiral(0.5, 0.6);
}

/* Draw two spokes. */
static void DrawSpokes(GLfloat x, GLfloat y)
{
	GLfloat deltaX, deltaY, radius;
	const int delay = 10;

	radius = WindowWidth * 0.02;
	deltaX = radius / 1.4;
	deltaY = radius / 1.5;

	glColor3ub(0, 0, 0);
	glBegin(GL_LINES);
	if (SpokesRotation >= delay && (CarX < WindowWidth * 0.65 || CarX > WindowWidth * 0.7 || Time == SunnyDay))
	{
		glVertex2f(x - deltaX, y + deltaY);
		glVertex2f(x + deltaX, y - deltaY);
		glVertex2f(x + deltaX, y + deltaY);
		glVertex2f(x - deltaX, y - deltaY);
		if (SpokesRotation++ >= delay * 2) SpokesRotation = 0;
	}
	else
	{
		glVertex2f(x - radius, y);
		glVertex2f(x + radius, y);
		glVertex2f(x, y - radius);
		glVertex2f(x, y + radius);
		SpokesRotation++;
	}
	glEnd();
}

/* Draw a snowman. */
static void DrawSnowMan()
{
	glPushMatrix();
	glColor3f(1.0, 1.0, 1.0);
	DrawRoughCircle(WindowWidth * 0.6, WindowHeight * 0.1, WindowWidth * 0.04, ColorMountains);		// Body
	DrawRoughCircle(WindowWidth * 0.6, WindowHeight * 0.18, WindowWidth * 0.03, ColorMountains);	// Head
	DrawRoughCircle(WindowWidth * 0.59, WindowHeight * 0.2, WindowWidth * 0.004, Black);			// Left eye
	DrawRoughCircle(WindowWidth * 0.61, WindowHeight * 0.2, WindowWidth * 0.004, Black);			// Right eye
	DrawRoughCircle(WindowWidth * 0.6, WindowHeight * 0.185, WindowWidth * 0.006, OrangeWall);		// Nose
	glPopMatrix();
}

/* Draw a car. */
static void DrawCar()
{
	GLfloat x1, x2, y1, y2, wheel1, wheel2, wheelHeight;

	wheelHeight = WindowHeight * 0.07;
	wheel1 = CarX;
	wheel2 = CarX + WindowWidth * 0.1;

	glPushMatrix();
	// Body
	glColor3f(ColorCar.R, ColorCar.G, ColorCar.B);

	x1 = wheel1 - WindowWidth * 0.05;
	y1 = wheelHeight + WindowHeight * 0.05;
	x2 = wheel2 + WindowWidth * 0.05;
	y2 = wheelHeight + WindowHeight * 0.005;
	glRectf(x1, y1, x2, y2);

	x1 = wheel1 + (wheel2 - wheel1) / 4;
	y1 = wheelHeight + WindowHeight * 0.1;
	x2 = x1 + (wheel2 - wheel1) / 2;
	y2 = wheelHeight + WindowHeight * 0.005;
	glRectf(x1, y1, x2, y2);

	// Wheels
	DrawSmoothCircle(wheel1, wheelHeight, WindowWidth * 0.02, SilverWheel);
	DrawSmoothCircle(wheel2, wheelHeight, WindowWidth * 0.02, SilverWheel);
	DrawSpokes(wheel1, wheelHeight);
	DrawSpokes(wheel2, wheelHeight);
	glPopMatrix();
}

/* Display callback. */
void Display(void)
{	
	glPushMatrix();
	glLineWidth(2);
	DrawGroundSky();
	DrawMountains();
	if (Time == SunnyDay) DrawRainbow();
	DrawSpiralFlowers();
	switch (Time)
	{
	case StarryNight:
		DrawStars();
		DrawMoon();
		break;
	case SunnyDay:
		DrawSun();
		break;
	case Eclipse:
		DrawSun();
		EclipseX -= WindowWidth * 0.0002;
		DrawEclipse();
		break;
	case SnowyNight:
		DrawSnowMan();
	}
	DrawHouse();
	DrawCar();
	glPopMatrix();
	glFlush();
}

/* Reshape callback. */
void Reshape(int Width, int Height)
{
	WindowWidth = Width;
	WindowHeight = Height;
	for (int i = 0; i < DropsCount; i++)
	{
		SnowDrop[i].X = RainDrop[i].X = rand() % Width;
		SnowDrop[i].Y = RainDrop[i].Y = rand() % Height;
	}
	glViewport(0, 0, (GLsizei)Width, (GLsizei)Height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)Width, 0.0, (GLdouble)Height);
	Display();
}

/* Timer Tick callback. */
void TimerFunction(int value)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Flower rotation
	FlowerRotation += 1;

	// Eclipse happening
	if (Time == Eclipse)
	{
		bool Finished = true, TotalEclipse = true;
		const float Speed = 0.002;
		for each (Color *color in EclipseColors)
		{
			if (EclipseEnding)
			{
				color->R = (color->R + Speed < color->R2) ? color->R + Speed : color->R2;
				color->G = (color->G + Speed < color->G2) ? color->G + Speed : color->G2;
				color->B = (color->B + Speed < color->B2) ? color->B + Speed : color->B2;
				if (color->R != color->R2 || color->G != color->G2 || color->B != color->B2)
					Finished = false;
			}
			else
			{
				color->R = (color->R - Speed > 0) ? color->R - Speed : 0;
				color->G = (color->G - Speed > 0) ? color->G - Speed : 0;
				color->B = (color->B - Speed > 0) ? color->B - Speed : 0;
				if (color->R + color->G + color->B > 0)
					TotalEclipse = false;
			}
		}
		if (!EclipseEnding && TotalEclipse)
			EclipseEnding = true;
		else if (EclipseEnding && Finished)
		{
			EclipseEnding = false;
			Time = SunnyDay;
		}
	}

	// Car movement
	if (CarX < WindowWidth * 0.65 || CarX > WindowWidth * 0.7 || Time == SunnyDay)
	{
		CarX += WindowWidth * 0.005;
		if (CarX >= WindowWidth) CarX = 0;
		ColorWindow.Clone(BlueSky);
	}
	/* If the car has reached the threshold and it's not a sunny day,
	then the car stops and the window becomes yellow since an inside light has been turned on. */
	else
		ColorWindow.Clone(YellowWindow);

	Display();

	// Rain dropping
	if (Time == RainyNight)
	{
		glBegin(GL_LINES);
		glColor3f(0.0, 0.0, 1.0);
		for (int i = 0; i < DropsCount; i++)
		{
			if (RainDrop[i].X < 0) RainDrop[i].X = WindowWidth;
			if (RainDrop[i].Y < 0) RainDrop[i].Y = WindowHeight;
			glVertex2f(RainDrop[i].X, RainDrop[i].Y);
			glVertex2f(RainDrop[i].X + 10, RainDrop[i].Y + 10);
			RainDrop[i].X -= 15;
			RainDrop[i].Y -= 15;
		}
		glEnd();
	}

	// Snow dropping
	if (Time == SnowyNight)
	{
		Flake.Set(WindowWidth, WindowHeight);
		for (int i = 0; i < DropsCount; i++)
		{
			if (SnowDrop[i].X < 0) SnowDrop[i].X = WindowWidth;
			if (SnowDrop[i].Y < 0) SnowDrop[i].Y = WindowHeight;
			Flake.Draw(SnowDrop[i].X, SnowDrop[i].Y);
			SnowDrop[i].X -= 1;
			SnowDrop[i].Y -= 1;
		}
	}

	glFlush();
	glutTimerFunc(25, TimerFunction, 0);
}

/* Menu Click callback */
static void MenuClick(int option)
{
	// Restore the original colors if they have been changed
	if (Time == Eclipse)
		for each (Color *color in EclipseColors)
			color->Restore();
	else if (Time == SnowyNight)
		for each (Color *color in SnowColors)
			color->Restore();

	switch (option)
	{
	case 1:
		Time = SunnyDay;
		break;
	case 2:
		Time = RainyNight;
		break;
	case 3:
		Time = StarryNight;
		break;
	case 4:
		Time = SnowyNight;
		for each (Color *color in SnowColors)
			color->Set(WhiteSnow);
		break;
	case 5:
		Time = Eclipse;
		EclipseX = WindowWidth;
		break;
	case 6:
		exit(0);
	}
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowPosition(500, 200);
	glutInitWindowSize(WindowWidth, WindowHeight);
	glutCreateWindow("2D Animation");
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutCreateMenu(MenuClick);
	glutAddMenuEntry("A sunny day", 1);
	glutAddMenuEntry("A rainy night", 2);
	glutAddMenuEntry("A starry night", 3);
	glutAddMenuEntry("A snowy night", 4);
	glutAddMenuEntry("A solar eclipse", 5);
	glutAddMenuEntry("Quit", 6);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutTimerFunc(25, TimerFunction, 0);
	glutMainLoop();
	return 0;
}