/*
 *  CSCI 441, Computer Graphics, Fall 2017
 *
 *  Project: lab03
 *  File: main.cpp
 *
 *	Author: Dr. Jeffrey Paone - Fall 2015
 *
 *  Description:
 *      Contains the base code for 3D Bezier Curve visualizer.
 *
 */

// HEADERS ///////////////////////////////////////////////////////////////////// 

// include the OpenGL library header
#ifdef __APPLE__				// if compiling on Mac OS
#include <OpenGL/gl.h>
#else							// if compiling on Linux or Windows OS
#include <GL/gl.h>
#endif

#include <GLFW/glfw3.h>			// include GLFW framework header

#include <CSCI441/objects.hpp> // for our 3D objects
#include "hero_base.hpp"
#include "ire.hpp"
#include "hans.hpp"
#include "targa.hpp"

// include GLM libraries and matrix functions
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <math.h>				// for cos(), sin() functionality
#include <stdio.h>			// for printf functionality
#include <stdlib.h>			// for exit functionality
#include <time.h>			  // for time() functionality

#include <fstream>			// for file I/O
#include <sstream>
#include <vector>				// for vector
#include <string>     // std::string, std::stoi
using namespace std;

//*************************************************************************************
//
// Global Parameters

// global variables to keep track of window width and height.
// set to initial values for convenience, but we need variables
// for later on in case the window gets resized.
int windowWidth = 640, windowHeight = 480;

int leftMouseButton;    	 									// status of the mouse button
glm::vec2 mousePosition;				            // last known X and Y of the mouse

// Camera index to choose between 3 camera modes
int camIndex = 0;
// Use to CTRL + drag to zoom
bool cameraZoom = false;

// Freecam variables
glm::vec3 freePos;
glm::vec3 freeDir;
float freeRadius;
float freeTheta;
float freePhi;

// Arcball cam variables
glm::vec3 arcballPos;
glm::vec3 arcballDir;
float arcballRadius;
float arcballTheta;
float arcballPhi;
int arcballHero = 0;

// First person cam variables
// No angles as it always faces direction character is facing
glm::vec3 fpPos;
glm::vec3 fpDir;
int fpHero = 0;

// Bezier variables
vector<glm::vec3> controlPoints;
vector<glm::vec3> patchPoints;
vector<glm::vec3> coeffs;
float trackPointVal = 0.0f;
int numSegments = 0;

int wandererPatchPointCount = 5;

bool moveWanderer = false;
float wandererU = 0.0;
float wandererV = 0.0;
float wandererTheta = 0;
float wandererStepSize = 0.01;
float wandererTurnMag = 0.05;
float wandererMoveSign = 0.0;
float wandererTurnSign = 0.0;

// Store heros in vector for easy access when switching cameras
vector<Hero*> heros;
Ire* ire = new Ire();
Hans* hans = new Hans();
Targa* targa = new Targa();

int currentCurvePointParametric = 0;
int currentCurvePointArc = 0;

vector<glm::vec3> curvePoints;
vector<glm::vec3> curveDirections;
int r = 0;

//*************************************************************************************
//
// Helper Function

// inspired by https://stackoverflow.com/questions/236129/most-elegant-way-to-split-a-string
vector<string> split(const std::string &s, char delim) {
	vector<string> fields;
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        fields.push_back(item);
    }
	return fields;
}

void parseCSVFields(ifstream &file, vector<glm::vec3> &points) {
	int num_points;
	file >> num_points;
	for (int i = 0; i < num_points; i++) {
		string line;
		file >> line;
		// printf("%s\n", line.c_str());
		vector<string> fields = split(line, ',');
		
		float x,y,z;
		x = atoi(fields.at(0).c_str());
		y = atoi(fields.at(1).c_str());
		z = atoi(fields.at(2).c_str());
		points.push_back(glm::vec3(x,y,z));
		printf("%f %f %f\n", x, y, z);
	}
}

