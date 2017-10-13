#ifndef __HERO_1_HPP__
#define __HERO_1_HPP__

#ifdef __APPLE__
	#include <OpenGL/gl.h>
#else
	#include <GL/gl.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <math.h>
#include <time.H>
#include <CSCI441/objects.hpp>

#include "hero_base.hpp"

class Ire : public Hero {
	public:
		void drawHero();
		void animateHero();
	private:
		float vehicleTurretAngle = 0.0;
		float vehicleBigPylonAngle = 0.0;
		float vehicleSmallPylonAngle = 0.0;
		
		void drawTurret(float width, float length);
		void drawTurrets(float spacing, float width, float length, float turretAngle);
		void drawPylon(float height, float width, float bigPylonAngle, float smallPylonAngle);
		void drawBase(float baseSideLength);
		void drawVehicle(float baseSideLength);
};

void Ire::drawHero() {
	drawVehicle( 1.0 );
}

void Ire::animateHero() {
	// if (vehicleMoveSign != 0.0f)
	vehicleBigPylonAngle += 0.1;
	vehicleSmallPylonAngle -= 0.1;
}

void Ire::drawTurret(float width, float length) {
	glColor3f(0.8, 0.0, 0.0);
	CSCI441::drawSolidDisk( 0.0f, width, 15, 15);
	
	glm::mat4 rotateMtx = glm::rotate(glm::mat4(), (float) M_PI / 2, glm::vec3(0.0, 0.0, 1.0));
	glMultMatrixf(&rotateMtx[0][0]);
	
	glColor3f(1.0, 1.0, 1.0);
	CSCI441::drawSolidCylinder( width / 4, width / 4, length, 1, 15);
	
	glMultMatrixf(&(glm::inverse(rotateMtx))[0][0]);
}

void Ire::drawTurrets(float spacing, float width, float length, float turretAngle) {
	// draw left
	glm::mat4 transMtx = glm::translate(glm::mat4(), glm::vec3(0, 0, spacing / 2));
	glMultMatrixf(&transMtx[0][0]);
	glm::mat4 rotateMtx = glm::rotate(glm::mat4(), turretAngle, glm::vec3(0.0, 0.0, 1.0));
	glMultMatrixf(&rotateMtx[0][0]);
	
	drawTurret(width, length);
	
	glMultMatrixf(&(glm::inverse(rotateMtx))[0][0]);
	glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
	
	// draw right
	transMtx = glm::translate(glm::mat4(), glm::vec3(0, 0, -spacing / 2));
	glMultMatrixf(&transMtx[0][0]);
	glMultMatrixf(&rotateMtx[0][0]);
	
	drawTurret(width, length);
	
	glMultMatrixf(&(glm::inverse(rotateMtx))[0][0]);
	glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
}

void Ire::drawPylon(float height, float width, float bigPylonAngle, float smallPylonAngle) {
	float smallPylonHeight = height * 1.25;
	
	// draw large pylon
	glm::mat4 rotateMtx = glm::rotate(glm::mat4(), (float) M_PI / 2, glm::vec3(1.0, 0.0, 0.0));
	glMultMatrixf(&rotateMtx[0][0]);
	
	glm::mat4 transMtx = glm::translate(glm::mat4(), glm::vec3(0, -height / 2, 0));
	glMultMatrixf(&transMtx[0][0]);
	glm::mat4 spinMtx = glm::rotate(glm::mat4(), bigPylonAngle, glm::vec3(0.0, 1.0, 0.0));
	glMultMatrixf(&spinMtx[0][0]);
	
	glColor3f(0.5, 0.5, 0.5);
	CSCI441::drawSolidCylinder(width, width, height, 1, 10);
	
	glMultMatrixf(&(glm::inverse(spinMtx))[0][0]);
	glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
	
	// draw small pylon
	transMtx = glm::translate(glm::mat4(), glm::vec3(0, -smallPylonHeight / 2, 0));
	glMultMatrixf(&transMtx[0][0]);
	spinMtx = glm::rotate(glm::mat4(), smallPylonAngle, glm::vec3(0.0, 1.0, 0.0));
	glMultMatrixf(&spinMtx[0][0]);
	
	glColor3f(0.8, 0.0, 0.0);
	CSCI441::drawSolidCylinder(width / 4, width / 4, smallPylonHeight, 1, 10);
	
	glMultMatrixf(&(glm::inverse(spinMtx))[0][0]);
	glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
	
	glMultMatrixf(&(glm::inverse(rotateMtx))[0][0]);
}

void Ire::drawBase(float baseSideLength) {
	glColor3f(0.3, 0.3, 0.3);
	CSCI441::drawSolidCube( baseSideLength );
	glColor3f(0.8, 0.0, 0.0);
	CSCI441::drawSolidTorus( baseSideLength / 2, baseSideLength, 3, 3 );
	
	// draw rotating inner section
	drawPylon(baseSideLength * 1.25, baseSideLength / 2, vehicleBigPylonAngle, vehicleSmallPylonAngle);
}

void Ire::drawVehicle(float baseSideLength) {
	// draw  base
	drawBase(baseSideLength);
	
	// draw side turrets
	drawTurrets( baseSideLength * 1.25, baseSideLength / 2, baseSideLength, vehicleTurretAngle);
	
	// CSCI441::drawSolidTeapot( baseSideLength );
}

#endif // __HERO_1_HPP__
