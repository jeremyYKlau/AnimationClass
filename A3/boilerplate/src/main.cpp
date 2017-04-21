/**
 * Author:	Andrew Robert Owens
 * Email:	arowens [at] ucalgary.ca
 * Date:	January, 2017
 * Course:	CPSC 587/687 Fundamental of Computer Animation
 * Organization: University of Calgary
 *
 * Copyright (c) 2017 - Please give credit to the author.
 *
 * File:	main.cpp
 *
 * Summary:
 *
 * This is a (very) basic program to
 * 1) load shaders from external files, and make a shader program
 * 2) make Vertex Array Object and Vertex Buffer Object for the quad
 *
 * take a look at the following sites for further readings:
 * opengl-tutorial.org -> The first triangle (New OpenGL, great start)
 * antongerdelan.net -> shaders pipeline explained
 * ogldev.atspace.co.uk -> good resource
 */

#include <iostream>
#include <cmath>
#include <chrono>
#include <limits>

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "ShaderTools.h"
#include "Vec3f.h"
#include "Mat4f.h"
#include "OpenGLMatrixTools.h"
#include "Camera.h"
#include "Mass.h"
#include "Spring.h"

using namespace std;

//==================== GLOBAL VARIABLES ====================//
/*	Put here for simplicity. Feel free to restructure into
*	appropriate classes or abstractions.
*/

// Drawing Program
GLuint basicProgramID;

// Data needed for Quad
GLuint vaoID;
GLuint vertBufferID;
Mat4f M;

// Data needed for Line 
GLuint line_vaoID;
GLuint line_vertBufferID;
Mat4f line_M;

// Only one camera so only one veiw and perspective matrix are needed.
Mat4f V;
Mat4f P;

// Only one thing is rendered at a time, so only need one MVP
// When drawing different objects, update M and MVP = M * V * P
Mat4f MVP;

// Camera and veiwing Stuff
Camera camera;
int g_moveUpDown = 0;
int g_moveLeftRight = 0;
int g_moveBackForward = 0;
int g_rotateLeftRight = 0;
int g_rotateUpDown = 0;
int g_rotateRoll = 0;
float g_rotationSpeed = 0.015625;
float g_panningSpeed = 0.25;
bool g_cursorLocked;
float g_cursorX, g_cursorY;

bool g_play = false;

int WIN_WIDTH = 800, WIN_HEIGHT = 600;
int FB_WIDTH = 800, FB_HEIGHT = 600;
float WIN_FOV = 60;
float WIN_NEAR = 0.01;
float WIN_FAR = 1000;

//==================== FUNCTION DECLARATIONS ====================//
void displayFunc();
void resizeFunc();
void init();
void generateIDs();
void deleteIDs();
void setupVAO();
void loadQuadGeometryToGPU();
void reloadProjectionMatrix();
void loadModelViewMatrix();
void setupModelViewProjectionTransform();

void windowSetSizeFunc();
void windowKeyFunc(GLFWwindow *window, int key, int scancode, int action,
                   int mods);
void windowMouseMotionFunc(GLFWwindow *window, double x, double y);
void windowSetSizeFunc(GLFWwindow *window, int width, int height);
void windowSetFramebufferSizeFunc(GLFWwindow *window, int width, int height);
void windowMouseButtonFunc(GLFWwindow *window, int button, int action,
                           int mods);
void windowMouseMotionFunc(GLFWwindow *window, double x, double y);
void windowKeyFunc(GLFWwindow *window, int key, int scancode, int action,
                   int mods);
void animateQuad(float t);
void moveCamera();
void reloadMVPUniform();
void reloadColorUniform(float r, float g, float b);
std::string GL_ERROR();
int main(int, char **);

//global storage of all masses and springs
std::vector<Mass> masses;
std::vector<Spring> springs;

bool cubemode = false;
float rest = 2.0;

//==================== FUNCTION DEFINITIONS ====================//