// loadControlPoints() /////////////////////////////////////////////////////////
//
//  Load our control points from file and store them in
//	the global variable controlPoints
//
////////////////////////////////////////////////////////////////////////////////
bool loadControlPoints( char* filename ) {
	// TODO #02: read in control points from file.  Make sure the file can be
	// opened and handle it appropriately.
	ifstream file ( filename );
	if (!file.good()) {
		printf ("Invalid CSV file");
		exit(1);
	}
	
	parseCSVFields(file, controlPoints);
	parseCSVFields(file, patchPoints);
	
	return true;
}


// recomputeOrientation() //////////////////////////////////////////////////////
//
// This function updates the camera's position in cartesian coordinates based
//  on its position in spherical coordinates. Should be called every time
//  cameraTheta, cameraPhi, or cameraRadius is updated.
//
////////////////////////////////////////////////////////////////////////////////
void recomputeOrientation() {
	switch(camIndex) {
		case 0:		// Free cam (1 on keyboard)
			freeDir.x =  sinf(freeTheta)*sinf(freePhi);
			freeDir.z = -cosf(freeTheta)*sinf(freePhi);
			freeDir.y = -cosf(freePhi);
			//and normalize this directional vector!
			freeDir = glm::normalize( freeDir );
			break;
		case 1:		// Arcball cam on hero (2 on keyboard)
			arcballDir.x = arcballRadius * sinf(arcballTheta)*sinf(arcballPhi);
			arcballDir.z = arcballRadius * -cosf(arcballTheta)*sinf(arcballPhi);
			arcballDir.y = arcballRadius * -cosf(arcballPhi);
			arcballPos = heros.at(arcballHero)->getPosition() - arcballDir;
			arcballDir = glm::normalize(arcballDir);
			break;
		case 2:		// First person cam on hero (3 on keyboard)
		
			break;
	}
}

// evaluateBezierCurve() ////////////////////////////////////////////////////////
//
// Computes a location along a Bezier Curve.
//
////////////////////////////////////////////////////////////////////////////////
glm::vec3 evaluateBezierCurve( glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t, bool derivative) {
	// TODO #06: Compute a point along a Bezier curve
	glm::vec3 a = -1.0f * p0 + 3.0f * p1 - 3.0f * p2 + p3;
	glm::vec3 b = 3.0f * p0 - 6.0f * p1 + 3.0f * p2;
	glm::vec3 c = -3.0f * p0 + 3.0f * p1;
	glm::vec3 d = p0;
	glm::vec3 point;
	if(derivative){
		point = (3 * (t * t ) * a) + (2 * (t) * b) + (c);
	}
	else {
		point = (t * t * t) * a + (t * t) * b + t * c + d;
	}
	
	return point;
}

glm::vec3 evaluateBezierPatch(vector<glm::vec3> points, float u, float v ) {
	vector<glm::vec3> p = points;
	vector<glm::vec3> r_p;
	for (int i = 0; i < 16; i += 4) {
		r_p.push_back(evaluateBezierCurve(p.at(i), p.at(i+1), p.at(i+2), p.at(i+3), u, false));
	}
	
	return evaluateBezierCurve(r_p.at(0), r_p.at(1), r_p.at(2), r_p.at(3), v, false);
}

glm::vec3 evaluateBezierPatchNormal(vector<glm::vec3> points, float u, float v, int numPoints) {
	glm::vec3 point1 = evaluateBezierPatch(patchPoints, u, v);
	glm::vec3 point2 = evaluateBezierPatch(patchPoints, u + (1.0 / numPoints), v);				
	glm::vec3 point4 = evaluateBezierPatch(patchPoints, u, v + (1.0 / numPoints));
	glm::vec3 point3 = evaluateBezierPatch(patchPoints, u + (1.0 / numPoints), v + (1.0 / numPoints));
	return glm::cross(point2 - point1, point3 - point1);
}

