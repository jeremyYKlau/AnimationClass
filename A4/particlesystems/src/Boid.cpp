#include "Boid.h"

using namespace std;

Boid::Boid() {

}

Boid::Boid(Vec3f pos, Vec3f v) {
	this->position = pos;
	this->velocity = v;
}

Boid::~Boid() {}

void Boid::semiEuler(float dt, Vec3f h) {
	this->velocity = this->velocity + (h*dt);
	this->position = this->position + (this->velocity)*dt;
}