void solveMassSpring(float dt, float xO){
	for(unsigned int s = 0; s < springs.size(); s++){
		springs[s].applyForce(springs[s].springForce(xO));
		//cout << "springs force " << springs[s].springForce() << endl;
	}
	for(unsigned int m = 0; m < masses.size(); m++){
		if (masses[m].fixed != true){
		masses[m].resolveForce(dt, springs[m].damping);
		masses[m].force = Vec3f(0,0,0);
		}
	}
}          

void solveMassSpringCube(float dt, float xO){
	for(unsigned int s = 0; s < springs.size(); s++){
		springs[s].applyForce(springs[s].springForce(xO));
		//cout << "springs force " << springs[s].springForce() << endl;
	}
	for(unsigned int m = 0; m < masses.size(); m++){
		if (masses[m].fixed != true){
		masses[m].resolveForceCube(dt, springs[m].damping);
		masses[m].force = Vec3f(0,0,0);
		}
	}
}          
                                                                                                                         
void createSpringMass() {
	  //for the spring mass initialize the masses and springs with values
  Mass m1 = Mass(Vec3f(0, 0, 0), Vec3f(0, 1, 0), Vec3f(0, 1, 0), 0.5, true);
  Mass m2 = Mass(Vec3f(0, 0, 0), Vec3f(0, -1, 0), Vec3f(0, 1, 0), 1, false);

    
  //adds the masses and springs into a vector for storage
  masses.push_back(m1);
  masses.push_back(m2);
  
  Spring s1 = Spring(5, 0.2, &masses[0], &masses[1]);
  springs.push_back(s1);
}

void createPendulum(){
	  //initializing the pendulum masses and springs
  Mass m1 = Mass(Vec3f(0, 0, 0), Vec3f(0, 1, 0), Vec3f(0, 0, 0), 1, true);
  Mass m2 = Mass(Vec3f(0, 0, 0), Vec3f(0.3, 0.9, 0), Vec3f(0, 0, 0), 1, false);
  Mass m3 = Mass(Vec3f(0, 0, 0), Vec3f(0.6, 0.8, 0), Vec3f(0, 0, 0), 1, false);
  Mass m4 = Mass(Vec3f(0, 0, 0), Vec3f(0.9, 0.7, 0), Vec3f(0, 0, 0), 1, false);
  Mass m5 = Mass(Vec3f(0, 0, 0), Vec3f(1.2, 1, 0), Vec3f(0, 0, 0), 1, false);

  masses.push_back(m1);
  masses.push_back(m2);
  masses.push_back(m3);
  masses.push_back(m4);
  masses.push_back(m5);
  
  Spring s1 = Spring(50, 0.5, &masses[0], &masses[1]);
  Spring s2 = Spring(50, 0.5, &masses[1], &masses[2]);
  Spring s3 = Spring(50, 0.5, &masses[2], &masses[3]);
  Spring s4 = Spring(50, 0.5, &masses[3], &masses[4]);
   

  springs.push_back(s1);
  springs.push_back(s2);
  springs.push_back(s3);
  springs.push_back(s4);
}
//to create the cloth using a double for loop
void createCloth(int u, int v) {
	for (unsigned int i = 5; i>0; i--){
		for (unsigned int j = 5; j>0; j--){
			if(j==5){
				Mass m = Mass(Vec3f(0, 0, 0), Vec3f(i, j, 0), Vec3f(0, 0, 0), 1 , true);
				masses.push_back(m);
			}
			else{
				Mass m = Mass(Vec3f(0, 0, 0), Vec3f(i, j, 0), Vec3f(0, 0, 0), 1 , false);
				masses.push_back(m);
			}
		}
	}
	cout << "number of masses " << masses.size() << endl;
}

//creates the masses for the cube including the middle mass at the end of the list
void createCube(int u, int v, int w) {
	for (unsigned int i = 0; i<=3; i++){
		for (unsigned int j = 0; j<=3; j++){
			for (unsigned int k = 0; k<=3; k++) {
				Mass m = Mass(Vec3f(0, 0, 0), Vec3f(i, j, k), Vec3f(0, 0, 0), 0.3 , false);
				masses.push_back(m);
			}
		}
	}
	Mass centerM = Mass(Vec3f(0,1,0), Vec3f(1,1,1), Vec3f(0,1,0), 1, false);
	masses.push_back(centerM);
}