void loadCurvePoints() {
	int n = 0;
	while(n + 3 <= controlPoints.size() - 1){
		glm::vec3 lineTo;
		for(float t = 0; t < 1; t+=.01) {
			lineTo = evaluateBezierCurve(controlPoints.at(n), controlPoints.at(n + 1), controlPoints.at(n + 2),controlPoints.at(n + 3), t, false);
			curvePoints.push_back(lineTo);
			lineTo = evaluateBezierCurve(controlPoints.at(n), controlPoints.at(n + 1), controlPoints.at(n + 2),controlPoints.at(n + 3), t, true);
			curveDirections.push_back(lineTo);
		}
		n+=3;
	}
}

// renderBezierCurve() //////////////////////////////////////////////////////////
//
// Responsible for drawing a Bezier Curve as defined by four control points.
//  Breaks the curve into n segments as specified by the resolution.
//
////////////////////////////////////////////////////////////////////////////////
void renderBezierCurve(vector<glm::vec3> points, int numPoints) {
    float t = 0.0;
	glColor3f( 0.0, 0.0, 1.0 );
	for (unsigned int j = 0; j < points.size() - 1; j++) {
		if ((j % 3) == 0) {
			glBegin(GL_LINE_STRIP); {
				for (int i = 0; i <= numPoints; i++) {
					t = i * (float) (1.0 / numPoints);
					glm::vec3 point = evaluateBezierCurve(points.at(j), points.at(j + 1), points.at(j + 2), points.at(j + 3), t, true);
					glVertex3f(point.x, point.y, point.z);
				}
			}; glEnd();
		}
	}
}

void renderBezierPatch(int numPoints) {
	
	glEnable(GL_LIGHTING);
	GLfloat matColorD[4] = { 0.0, 0.0, 0.0, 1.0 }; 
	glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, matColorD);

	glBegin(GL_TRIANGLE_STRIP); {
		for (int u = 0; u < numPoints ; u++) {
			for (int v = 0; v < numPoints; v++) {
				glm::vec3 point1 = evaluateBezierPatch(patchPoints, (float) u / numPoints, (float) v / numPoints);
				glm::vec3 point2 = evaluateBezierPatch(patchPoints, (float) (u + 1) / numPoints, (float) v / numPoints);				
				glm::vec3 point4 = evaluateBezierPatch(patchPoints, (float) u / numPoints, (float) (v + 1) / numPoints);
				glm::vec3 point3 = evaluateBezierPatch(patchPoints, (float) (u + 1) / numPoints, (float) (v + 1) / numPoints);
				glm::vec3 normal = glm::cross(point2 - point1, point3 - point1); 
				
				glNormal3f(normal.x, normal.y, normal.z);
				glVertex3f(point1.x, point1.y, point1.z);
				glVertex3f(point2.x, point2.y, point2.z);
				glVertex3f(point4.x, point4.y, point4.z);
				glVertex3f(point3.x, point3.y, point3.z);
			}
		}
	}; glEnd();
}

//*************************************************************************************
// Event Callbacks

//
//	void error_callback( int error, const char* description )
//
//		We will register this function as GLFW's error callback.
//	When an error within GLFW occurs, GLFW will tell us by calling
//	this function.  We can then print this info to the terminal to
//	alert the user.
//
static void error_callback( int error, const char* description ) {
	fprintf( stderr, "[ERROR]: %s\n", description );
}

