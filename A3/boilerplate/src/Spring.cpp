#include "Spring.h"

Spring::Spring() {

}

Spring::Spring(float s, float d, Mass *a, Mass *b){
	this->stiffness = s;
	this->damping = d;
	this->a = a;
	this->b = b;
}

Spring::~Spring() {}

Vec3f Spring::springForce(){
	Vec3f distance = this->a->position - this->b->position;
	float normalizedLength = distance.length();
	//std::cout << "normalized length " << normalizedLength << std::endl;
	Vec3f fs = (-1*this->stiffness)*(normalizedLength - (normalizedLength/2))*(distance/normalizedLength);
	return fs;
}

void Spring::applyForce(Vec3f springF){
	this->a->force += springF;
	this->b->force -= springF;
}
