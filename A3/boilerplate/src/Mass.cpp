#include "Mass.h"

Mass::Mass() {

}

Mass::Mass(Vec3f f, Vec3f pos, Vec3f v, float m){
	this->force = f;
	this->position = pos;
	this->velocity = v;
	this->mass = m;
}

Mass::~Mass() {}

void Mass::semiEuler() {
	this->velocity = this->velocity + (this->mass*this->force);
	this->position = this->position + this->velocity;
}

float Mass::resolveForce(float dT){
	return 0;
}


