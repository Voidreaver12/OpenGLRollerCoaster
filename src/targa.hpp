#ifndef __TARGA_HPP__
#define __TARGA_HPP__

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

class Targa : public Hero {
	public:
		void drawHero();
		void animateHero();
	private:
		float armTheta = 0.0f;
		bool mascotArmSwing = true;
		void drawTargaEye();
		void drawTargaEar();
		void drawTargaEyes();
		void drawTargaHead();
		void drawTargaLimb(int limb);
};

void Targa::drawHero() {
	glColor3f(0.95, 0.83, 0.83);
	// Main body
	glm::mat4 scaleMtx = glm::scale(glm::mat4(), glm::vec3(0.5, 0.6, 0.3));
	glMultMatrixf(&scaleMtx[0][0]);
	CSCI441::drawSolidSphere(1.0, 20, 20);
	glMultMatrixf(&(glm::inverse(scaleMtx))[0][0]);
	// Head
	glm::mat4 transMtx = glm::translate(glm::mat4(), glm::vec3(0.0, 0.8, 0.0));
	glMultMatrixf(&transMtx[0][0]);
	drawTargaHead();
	glMultMatrixf(&(glm::inverse(transMtx))[0][0]);

	glColor3f(0.95, 0.83, 0.83);
	// Right leg
	// move outwards
	glm::mat4 transMtx0 = glm::translate(glm::mat4(), glm::vec3(0.3, 0.0, 0.0));
	glMultMatrixf(&transMtx0[0][0]);
	// rotate leg
	glm::mat4 rotMtx = glm::rotate(glm::mat4(), (3.1415f * (-3.0f * armTheta) / 180.0f), glm::vec3(1, 0, 0));
	glMultMatrixf(&rotMtx[0][0]);
	// move away from center
	transMtx = glm::translate(glm::mat4(), glm::vec3(0.0, -0.5, 0.0));
	glMultMatrixf(&transMtx[0][0]);
	drawTargaLimb(1);
	glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
	glMultMatrixf(&(glm::inverse(rotMtx))[0][0]);
	glMultMatrixf(&(glm::inverse(transMtx0))[0][0]);
	// Left leg
	// move outwards
	transMtx0 = glm::translate(glm::mat4(), glm::vec3(-0.3, 0.0, 0.0));
	glMultMatrixf(&transMtx0[0][0]);
	// rotate leg
	rotMtx = glm::rotate(glm::mat4(), (3.1415f * (3.0f * armTheta) / 180.0f), glm::vec3(1, 0, 0));
	glMultMatrixf(&rotMtx[0][0]);
	// move away from center
	transMtx = glm::translate(glm::mat4(), glm::vec3(0.0, -0.5, 0.0));
	glMultMatrixf(&transMtx[0][0]);
	drawTargaLimb(1);
	glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
	glMultMatrixf(&(glm::inverse(rotMtx))[0][0]);
	glMultMatrixf(&(glm::inverse(transMtx0))[0][0]);

	// Move arms up
	transMtx0 = glm::translate(glm::mat4(), glm::vec3(0.0, 0.2, 0.0));
	glMultMatrixf(&transMtx0[0][0]);
	// Right arm
	// rotate arm
	rotMtx = glm::rotate(glm::mat4(), (3.1415f * (80.0f + armTheta) / 180.0f), glm::vec3(0, 0, 1));
	glMultMatrixf(&rotMtx[0][0]);
	// move arm out of center
	transMtx = glm::translate(glm::mat4(), glm::vec3(0.0, 0.65, 0.0));
	glMultMatrixf(&transMtx[0][0]);
	drawTargaLimb(2);
	glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
	glMultMatrixf(&(glm::inverse(rotMtx))[0][0]);

	// Left arm
	// rotate arm
	rotMtx = glm::rotate(glm::mat4(), (3.1415f * -(80.0f + armTheta) / 180.0f), glm::vec3(0, 0, 1));
	glMultMatrixf(&rotMtx[0][0]);
	// move arm out of center
	transMtx = glm::translate(glm::mat4(), glm::vec3(0.0, 0.65, 0.0));
	glMultMatrixf(&transMtx[0][0]);
	drawTargaLimb(2);
	glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
	glMultMatrixf(&(glm::inverse(rotMtx))[0][0]);  
	// undo moving arms up
	glMultMatrixf(&(glm::inverse(transMtx0))[0][0]);
}

void Targa::animateHero() {
	// Swing arms and legs
	if (mascotArmSwing) {
		armTheta += 0.5f;
		if (armTheta >= 10.0f) {
				mascotArmSwing = false;
		}
	} else if (!mascotArmSwing) {
		armTheta -= 0.5f;
		if (armTheta <= -10.0f) {
				mascotArmSwing = true;
		}
	}
}