static void keyboard_callback( GLFWwindow *window, int key, int scancode, int action, int mods ) {
	//because the direction vector is unit length, and we probably don't want
	//to move one full unit every time a button is pressed, just create a constant
	//to keep track of how far we want to move at each step. you could make
	//this change w.r.t. the amount of time the button's held down for
	//simple scale-sensitive movement!
	//float movementConstant = 0.3f;

	if( action == GLFW_PRESS || action == GLFW_REPEAT ) {
		switch( key ) {
			case GLFW_KEY_ESCAPE:
			case GLFW_KEY_Q:
				exit(EXIT_SUCCESS);
				break;
			
			case GLFW_KEY_LEFT_CONTROL:
			case GLFW_KEY_RIGHT_CONTROL:
				cameraZoom = true;
				break;
				
			case GLFW_KEY_1:	// free cam
				camIndex = 0;
				break;
			case GLFW_KEY_2:	// arcball cam
				if (camIndex == 1) {
					arcballHero += 1;
					if (arcballHero >= 3) { arcballHero = 0; }
				}
				camIndex = 1;
				arcballTheta = -M_PI / 3.0f;
				arcballPhi = M_PI / 2.8f;
				arcballRadius = 15.0f;
				recomputeOrientation();
				break;
			case GLFW_KEY_3:	// first person cam
				if (camIndex == 2) {
					fpHero += 1;
					if (fpHero >= 3) { fpHero = 0; }
				}
				camIndex = 2;
				fpPos = heros.at(fpHero)->getPosition();
				fpDir = heros.at(fpHero)->getDirection();
				// move camera forward a bit so it doesnt see the inside of hero
				fpPos += fpDir;
				break;
			case GLFW_KEY_7:
				moveWanderer = !moveWanderer;
				break;
			
			//move forward!
			case GLFW_KEY_W:
				if (moveWanderer) {
					wandererMoveSign = -1.0;
				}
				break;
			// move left!
			case GLFW_KEY_A:
				if (moveWanderer) {
					wandererTurnSign = 1.0;
				}
				break;
			
			//move backwards!
			case GLFW_KEY_S:
				if (moveWanderer) {
					wandererMoveSign = 1.0;
				}
				break;
			
			// move right!
			case GLFW_KEY_D:
				if (moveWanderer) {
					wandererTurnSign = -1.0;
				}
				break; 
			
		}
	}
	else if ( action == GLFW_RELEASE ) {
		switch ( key ) {
			case GLFW_KEY_LEFT_CONTROL:
			case GLFW_KEY_RIGHT_CONTROL:
				cameraZoom = false;
				break;
			case GLFW_KEY_W:
			case GLFW_KEY_S:
				wandererMoveSign = 0.0;
				break;
			case GLFW_KEY_A:
			case GLFW_KEY_D:
				wandererTurnSign = 0.0;
				break;
		}
	}
}

//
//	void cursor_callback
//
//	handles mouse movement.  keeps global state of our mouse cursor.
//		active motion with left mouse button pans our free cam
//
static void cursor_callback( GLFWwindow *window, double x, double y ) {
	if(leftMouseButton == GLFW_PRESS) {
		switch(camIndex) {
			case 0:
				if (cameraZoom) {
					freePos += (freeDir * 0.1f * (float)((x - mousePosition.x) + (mousePosition.y - y)));
				}
				else {
					freeTheta += (x - mousePosition.x)*0.005;
					freePhi += (mousePosition.y - y)*0.005;
					if(freePhi <= 0)
							freePhi = 0+0.001;
					if(freePhi >= M_PI)
							freePhi = M_PI-0.001;
				}
				recomputeOrientation();
				break;
			case 1:
				if (cameraZoom) {
					arcballRadius -= (0.01 * ((x - mousePosition.x) + (mousePosition.y - y)));
            		if (arcballRadius < 1.0) { arcballRadius = 1.0; }
            		if (arcballRadius > 25.0) { arcballRadius = 25.0; }
        		}
        		else {
            		arcballTheta += (0.005 * (x - mousePosition.x));
            		arcballPhi += (0.005 * (mousePosition.y - y));
            		if (arcballPhi <= 0) { arcballPhi = 0.0001; }
            		if (arcballPhi >= M_PI / 2) { arcballPhi = (M_PI / 2) - 0.0001; }
        		}
				recomputeOrientation();
				break;
		}
	}

	mousePosition.x = x;
	mousePosition.y = y;
}

//
//	void mouse_button_callback
//
//	handles mouse clicks.  keeps global state of our left mouse button (pressed or released)
//
static void mouse_button_callback( GLFWwindow *window, int button, int action, int mods ) {
	if( button == GLFW_MOUSE_BUTTON_LEFT ) {
		leftMouseButton = action;
	}
}