//for cloth springs based off distance
void distanceCloth(std::vector<Mass> m){
	for (unsigned int i = 0; i<=m.size(); i++){
		for (unsigned int j = 0; j<=m.size(); j++){
			float distance = (m[i].position-m[j].position).length();
			//sqrt2*1.05 to make sure it still draws diagonal springs
			if ((distance <= (sqrt(2)))&&(distance>0.1)){
				//this if is to create the cross springs that need different coefficients at spring set up
				if((distance <= (sqrt(2)))&&(distance>1.0)) {
					Spring s = Spring(5, 0.5, &masses[i], &masses[j]);
					springs.push_back(s);
				}
				else{
					Spring s = Spring(5, 1, &masses[i], &masses[j]);
					springs.push_back(s);
				}
			}
		}
	}
}

//for cube springs
void distanceCube(std::vector<Mass> m) {
	for (unsigned int i = 0; i<m.size(); i++){
		for (unsigned int j = 0; j<m.size(); j++){
			float distance = (m[i].position-m[j].position).length();
			if ((distance <= (sqrt(3)))&&(distance>0.1)){
				//same as cloth cross springs need different values
				if((distance <= (sqrt(3)))&&(distance>1.01)) {
					Spring s = Spring(10, 1, &masses[i], &masses[j]);
					springs.push_back(s);
					//cout << "Intermediate " << s.a->position << " and " << s.b->position << endl;
				}
				else{
					Spring s = Spring(10, 1, &masses[i], &masses[j]);
					springs.push_back(s);
					//cout << "Intermediate " << s.a->position << " and " << s.b->position << endl;
				}
			Spring sC = Spring(0.5, 1, &masses[i], &masses[masses.size()-1]);
			springs.push_back(sC);
			}
		}
	}
}

void displayFunc() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glPointSize(15);
  // Use our shader
  glUseProgram(basicProgramID);

  // ===== DRAW QUAD ====== //
  MVP = P * V * M;
  reloadMVPUniform();
  reloadColorUniform(1, 0, 1);

  // Use VAO that holds buffer bindings
  // and attribute config of buffers
  glBindVertexArray(vaoID);
  // Draw Quads, start at vertex 0, draw 4 of them (for a quad)
  //glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  // Draw masses as single points for now
  glDrawArrays(GL_POINTS, 0, masses.size());
  // ==== DRAW LINE ===== //
  MVP = P * V * line_M;
  reloadMVPUniform();

  reloadColorUniform(0, 1, 1);

  // Use VAO that holds buffer bindings
  // and attribute config of buffers
  glBindVertexArray(line_vaoID);
  // Draw lines
  glDrawArrays(GL_LINES, 0, springs.size()*2);
  
}

//for jello cube and cloth
void drawQuad(Vec3f pos1, Vec3f pos2, Vec3f pos3, Vec3f pos4) {
  // Just basic layout of floats, for a quad
  // 4 vertices from mass positions
  std::vector<Vec3f> vertice;
  vertice.push_back(pos1);
  vertice.push_back(pos2);
  vertice.push_back(pos3);
  vertice.push_back(pos4);
  
  glBindBuffer(GL_ARRAY_BUFFER, vertBufferID);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(Vec3f) * 4, // byte size of Vec3f, 4 of them
               vertice.data(),      // pointer (Vec3f*) to contents of verts
               GL_STATIC_DRAW);   // Usage pattern of GPU buffer
}

//takes a mass and draws it's position
void drawMass(std::vector<Mass> ma) {
  std::vector<Vec3f> pos;
  for (unsigned int m = 0; m < ma.size(); m++){
	pos.push_back(ma[m].position);  
  }
  glBindBuffer(GL_ARRAY_BUFFER, vertBufferID);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(Vec3f) * ma.size(), // byte size of Vec3f, 4 of them
               pos.data(),      // pointer (Vec3f*) to contents of verts
               GL_STATIC_DRAW);   // Usage pattern of GPU buffer
}

