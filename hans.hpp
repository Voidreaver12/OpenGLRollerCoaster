#ifndef __Hans_HPP__
#define __Hans_HPP__

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

class Hans : public Hero {
	public:
		void drawHero();
		void animateHero();
	private:
		int n = 0;
		double insideRocketColor_0 = 1.0;
		double insideRocketColor_1 = .271;
		double insideRocketColor_2 = 0.0;

		double outsideRocketColor_0 = .863;
		double outsideRocketColor_1 = .078;
		double outsideRocketColor_2 = 0.235;
		
		void drawRocket();
		void drawCabin();
		void drawMover();
		
};
void Hans::drawHero() {
	glm::mat4 rotateMtx = glm::rotate(glm::mat4(), 3.14159f / 2, glm::vec3(0.0f, 0.0f, 1.0f));
	glMultMatrixf( &rotateMtx[0][0] );
	drawMover();
	glMultMatrixf( &(glm::inverse( rotateMtx ))[0][0] );
}

void Hans::animateHero() {
	if(n % 5 == 0) {
		double temp = insideRocketColor_0;
		insideRocketColor_0 = outsideRocketColor_0;
		outsideRocketColor_0 = temp;
		
		temp = insideRocketColor_1;
		insideRocketColor_1 = outsideRocketColor_1;
		outsideRocketColor_1 = temp;
		
		temp = insideRocketColor_2;
		insideRocketColor_2 = outsideRocketColor_2;
		outsideRocketColor_2 = temp;
	}
	n++;
}
void Hans::drawRocket() {
	glColor3f(outsideRocketColor_0, outsideRocketColor_1, outsideRocketColor_2);
	CSCI441::drawWireCylinder( 1, .1, 3, 50, 10 );
	glColor3f(insideRocketColor_0, insideRocketColor_1, insideRocketColor_2);
	CSCI441::drawWireCylinder( .5, .05, 3, 50, 10 );
	glColor3f(0, 0, 0);
	CSCI441::drawSolidCylinder( 1.1, 1.0, .5, 50, 10 );
}
void Hans::drawCabin() {
	glColor3f(0.999, 0.999, 0.999);
	CSCI441::drawSolidCylinder( 1, 1, 5, 50, 10 );
	glColor3f(1.000, 0.843, 0.000);
	glm::mat4 transMtx = glm::translate(glm::mat4(), glm::vec3(0, 5, 0));
	glMultMatrixf( &transMtx[0][0] );
	CSCI441::drawSolidCylinder( 1, .1, 5, 50, 10 );
	glMultMatrixf( &( glm::inverse( transMtx ) )[0][0] );
}
void Hans::drawMover() {
	drawCabin();
	glm::mat4 transMtx = glm::translate(glm::mat4(), glm::vec3(0, 0, 0));
	glMultMatrixf( &transMtx[0][0] );
	glm::mat4 rotMtx = glm::rotate(glm::mat4(), (2 * 1.57f), glm::vec3(1, 0, 0));
	glMultMatrixf( &rotMtx[0][0] );
	drawRocket();
	glMultMatrixf( &( glm::inverse( rotMtx ) )[0][0] );
	glMultMatrixf( &( glm::inverse( transMtx ) )[0][0] );
}
#endif // __Hans_HPP__