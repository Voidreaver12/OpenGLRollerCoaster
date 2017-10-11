#ifndef __HERO_2_HPP__
#define __HERO_2_HPP__

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

class Hero_2 : public Hero {
	public:
		void drawHero();
		void animateHero();
};

void Hero_2::drawHero() {
	glColor3f(0,1,0);
	CSCI441::drawSolidCube(1);
}

void Hero_2::animateHero() {
}

#endif // __HERO_2_HPP__
