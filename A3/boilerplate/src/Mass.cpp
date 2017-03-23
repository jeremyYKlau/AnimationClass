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
	if (this->position.y() <= -3){
		this->position.y() = -3;
		this->velocity.y() = 0;
	}
}

void Mass::resolveForce(float dT, float d){
	//Vec3f accel = (this->force / this->mass);
	this->force += Vec3f(0, -9.81, 0)*mass;
	//this->force += mass*accel;
	this->force += (-d*this->velocity);
	semiEuler(dT);
}


