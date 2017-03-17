#include "Spring.h"

Spring::Spring() {

}

Spring::Spring(float s, float d, Mass &a, Mass &b){
	this->stiffness = s;
	this->damping = d;
	this->a = &a;
	this->b = &b;
}

Spring::~Spring() {}

Vec3f Spring::springForce(){
	Vec3f fs = this->stiffness*(sqrt(this->a->position*this->a->position + this->b->position*this->b->position))*(this->a->position-this->b->position);
	return fs;
}

float Spring::applyForce(){
	return 0;
}
