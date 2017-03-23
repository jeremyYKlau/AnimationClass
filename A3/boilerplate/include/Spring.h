/*
 * Spring class that will have functions within it 
 */

#ifndef Spring_H
#define Spring_H

#include <iostream>

#include "Vec3f.h"
#include "Mat4f.h"
#include "Camera.h"
#include "Mass.h"

#include "glad/glad.h"
#include <GLFW/glfw3.h>

using namespace std;

class Spring {
public:
	Spring();
	~Spring();
	
//structure for the spring
	Mass *a; 
	Mass *b;
	float stiffness; //k
	float damping; //-b*v

	Spring(float s, float d, Mass *a, Mass *b);

	Vec3f springForce();
	void applyForce(Vec3f springF);
};
#endif // Spring_H