//drawSpring takes in a spring and draws all the lines
void drawSpring(std::vector<Spring> sp) {
  std::vector<Vec3f> verts;
  for (unsigned int s = 0; s < sp.size(); s++){
    verts.push_back(sp[s].a->position);
    verts.push_back(sp[s].b->position); 
  } 

  glBindBuffer(GL_ARRAY_BUFFER, line_vertBufferID);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(Vec3f) * (sp.size()*2), // byte size of Vec3f, 4 of them
               verts.data(),      // pointer (Vec3f*) to contents of verts
               GL_STATIC_DRAW);   // Usage pattern of GPU buffer
}

void setupVAO() {
  glBindVertexArray(vaoID);

  glEnableVertexAttribArray(0); // match layout # in shader
  glBindBuffer(GL_ARRAY_BUFFER, vertBufferID);
  glVertexAttribPointer(0,        // attribute layout # above
                        3,        // # of components (ie XYZ )
                        GL_FLOAT, // type of components
                        GL_FALSE, // need to be normalized?
                        0,        // stride
                        (void *)0 // array buffer offset
                        );

  glBindVertexArray(line_vaoID);

  glEnableVertexAttribArray(0); // match layout # in shader
  glBindBuffer(GL_ARRAY_BUFFER, line_vertBufferID);
  glVertexAttribPointer(0,        // attribute layout # above
                        3,        // # of components (ie XYZ )
                        GL_FLOAT, // type of components
                        GL_FALSE, // need to be normalized?
                        0,        // stride
                        (void *)0 // array buffer offset
                        );

  glBindVertexArray(0); // reset to default
}

//spring doesn't update mass positions springs point to but mass on its own is fine
void update(std::vector<Mass> ma, std::vector<Spring> &sp, float dt) {
	drawMass(ma);
	drawSpring(sp);
}


void reloadProjectionMatrix() {
  // Perspective Only

  // field of view angle 60 degrees
  // window aspect ratio
  // near Z plane > 0
  // far Z plane

  P = PerspectiveProjection(WIN_FOV, // FOV
                            static_cast<float>(WIN_WIDTH) /
                                WIN_HEIGHT, // Aspect
                            WIN_NEAR,       // near plane
                            WIN_FAR);       // far plane depth
}

void loadModelViewMatrix() {
  M = IdentityMatrix();
  line_M = IdentityMatrix();
  // view doesn't change, but if it did you would use this
  V = camera.lookatMatrix();
}

void reloadViewMatrix() { V = camera.lookatMatrix(); }

void setupModelViewProjectionTransform() {
  MVP = P * V * M; // transforms vertices from right to left (odd huh?)
}

void reloadMVPUniform() {
  GLint id = glGetUniformLocation(basicProgramID, "MVP");

  glUseProgram(basicProgramID);
  glUniformMatrix4fv(id,        // ID
                     1,         // only 1 matrix
                     GL_TRUE,   // transpose matrix, Mat4f is row major
                     MVP.data() // pointer to data in Mat4f
                     );
}

void reloadColorUniform(float r, float g, float b) {
  GLint id = glGetUniformLocation(basicProgramID, "inputColor");

  glUseProgram(basicProgramID);
  glUniform3f(id, // ID in basic_vs.glsl
              r, g, b);
}

void generateIDs() {
  // shader ID from OpenGL
  std::string vsSource = loadShaderStringfromFile("./shaders/basic_vs.glsl");
  std::string fsSource = loadShaderStringfromFile("./shaders/basic_fs.glsl");
  basicProgramID = CreateShaderProgram(vsSource, fsSource);

  // VAO and buffer IDs given from OpenGL
  glGenVertexArrays(1, &vaoID);
  glGenBuffers(1, &vertBufferID);
  glGenVertexArrays(1, &line_vaoID);
  glGenBuffers(1, &line_vertBufferID);
}

void deleteIDs() {
  glDeleteProgram(basicProgramID);

  glDeleteVertexArrays(1, &vaoID);
  glDeleteBuffers(1, &vertBufferID);
  glDeleteVertexArrays(1, &line_vaoID);
  glDeleteBuffers(1, &line_vertBufferID);
}

