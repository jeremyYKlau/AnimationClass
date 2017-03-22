/*
 * Mass class that will have functions within it 
 */

#ifndef Mass_H
#define Mass_H

#include <iostream>

#include "Vec3f.h"
#include "Mat4f.h"
#include "Camera.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

class Mass {
public:
	Vec3f force;
	Vec3f position;
	Vec3f velocity;
	float mass;
	
	Vec3f prevVelocity;
	
	Mass();
	~Mass();

	Mass(Vec3f f, Vec3f pos, Vec3f v, float m);
	
	void semiEuler(float dt);
	void dampingForce(float d);
	void resolveForce(float dT);
};
#endif // Mass_H
