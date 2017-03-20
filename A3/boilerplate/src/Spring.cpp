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
	Vec3f length = this->a->position - this->b->position;
	float normalizedLength = sqrt((length.x()*length.x())+(length.y()*length.y())+(length.z()*length.z())); 
	Vec3f fs = (-1*this->stiffness)*(normalizedLength)*(length/normalizedLength);
	return fs;
}

Vec3f Spring::applyForce(){
	
}