// drawGrid() //////////////////////////////////////////////////////////////////
//
//  Function to draw a grid in the XZ-Plane using OpenGL 2D Primitives (GL_LINES)
//
////////////////////////////////////////////////////////////////////////////////
void drawCoaster(){
	glm::mat4 transMtx;
	glDisable( GL_LIGHTING );
	for(int i = 0; i < controlPoints.size(); i++) {
		glColor3f(0.000, 0.502, 0.000);
		transMtx = glm::translate(glm::mat4(), glm::vec3(controlPoints.at(i).x, controlPoints.at(i).y, controlPoints.at(i).z));
	    glMultMatrixf( &transMtx[0][0] );
		//CSCI441::drawSolidSphere( .333, 10, 20 );
		glMultMatrixf( &( glm::inverse( transMtx ) )[0][0] );
		
	}
	for(int i = 0; i < controlPoints.size() - 1; i++) {
		glColor3f(1.000, 1.000, 0.000);
		glLineWidth(5);
		glBegin(GL_LINES);
		//glVertex3f(controlPoints.at(i).x, controlPoints.at(i).y, controlPoints.at(i).z);
		//glVertex3f(controlPoints.at(i + 1).x, controlPoints.at(i + 1).y, controlPoints.at(i + 1).z);
		glEnd();
		glLineWidth(1);
		
	}
	glEnable( GL_LIGHTING );
	int n = 0;
	while(n + 3 <= controlPoints.size() - 1){
		glm::vec3 lineTo;
		glDisable( GL_LIGHTING );
		for(float t = 0; t < 1; t+=.01) {
			glColor3f(0.000, 0.000, 1.000);
			glm::vec3 lineTo = evaluateBezierCurve(controlPoints.at(n),controlPoints.at(n + 1),controlPoints.at(n + 2), controlPoints.at(n + 3), t, false);
			transMtx = glm::translate(glm::mat4(), lineTo);
			glMultMatrixf( &transMtx[0][0] );
			CSCI441::drawSolidSphere( .1, 3, 3 );
			glMultMatrixf( &( glm::inverse( transMtx ) )[0][0] );
		}
		glEnable( GL_LIGHTING );
		n+=3;
	}
	if(currentCurvePointParametric >= curvePoints.size() - 1) {
		currentCurvePointParametric = 0;
	}
	
	
	hans->setPosition(curvePoints.at(currentCurvePointParametric));
	hans->setDirection(curveDirections.at(currentCurvePointParametric));
	hans->draw();
	hans->animateHero();
	currentCurvePointParametric++;
	
	int p = 1;
	while(sqrt(pow(curvePoints.at(currentCurvePointArc).x - curvePoints.at(currentCurvePointArc + p).x, 2) + pow(curvePoints.at(currentCurvePointArc).y - curvePoints.at(currentCurvePointArc + p).y, 2) + pow(curvePoints.at(currentCurvePointArc).z - curvePoints.at(currentCurvePointArc + p).z, 2)) < .2) {
		p++;
		if(currentCurvePointArc + p >= curvePoints.size() - 1) {
			currentCurvePointArc = 0;
		}
	}
	currentCurvePointArc = currentCurvePointArc + p;
	if(currentCurvePointArc >= curvePoints.size() - 1) {
		currentCurvePointArc = 0;
	}
	ire->setPosition(curvePoints.at(currentCurvePointArc));
	ire->rotate(0, curveDirections.at(currentCurvePointArc));
	ire->draw();
	ire->animateHero();
	
		
}
void drawGrid() {
    /*
     *	We will get to why we need to do this when we talk about lighting,
     *	but for now whenever we want to draw something with an OpenGL
     *	Primitive - like a line, quad, point - we need to disable lighting
     *	and then reenable it for use with the GLUT 3D Primitives.
     */
    glDisable( GL_LIGHTING );

    // draw our grid....what? triple nested for loops!  crazy!  but it works :)
    glColor3f( 1, 1, 1 );
    for( int dir = 0; dir < 2; dir++ ) {
        for( int i = -5; i < 6; i++ ) {
            glBegin( GL_LINE_STRIP ); {
                for( int j = -5; j < 6; j++ )
                    glVertex3f( dir < 1 ? i : j,
																0,
																dir < 1 ? j : i );
            }; glEnd();
        }
    }

    /*
     *	As noted above, we are done drawing with OpenGL Primitives, so we
     *	must turn lighting back on.
     */
    glEnable( GL_LIGHTING );
}

