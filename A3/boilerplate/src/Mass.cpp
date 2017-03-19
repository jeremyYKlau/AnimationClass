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

void Mass::resolveForce(float dT){
	this->force = this->force + (this->mass * Vec3f(0.0, 9.81/10, 0.0));
}


