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

class Hero_1 : public Hero {
	public:
		void drawHero();
		void animateHero();
};

void Hero_1::drawHero() {
	glColor3f(1,0,0);
	CSCI441::drawSolidCube(1);
}

void Hero_1::animateHero() {
}
#endif // __HERO_1_HPP__