void performWandererMovement() {
	wandererTheta += wandererTurnSign * wandererTurnMag;
	wandererU += wandererMoveSign * wandererStepSize * sin(wandererTheta);
	wandererV += wandererMoveSign * wandererStepSize * cos(wandererTheta);
	targa->setDirection(glm::vec3(sin(wandererTheta - M_PI / 2), 0.0, cos(wandererTheta - M_PI / 2)));
	
	if (wandererMoveSign != 0 || wandererTurnSign != 0) targa->animateHero();
	
	if (wandererU > 1) wandererU = 1;
	else if (wandererU < 0) wandererU = 0;
	if (wandererV > 1) wandererV = 1;
	else if (wandererV < 0) wandererV = 0;
}

void drawWandererWorld() {
	renderBezierPatch(wandererPatchPointCount); // then draw the bezier patch
	glm::vec3 wandererPos = evaluateBezierPatch(patchPoints, wandererU, wandererV);
	wandererPos.y += 1;
	targa->setPosition(wandererPos);
	targa->rotate((wandererTheta - M_PI / 2) * (3.1415f/180.0f), glm::vec3(0.0, 1.0, 0.0));
	targa->draw();
}

// renderScene() ///////////////////////////////////////////////////////////////
//
//  GLUT callback for scene rendering. Sets up the modelview matrix, renders
//      a scene to the back buffer, and switches the back buffer with the
//      front buffer (what the user sees).
//
////////////////////////////////////////////////////////////////////////////////
void renderScene(void)  {
	
	drawGrid();				// first draw our grid
	
	glEnable( GL_LIGHTING );
	 
	ire->animateHero();
	drawWandererWorld();
	
	glm::mat4 transMtx1 = glm::translate(glm::mat4(), glm::vec3(-4.0f, 0.0f, 0.0f));
	glMultMatrixf( &transMtx1[0][0] );
	drawCoaster();
	glMultMatrixf( &( glm::inverse( transMtx1 ) )[0][0] );
	
	
}

//*************************************************************************************
//
// Setup Functions

//
//  void setupGLFW()
//
//      Used to setup everything GLFW related.  This includes the OpenGL context
//	and our window.
//
GLFWwindow* setupGLFW() {
	// set what function to use when registering errors
	// this is the ONLY GLFW function that can be called BEFORE GLFW is initialized
	// all other GLFW calls must be performed after GLFW has been initialized
	glfwSetErrorCallback( error_callback );

	// initialize GLFW
	if( !glfwInit() ) {
		fprintf( stderr, "[ERROR]: Could not initialize GLFW\n" );
		exit( EXIT_FAILURE );
	} else {
		fprintf( stdout, "[INFO]: GLFW initialized\n" );
	}

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 2 );	// request OpenGL v2.X
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 );	// request OpenGL v2.1
	glfwWindowHint( GLFW_RESIZABLE, GLFW_FALSE );		// do not allow our window to be able to be resized

	// create a window for a given size, with a given title
	GLFWwindow *window = glfwCreateWindow( windowWidth, windowHeight, "CSCI441 Roller Coaster Tycoon", NULL, NULL );
	if( !window ) {						// if the window could not be created, NULL is returned
		fprintf( stderr, "[ERROR]: GLFW Window could not be created\n" );
		glfwTerminate();
		exit( EXIT_FAILURE );
	} else {
		fprintf( stdout, "[INFO]: GLFW Window created\n" );
	}

	glfwMakeContextCurrent(window);		// make the created window the current window
	glfwSwapInterval(1);				     	// update our screen after at least 1 screen refresh

	glfwSetKeyCallback( window, keyboard_callback );							// set our keyboard callback function
	glfwSetCursorPosCallback( window, cursor_callback );					// set our cursor position callback function
	glfwSetMouseButtonCallback( window, mouse_button_callback );	// set our mouse button callback function

	return window;						       // return the window that was created
}

