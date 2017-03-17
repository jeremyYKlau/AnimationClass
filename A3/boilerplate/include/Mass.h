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
	float force;
	Vec3f position;
	float velocity;
	float mass;
	
	explicit Mass();
	~Mass();

	Mass(float f, Vec3f pos, float v, float m);

float resolveForce(float dT);
};
#endif // MAT4F_H
