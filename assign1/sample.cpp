#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#include "glew.h"
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"


//	This is a sample OpenGL / GLUT program
//
//	The objective is to draw a 3d object and change the color of the axes
//		with a glut menu
//
//	The left mouse button does rotation
//	The middle mouse button does scaling
//	The user interface allows:
//		1. The axes to be turned on and off
//		2. The color of the axes to be changed
//		3. Debugging to be turned on and off
//		4. Depth cueing to be turned on and off
//		5. The projection to be changed
//		6. The transformations to be reset
//		7. The program to quit
//
//	Author:			Joe Graphics

// NOTE: There are a lot of good reasons to use const variables instead
// of #define's.  However, Visual C++ does not allow a const variable
// to be used as an array size or as the case in a switch( ) statement.  So in
// the following, all constants are const variables except those which need to
// be array sizes or cases in switch( ) statements.  Those are #defines.


// title of these windows:

const char *WINDOWTITLE = { "OpenGL / GLUT Sample -- Joe Graphics" };
const char *GLUITITLE   = { "User Interface Window" };


// what the glui package defines as true and false:

const int GLUITRUE  = { true  };
const int GLUIFALSE = { false };


// the escape key:

#define ESCAPE		0x1b


// initial window size:

const int INIT_WINDOW_SIZE = { 600 };


// size of the box:

const float BOXSIZE = { 2.f };



// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = { 1. };
const float SCLFACT = { 0.005f };


// minimum allowable scale factor:

const float MINSCALE = { 0.05f };


// active mouse buttons (or them together):

const int LEFT   = { 4 };
const int MIDDLE = { 2 };
const int RIGHT  = { 1 };


// which projection:

enum Projections
{
	ORTHO,
	PERSP
};


// which button:

enum ButtonVals
{
	RESET,
	QUIT
};


// window background color (rgba):

const GLfloat BACKCOLOR[ ] = { 0., 0., 0., 1. };


// line width for the axes:

const GLfloat AXES_WIDTH   = { 3. };


// the color numbers:
// this order must match the radio button order

enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA,
	WHITE,
	BLACK
};

char * ColorNames[ ] =
{
	"Red",
	"Yellow",
	"Green",
	"Cyan",
	"Blue",
	"Magenta",
	"White",
	"Black"
};


// the color definitions:
// this order must match the menu order

const GLfloat Colors[ ][3] = 
{
	{ 1., 0., 0. },		// red
	{ 1., 1., 0. },		// yellow
	{ 0., 1., 0. },		// green
	{ 0., 1., 1. },		// cyan
	{ 0., 0., 1. },		// blue
	{ 1., 0., 1. },		// magenta
	{ 1., 1., 1. },		// white
	{ 0., 0., 0. },		// black
};


// fog parameters:

const GLfloat FOGCOLOR[4] = { .0, .0, .0, 1. };
const GLenum  FOGMODE     = { GL_LINEAR };
const GLfloat FOGDENSITY  = { 0.30f };
const GLfloat FOGSTART    = { 1.5 };
const GLfloat FOGEND      = { 4. };


// non-constant global variables:

int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
int		DebugOn;				// != 0 means to print debugging info
int		DepthCueOn;				// != 0 means to use intensity depth cueing
int		DepthBufferOn;			// != 0 means to use the z-buffer
int		DepthFightingOn;		// != 0 means to use the z-buffer
GLuint	BoxList;				// object display list
int		MainWindow;				// window id for main graphics window
float	Scale;					// scaling factor
int		WhichColor;				// index into Colors[ ]
int		WhichProjection;		// ORTHO or PERSP
int		Xmouse, Ymouse;			// mouse values
float	Xrot, Yrot;				// rotation angles in degrees


// function prototypes:

void	Animate( );
void	Display( );
void	DoAxesMenu( int );
void	DoColorMenu( int );
void	DoDepthBufferMenu( int );
void	DoDepthFightingMenu( int );
void	DoDepthMenu( int );
void	DoDebugMenu( int );
void	DoMainMenu( int );
void	DoProjectMenu( int );
void	DoRasterString( float, float, float, char * );
void	DoStrokeString( float, float, float, float, char * );
float	ElapsedSeconds( );
void	InitGraphics( );
void	InitLists( );
void	InitMenus( );
void	Keyboard( unsigned char, int, int );
void	MouseButton( int, int, int, int );
void	MouseMotion( int, int );
void	Reset( );
void	Resize( int, int );
void	Visibility( int );

void	Axes( float );
void	HsvRgb( float[3], float [3] );

// main program:

int
main( int argc, char *argv[ ] )
{
	// turn on the glut package:
	// (do this before checking argc and argv since it might
	// pull some command line arguments out)

	glutInit( &argc, argv );


	// setup all the graphics stuff:

	InitGraphics( );


	// create the display structures that will not change:

	InitLists( );


	// init all the global variables used by Display( ):
	// this will also post a redisplay

	Reset( );


	// setup all the user interface stuff:

	InitMenus( );


	// draw the scene once and wait for some interaction:
	// (this will never return)

	glutSetWindow( MainWindow );
	glutMainLoop( );


	// this is here to make the compiler happy:

	return 0;
}


// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutMainLoop( ) do it