//
//  void setupOpenGL()
//
//      Used to setup everything OpenGL related.  For now, the only setting
//	we need is what color to make the background of our window when we clear
//	the window.  In the future we will be adding many more settings to this
//	function.
//
void setupOpenGL() {
	// tell OpenGL to perform depth testing with the Z-Buffer to perform hidden
	//		surface removal.  We will discuss this more very soon.
  glEnable( GL_DEPTH_TEST );

	//******************************************************************
  // this is some code to enable a default light for the scene;
  // feel free to play around with this, but we won't talk about
  // lighting in OpenGL for another couple of weeks yet.
  float lightCol[4] = { 1, 1, 1, 1};
  float ambientCol[4] = { 0.0, 0.0, 0.0, 1.0 };
  float lPosition[4] = { 10, 10, 10, 1 };
  glLightfv( GL_LIGHT0, GL_POSITION,lPosition );
  glLightfv( GL_LIGHT0, GL_DIFFUSE,lightCol );
  glLightfv( GL_LIGHT0, GL_AMBIENT, ambientCol );
  glEnable( GL_LIGHTING );
  glEnable( GL_LIGHT0 );

  // tell OpenGL not to use the material system; just use whatever we
	// pass with glColor*()
  glEnable( GL_COLOR_MATERIAL );
  glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
	//******************************************************************

  // tells OpenGL to blend colors across triangles. Once lighting is
  // enabled, this means that objects will appear smoother - if your object
  // is rounded or has a smooth surface, this is probably a good idea;
  // if your object has a blocky surface, you probably want to disable this.
  glShadeModel( GL_SMOOTH );

	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );	// set the clear color to black
}

//
//	setupScene()
//
//	setup everything specific to our scene.  in this case,
//	position our camera
//
void setupScene() {
	// give the camera a scenic starting point.
	camIndex = 0; // freecam
	freePos.x = 6;
	freePos.y = 4;
	freePos.z = 3;
	freeTheta = -M_PI / 3.0f;	// theta
	freePhi = M_PI / 2.8f;	// phi
	recomputeOrientation();

	// some initial positions and scales to make cubes
	// for testing cameras
	// remove later once 3 heros are added
	heros.push_back(hans);
	heros.push_back(ire);
	heros.push_back(targa);

	hans->setPosition(glm::vec3(0,0,0));
	hans->setScale(glm::vec3(.1,.1,.1));
	
	ire->setPosition(glm::vec3(0,0,0));
	ire->setScale(glm::vec3(.4,.2,.4));
	ire->setDirection(glm::vec3(0,0,1));

	targa->setPosition(glm::vec3(-2,1,2));
	targa->setScale(glm::vec3(1,1,1));
	targa->setDirection(glm::vec3(0,0,1));
}

///*************************************************************************************
//
// Our main function

