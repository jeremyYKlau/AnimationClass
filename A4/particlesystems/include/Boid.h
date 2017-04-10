// Set up the boid structure with semiEuler for actual position drawing

#ifndef BOID_H
#define BOID_H

#include <iostream>

#include "Vec3f.h"
#include "Mat4f.h"
#include "Camera.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

class Boid {
public:;
	Vec3f position;
	Vec3f velocity;
	Mat4f orientation;
	
	Boid();
	~Boid();
	Boid(Vec3f Pos, Vec3f V);
	Boid(Vec3f Pos, Vec3f V, Mat4f orientation);
	
	void orientate(Vec3f heading, Vec3f up);
	void semiEuler(float dt, Vec3f h);
private:

};

#endif /* Boid */
