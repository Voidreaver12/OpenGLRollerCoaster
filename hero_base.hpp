#ifndef __HERO_BASE_HPP__
#define __HERO_BASE_HPP__

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

class Hero {
	public:
		//Hero() {}
		void draw();
		virtual void drawHero() = 0;
		virtual void animateHero() = 0;
	
		// Getters/Setters
		glm::vec3 getPosition();
		glm::vec3 getDirection();
		
		void setDirection(glm::vec3 d);
		void rotate(float theta, glm::vec3 dir);
		void setScale(glm::vec3 s);
		void setPosition(glm::vec3 p);
	private:
		glm::vec3 direction;
		glm::vec3 position;
		glm::mat4 rotation;
		glm::vec3 scale;
};


void Hero::draw() {
	glm::mat4 transMtx = glm::translate( glm::mat4(), position );
	glMultMatrixf( &transMtx[0][0] );
	glm::mat4 rotateMtx = rotation;
	glMultMatrixf( &rotateMtx[0][0] );
	glm::mat4 scaleMtx = glm::scale( glm::mat4(), scale );
	glMultMatrixf( &scaleMtx[0][0] );
	drawHero();
	glMultMatrixf( &(glm::inverse( scaleMtx ))[0][0] );
	glMultMatrixf( &(glm::inverse( rotateMtx ))[0][0] );
	glMultMatrixf( &(glm::inverse( transMtx ))[0][0] );
}

glm::vec3 Hero::getPosition() {
	return position;
}

glm::vec3 Hero::getDirection() {
	return direction;
}

void Hero::setDirection(glm::vec3 d) {
	direction = d;
}

void Hero::setPosition(glm::vec3 p) {
	position = p;
}

void Hero::setScale(glm::vec3 s) {
	scale = s;
}

void Hero::rotate(float theta, glm::vec3 dir) {
	rotation = glm::rotate(glm::mat4(), (180.0f/3.1415f) * theta, dir);
}
#endif // __HERO_BASE_HPP__