void Targa::drawTargaEye() {
	glm::mat4 scaleMtx = glm::scale(glm::mat4(), glm::vec3(0.2, 0.2, 0.2));
	glMultMatrixf(&scaleMtx[0][0]);
	// Inner eye (yellow)
	glDisable(GL_LIGHTING);
	glColor3f(0.9, 0.9, 0.1);
	glm::mat4 transMtx = glm::translate(glm::mat4(), glm::vec3(0.0, 0.0, 0.1));
	glMultMatrixf(&transMtx[0][0]);
	CSCI441::drawSolidDisk(0.0, 1.0, 10, 10);
	glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
	glEnable(GL_LIGHTING);
	// Outer eye (black)
	glColor3f(0.1, 0.1, 0.1);
	CSCI441::drawSolidTorus(0.3, 1.0, 15, 15);

	glMultMatrixf(&(glm::inverse(scaleMtx))[0][0]);
}

void Targa::drawTargaEar() {
	glm::mat4 scaleMtx = glm::scale(glm::mat4(), glm::vec3(0.2, 0.2, 0.2));
	glMultMatrixf(&scaleMtx[0][0]);
	// Inner ear (pink)
	glDisable(GL_LIGHTING);
	glColor3f(1.0, 0.6, 0.6);
	CSCI441::drawSolidDisk(0.0, 1.0, 10, 10);
	glEnable(GL_LIGHTING);
	// Outer ear (brownish pink)
	glColor3f(0.95, 0.83, 0.83);
	CSCI441::drawSolidTorus(0.3, 1.0, 15, 15);
	glMultMatrixf(&(glm::inverse(scaleMtx))[0][0]);
}

void Targa::drawTargaEyes() {
	// eyes
	glm::mat4 rotMtx1 = glm::rotate(glm::mat4(), (3.1415f * (30.0f) / 180.0f), glm::vec3(0, 1, 0));
	glMultMatrixf(&rotMtx1[0][0]);
	glm::mat4 rotMtx2 = glm::rotate(glm::mat4(), (3.1415f * (-30.0f) / 180.0f), glm::vec3(1, 0, 0));
	glMultMatrixf(&rotMtx2[0][0]);

	glm::mat4 transMtx = glm::translate(glm::mat4(), glm::vec3(0.0, 0.0, 1.0));
	glMultMatrixf(&transMtx[0][0]);
	drawTargaEye();
	glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
	
	glMultMatrixf(&(glm::inverse(rotMtx2))[0][0]);
	glMultMatrixf(&(glm::inverse(rotMtx1))[0][0]);

	// other eye
	rotMtx1 = glm::rotate(glm::mat4(), (3.1415f * (-30.0f) / 180.0f), glm::vec3(0, 1, 0));
	glMultMatrixf(&rotMtx1[0][0]);
	rotMtx2 = glm::rotate(glm::mat4(), (3.1415f * (-30.0f) / 180.0f), glm::vec3(1, 0, 0));
	glMultMatrixf(&rotMtx2[0][0]);

	transMtx = glm::translate(glm::mat4(), glm::vec3(0.0, 0.0, 1.0));
	glMultMatrixf(&transMtx[0][0]);
	drawTargaEye();
	glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
	
	glMultMatrixf(&(glm::inverse(rotMtx2))[0][0]);
	glMultMatrixf(&(glm::inverse(rotMtx1))[0][0]);

}

void Targa::drawTargaHead() {
	//Head
	glm::mat4 scaleMtx = glm::scale(glm::mat4(), glm::vec3(0.5, 0.5, 0.4));
	glMultMatrixf(&scaleMtx[0][0]);
	CSCI441::drawSolidSphere(1.0, 20, 20);
	drawTargaEyes();
	glMultMatrixf(&(glm::inverse(scaleMtx))[0][0]);
	// Left ear
	glm::mat4 transMtx = glm::translate(glm::mat4(), glm::vec3(0.4, 0.4, 0.0));
	glMultMatrixf(&transMtx[0][0]);
	drawTargaEar();
	glMultMatrixf(&(glm::inverse(transMtx))[0][0]);
	// Right ear
	transMtx = glm::translate(glm::mat4(), glm::vec3(-0.4, 0.4, 0.0));
	glMultMatrixf(&transMtx[0][0]);
	drawTargaEar();
	glMultMatrixf(&(glm::inverse(transMtx))[0][0]);

}

void Targa::drawTargaLimb(int limb) {
	if (limb == 1) {    // leg
			glm::mat4 scaleMtx = glm::scale(glm::mat4(), glm::vec3(0.2, 0.4, 0.2));
			glMultMatrixf(&scaleMtx[0][0]);
			CSCI441::drawSolidSphere(1.0, 20, 20);
			glMultMatrixf(&(glm::inverse(scaleMtx))[0][0]);
	}
	if (limb == 2) {    // arm
			glm::mat4 scaleMtx = glm::scale(glm::mat4(), glm::vec3(0.15, 0.5, 0.15));
			glMultMatrixf(&scaleMtx[0][0]);
			CSCI441::drawSolidSphere(1.0, 20, 20);
			glMultMatrixf(&(glm::inverse(scaleMtx))[0][0]);
	}
}


#endif // __TARGA_HPP__