void
Animate( )
{
	// put animation stuff in here -- change some global variables
	// for Display( ) to find:

	// force a call to Display( ) next time it is convenient:

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// draw the complete scene:

void
Display( )
{
	if( DebugOn != 0 )
	{
		fprintf( stderr, "Display\n" );
	}


	// set which window we want to do the graphics into:

	glutSetWindow( MainWindow );


	// erase the background:

	glDrawBuffer( GL_BACK );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	if( DepthBufferOn != 0 )
		glEnable( GL_DEPTH_TEST );
	else
		glDisable( GL_DEPTH_TEST );


	// specify shading to be flat:

	glShadeModel( GL_FLAT );


	// set the viewport to a square centered in the window:

	GLsizei vx = glutGet( GLUT_WINDOW_WIDTH );
	GLsizei vy = glutGet( GLUT_WINDOW_HEIGHT );
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = ( vx - v ) / 2;
	GLint yb = ( vy - v ) / 2;
	glViewport( xl, yb,  v, v );


	// set the viewing volume:
	// remember that the Z clipping  values are actually
	// given as DISTANCES IN FRONT OF THE EYE
	// USE gluOrtho2D( ) IF YOU ARE DOING 2D !

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	if( WhichProjection == ORTHO )
		glOrtho( -3., 3.,     -3., 3.,     0.1, 1000. );
	else
		gluPerspective( 90., 1.,	0.1, 1000. );


	// place the objects into the scene:

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );


	// set the eye position, look-at position, and up-vector:

	gluLookAt( 0., 0., 3.,     0., 0., 0.,     0., 1., 0. );


	// rotate the scene:

	glRotatef( (GLfloat)Yrot, 0., 1., 0. );
	glRotatef( (GLfloat)Xrot, 1., 0., 0. );


	// uniformly scale the scene:

	if( Scale < MINSCALE )
		Scale = MINSCALE;
	glScalef( (GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale );


	// set the fog parameters:

	if( DepthCueOn != 0 )
	{
		glFogi( GL_FOG_MODE, FOGMODE );
		glFogfv( GL_FOG_COLOR, FOGCOLOR );
		glFogf( GL_FOG_DENSITY, FOGDENSITY );
		glFogf( GL_FOG_START, FOGSTART );
		glFogf( GL_FOG_END, FOGEND );
		glEnable( GL_FOG );
	}
	else
	{
		glDisable( GL_FOG );
	}


	// possibly draw the axes:

	if( AxesOn != 0 )
	{
		glColor3fv( &Colors[WhichColor][0] );
		glCallList( AxesList );
	}


	// since we are using glScalef( ), be sure normals get unitized:

	glEnable( GL_NORMALIZE );


	// draw the current object:

	glCallList( BoxList );

	if( DepthFightingOn != 0 )
	{
		glPushMatrix( );
			glRotatef( 90.,   0., 1., 0. );
			glCallList( BoxList );
		glPopMatrix( );
	}

	// swap the double-buffered framebuffers:

	glutSwapBuffers( );


	// be sure the graphics buffer has been sent:
	// note: be sure to use glFlush( ) here, not glFinish( ) !

	glFlush( );
}


void
DoAxesMenu( int id )
{
	AxesOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoColorMenu( int id )
{
	WhichColor = id - RED;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDebugMenu( int id )
{
	DebugOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthBufferMenu( int id )
{
	DepthBufferOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthFightingMenu( int id )
{
	DepthFightingOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthMenu( int id )
{
	DepthCueOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// main menu callback:

void
DoMainMenu( int id )
{
	switch( id )
	{
		case RESET:
			Reset( );
			break;

		case QUIT:
			// gracefully close out the graphics:
			// gracefully close the graphics window:
			// gracefully exit the program:
			glutSetWindow( MainWindow );
			glFinish( );
			glutDestroyWindow( MainWindow );
			exit( 0 );
			break;

		default:
			fprintf( stderr, "Don't know what to do with Main Menu ID %d\n", id );
	}

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoProjectMenu( int id )
{
	WhichProjection = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// use glut to display a string of characters using a raster font:

void
DoRasterString( float x, float y, float z, char *s )
{
	glRasterPos3f( (GLfloat)x, (GLfloat)y, (GLfloat)z );

	char c;			// one character to print
	for( ; ( c = *s ) != '\0'; s++ )
	{
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, c );
	}
}


// use glut to display a string of characters using a stroke font:

void
DoStrokeString( float x, float y, float z, float ht, char *s )
{
	glPushMatrix( );
		glTranslatef( (GLfloat)x, (GLfloat)y, (GLfloat)z );
		float sf = ht / ( 119.05f + 33.33f );
		glScalef( (GLfloat)sf, (GLfloat)sf, (GLfloat)sf );
		char c;			// one character to print
		for( ; ( c = *s ) != '\0'; s++ )
		{
			glutStrokeCharacter( GLUT_STROKE_ROMAN, c );
		}
	glPopMatrix( );
}


// return the number of seconds since the start of the program:

float
ElapsedSeconds( )
{
	// get # of milliseconds since the start of the program:

	int ms = glutGet( GLUT_ELAPSED_TIME );

	// convert it to seconds:

	return (float)ms / 1000.f;
}


// initialize the glui window:

void
InitMenus( )
{
	glutSetWindow( MainWindow );

	int numColors = sizeof( Colors ) / ( 3*sizeof(int) );
	int colormenu = glutCreateMenu( DoColorMenu );
	for( int i = 0; i < numColors; i++ )
	{
		glutAddMenuEntry( ColorNames[i], i );
	}

	int axesmenu = glutCreateMenu( DoAxesMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthcuemenu = glutCreateMenu( DoDepthMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthbuffermenu = glutCreateMenu( DoDepthBufferMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthfightingmenu = glutCreateMenu( DoDepthFightingMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int debugmenu = glutCreateMenu( DoDebugMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int projmenu = glutCreateMenu( DoProjectMenu );
	glutAddMenuEntry( "Orthographic",  ORTHO );
	glutAddMenuEntry( "Perspective",   PERSP );

	int mainmenu = glutCreateMenu( DoMainMenu );
	glutAddSubMenu(   "Axes",          axesmenu);
	glutAddSubMenu(   "Colors",        colormenu);
	glutAddSubMenu(   "Depth Buffer",  depthbuffermenu);
	glutAddSubMenu(   "Depth Fighting",depthfightingmenu);
	glutAddSubMenu(   "Depth Cue",     depthcuemenu);
	glutAddSubMenu(   "Projection",    projmenu );
	glutAddMenuEntry( "Reset",         RESET );
	glutAddSubMenu(   "Debug",         debugmenu);
	glutAddMenuEntry( "Quit",          QUIT );

// attach the pop-up menu to the right mouse button:

	glutAttachMenu( GLUT_RIGHT_BUTTON );
}



// initialize the glut and OpenGL libraries:
//	also setup display lists and callback functions

void
InitGraphics( )
{
	// request the display modes:
	// ask for red-green-blue-alpha color, double-buffering, and z-buffering:

	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );

	// set the initial window configuration:

	glutInitWindowPosition( 0, 0 );
	glutInitWindowSize( INIT_WINDOW_SIZE, INIT_WINDOW_SIZE );

	// open the window and set its title:

	MainWindow = glutCreateWindow( WINDOWTITLE );
	glutSetWindowTitle( WINDOWTITLE );

	// set the framebuffer clear values:

	glClearColor( BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3] );

	// setup the callback functions:
	// DisplayFunc -- redraw the window
	// ReshapeFunc -- handle the user resizing the window
	// KeyboardFunc -- handle a keyboard input
	// MouseFunc -- handle the mouse button going down or up
	// MotionFunc -- handle the mouse moving with a button down
	// PassiveMotionFunc -- handle the mouse moving with a button up
	// VisibilityFunc -- handle a change in window visibility
	// EntryFunc	-- handle the cursor entering or leaving the window
	// SpecialFunc -- handle special keys on the keyboard
	// SpaceballMotionFunc -- handle spaceball translation
	// SpaceballRotateFunc -- handle spaceball rotation
	// SpaceballButtonFunc -- handle spaceball button hits
	// ButtonBoxFunc -- handle button box hits
	// DialsFunc -- handle dial rotations
	// TabletMotionFunc -- handle digitizing tablet motion
	// TabletButtonFunc -- handle digitizing tablet button hits
	// MenuStateFunc -- declare when a pop-up menu is in use
	// TimerFunc -- trigger something to happen a certain time from now
	// IdleFunc -- what to do when nothing else is going on

	glutSetWindow( MainWindow );
	glutDisplayFunc( Display );
	glutReshapeFunc( Resize );
	glutKeyboardFunc( Keyboard );
	glutMouseFunc( MouseButton );
	glutMotionFunc( MouseMotion );
	glutPassiveMotionFunc( NULL );
	glutVisibilityFunc( Visibility );
	glutEntryFunc( NULL );
	glutSpecialFunc( NULL );
	glutSpaceballMotionFunc( NULL );
	glutSpaceballRotateFunc( NULL );
	glutSpaceballButtonFunc( NULL );
	glutButtonBoxFunc( NULL );
	glutDialsFunc( NULL );
	glutTabletMotionFunc( NULL );
	glutTabletButtonFunc( NULL );
	glutMenuStateFunc( NULL );
	glutTimerFunc( -1, NULL, 0 );
	glutIdleFunc( NULL );

	// init glew (a window must be open to do this):

#ifdef WIN32
	GLenum err = glewInit( );
	if( err != GLEW_OK )
	{
		fprintf( stderr, "glewInit Error\n" );
	}
	else
		fprintf( stderr, "GLEW initialized OK\n" );
	fprintf( stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

}


// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )

void
InitLists( )
{
	float dx = BOXSIZE / 2.f;
	float dy = BOXSIZE / 2.f;
	float dz = BOXSIZE / 2.f;
	glutSetWindow( MainWindow );

	// create the object:

	BoxList = glGenLists( 1 );
	glNewList( BoxList, GL_COMPILE );

		glBegin( GL_TRIANGLES );

		glColor3f(0.150, 0.173, 0.034);
			glVertex3f(-1.000000, 0.800000, -1.000000);
			glVertex3f(0.999999, 0.800000, 1.000001);
			glVertex3f(1.000000, 0.800000, -0.999999);
			glVertex3f(-1.000000, 0.800000, -1.000000);
			glVertex3f(-1.000000, 0.800000, 1.000000);
			glVertex3f(0.999999, 0.800000, 1.000001);
			glColor3f(0.429, 0.171, 0.041);
			glVertex3f(1.000000, 0.800000, -0.999999);
			glVertex3f(1.000000, 0.600000, 1.000000);
			glVertex3f(1.000000, 0.600000, -1.000000);
			glVertex3f(0.999999, 0.800000, 1.000001);
			glVertex3f(-1.000000, 0.600000, 1.000000);
			glVertex3f(1.000000, 0.600000, 1.000000);
			glVertex3f(-1.000000, 0.800000, 1.000000);
			glVertex3f(-1.000000, 0.600000, -1.000000);
			glVertex3f(-1.000000, 0.600000, 1.000000);
			glVertex3f(1.000000, 0.600000, -1.000000);
			glVertex3f(-1.000000, 0.800000, -1.000000);
			glVertex3f(1.000000, 0.800000, -0.999999);
			glVertex3f(1.000000, 0.800000, -0.999999);
			glVertex3f(0.999999, 0.800000, 1.000001);
			glVertex3f(1.000000, 0.600000, 1.000000);
			glVertex3f(0.999999, 0.800000, 1.000001);
			glVertex3f(-1.000000, 0.800000, 1.000000);
			glVertex3f(-1.000000, 0.600000, 1.000000);
			glVertex3f(-1.000000, 0.800000, 1.000000);
			glVertex3f(-1.000000, 0.800000, -1.000000);
			glVertex3f(-1.000000, 0.600000, -1.000000);
			glVertex3f(1.000000, 0.600000, -1.000000);
			glVertex3f(-1.000000, 0.600000, -1.000000);
			glVertex3f(-1.000000, 0.800000, -1.000000);
		glColor3f(0.254, 0.102, 0.026);
			glVertex3f(1.000000, 0.400000, -1.000000);
			glVertex3f(-1.000000, 0.600000, -1.000000);
			glVertex3f(1.000000, 0.600000, -1.000000);
			glVertex3f(-1.000000, 0.600000, 1.000000);
			glVertex3f(-1.000000, 0.400000, -1.000000);
			glVertex3f(-1.000000, 0.400000, 1.000000);
			glVertex3f(1.000000, 0.600000, 1.000000);
			glVertex3f(-1.000000, 0.400000, 1.000000);
			glVertex3f(1.000000, 0.400000, 1.000000);
			glVertex3f(1.000000, 0.600000, -1.000000);
			glVertex3f(1.000000, 0.400000, 1.000000);
			glVertex3f(1.000000, 0.400000, -1.000000);
			glVertex3f(1.000000, 0.400000, -1.000000);
			glVertex3f(-1.000000, 0.400000, -1.000000);
			glVertex3f(-1.000000, 0.600000, -1.000000);
			glVertex3f(-1.000000, 0.600000, 1.000000);
			glVertex3f(-1.000000, 0.600000, -1.000000);
			glVertex3f(-1.000000, 0.400000, -1.000000);
			glVertex3f(1.000000, 0.600000, 1.000000);
			glVertex3f(-1.000000, 0.600000, 1.000000);
			glVertex3f(-1.000000, 0.400000, 1.000000);
			glVertex3f(1.000000, 0.600000, -1.000000);
			glVertex3f(1.000000, 0.600000, 1.000000);
			glVertex3f(1.000000, 0.400000, 1.000000);
		glColor3f(0.138, 0.058, 0.016);
			glVertex3f(1.000000, 0.200000, -1.000000);
			glVertex3f(-1.000000, 0.400000, -1.000000);
			glVertex3f(1.000000, 0.400000, -1.000000);
			glVertex3f(-1.000000, 0.400000, 1.000000);
			glVertex3f(-1.000000, 0.200000, -1.000000);
			glVertex3f(-1.000000, 0.200000, 1.000000);
			glVertex3f(1.000000, 0.400000, 1.000000);
			glVertex3f(-1.000000, 0.200000, 1.000000);
			glVertex3f(1.000000, 0.200000, 1.000000);
			glVertex3f(1.000000, 0.400000, -1.000000);
			glVertex3f(1.000000, 0.200000, 1.000000);
			glVertex3f(1.000000, 0.200000, -1.000000);
			glVertex3f(1.000000, 0.200000, -1.000000);
			glVertex3f(-1.000000, 0.200000, -1.000000);
			glVertex3f(-1.000000, 0.400000, -1.000000);
			glVertex3f(-1.000000, 0.400000, 1.000000);
			glVertex3f(-1.000000, 0.400000, -1.000000);
			glVertex3f(-1.000000, 0.200000, -1.000000);
			glVertex3f(1.000000, 0.400000, 1.000000);
			glVertex3f(-1.000000, 0.400000, 1.000000);
			glVertex3f(-1.000000, 0.200000, 1.000000);
			glVertex3f(1.000000, 0.400000, -1.000000);
			glVertex3f(1.000000, 0.400000, 1.000000);
			glVertex3f(1.000000, 0.200000, 1.000000);
		glColor3f(0.050, 0.022, 0.007);
			glVertex3f(1.000000, 0.000000, 1.000000);
			glVertex3f(-1.000000, 0.000000, -1.000000);
			glVertex3f(1.000000, 0.000000, -1.000000);
			glVertex3f(1.000000, 0.000000, -1.000000);
			glVertex3f(-1.000000, 0.200000, -1.000000);
			glVertex3f(1.000000, 0.200000, -1.000000);
			glVertex3f(-1.000000, 0.200000, 1.000000);
			glVertex3f(-1.000000, 0.000000, -1.000000);
			glVertex3f(-1.000000, 0.000000, 1.000000);
			glVertex3f(1.000000, 0.200000, 1.000000);
			glVertex3f(-1.000000, 0.000000, 1.000000);
			glVertex3f(1.000000, 0.000000, 1.000000);
			glVertex3f(1.000000, 0.200000, -1.000000);
			glVertex3f(1.000000, 0.000000, 1.000000);
			glVertex3f(1.000000, 0.000000, -1.000000);
			glVertex3f(1.000000, 0.000000, 1.000000);
			glVertex3f(-1.000000, 0.000000, 1.000000);
			glVertex3f(-1.000000, 0.000000, -1.000000);
			glVertex3f(1.000000, 0.000000, -1.000000);
			glVertex3f(-1.000000, 0.000000, -1.000000);
			glVertex3f(-1.000000, 0.200000, -1.000000);
			glVertex3f(-1.000000, 0.200000, 1.000000);
			glVertex3f(-1.000000, 0.200000, -1.000000);
			glVertex3f(-1.000000, 0.000000, -1.000000);
			glVertex3f(1.000000, 0.200000, 1.000000);
			glVertex3f(-1.000000, 0.200000, 1.000000);
			glVertex3f(-1.000000, 0.000000, 1.000000);
			glVertex3f(1.000000, 0.200000, -1.000000);
			glVertex3f(1.000000, 0.200000, 1.000000);
			glVertex3f(1.000000, 0.000000, 1.000000);
		glColor3f(0.458, 0.038, 0.007);
			glVertex3f(-0.414269, 1.237640, -0.545140);
			glVertex3f(0.148753, 0.989535, 0.146431);
			glVertex3f(-0.269882, 0.989535, -0.623634);
			glVertex3f(-0.558656, 0.989535, -0.466646);
			glVertex3f(-0.284408, 0.799389, 0.381913);
			glVertex3f(-0.140021, 0.989535, 0.303419);
			glVertex3f(-0.558656, 0.989535, -0.466646);
			glVertex3f(0.004366, 1.237640, 0.224925);
			glVertex3f(-0.414269, 1.237640, -0.545140);
			glVertex3f(-0.269882, 0.989535, -0.623634);
			glVertex3f(0.293140, 0.799389, 0.067937);
			glVertex3f(-0.125495, 0.799389, -0.702128);
			glVertex3f(-0.414269, 1.237640, -0.545140);
			glVertex3f(0.004366, 1.237640, 0.224925);
			glVertex3f(0.148753, 0.989535, 0.146431);
			glVertex3f(-0.558656, 0.989535, -0.466646);
			glVertex3f(-0.703043, 0.799389, -0.388152);
			glVertex3f(-0.284408, 0.799389, 0.381913);
			glVertex3f(-0.558656, 0.989535, -0.466646);
			glVertex3f(-0.140021, 0.989535, 0.303419);
			glVertex3f(0.004366, 1.237640, 0.224925);
			glVertex3f(-0.269882, 0.989535, -0.623634);
			glVertex3f(0.148753, 0.989535, 0.146431);
			glVertex3f(0.293140, 0.799389, 0.067937);
		glColor3f(0.063, 0.016, 0.005);
			glVertex3f(-0.415380, 1.234626, -0.547185);
			glVertex3f(-0.409888, 0.799389, -0.545028);
			glVertex3f(-0.416362, 0.799389, -0.548990);
			glVertex3f(0.002120, 1.234626, 0.224867);
			glVertex3f(0.006654, 0.799389, 0.221187);
			glVertex3f(-0.000015, 0.799389, 0.224812);
			glVertex3f(-0.411943, 1.234626, -0.545080);
			glVertex3f(-0.416557, 0.799389, -0.541402);
			glVertex3f(-0.409888, 0.799389, -0.545028);
			glVertex3f(0.005539, 1.234626, 0.223008);
			glVertex3f(0.006459, 0.799389, 0.228775);
			glVertex3f(0.006654, 0.799389, 0.221187);
			glVertex3f(-0.415484, 1.234626, -0.543155);
			glVertex3f(-0.416362, 0.799389, -0.548990);
			glVertex3f(-0.416557, 0.799389, -0.541402);
			glVertex3f(0.005439, 1.234626, 0.226899);
			glVertex3f(-0.000015, 0.799389, 0.224812);
			glVertex3f(0.006459, 0.799389, 0.228775);
			glVertex3f(0.465835, 0.752276, -0.640306);
			glVertex3f(0.465835, 2.652599, -0.609161);
			glVertex3f(0.495455, 0.752276, -0.618785);
			glVertex3f(0.495455, 0.752276, -0.618785);
			glVertex3f(0.465835, 2.652599, -0.609161);
			glVertex3f(0.484141, 0.752276, -0.583964);
			glVertex3f(0.484141, 0.752276, -0.583964);
			glVertex3f(0.465835, 2.652599, -0.609161);
			glVertex3f(0.447528, 0.752276, -0.583964);
			glVertex3f(0.447528, 0.752276, -0.583964);
			glVertex3f(0.465835, 2.652599, -0.609161);
			glVertex3f(0.436214, 0.752276, -0.618785);
			glVertex3f(0.436214, 0.752276, -0.618785);
			glVertex3f(0.465835, 2.652599, -0.609161);
			glVertex3f(0.465835, 0.752276, -0.640306);
			glVertex3f(-0.665343, 0.556179, 0.533498);
			glVertex3f(-0.687238, 2.131036, 0.547164);
			glVertex3f(-0.667475, 0.556179, 0.563765);
			glVertex3f(-0.667475, 0.556179, 0.563765);
			glVertex3f(-0.687238, 2.131036, 0.547164);
			glVertex3f(-0.696919, 0.556179, 0.571091);
			glVertex3f(-0.696919, 0.556179, 0.571091);
			glVertex3f(-0.687238, 2.131036, 0.547164);
			glVertex3f(-0.712985, 0.556179, 0.545351);
			glVertex3f(-0.712985, 0.556179, 0.545351);
			glVertex3f(-0.687238, 2.131036, 0.547164);
			glVertex3f(-0.693470, 0.556179, 0.522117);
			glVertex3f(-0.693470, 0.556179, 0.522117);
			glVertex3f(-0.687238, 2.131036, 0.547164);
			glVertex3f(-0.665343, 0.556179, 0.533498);
			glVertex3f(0.742322, 0.632933, 0.021099);
			glVertex3f(0.752542, 1.740700, 0.036105);
			glVertex3f(0.763655, 0.632933, 0.021749);
			glVertex3f(0.763655, 0.632933, 0.021749);
			glVertex3f(0.752542, 1.740700, 0.036105);
			glVertex3f(0.769630, 0.632933, 0.042238);
			glVertex3f(0.769630, 0.632933, 0.042238);
			glVertex3f(0.752542, 1.740700, 0.036105);
			glVertex3f(0.751989, 0.632933, 0.054252);
			glVertex3f(0.751989, 0.632933, 0.054252);
			glVertex3f(0.752542, 1.740700, 0.036105);
			glVertex3f(0.735112, 0.632933, 0.041187);
			glVertex3f(0.735112, 0.632933, 0.041187);
			glVertex3f(0.752542, 1.740700, 0.036105);
			glVertex3f(0.742322, 0.632933, 0.021099);
			glVertex3f(-0.680442, 0.715401, -0.710637);
			glVertex3f(-0.687506, 2.016956, -0.730765);
			glVertex3f(-0.704466, 0.715401, -0.717827);
			glVertex3f(-0.704466, 0.715401, -0.717827);
			glVertex3f(-0.687506, 2.016956, -0.730765);
			glVertex3f(-0.705052, 0.715401, -0.742897);
			glVertex3f(-0.705052, 0.715401, -0.742897);
			glVertex3f(-0.687506, 2.016956, -0.730765);
			glVertex3f(-0.681390, 0.715401, -0.751201);
			glVertex3f(-0.681390, 0.715401, -0.751201);
			glVertex3f(-0.687506, 2.016956, -0.730765);
			glVertex3f(-0.666181, 0.715401, -0.731263);
			glVertex3f(-0.666181, 0.715401, -0.731263);
			glVertex3f(-0.687506, 2.016956, -0.730765);
			glVertex3f(-0.680442, 0.715401, -0.710637);
			glVertex3f(0.522916, 0.822560, 0.471521);
			glVertex3f(0.471543, 0.793118, 0.543336);
			glVertex3f(0.459264, 0.822560, 0.531580);
			glVertex3f(0.535521, 0.792335, 0.483589);
			glVertex3f(0.496100, 0.793118, 0.566846);
			glVertex3f(0.471543, 0.793118, 0.543336);
			glVertex3f(0.560731, 0.792335, 0.507724);
			glVertex3f(0.508379, 0.822560, 0.578602);
			glVertex3f(0.496100, 0.793118, 0.566846);
			glVertex3f(0.508379, 0.822560, 0.578602);
			glVertex3f(0.560731, 0.852785, 0.507724);
			glVertex3f(0.496100, 0.852003, 0.566846);
			glVertex3f(0.496100, 0.852003, 0.566846);
			glVertex3f(0.535521, 0.852785, 0.483589);
			glVertex3f(0.471543, 0.852003, 0.543336);
			glVertex3f(0.471543, 0.852003, 0.543336);
			glVertex3f(0.522916, 0.822560, 0.471521);
			glVertex3f(0.459264, 0.822560, 0.531580);
			glVertex3f(0.352143, 0.848295, 0.671021);
			glVertex3f(0.285769, 0.822560, 0.728473);
			glVertex3f(0.296065, 0.847247, 0.738330);
			glVertex3f(0.412841, 0.848666, 0.608133);
			glVertex3f(0.341271, 0.822223, 0.660611);
			glVertex3f(0.352143, 0.848295, 0.671021);
			glVertex3f(0.412841, 0.848666, 0.608133);
			glVertex3f(0.459264, 0.822560, 0.531580);
			glVertex3f(0.401321, 0.821042, 0.597104);
			glVertex3f(0.373889, 0.848295, 0.691839);
			glVertex3f(0.296065, 0.847247, 0.738330);
			glVertex3f(0.316655, 0.847247, 0.758043);
			glVertex3f(0.373889, 0.848295, 0.691839);
			glVertex3f(0.412841, 0.848666, 0.608133);
			glVertex3f(0.352143, 0.848295, 0.671021);
			glVertex3f(0.496100, 0.852003, 0.566846);
			glVertex3f(0.412841, 0.848666, 0.608133);
			glVertex3f(0.435882, 0.848666, 0.630192);
			glVertex3f(0.384762, 0.822223, 0.702248);
			glVertex3f(0.316655, 0.847247, 0.758043);
			glVertex3f(0.326951, 0.822560, 0.767899);
			glVertex3f(0.384762, 0.822223, 0.702248);
			glVertex3f(0.435882, 0.848666, 0.630192);
			glVertex3f(0.373889, 0.848295, 0.691839);
			glVertex3f(0.508379, 0.822560, 0.578602);
			glVertex3f(0.435882, 0.848666, 0.630192);
			glVertex3f(0.447402, 0.821042, 0.641222);
			glVertex3f(0.316655, 0.797874, 0.758043);
			glVertex3f(0.384762, 0.822223, 0.702248);
			glVertex3f(0.326951, 0.822560, 0.767899);
			glVertex3f(0.373889, 0.796152, 0.691839);
			glVertex3f(0.447402, 0.821042, 0.641222);
			glVertex3f(0.384762, 0.822223, 0.702248);
			glVertex3f(0.496100, 0.793118, 0.566846);
			glVertex3f(0.447402, 0.821042, 0.641222);
			glVertex3f(0.435882, 0.793417, 0.630192);
			glVertex3f(0.296065, 0.797874, 0.738330);
			glVertex3f(0.373889, 0.796152, 0.691839);
			glVertex3f(0.316655, 0.797874, 0.758043);
			glVertex3f(0.412841, 0.793417, 0.608133);
			glVertex3f(0.373889, 0.796152, 0.691839);
			glVertex3f(0.352143, 0.796152, 0.671021);
			glVertex3f(0.412841, 0.793417, 0.608133);
			glVertex3f(0.496100, 0.793118, 0.566846);
			glVertex3f(0.435882, 0.793417, 0.630192);
			glVertex3f(0.285769, 0.822560, 0.728473);
			glVertex3f(0.352143, 0.796152, 0.671021);
			glVertex3f(0.296065, 0.797874, 0.738330);
			glVertex3f(0.401321, 0.821042, 0.597104);
			glVertex3f(0.352143, 0.796152, 0.671021);
			glVertex3f(0.341271, 0.822223, 0.660611);
			glVertex3f(0.401321, 0.821042, 0.597104);
			glVertex3f(0.471543, 0.793118, 0.543336);
			glVertex3f(0.412841, 0.793417, 0.608133);
			glVertex3f(-0.810552, 0.960295, -0.050614);
			glVertex3f(-0.787253, 1.047834, -0.063372);
			glVertex3f(-0.813675, 1.047831, -0.052672);
			glVertex3f(-0.812053, 0.872939, -0.053020);
			glVertex3f(-0.782648, 0.960299, -0.061914);
			glVertex3f(-0.810552, 0.960295, -0.050614);
			glVertex3f(-0.812053, 0.872939, -0.053020);
			glVertex3f(-0.781689, 0.785469, -0.064151);
			glVertex3f(-0.782487, 0.872943, -0.064993);
			glVertex3f(-0.834285, 0.961091, -0.069118);
			glVertex3f(-0.813675, 1.047831, -0.052672);
			glVertex3f(-0.836148, 1.048584, -0.070193);
			glVertex3f(-0.834285, 0.961091, -0.069118);
			glVertex3f(-0.812053, 0.872939, -0.053020);
			glVertex3f(-0.810552, 0.960295, -0.050614);
			glVertex3f(-0.840002, 0.786363, -0.072287);
			glVertex3f(-0.812053, 0.872939, -0.053020);
			glVertex3f(-0.837200, 0.873782, -0.072626);
			glVertex3f(-0.830114, 0.961890, -0.098922);
			glVertex3f(-0.836148, 1.048584, -0.070193);
			glVertex3f(-0.832199, 1.049340, -0.098414);
			glVertex3f(-0.830114, 0.961890, -0.098922);
			glVertex3f(-0.837200, 0.873782, -0.072626);
			glVertex3f(-0.834285, 0.961091, -0.069118);
			glVertex3f(-0.835293, 0.787265, -0.105944);
			glVertex3f(-0.837200, 0.873782, -0.072626);
			glVertex3f(-0.832781, 0.874629, -0.104206);
			glVertex3f(-0.805777, 1.049344, -0.109114);
			glVertex3f(-0.830114, 0.961890, -0.098922);
			glVertex3f(-0.832199, 1.049340, -0.098414);
			glVertex3f(-0.802211, 0.961893, -0.110222);
			glVertex3f(-0.832781, 0.874629, -0.104206);
			glVertex3f(-0.830114, 0.961890, -0.098922);
			glVertex3f(-0.803781, 0.787269, -0.118705);
			glVertex3f(-0.832781, 0.874629, -0.104206);
			glVertex3f(-0.803216, 0.874632, -0.116179);
			glVertex3f(-0.783305, 1.048591, -0.091593);
			glVertex3f(-0.802211, 0.961893, -0.110222);
			glVertex3f(-0.805777, 1.049344, -0.109114);
			glVertex3f(-0.778069, 0.873789, -0.096572);
			glVertex3f(-0.802211, 0.961893, -0.110222);
			glVertex3f(-0.778478, 0.961098, -0.091718);
			glVertex3f(-0.778069, 0.873789, -0.096572);
			glVertex3f(-0.803781, 0.787269, -0.118705);
			glVertex3f(-0.803216, 0.874632, -0.116179);
			glVertex3f(-0.787253, 1.047834, -0.063372);
			glVertex3f(-0.778478, 0.961098, -0.091718);
			glVertex3f(-0.783305, 1.048591, -0.091593);
			glVertex3f(-0.782487, 0.872943, -0.064993);
			glVertex3f(-0.778478, 0.961098, -0.091718);
			glVertex3f(-0.782648, 0.960299, -0.061914);
			glVertex3f(-0.782487, 0.872943, -0.064993);
			glVertex3f(-0.776980, 0.786371, -0.097809);
			glVertex3f(-0.778069, 0.873789, -0.096572);
			glVertex3f(0.623284, 0.786418, 0.465051);
			glVertex3f(0.654414, 0.852292, 0.436958);
			glVertex3f(0.621010, 0.852288, 0.450485);
			glVertex3f(0.583751, 0.787743, 0.434228);
			glVertex3f(0.621010, 0.852288, 0.450485);
			glVertex3f(0.592597, 0.853240, 0.428333);
			glVertex3f(0.590698, 0.789074, 0.384583);
			glVertex3f(0.592597, 0.853240, 0.428333);
			glVertex3f(0.597590, 0.854197, 0.392654);
			glVertex3f(0.630994, 0.854201, 0.379126);
			glVertex3f(0.590698, 0.789074, 0.384583);
			glVertex3f(0.597590, 0.854197, 0.392654);
			glVertex3f(0.659407, 0.853249, 0.401278);
			glVertex3f(0.637178, 0.789080, 0.365761);
			glVertex3f(0.630994, 0.854201, 0.379126);
			glVertex3f(0.654414, 0.852292, 0.436958);
			glVertex3f(0.676711, 0.787755, 0.396583);
			glVertex3f(0.659407, 0.853249, 0.401278);
			glVertex3f(-0.415380, 1.234626, -0.547185);
			glVertex3f(-0.411943, 1.234626, -0.545080);
			glVertex3f(-0.409888, 0.799389, -0.545028);
			glVertex3f(0.002120, 1.234626, 0.224867);
			glVertex3f(0.005539, 1.234626, 0.223008);
			glVertex3f(0.006654, 0.799389, 0.221187);
			glVertex3f(-0.411943, 1.234626, -0.545080);
			glVertex3f(-0.415484, 1.234626, -0.543155);
			glVertex3f(-0.416557, 0.799389, -0.541402);
			glVertex3f(0.005539, 1.234626, 0.223008);
			glVertex3f(0.005439, 1.234626, 0.226899);
			glVertex3f(0.006459, 0.799389, 0.228775);
			glVertex3f(-0.415484, 1.234626, -0.543155);
			glVertex3f(-0.415380, 1.234626, -0.547185);
			glVertex3f(-0.416362, 0.799389, -0.548990);
			glVertex3f(0.005439, 1.234626, 0.226899);
			glVertex3f(0.002120, 1.234626, 0.224867);
			glVertex3f(-0.000015, 0.799389, 0.224812);
			glVertex3f(0.522916, 0.822560, 0.471521);
			glVertex3f(0.535521, 0.792335, 0.483589);
			glVertex3f(0.471543, 0.793118, 0.543336);
			glVertex3f(0.535521, 0.792335, 0.483589);
			glVertex3f(0.560731, 0.792335, 0.507724);
			glVertex3f(0.496100, 0.793118, 0.566846);
			glVertex3f(0.560731, 0.792335, 0.507724);
			glVertex3f(0.573336, 0.822560, 0.519792);
			glVertex3f(0.508379, 0.822560, 0.578602);
			glVertex3f(0.508379, 0.822560, 0.578602);
			glVertex3f(0.573336, 0.822560, 0.519792);
			glVertex3f(0.560731, 0.852785, 0.507724);
			glVertex3f(0.496100, 0.852003, 0.566846);
			glVertex3f(0.560731, 0.852785, 0.507724);
			glVertex3f(0.535521, 0.852785, 0.483589);
			glVertex3f(0.471543, 0.852003, 0.543336);
			glVertex3f(0.535521, 0.852785, 0.483589);
			glVertex3f(0.522916, 0.822560, 0.471521);
			glVertex3f(0.352143, 0.848295, 0.671021);
			glVertex3f(0.341271, 0.822223, 0.660611);
			glVertex3f(0.285769, 0.822560, 0.728473);
			glVertex3f(0.412841, 0.848666, 0.608133);
			glVertex3f(0.401321, 0.821042, 0.597104);
			glVertex3f(0.341271, 0.822223, 0.660611);
			glVertex3f(0.412841, 0.848666, 0.608133);
			glVertex3f(0.471543, 0.852003, 0.543336);
			glVertex3f(0.459264, 0.822560, 0.531580);
			glVertex3f(0.373889, 0.848295, 0.691839);
			glVertex3f(0.352143, 0.848295, 0.671021);
			glVertex3f(0.296065, 0.847247, 0.738330);
			glVertex3f(0.373889, 0.848295, 0.691839);
			glVertex3f(0.435882, 0.848666, 0.630192);
			glVertex3f(0.412841, 0.848666, 0.608133);
			glVertex3f(0.496100, 0.852003, 0.566846);
			glVertex3f(0.471543, 0.852003, 0.543336);
			glVertex3f(0.412841, 0.848666, 0.608133);
			glVertex3f(0.384762, 0.822223, 0.702248);
			glVertex3f(0.373889, 0.848295, 0.691839);
			glVertex3f(0.316655, 0.847247, 0.758043);
			glVertex3f(0.384762, 0.822223, 0.702248);
			glVertex3f(0.447402, 0.821042, 0.641222);
			glVertex3f(0.435882, 0.848666, 0.630192);
			glVertex3f(0.508379, 0.822560, 0.578602);
			glVertex3f(0.496100, 0.852003, 0.566846);
			glVertex3f(0.435882, 0.848666, 0.630192);
			glVertex3f(0.316655, 0.797874, 0.758043);
			glVertex3f(0.373889, 0.796152, 0.691839);
			glVertex3f(0.384762, 0.822223, 0.702248);
			glVertex3f(0.373889, 0.796152, 0.691839);
			glVertex3f(0.435882, 0.793417, 0.630192);
			glVertex3f(0.447402, 0.821042, 0.641222);
			glVertex3f(0.496100, 0.793118, 0.566846);
			glVertex3f(0.508379, 0.822560, 0.578602);
			glVertex3f(0.447402, 0.821042, 0.641222);
			glVertex3f(0.296065, 0.797874, 0.738330);
			glVertex3f(0.352143, 0.796152, 0.671021);
			glVertex3f(0.373889, 0.796152, 0.691839);
			glVertex3f(0.412841, 0.793417, 0.608133);
			glVertex3f(0.435882, 0.793417, 0.630192);
			glVertex3f(0.373889, 0.796152, 0.691839);
			glVertex3f(0.412841, 0.793417, 0.608133);
			glVertex3f(0.471543, 0.793118, 0.543336);
			glVertex3f(0.496100, 0.793118, 0.566846);
			glVertex3f(0.285769, 0.822560, 0.728473);
			glVertex3f(0.341271, 0.822223, 0.660611);
			glVertex3f(0.352143, 0.796152, 0.671021);
			glVertex3f(0.401321, 0.821042, 0.597104);
			glVertex3f(0.412841, 0.793417, 0.608133);
			glVertex3f(0.352143, 0.796152, 0.671021);
			glVertex3f(0.401321, 0.821042, 0.597104);
			glVertex3f(0.459264, 0.822560, 0.531580);
			glVertex3f(0.471543, 0.793118, 0.543336);
			glVertex3f(-0.810552, 0.960295, -0.050614);
			glVertex3f(-0.782648, 0.960299, -0.061914);
			glVertex3f(-0.787253, 1.047834, -0.063372);
			glVertex3f(-0.812053, 0.872939, -0.053020);
			glVertex3f(-0.782487, 0.872943, -0.064993);
			glVertex3f(-0.782648, 0.960299, -0.061914);
			glVertex3f(-0.812053, 0.872939, -0.053020);
			glVertex3f(-0.813200, 0.785465, -0.051391);
			glVertex3f(-0.781689, 0.785469, -0.064151);
			glVertex3f(-0.834285, 0.961091, -0.069118);
			glVertex3f(-0.810552, 0.960295, -0.050614);
			glVertex3f(-0.813675, 1.047831, -0.052672);
			glVertex3f(-0.834285, 0.961091, -0.069118);
			glVertex3f(-0.837200, 0.873782, -0.072626);
			glVertex3f(-0.812053, 0.872939, -0.053020);
			glVertex3f(-0.840002, 0.786363, -0.072287);
			glVertex3f(-0.813200, 0.785465, -0.051391);
			glVertex3f(-0.812053, 0.872939, -0.053020);
			glVertex3f(-0.830114, 0.961890, -0.098922);
			glVertex3f(-0.834285, 0.961091, -0.069118);
			glVertex3f(-0.836148, 1.048584, -0.070193);
			glVertex3f(-0.830114, 0.961890, -0.098922);
			glVertex3f(-0.832781, 0.874629, -0.104206);
			glVertex3f(-0.837200, 0.873782, -0.072626);
			glVertex3f(-0.835293, 0.787265, -0.105944);
			glVertex3f(-0.840002, 0.786363, -0.072287);
			glVertex3f(-0.837200, 0.873782, -0.072626);
			glVertex3f(-0.805777, 1.049344, -0.109114);
			glVertex3f(-0.802211, 0.961893, -0.110222);
			glVertex3f(-0.830114, 0.961890, -0.098922);
			glVertex3f(-0.802211, 0.961893, -0.110222);
			glVertex3f(-0.803216, 0.874632, -0.116179);
			glVertex3f(-0.832781, 0.874629, -0.104206);
			glVertex3f(-0.803781, 0.787269, -0.118705);
			glVertex3f(-0.835293, 0.787265, -0.105944);
			glVertex3f(-0.832781, 0.874629, -0.104206);
			glVertex3f(-0.783305, 1.048591, -0.091593);
			glVertex3f(-0.778478, 0.961098, -0.091718);
			glVertex3f(-0.802211, 0.961893, -0.110222);
			glVertex3f(-0.778069, 0.873789, -0.096572);
			glVertex3f(-0.803216, 0.874632, -0.116179);
			glVertex3f(-0.802211, 0.961893, -0.110222);
			glVertex3f(-0.778069, 0.873789, -0.096572);
			glVertex3f(-0.776980, 0.786371, -0.097809);
			glVertex3f(-0.803781, 0.787269, -0.118705);
			glVertex3f(-0.787253, 1.047834, -0.063372);
			glVertex3f(-0.782648, 0.960299, -0.061914);
			glVertex3f(-0.778478, 0.961098, -0.091718);
			glVertex3f(-0.782487, 0.872943, -0.064993);
			glVertex3f(-0.778069, 0.873789, -0.096572);
			glVertex3f(-0.778478, 0.961098, -0.091718);
			glVertex3f(-0.782487, 0.872943, -0.064993);
			glVertex3f(-0.781689, 0.785469, -0.064151);
			glVertex3f(-0.776980, 0.786371, -0.097809);
			glVertex3f(0.623284, 0.786418, 0.465051);
			glVertex3f(0.669764, 0.786424, 0.446229);
			glVertex3f(0.654414, 0.852292, 0.436958);
			glVertex3f(0.583751, 0.787743, 0.434228);
			glVertex3f(0.623284, 0.786418, 0.465051);
			glVertex3f(0.621010, 0.852288, 0.450485);
			glVertex3f(0.590698, 0.789074, 0.384583);
			glVertex3f(0.583751, 0.787743, 0.434228);
			glVertex3f(0.592597, 0.853240, 0.428333);
			glVertex3f(0.630994, 0.854201, 0.379126);
			glVertex3f(0.637178, 0.789080, 0.365761);
			glVertex3f(0.590698, 0.789074, 0.384583);
			glVertex3f(0.659407, 0.853249, 0.401278);
			glVertex3f(0.676711, 0.787755, 0.396583);
			glVertex3f(0.637178, 0.789080, 0.365761);
			glVertex3f(0.654414, 0.852292, 0.436958);
			glVertex3f(0.669764, 0.786424, 0.446229);
			glVertex3f(0.676711, 0.787755, 0.396583);
		glColor3f(0.147, 0.147, 0.147);
			glVertex3f(-0.290524, 0.834525, 0.383057);
			glVertex3f(-0.283936, 0.834980, 0.386800);
			glVertex3f(-0.284042, 0.835486, 0.379227);
			glVertex3f(-0.279132, 0.769789, 0.378568);
			glVertex3f(-0.290524, 0.834525, 0.383057);
			glVertex3f(-0.284042, 0.835486, 0.379227);
			glVertex3f(-0.279132, 0.769789, 0.378568);
			glVertex3f(-0.284042, 0.835486, 0.379227);
			glVertex3f(-0.283936, 0.834980, 0.386800);
			glVertex3f(-0.279132, 0.769789, 0.378568);
			glVertex3f(-0.283936, 0.834980, 0.386800);
			glVertex3f(-0.290524, 0.834525, 0.383057);
			glVertex3f(-0.710802, 0.834250, -0.392338);
			glVertex3f(-0.704156, 0.835648, -0.388948);
			glVertex3f(-0.704432, 0.834559, -0.396455);
			glVertex3f(-0.697506, 0.770323, -0.383556);
			glVertex3f(-0.710802, 0.834250, -0.392338);
			glVertex3f(-0.704432, 0.834559, -0.396455);
			glVertex3f(-0.697506, 0.770323, -0.383556);
			glVertex3f(-0.704432, 0.834559, -0.396455);
			glVertex3f(-0.704156, 0.835648, -0.388948);
			glVertex3f(-0.697506, 0.770323, -0.383556);
			glVertex3f(-0.704156, 0.835648, -0.388948);
			glVertex3f(-0.710802, 0.834250, -0.392338);
			glVertex3f(0.291629, 0.835636, 0.067903);
			glVertex3f(0.297885, 0.834502, 0.072049);
			glVertex3f(0.298307, 0.834525, 0.064470);
			glVertex3f(0.284740, 0.770116, 0.067325);
			glVertex3f(0.291629, 0.835636, 0.067903);
			glVertex3f(0.298307, 0.834525, 0.064470);
			glVertex3f(0.284740, 0.770116, 0.067325);
			glVertex3f(0.298307, 0.834525, 0.064470);
			glVertex3f(0.297885, 0.834502, 0.072049);
			glVertex3f(0.284740, 0.770116, 0.067325);
			glVertex3f(0.297885, 0.834502, 0.072049);
			glVertex3f(0.291629, 0.835636, 0.067903);
			glVertex3f(-0.128384, 0.835159, -0.708216);
			glVertex3f(-0.121848, 0.835004, -0.704358);
			glVertex3f(-0.121940, 0.833239, -0.711740);
			glVertex3f(-0.134530, 0.771378, -0.692889);
			glVertex3f(-0.128384, 0.835159, -0.708216);
			glVertex3f(-0.121940, 0.833239, -0.711740);
			glVertex3f(-0.134530, 0.771378, -0.692889);
			glVertex3f(-0.121940, 0.833239, -0.711740);
			glVertex3f(-0.121848, 0.835004, -0.704358);
			glVertex3f(-0.134530, 0.771378, -0.692889);
			glVertex3f(-0.121848, 0.835004, -0.704358);
			glVertex3f(-0.128384, 0.835159, -0.708216);
		glColor3f(0.063, 0.196, 0.075);
			glVertex3f(0.365030, 2.279926, -0.641914);
			glVertex3f(0.465835, 2.675867, -0.609161);
			glVertex3f(0.465835, 2.304331, -0.715154);
			glVertex3f(0.566640, 2.273421, -0.641914);
			glVertex3f(0.465835, 2.675867, -0.609161);
			glVertex3f(0.528136, 2.268395, -0.523411);
			glVertex3f(0.465835, 2.304331, -0.715154);
			glVertex3f(0.465835, 2.675867, -0.609161);
			glVertex3f(0.566640, 2.273421, -0.641914);
			glVertex3f(0.403534, 2.330759, -0.523411);
			glVertex3f(0.465835, 2.675867, -0.609161);
			glVertex3f(0.365030, 2.279926, -0.641914);
			glVertex3f(0.528136, 2.268395, -0.523411);
			glVertex3f(0.465835, 2.675867, -0.609161);
			glVertex3f(0.403534, 2.330759, -0.523411);
			glVertex3f(0.318833, 2.086245, -0.656925);
			glVertex3f(0.465835, 2.456720, -0.609161);
			glVertex3f(0.465835, 2.040437, -0.763727);
			glVertex3f(0.612836, 2.100837, -0.656924);
			glVertex3f(0.465835, 2.456720, -0.609161);
			glVertex3f(0.556686, 2.029778, -0.484114);
			glVertex3f(0.465835, 2.040437, -0.763727);
			glVertex3f(0.465835, 2.456720, -0.609161);
			glVertex3f(0.612836, 2.100837, -0.656924);
			glVertex3f(0.374983, 2.093251, -0.484114);
			glVertex3f(0.465835, 2.456720, -0.609161);
			glVertex3f(0.318833, 2.086245, -0.656925);
			glVertex3f(0.556686, 2.029778, -0.484114);
			glVertex3f(0.465835, 2.456720, -0.609161);
			glVertex3f(0.374983, 2.093251, -0.484114);
			glVertex3f(0.271200, 1.779064, -0.672402);
			glVertex3f(0.465835, 2.324711, -0.609161);
			glVertex3f(0.465835, 1.829467, -0.813812);
			glVertex3f(0.660469, 1.837812, -0.672401);
			glVertex3f(0.465835, 2.324711, -0.609161);
			glVertex3f(0.586125, 1.857571, -0.443595);
			glVertex3f(0.465835, 1.829467, -0.813812);
			glVertex3f(0.465835, 2.324711, -0.609161);
			glVertex3f(0.660469, 1.837812, -0.672401);
			glVertex3f(0.345544, 1.843675, -0.443595);
			glVertex3f(0.465835, 2.324711, -0.609161);
			glVertex3f(0.271200, 1.779064, -0.672402);
			glVertex3f(0.586125, 1.857571, -0.443595);
			glVertex3f(0.465835, 2.324711, -0.609161);
			glVertex3f(0.345544, 1.843675, -0.443595);
			glVertex3f(0.223740, 1.630287, -0.687822);
			glVertex3f(0.465835, 2.100115, -0.609161);
			glVertex3f(0.465835, 1.580375, -0.863715);
			glVertex3f(0.707930, 1.609669, -0.687822);
			glVertex3f(0.465835, 2.100115, -0.609161);
			glVertex3f(0.615458, 1.580375, -0.403223);
			glVertex3f(0.465835, 1.580375, -0.863715);
			glVertex3f(0.465835, 2.100115, -0.609161);
			glVertex3f(0.707930, 1.609669, -0.687822);
			glVertex3f(0.316212, 1.651515, -0.403223);
			glVertex3f(0.465835, 2.100115, -0.609161);
			glVertex3f(0.223740, 1.630287, -0.687822);
			glVertex3f(0.615458, 1.580375, -0.403223);
			glVertex3f(0.465835, 2.100115, -0.609161);
			glVertex3f(0.316212, 1.651515, -0.403223);
			glVertex3f(0.163262, 1.353264, -0.707473);
			glVertex3f(0.465835, 1.873004, -0.609161);
			glVertex3f(0.465835, 1.384904, -0.927305);
			glVertex3f(0.768407, 1.353264, -0.707473);
			glVertex3f(0.465835, 1.873004, -0.609161);
			glVertex3f(0.652835, 1.394177, -0.351777);
			glVertex3f(0.465835, 1.384904, -0.927305);
			glVertex3f(0.465835, 1.873004, -0.609161);
			glVertex3f(0.768407, 1.353264, -0.707473);
			glVertex3f(0.278834, 1.392158, -0.351777);
			glVertex3f(0.465835, 1.873004, -0.609161);
			glVertex3f(0.163262, 1.353264, -0.707473);
			glVertex3f(0.652835, 1.394177, -0.351777);
			glVertex3f(0.465835, 1.873004, -0.609161);
			glVertex3f(0.278834, 1.392158, -0.351777);
			glVertex3f(0.076767, 1.156397, -0.735577);
			glVertex3f(0.465835, 1.702143, -0.609161);
			glVertex3f(0.465835, 1.158883, -1.018251);
			glVertex3f(0.854902, 1.210137, -0.735577);
			glVertex3f(0.465835, 1.702143, -0.609161);
			glVertex3f(0.706292, 1.182402, -0.278200);
			glVertex3f(0.465835, 1.158883, -1.018251);
			glVertex3f(0.465835, 1.702143, -0.609161);
			glVertex3f(0.854902, 1.210137, -0.735577);
			glVertex3f(0.225378, 1.143382, -0.278200);
			glVertex3f(0.465835, 1.702143, -0.609161);
			glVertex3f(0.076767, 1.156397, -0.735577);
			glVertex3f(0.706292, 1.182402, -0.278200);
			glVertex3f(0.465835, 1.702143, -0.609161);
			glVertex3f(0.225378, 1.143382, -0.278200);
			glVertex3f(-0.708445, 1.822190, 0.461923);
			glVertex3f(-0.687238, 2.150319, 0.547164);
			glVertex3f(-0.612723, 1.842415, 0.500654);
			glVertex3f(-0.619978, 1.816799, 0.603661);
			glVertex3f(-0.687238, 2.150319, 0.547164);
			glVertex3f(-0.720185, 1.812634, 0.628591);
			glVertex3f(-0.612723, 1.842415, 0.500654);
			glVertex3f(-0.687238, 2.150319, 0.547164);
			glVertex3f(-0.619978, 1.816799, 0.603661);
			glVertex3f(-0.774861, 1.864317, 0.540992);
			glVertex3f(-0.687238, 2.150319, 0.547164);
			glVertex3f(-0.708445, 1.822190, 0.461923);
			glVertex3f(-0.720185, 1.812634, 0.628591);
			glVertex3f(-0.687238, 2.150319, 0.547164);
			glVertex3f(-0.774861, 1.864317, 0.540992);
			glVertex3f(-0.718164, 1.661680, 0.422859);
			glVertex3f(-0.687238, 1.968705, 0.547164);
			glVertex3f(-0.578574, 1.623718, 0.479340);
			glVertex3f(-0.589155, 1.673774, 0.629551);
			glVertex3f(-0.687238, 1.968705, 0.547164);
			glVertex3f(-0.735284, 1.614885, 0.665906);
			glVertex3f(-0.578574, 1.623718, 0.479340);
			glVertex3f(-0.687238, 1.968705, 0.547164);
			glVertex3f(-0.589155, 1.673774, 0.629551);
			glVertex3f(-0.815016, 1.667487, 0.538164);
			glVertex3f(-0.687238, 1.968705, 0.547164);
			glVertex3f(-0.718164, 1.661680, 0.422859);
			glVertex3f(-0.735284, 1.614885, 0.665906);
			glVertex3f(-0.687238, 1.968705, 0.547164);
			glVertex3f(-0.815016, 1.667487, 0.538164);
			glVertex3f(-0.728185, 1.407110, 0.382581);
			glVertex3f(-0.687238, 1.859305, 0.547164);
			glVertex3f(-0.543363, 1.448880, 0.457363);
			glVertex3f(-0.557372, 1.455796, 0.656247);
			glVertex3f(-0.687238, 1.859305, 0.547164);
			glVertex3f(-0.750852, 1.472172, 0.704383);
			glVertex3f(-0.543363, 1.448880, 0.457363);
			glVertex3f(-0.687238, 1.859305, 0.547164);
			glVertex3f(-0.557372, 1.455796, 0.656247);
			glVertex3f(-0.856420, 1.460655, 0.535247);
			glVertex3f(-0.687238, 1.859305, 0.547164);
			glVertex3f(-0.728185, 1.407110, 0.382581);
			glVertex3f(-0.750852, 1.472172, 0.704383);
			glVertex3f(-0.687238, 1.859305, 0.547164);
			glVertex3f(-0.856420, 1.460655, 0.535247);
			glVertex3f(-0.738169, 1.283814, 0.342448);
			glVertex3f(-0.687238, 1.673175, 0.547164);
			glVertex3f(-0.508280, 1.242450, 0.435465);
			glVertex3f(-0.525705, 1.266727, 0.682847);
			glVertex3f(-0.687238, 1.673175, 0.547164);
			glVertex3f(-0.766364, 1.242450, 0.742719);
			glVertex3f(-0.508280, 1.242450, 0.435465);
			glVertex3f(-0.687238, 1.673175, 0.547164);
			glVertex3f(-0.525705, 1.266727, 0.682847);
			glVertex3f(-0.897674, 1.301406, 0.532341);
			glVertex3f(-0.687238, 1.673175, 0.547164);
			glVertex3f(-0.738169, 1.283814, 0.342448);
			glVertex3f(-0.766364, 1.242450, 0.742719);
			glVertex3f(-0.687238, 1.673175, 0.547164);
			glVertex3f(-0.897674, 1.301406, 0.532341);
			glVertex3f(-0.750892, 1.054236, 0.291308);
			glVertex3f(-0.687238, 1.484961, 0.547164);
			glVertex3f(-0.463575, 1.080457, 0.407562);
			glVertex3f(-0.485353, 1.054236, 0.716742);
			glVertex3f(-0.687238, 1.484961, 0.547164);
			glVertex3f(-0.786130, 1.088142, 0.791571);
			glVertex3f(-0.463575, 1.080457, 0.407562);
			glVertex3f(-0.687238, 1.484961, 0.547164);
			glVertex3f(-0.485353, 1.054236, 0.716742);
			glVertex3f(-0.950242, 1.086469, 0.528638);
			glVertex3f(-0.687238, 1.484961, 0.547164);
			glVertex3f(-0.750892, 1.054236, 0.291308);
			glVertex3f(-0.786130, 1.088142, 0.791571);
			glVertex3f(-0.687238, 1.484961, 0.547164);
			glVertex3f(-0.950242, 1.086469, 0.528638);
			glVertex3f(-0.769089, 0.891086, 0.218167);
			glVertex3f(-0.687238, 1.343363, 0.547164);
			glVertex3f(-0.399637, 0.893147, 0.367655);
			glVertex3f(-0.427641, 0.935623, 0.765218);
			glVertex3f(-0.687238, 1.343363, 0.547164);
			glVertex3f(-0.814400, 0.912638, 0.861438);
			glVertex3f(-0.399637, 0.893147, 0.367655);
			glVertex3f(-0.687238, 1.343363, 0.547164);
			glVertex3f(-0.427641, 0.935623, 0.765218);
			glVertex3f(-1.025426, 0.880301, 0.523343);
			glVertex3f(-0.687238, 1.343363, 0.547164);
			glVertex3f(-0.769089, 0.891086, 0.218167);
			glVertex3f(-0.814400, 0.912638, 0.861438);
			glVertex3f(-0.687238, 1.343363, 0.547164);
			glVertex3f(-1.025426, 0.880301, 0.523343);
			glVertex3f(0.693225, 1.523455, 0.053400);
			glVertex3f(0.752542, 1.754263, 0.036105);
			glVertex3f(0.717763, 1.537682, -0.014964);
			glVertex3f(0.790364, 1.519663, -0.012753);
			glVertex3f(0.752542, 1.754263, 0.036105);
			glVertex3f(0.810696, 1.516733, 0.056979);
			glVertex3f(0.717763, 1.537682, -0.014964);
			glVertex3f(0.752542, 1.754263, 0.036105);
			glVertex3f(0.790364, 1.519663, -0.012753);
			glVertex3f(0.750660, 1.553088, 0.097863);
			glVertex3f(0.752542, 1.754263, 0.036105);
			glVertex3f(0.693225, 1.523455, 0.053400);
			glVertex3f(0.810696, 1.516733, 0.056979);
			glVertex3f(0.752542, 1.754263, 0.036105);
			glVertex3f(0.750660, 1.553088, 0.097863);
			glVertex3f(0.666041, 1.410551, 0.061326);
			glVertex3f(0.752542, 1.626515, 0.036105);
			glVertex3f(0.701825, 1.383849, -0.038368);
			glVertex3f(0.807697, 1.419058, -0.035143);
			glVertex3f(0.752542, 1.626515, 0.036105);
			glVertex3f(0.837346, 1.377635, 0.066544);
			glVertex3f(0.701825, 1.383849, -0.038368);
			glVertex3f(0.752542, 1.626515, 0.036105);
			glVertex3f(0.807697, 1.419058, -0.035143);
			glVertex3f(0.749798, 1.414636, 0.126165);
			glVertex3f(0.752542, 1.626515, 0.036105);
			glVertex3f(0.666041, 1.410551, 0.061326);
			glVertex3f(0.837346, 1.377635, 0.066544);
			glVertex3f(0.752542, 1.626515, 0.036105);
			glVertex3f(0.749798, 1.414636, 0.126165);
			glVertex3f(0.638012, 1.231485, 0.069498);
			glVertex3f(0.752542, 1.549562, 0.036105);
			glVertex3f(0.685391, 1.260866, -0.062500);
			glVertex3f(0.825570, 1.265731, -0.058230);
			glVertex3f(0.752542, 1.549562, 0.036105);
			glVertex3f(0.864826, 1.277249, 0.076408);
			glVertex3f(0.685391, 1.260866, -0.062500);
			glVertex3f(0.752542, 1.549562, 0.036105);
			glVertex3f(0.825570, 1.265731, -0.058230);
			glVertex3f(0.748909, 1.269149, 0.155348);
			glVertex3f(0.752542, 1.549562, 0.036105);
			glVertex3f(0.638012, 1.231485, 0.069498);
			glVertex3f(0.864826, 1.277249, 0.076408);
			glVertex3f(0.752542, 1.549562, 0.036105);
			glVertex3f(0.748909, 1.269149, 0.155348);
			glVertex3f(0.610085, 1.144757, 0.077641);
			glVertex3f(0.752542, 1.418637, 0.036105);
			glVertex3f(0.669017, 1.115662, -0.086544);
			glVertex3f(0.843377, 1.132738, -0.081232);
			glVertex3f(0.752542, 1.418637, 0.036105);
			glVertex3f(0.892206, 1.115662, 0.086235);
			glVertex3f(0.669017, 1.115662, -0.086544);
			glVertex3f(0.752542, 1.418637, 0.036105);
			glVertex3f(0.843377, 1.132738, -0.081232);
			glVertex3f(0.748023, 1.157132, 0.184425);
			glVertex3f(0.752542, 1.418637, 0.036105);
			glVertex3f(0.610085, 1.144757, 0.077641);
			glVertex3f(0.892206, 1.115662, 0.086235);
			glVertex3f(0.752542, 1.418637, 0.036105);
			glVertex3f(0.748023, 1.157132, 0.184425);
			glVertex3f(0.574498, 0.983271, 0.088017);
			glVertex3f(0.752542, 1.286246, 0.036105);
			glVertex3f(0.648151, 1.001715, -0.117183);
			glVertex3f(0.866069, 0.983271, -0.110545);
			glVertex3f(0.752542, 1.286246, 0.036105);
			glVertex3f(0.927095, 1.007120, 0.098758);
			glVertex3f(0.648151, 1.001715, -0.117183);
			glVertex3f(0.752542, 1.286246, 0.036105);
			glVertex3f(0.866069, 0.983271, -0.110545);
			glVertex3f(0.746895, 1.005943, 0.221476);
			glVertex3f(0.752542, 1.286246, 0.036105);
			glVertex3f(0.574498, 0.983271, 0.088017);
			glVertex3f(0.927095, 1.007120, 0.098758);
			glVertex3f(0.752542, 1.286246, 0.036105);
			glVertex3f(0.746895, 1.005943, 0.221476);
			glVertex3f(0.523601, 0.868509, 0.102857);
			glVertex3f(0.752542, 1.186644, 0.036105);
			glVertex3f(0.618310, 0.869959, -0.161002);
			glVertex3f(0.898522, 0.899837, -0.152467);
			glVertex3f(0.752542, 1.186644, 0.036105);
			glVertex3f(0.976994, 0.883669, 0.116669);
			glVertex3f(0.618310, 0.869959, -0.161002);
			glVertex3f(0.752542, 1.186644, 0.036105);
			glVertex3f(0.898522, 0.899837, -0.152467);
			glVertex3f(0.745280, 0.860923, 0.274467);
			glVertex3f(0.752542, 1.186644, 0.036105);
			glVertex3f(0.523601, 0.868509, 0.102857);
			glVertex3f(0.976994, 0.883669, 0.116669);
			glVertex3f(0.752542, 1.186644, 0.036105);
			glVertex3f(0.745280, 0.860923, 0.274467);
			glVertex3f(-0.614931, 1.761708, -0.732461);
			glVertex3f(-0.687506, 2.032892, -0.730765);
			glVertex3f(-0.663466, 1.778423, -0.662265);
			glVertex3f(-0.745225, 1.757252, -0.686734);
			glVertex3f(-0.687506, 2.032892, -0.730765);
			glVertex3f(-0.747218, 1.753810, -0.772052);
			glVertex3f(-0.663466, 1.778423, -0.662265);
			glVertex3f(-0.687506, 2.032892, -0.730765);
			glVertex3f(-0.745225, 1.757252, -0.686734);
			glVertex3f(-0.666692, 1.796524, -0.800313);
			glVertex3f(-0.687506, 2.032892, -0.730765);
			glVertex3f(-0.614931, 1.761708, -0.732461);
			glVertex3f(-0.747218, 1.753810, -0.772052);
			glVertex3f(-0.687506, 2.032892, -0.730765);
			glVertex3f(-0.666692, 1.796524, -0.800313);
			glVertex3f(-0.581671, 1.629053, -0.733238);
			glVertex3f(-0.687506, 1.882796, -0.730765);
			glVertex3f(-0.652449, 1.597679, -0.630874);
			glVertex3f(-0.771675, 1.639048, -0.666555);
			glVertex3f(-0.687506, 1.882796, -0.730765);
			glVertex3f(-0.774583, 1.590378, -0.790972);
			glVertex3f(-0.652449, 1.597679, -0.630874);
			glVertex3f(-0.687506, 1.882796, -0.730765);
			glVertex3f(-0.771675, 1.639048, -0.666555);
			glVertex3f(-0.657154, 1.633852, -0.832185);
			glVertex3f(-0.687506, 1.882796, -0.730765);
			glVertex3f(-0.581671, 1.629053, -0.733238);
			glVertex3f(-0.774583, 1.590378, -0.790972);
			glVertex3f(-0.687506, 1.882796, -0.730765);
			glVertex3f(-0.657154, 1.633852, -0.832185);
			glVertex3f(-0.547377, 1.418661, -0.734040);
			glVertex3f(-0.687506, 1.792382, -0.730765);
			glVertex3f(-0.641089, 1.453182, -0.598506);
			glVertex3f(-0.798949, 1.458898, -0.645749);
			glVertex3f(-0.687506, 1.792382, -0.730765);
			glVertex3f(-0.802798, 1.472432, -0.810482);
			glVertex3f(-0.641089, 1.453182, -0.598506);
			glVertex3f(-0.687506, 1.792382, -0.730765);
			glVertex3f(-0.798949, 1.458898, -0.645749);
			glVertex3f(-0.647318, 1.462914, -0.865048);
			glVertex3f(-0.687506, 1.792382, -0.730765);
			glVertex3f(-0.547377, 1.418661, -0.734040);
			glVertex3f(-0.802798, 1.472432, -0.810482);
			glVertex3f(-0.687506, 1.792382, -0.730765);
			glVertex3f(-0.647318, 1.462914, -0.865048);
			glVertex3f(-0.513207, 1.316762, -0.734838);
			glVertex3f(-0.687506, 1.638553, -0.730765);
			glVertex3f(-0.629771, 1.282577, -0.566255);
			glVertex3f(-0.826123, 1.302640, -0.625019);
			glVertex3f(-0.687506, 1.638553, -0.730765);
			glVertex3f(-0.830912, 1.282577, -0.829920);
			glVertex3f(-0.629771, 1.282577, -0.566255);
			glVertex3f(-0.687506, 1.638553, -0.730765);
			glVertex3f(-0.826123, 1.302640, -0.625019);
			glVertex3f(-0.637519, 1.331301, -0.897792);
			glVertex3f(-0.687506, 1.638553, -0.730765);
			glVertex3f(-0.513207, 1.316762, -0.734838);
			glVertex3f(-0.830912, 1.282577, -0.829920);
			glVertex3f(-0.687506, 1.638553, -0.730765);
			glVertex3f(-0.637519, 1.331301, -0.897792);
			glVertex3f(-0.469665, 1.127025, -0.735856);
			glVertex3f(-0.687506, 1.483002, -0.730765);
			glVertex3f(-0.615348, 1.148696, -0.525159);
			glVertex3f(-0.860751, 1.127025, -0.598602);
			glVertex3f(-0.687506, 1.483002, -0.730765);
			glVertex3f(-0.866736, 1.155047, -0.854690);
			glVertex3f(-0.615348, 1.148696, -0.525159);
			glVertex3f(-0.687506, 1.483002, -0.730765);
			glVertex3f(-0.860751, 1.127025, -0.598602);
			glVertex3f(-0.625031, 1.153664, -0.939517);
			glVertex3f(-0.687506, 1.483002, -0.730765);
			glVertex3f(-0.469665, 1.127025, -0.735856);
			glVertex3f(-0.866736, 1.155047, -0.854690);
			glVertex3f(-0.687506, 1.483002, -0.730765);
			glVertex3f(-0.625031, 1.153664, -0.939517);
			glVertex3f(-0.407392, 0.992188, -0.737311);
			glVertex3f(-0.687506, 1.365977, -0.730765);
			glVertex3f(-0.594721, 0.993891, -0.466383);
			glVertex3f(-0.910276, 1.028996, -0.560822);
			glVertex3f(-0.687506, 1.365977, -0.730765);
			glVertex3f(-0.917971, 1.010000, -0.890116);
			glVertex3f(-0.594721, 0.993891, -0.466383);
			glVertex3f(-0.687506, 1.365977, -0.730765);
			glVertex3f(-0.910276, 1.028996, -0.560822);
			glVertex3f(-0.607172, 0.983275, -0.999192);
			glVertex3f(-0.687506, 1.365977, -0.730765);
			glVertex3f(-0.407392, 0.992188, -0.737311);
			glVertex3f(-0.917971, 1.010000, -0.890116);
			glVertex3f(-0.687506, 1.365977, -0.730765);
			glVertex3f(-0.607172, 0.983275, -0.999192);
		glColor3f(0.196, 0.069, 0.023);
			glVertex3f(0.522916, 0.822560, 0.471521);
			glVertex3f(0.560731, 0.852785, 0.507724);
			glVertex3f(0.560731, 0.792335, 0.507724);
			glVertex3f(0.296065, 0.797874, 0.738330);
			glVertex3f(0.326951, 0.822560, 0.767899);
			glVertex3f(0.285769, 0.822560, 0.728473);
			glVertex3f(-0.783305, 1.048591, -0.091593);
			glVertex3f(-0.832199, 1.049340, -0.098414);
			glVertex3f(-0.787253, 1.047834, -0.063372);
			glVertex3f(0.659407, 0.853249, 0.401278);
			glVertex3f(0.597590, 0.854197, 0.392654);
			glVertex3f(0.621010, 0.852288, 0.450485);
			glVertex3f(0.560731, 0.792335, 0.507724);
			glVertex3f(0.535521, 0.792335, 0.483589);
			glVertex3f(0.522916, 0.822560, 0.471521);
			glVertex3f(0.522916, 0.822560, 0.471521);
			glVertex3f(0.535521, 0.852785, 0.483589);
			glVertex3f(0.560731, 0.852785, 0.507724);
			glVertex3f(0.560731, 0.852785, 0.507724);
			glVertex3f(0.573336, 0.822560, 0.519792);
			glVertex3f(0.560731, 0.792335, 0.507724);
			glVertex3f(0.296065, 0.847247, 0.738330);
			glVertex3f(0.285769, 0.822560, 0.728473);
			glVertex3f(0.326951, 0.822560, 0.767899);
			glVertex3f(0.296065, 0.797874, 0.738330);
			glVertex3f(0.316655, 0.797874, 0.758043);
			glVertex3f(0.326951, 0.822560, 0.767899);
			glVertex3f(0.326951, 0.822560, 0.767899);
			glVertex3f(0.316655, 0.847247, 0.758043);
			glVertex3f(0.296065, 0.847247, 0.738330);
			glVertex3f(-0.813675, 1.047831, -0.052672);
			glVertex3f(-0.787253, 1.047834, -0.063372);
			glVertex3f(-0.832199, 1.049340, -0.098414);
			glVertex3f(-0.783305, 1.048591, -0.091593);
			glVertex3f(-0.805777, 1.049344, -0.109114);
			glVertex3f(-0.832199, 1.049340, -0.098414);
			glVertex3f(-0.832199, 1.049340, -0.098414);
			glVertex3f(-0.836148, 1.048584, -0.070193);
			glVertex3f(-0.813675, 1.047831, -0.052672);
			glVertex3f(0.621010, 0.852288, 0.450485);
			glVertex3f(0.654414, 0.852292, 0.436958);
			glVertex3f(0.659407, 0.853249, 0.401278);
			glVertex3f(0.659407, 0.853249, 0.401278);
			glVertex3f(0.630994, 0.854201, 0.379126);
			glVertex3f(0.597590, 0.854197, 0.392654);
			glVertex3f(0.597590, 0.854197, 0.392654);
			glVertex3f(0.592597, 0.853240, 0.428333);
			glVertex3f(0.621010, 0.852288, 0.450485);

		glEnd( );

	glEndList( );


	// create the axes:

	AxesList = glGenLists( 1 );
	glNewList( AxesList, GL_COMPILE );
		glLineWidth( AXES_WIDTH );
			Axes( 1.5 );
		glLineWidth( 1. );
	glEndList( );
}


// the keyboard callback:

void
Keyboard( unsigned char c, int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Keyboard: '%c' (0x%0x)\n", c, c );

	switch( c )
	{
		case 'o':
		case 'O':
			WhichProjection = ORTHO;
			break;

		case 'p':
		case 'P':
			WhichProjection = PERSP;
			break;

		case 'q':
		case 'Q':
		case ESCAPE:
			DoMainMenu( QUIT );	// will not return here
			break;				// happy compiler

		default:
			fprintf( stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c );
	}

	// force a call to Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// called when the mouse button transitions down or up:

void
MouseButton( int button, int state, int x, int y )
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	if( DebugOn != 0 )
		fprintf( stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y );

	
	// get the proper button bit mask:

	switch( button )
	{
		case GLUT_LEFT_BUTTON:
			b = LEFT;		break;

		case GLUT_MIDDLE_BUTTON:
			b = MIDDLE;		break;

		case GLUT_RIGHT_BUTTON:
			b = RIGHT;		break;

		default:
			b = 0;
			fprintf( stderr, "Unknown mouse button: %d\n", button );
	}


	// button down sets the bit, up clears the bit:

	if( state == GLUT_DOWN )
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}
}


// called when the mouse moves while a button is down:

void
MouseMotion( int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "MouseMotion: %d, %d\n", x, y );


	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if( ( ActiveButton & LEFT ) != 0 )
	{
		Xrot += ( ANGFACT*dy );
		Yrot += ( ANGFACT*dx );
	}


	if( ( ActiveButton & MIDDLE ) != 0 )
	{
		Scale += SCLFACT * (float) ( dx - dy );

		// keep object from turning inside-out or disappearing:

		if( Scale < MINSCALE )
			Scale = MINSCALE;
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void
Reset( )
{
	ActiveButton = 0;
	AxesOn = 1;
	DebugOn = 0;
	DepthBufferOn = 1;
	DepthFightingOn = 0;
	DepthCueOn = 0;
	Scale  = 1.0;
	WhichColor = WHITE;
	WhichProjection = PERSP;
	Xrot = Yrot = 0.;
}


// called when user resizes the window:

void
Resize( int width, int height )
{
	if( DebugOn != 0 )
		fprintf( stderr, "ReSize: %d, %d\n", width, height );

	// don't really need to do anything since window size is
	// checked each time in Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// handle a change to the window's visibility:

void
Visibility ( int state )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Visibility: %d\n", state );

	if( state == GLUT_VISIBLE )
	{
		glutSetWindow( MainWindow );
		glutPostRedisplay( );
	}
	else
	{
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it ...
	}
}



///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////


// the stroke characters 'X' 'Y' 'Z' :

static float xx[ ] = {
		0.f, 1.f, 0.f, 1.f
	      };

static float xy[ ] = {
		-.5f, .5f, .5f, -.5f
	      };

static int xorder[ ] = {
		1, 2, -3, 4
		};

static float yx[ ] = {
		0.f, 0.f, -.5f, .5f
	      };

static float yy[ ] = {
		0.f, .6f, 1.f, 1.f
	      };

static int yorder[ ] = {
		1, 2, 3, -2, 4
		};

static float zx[ ] = {
		1.f, 0.f, 1.f, 0.f, .25f, .75f
	      };

static float zy[ ] = {
		.5f, .5f, -.5f, -.5f, 0.f, 0.f
	      };

static int zorder[ ] = {
		1, 2, 3, 4, -5, 6
		};

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void
Axes( float length )
{
	glBegin( GL_LINE_STRIP );
		glVertex3f( length, 0., 0. );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., length, 0. );
	glEnd( );
	glBegin( GL_LINE_STRIP );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., 0., length );
	glEnd( );

	float fact = LENFRAC * length;
	float base = BASEFRAC * length;

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 4; i++ )
		{
			int j = xorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( base + fact*xx[j], fact*xy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 5; i++ )
		{
			int j = yorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( fact*yx[j], base + fact*yy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 6; i++ )
		{
			int j = zorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( 0.0, fact*zy[j], base + fact*zx[j] );
		}
	glEnd( );

}


// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );

void
HsvRgb( float hsv[3], float rgb[3] )
{
	// guarantee valid input:

	float h = hsv[0] / 60.f;
	while( h >= 6. )	h -= 6.;
	while( h <  0. ) 	h += 6.;

	float s = hsv[1];
	if( s < 0. )
		s = 0.;
	if( s > 1. )
		s = 1.;

	float v = hsv[2];
	if( v < 0. )
		v = 0.;
	if( v > 1. )
		v = 1.;

	// if sat==0, then is a gray:

	if( s == 0.0 )
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}

	// get an rgb from the hue itself:
	
	float i = floor( h );
	float f = h - i;
	float p = v * ( 1.f - s );
	float q = v * ( 1.f - s*f );
	float t = v * ( 1.f - ( s * (1.f-f) ) );

	float r, g, b;			// red, green, blue
	switch( (int) i )
	{
		case 0:
			r = v;	g = t;	b = p;
			break;
	
		case 1:
			r = q;	g = v;	b = p;
			break;
	
		case 2:
			r = p;	g = v;	b = t;
			break;
	
		case 3:
			r = p;	g = q;	b = v;
			break;
	
		case 4:
			r = t;	g = p;	b = v;
			break;
	
		case 5:
			r = v;	g = p;	b = q;
			break;
	}


	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}
