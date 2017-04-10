#include "Boid.h"

using namespace std;

Boid::Boid() {

}

Boid::Boid(Vec3f pos, Vec3f v) {
	this->position = pos;
	this->velocity = v;
}

Boid::Boid(Vec3f pos, Vec3f v, Mat4f o) {
	this->position = pos;
	this->velocity = v;
	this->orientation = o;
}

void Boid::orientate(Vec3f heading, Vec3f up){
	Vec3f right = up.crossProduct(heading);
}

Boid::~Boid() {}

void Boid::semiEuler(float dt, Vec3f h) {
	this->velocity = this->velocity + (h*dt);
	if((this->velocity.x()) > 30) {
		this->velocity.x() = 30;
	}	
	if((this->velocity.x()) < -30) {
		this->velocity.x() = -30;
	}	
	if((this->velocity.y()) > 30) {
		this->velocity.y() = 30;
	}
	if((this->velocity.y()) < -30) {
		this->velocity.y() = -30;
	}
	if((this->velocity.z()) > 30) {
		this->velocity.z() = 30;
	}
	if((this->velocity.z()) < -30) {
		this->velocity.z() = 30;
	}
	this->position = this->position + (this->velocity)*dt;
}
