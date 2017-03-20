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

void Mass::resolveForce(float dT){
	prevVelocity = this->velocity;
	this->force = this->force + (this->mass * Vec3f(0.0, 9.81/10, 0.0));
}


