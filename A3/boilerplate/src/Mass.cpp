#include "Mass.h"

Mass::Mass() {

}

Mass::Mass(float f, Vec3f pos, float v, float m){
	this->force = f;
	this->position = pos;
	this->velocity = v;
	this->mass = m;
}

Mass::~Mass() {}

float Mass::resolveForce(float dT){
	return 0;
}