void init() {
  glEnable(GL_DEPTH_TEST);
  glPointSize(50);

  camera = Camera(Vec3f{0, 0, 10}, Vec3f{0, 0, -1}, Vec3f{0, 1, 0});

  generateIDs();
  setupVAO();
  
  loadModelViewMatrix();
  reloadProjectionMatrix();
  setupModelViewProjectionTransform();
  reloadMVPUniform();
}

int main(int argc, char **argv) {
  GLFWwindow *window;

  if (!glfwInit()) {
    exit(EXIT_FAILURE);
  }

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window =
      glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "CPSC 587/687 Tut03", NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  glfwSetWindowSizeCallback(window, windowSetSizeFunc);
  glfwSetFramebufferSizeCallback(window, windowSetFramebufferSizeFunc);
  glfwSetKeyCallback(window, windowKeyFunc);
  glfwSetCursorPosCallback(window, windowMouseMotionFunc);
  glfwSetMouseButtonCallback(window, windowMouseButtonFunc);

  glfwGetFramebufferSize(window, &WIN_WIDTH, &WIN_HEIGHT);

  // Initialize glad
  if (!gladLoadGL()) {
    std::cerr << "Failed to initialise GLAD" << std::endl;
    return -1;
  }

  std::cout << "GL Version: :" << glGetString(GL_VERSION) << std::endl;
  std::cout << GL_ERROR() << std::endl;

  init(); // our own initialize stuff func

   //float t = 1000;
  float dt = 0.01;
  
  //creates the spring with one mass
  createSpringMass();

  
  while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
         !glfwWindowShouldClose(window)) {

	if (cubemode == true){
		solveMassSpringCube(dt, rest);
		update(masses, springs, dt);
		displayFunc();
		moveCamera();
	}
	else{
		solveMassSpring(dt, rest);
		update(masses, springs, dt);
		displayFunc();
		moveCamera();
	}
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // clean up after loop
  deleteIDs();

  return 0;
}

//==================== CALLBACK FUNCTIONS ====================//

void windowSetSizeFunc(GLFWwindow *window, int width, int height) {
  WIN_WIDTH = width;
  WIN_HEIGHT = height;

  reloadProjectionMatrix();
  setupModelViewProjectionTransform();
  reloadMVPUniform();
}

void windowSetFramebufferSizeFunc(GLFWwindow *window, int width, int height) {
  FB_WIDTH = width;
  FB_HEIGHT = height;

  glViewport(0, 0, FB_WIDTH, FB_HEIGHT);
}

void windowMouseButtonFunc(GLFWwindow *window, int button, int action,
                           int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT) {
    if (action == GLFW_PRESS) {
      g_cursorLocked = GL_TRUE;
    } else {
      g_cursorLocked = GL_FALSE;
    }
  }
}

void windowMouseMotionFunc(GLFWwindow *window, double x, double y) {
  if (g_cursorLocked) {
    float deltaX = (x - g_cursorX) * 0.01;
    float deltaY = (y - g_cursorY) * 0.01;
    camera.rotateAroundFocus(deltaX, deltaY);

    reloadViewMatrix();
    setupModelViewProjectionTransform();
    reloadMVPUniform();
  }

  g_cursorX = x;
  g_cursorY = y;
}

