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

void Mass::semiEuler(float dt) {
	this->velocity = this->velocity + ((this->force/this->mass))*dt;
	this->position = this->position + (this->velocity)*dt;
}
void Mass::dampingForce(float d){
	this->force += -d*this->velocity;
}

void Mass::resolveForce(float dT){
	Vec3f accel = (this->force / this->mass);
	accel += Vec3f(0, -9.81, 0);
	this->force += mass*accel;
	semiEuler(dT);
}