//
//	int main( int argc, char *argv[] )
//
//		Really you should know what this is by now.  We will make use of the parameters later
//
int main( int argc, char *argv[] ) {
	// TODO #01: make sure a control point CSV file was passed in.  Then read the points from file
	char* csv_file_location;
	if (argc < 2) {
		printf("CSV File argument required.");
		exit(1);
	}
	else {
		csv_file_location = argv[1];
	}
	
	loadControlPoints(csv_file_location);

	// GLFW sets up our OpenGL context so must be done first
	GLFWwindow *window = setupGLFW();	// initialize all of the GLFW specific information releated to OpenGL and our window
	setupOpenGL();										// initialize all of the OpenGL specific information
	setupScene();											// initialize objects in our scene

	fprintf(stdout, "[INFO]: /--------------------------------------------------------\\\n");
	fprintf(stdout, "[INFO]: | OpenGL Information                                     |\n");
	fprintf(stdout, "[INFO]: |--------------------------------------------------------|\n");
	fprintf(stdout, "[INFO]: |   OpenGL Version:  %35s |\n", glGetString(GL_VERSION));
	fprintf(stdout, "[INFO]: |   OpenGL Renderer: %35s |\n", glGetString(GL_RENDERER));
	fprintf(stdout, "[INFO]: |   OpenGL Vendor:   %35s |\n", glGetString(GL_VENDOR));
	fprintf(stdout, "[INFO]: \\--------------------------------------------------------/\n");

	//  This is our draw loop - all rendering is done here.  We use a loop to keep the window open
	//	until the user decides to close the window and quit the program.  Without a loop, the
	//	window will display once and then the program exits.
	loadCurvePoints();
	while( !glfwWindowShouldClose(window) ) {	// check if the window was instructed to be closed
    glDrawBuffer( GL_BACK );				// work with our back frame buffer
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );	// clear the current color contents and depth buffer in the window

		// update the projection matrix based on the window size
		// the GL_PROJECTION matrix governs properties of the view coordinates;
		// i.e. what gets seen - use a perspective projection that ranges
		// with a FOV of 45 degrees, for our current aspect ratio, and Z ranges from [0.001, 1000].
		glm::mat4 projMtx = glm::perspective( 45.0f, (GLfloat)windowWidth / (GLfloat)windowHeight, 0.001f, 1000.0f );
		glMatrixMode( GL_PROJECTION );	// change to the Projection matrix
		glLoadIdentity();				// set the matrix to be the identity
		glMultMatrixf( &projMtx[0][0] );// load our orthographic projection matrix into OpenGL's projection matrix state

		// Get the size of our framebuffer.  Ideally this should be the same dimensions as our window, but
		// when using a Retina display the actual window can be larger than the requested window.  Therefore
		// query what the actual size of the window we are rendering to is.
		GLint framebufferWidth, framebufferHeight;
		glfwGetFramebufferSize( window, &framebufferWidth, &framebufferHeight );

		// update the viewport - tell OpenGL we want to render to the whole window
		glViewport( 0, 0, framebufferWidth, framebufferHeight );

		glMatrixMode( GL_MODELVIEW );	// make the ModelView matrix current to be modified by any transformations
		glLoadIdentity();							// set the matrix to be the identity

		// set up our look at matrix to position our camera
		/*
		glm::mat4 viewMtx = glm::lookAt( camPos,
								 										 camPos + camDir,
																			glm::vec3(  0,  1,  0 ) );
		*/
		
		glm::mat4 viewMtx;
		switch(camIndex) {
			case 0:
				viewMtx = glm::lookAt(freePos, freePos + freeDir, glm::vec3(0,1,0));
				break;
			case 1:
				viewMtx = glm::lookAt(heros.at(arcballHero)->getPosition() - (arcballRadius * arcballDir), heros.at(arcballHero)->getPosition(), glm::vec3(0,1,0));
				break;
			case 2:
				viewMtx = glm::lookAt(fpPos, fpPos + fpDir, glm::vec3(0,1,0));
				break;
		}
		// multiply by the look at matrix - this is the same as our view martix
		glMultMatrixf( &viewMtx[0][0] );

		fpPos = heros.at(fpHero)->getPosition();
		fpDir = heros.at(fpHero)->getDirection();
		fpPos += fpDir;
		renderScene();					// draw everything to the window
		performWandererMovement();

		glfwSwapBuffers(window);// flush the OpenGL commands and make sure they get rendered!
		glfwPollEvents();				// check for any events and signal to redraw screen

		trackPointVal += 0.01f;
		if( trackPointVal > numSegments )
			trackPointVal = 0.0f;
	}

	glfwDestroyWindow( window );// clean up and close our window
	glfwTerminate();						// shut down GLFW to clean up our context

	return EXIT_SUCCESS;				// exit our program successfully!
}