void windowKeyFunc(GLFWwindow *window, int key, int scancode, int action,
                   int mods) {
  bool set = action != GLFW_RELEASE && GLFW_REPEAT;
  switch (key) {
  case GLFW_KEY_ESCAPE:
    glfwSetWindowShouldClose(window, GL_TRUE);
    break;
  case GLFW_KEY_1:{
	masses.clear();
	springs.clear();
    createSpringMass();
	cubemode = false;
	rest = 4;
	}
    break;
  case GLFW_KEY_2:{
	masses.clear();
	springs.clear();
	createPendulum();
	cubemode = false;
	rest = 1;
	}
    break;
  case GLFW_KEY_3:{
	masses.clear();
	springs.clear();
    createCloth(5,5);
    distanceCloth(masses);
	cubemode = false;
	rest = 2;
	}
    break;
  case GLFW_KEY_4:{
	masses.clear();
	springs.clear();
    createCube(2,2,2);
    distanceCube(masses);
	cubemode = true;
	rest = 2;
	}
    break;
  case GLFW_KEY_R:
	camera = Camera(Vec3f{0, 0, 10}, Vec3f{0, 0, -1}, Vec3f{0, 1, 0});
    break;
  case GLFW_KEY_W:
    g_moveBackForward = set ? 1 : 0;
    break;
  case GLFW_KEY_S:
    g_moveBackForward = set ? -1 : 0;
    break;
  case GLFW_KEY_A:
    g_moveLeftRight = set ? 1 : 0;
    break;
  case GLFW_KEY_D:
    g_moveLeftRight = set ? -1 : 0;
    break;
  case GLFW_KEY_Q:
    g_moveUpDown = set ? -1 : 0;
    break;
  case GLFW_KEY_E:
    g_moveUpDown = set ? 1 : 0;
    break;
  case GLFW_KEY_UP:
    g_rotateUpDown = set ? -1 : 0;
    break;
  case GLFW_KEY_DOWN:
    g_rotateUpDown = set ? 1 : 0;
    break;
  case GLFW_KEY_LEFT:
    if (mods == GLFW_MOD_SHIFT)
      g_rotateRoll = set ? -1 : 0;
    else
      g_rotateLeftRight = set ? 1 : 0;
    break;
  case GLFW_KEY_RIGHT:
    if (mods == GLFW_MOD_SHIFT)
      g_rotateRoll = set ? 1 : 0;
    else
      g_rotateLeftRight = set ? -1 : 0;
    break;
  case GLFW_KEY_SPACE:
    g_play = set ? !g_play : g_play;
    break;
  case GLFW_KEY_LEFT_BRACKET:
    if (mods == GLFW_MOD_SHIFT) {
      g_rotationSpeed *= 0.5;
    } else {
      g_panningSpeed *= 0.5;
    }
    break;
  case GLFW_KEY_RIGHT_BRACKET:
    if (mods == GLFW_MOD_SHIFT) {
      g_rotationSpeed *= 1.5;
    } else {
      g_panningSpeed *= 1.5;
    }
    break;
  default:
    break;
  }
}

//==================== OPENGL HELPER FUNCTIONS ====================//

void moveCamera() {
  Vec3f dir;

  if (g_moveBackForward) {
    dir += Vec3f(0, 0, g_moveBackForward * g_panningSpeed);
  }
  if (g_moveLeftRight) {
    dir += Vec3f(g_moveLeftRight * g_panningSpeed, 0, 0);
  }
  if (g_moveUpDown) {
    dir += Vec3f(0, g_moveUpDown * g_panningSpeed, 0);
  }

  if (g_rotateUpDown) {
    camera.rotateUpDown(g_rotateUpDown * g_rotationSpeed);
  }
  if (g_rotateLeftRight) {
    camera.rotateLeftRight(g_rotateLeftRight * g_rotationSpeed);
  }
  if (g_rotateRoll) {
    camera.rotateRoll(g_rotateRoll * g_rotationSpeed);
  }

  if (g_moveUpDown || g_moveLeftRight || g_moveBackForward ||
      g_rotateLeftRight || g_rotateUpDown || g_rotateRoll) {
    camera.move(dir);
    reloadViewMatrix();
    setupModelViewProjectionTransform();
    reloadMVPUniform();
  }
}

std::string GL_ERROR() {
  GLenum code = glGetError();

  switch (code) {
  case GL_NO_ERROR:
    return "GL_NO_ERROR";
  case GL_INVALID_ENUM:
    return "GL_INVALID_ENUM";
  case GL_INVALID_VALUE:
    return "GL_INVALID_VALUE";
  case GL_INVALID_OPERATION:
    return "GL_INVALID_OPERATION";
  case GL_INVALID_FRAMEBUFFER_OPERATION:
    return "GL_INVALID_FRAMEBUFFER_OPERATION";
  case GL_OUT_OF_MEMORY:
    return "GL_OUT_OF_MEMORY";
  default:
    return "Non Valid Error Code";
  }
}
