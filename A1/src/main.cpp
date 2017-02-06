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

//for my sphere
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ShaderTools.h"
#include "Vec3f.h"
#include "Mat4f.h"
#include "OpenGLMatrixTools.h"
#include "Camera.h"
#include "Vec3f_FileIO.h"


static float PI = 3.14159265359;


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

// Only one camera so only one view and perspective matrix are needed.
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
void subDivision(std::vector<Vec3f> points);
float wrap(float s);

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

//currently unused set position function
std::vector<Vec3f> position(float s);

//this is used for iT's intial starting point
void startingPoint();
//velocity to be multiplied by dt to get DS
float velocity(float highest, float height);

//algorithm 1 in the guideline sheet on algorithmic botany website http://algorithmicbotany.org/courses/CPSC587/Winter2017/CPSC587_687_RollerCoaster.pdf
Vec3f ArcLengthParameterization(Vec3f bT, std::vector<Vec3f> p, int &i, float DS);

//following methods for centripetal force and what is needed to simulate rollercoaster 
Vec3f tangent(int &i, std::vector<Vec3f> p);


float L; //length of line
float DS; //defined change in s that is used to traverse the piece wise curve
float highest;
float lowest;
float vmin = 0.2;
float speed;
int iT; // i variable for arcLengthParameterization 
int start; //startingpoint on the track
int peak; //actual highest i index on the track
float dt = 0.1; //changes the time interval to speed up or slow down simulation

Vec3f startingPos; //starting position currently highest point
Vec3f objectPos; //the vec3 to keep track of the model's position
float gravity = 9.81/10; // have to change gravity to make it scaled to the size of the track
std::vector<Vec3f> verts; //geometry vertices
std::vector<Vec3f> curvePoints; //actually curve points not just control points
std::vector<Vec3f> incline; //initial incline portion of the track

//==================== FUNCTION DEFINITIONS ====================//

void displayFunc() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Use our shader
  glUseProgram(basicProgramID);

  // ===== DRAW QUAD ====== //
  MVP = P * V * M;
  reloadMVPUniform();
  reloadColorUniform(1, 0, 0);

  // Use VAO that holds buffer bindings
  // and attribute config of buffers
  glBindVertexArray(vaoID);
  // Draw Quads, start at vertex 0, draw 4 of them (for a quad)
  glDrawArrays(GL_TRIANGLE_STRIP, 0, verts.size());

  // ==== DRAW LINE ===== //
  MVP = P * V * line_M;
  reloadMVPUniform();
  reloadColorUniform(0, 1, 1);

  // Use VAO that holds buffer bindings
  // and attribute config of buffers
  glBindVertexArray(line_vaoID);
  // Draw lines 
  glDrawArrays(GL_LINE_LOOP, 0, curvePoints.size());
  
}

//velocity of freefalling Note that this is the only velocity called right now as this simulates bead from highest point but not roller coaster alone
float velocityFreeFalling(float highest, float height)
{
	float vel = sqrt(2*gravity*(highest - height));
	if (vel == 0)
	{
		vel = 0.2;
	}
	return vel;
}

//the equation for lift portion of the roller coaster not implemented
float velocityIncline(float v)
{
	float vel = v;
	return vel;
}

//this one doesn't work
float velocityDecelerate(float vel, int dLength)
{
	vel = vel * (iT / dLength);
	return vel;
}

//for getting the temporary tangent at a point
Vec3f tangent(int &i, std::vector<Vec3f> p)
{
	Vec3f tangent = p[(i+1) % p.size()] - p[(i-1) % p.size()];
	return tangent;
}

//to calculate the normalDirection to use to get centripetal force
Vec3f normalDirection(int &i, std::vector<Vec3f> p)
{
	Vec3f nD = p[(i+1) % p.size()] - (2*p[i]) + p[(i-1) % p.size()];
	nD = nD / nD.length();
	return nD;
}

//curvature of the line
float curvature(int &i, std::vector<Vec3f> p)
{
	float x = ((p[(i+1) % p.size()] - (2*p[i]) + p[(i-1) % p.size()]).length())/2;
	float c = ((p[(i+1) % p.size()] - p[(i-1) % p.size()]).length())/2;
	float radius = ((x*x) + (c*c))/(2*x);
	float kurve = (2*x)/((x*x)+(c*c));
	return kurve;
}

//from curvature kurve is the k and from normalDirection nD is the n value in this function
Vec3f centripetalAcceleration(float k, Vec3f n)
{
	return k*n;
}

//pass speed into here
Vec3f normalForce(Vec3f aC)
{
	Vec3f grav = Vec3f(0.0, 9.81, 0.0);
	Vec3f fC;
	fC.y() = aC.y() - grav.y();
	return aC;
}

void startingPoint()
{
	float highestPos = 0;
	//following for getting the highest point
	//also for lowest depending on if i want to start the simulation at highest or incline part
	for (unsigned i = 0; i < curvePoints.size(); i++)
	{
		if (curvePoints[i].y() > highestPos)
		{
			highestPos = curvePoints[i].y();
			peak = i;
			//start = i;
			cout << "Highest point: " << peak << endl;
		}
		else if (curvePoints[i].y() < lowest)
		{
			lowest = curvePoints[i].y();
			start = i;
			cout << "starting lowest: " << start << endl;
		}
	}
	cout << "Total amount of points " << curvePoints.size() << endl;
	iT = start;
	lowest = start;
	highest = highestPos;
	M = TranslateMatrix(curvePoints[start].x(), curvePoints[start].y(), curvePoints[start].z());
	setupModelViewProjectionTransform();
	reloadMVPUniform();
}

void animateQuad(float t) {

	cout << "CURRENT POINT: " << iT << endl;
	int decel = (start - 50) % curvePoints.size();
	if ((iT < peak) && (iT >= (start - decel)))
	{
		cout << "INCLINE" << endl;
		speed = velocityIncline(vmin);
	}
	else if ((iT >= (decel)) && (iT <= start))
	{
		cout << "DECEL " << endl;
		speed = velocityDecelerate(speed, decel);
	}
	else 
	{
		speed = velocityFreeFalling(highest, curvePoints[(iT+1) % curvePoints.size()].y());
	}
	cout << "SPEED " << speed << endl;
	//speed = velocityIncline(vmin); 
	//speed = velocityFreeFalling(highest, curvePoints[(iT+1) % curvePoints.size()].y());
	//speed = velocityDecelerate(vmin, 10);
	DS = dt*speed;
	
	Vec3f trans = ArcLengthParameterization(objectPos, curvePoints, iT, DS);
	objectPos = trans;
	
	M = TranslateMatrix(trans.x(), trans.y(), trans.z());
	setupModelViewProjectionTransform();
	reloadMVPUniform();
}

void loadQuadGeometryToGPU() {
	
	/*
	verts.push_back(Vec3f(-0.2, -0.2, 0));
	verts.push_back(Vec3f(-0.2, 0.2, 0));
	verts.push_back(Vec3f(0.2, -0.2, 0));
	verts.push_back(Vec3f(0.2, 0.2, 0));
	*/
	
	//interval is for how much space to partition each angle when generating new triangle
	int interval = 15;
	//r is the radius of sphere
	float r = 0.5;
	//interval but calculated in sphereical coordinates
	float t;
	float x = 0;
	float y = 0;
	float z = 0;
	/*
	*NOTE*
	Very long code used in rendering cpsc 591 created by me to draw a sphere, I removed the texture part as I don't want to worry about having to use uv coordinates here and creating new vbo for them
	using phi and theta create intervals to create a single 4 point quad to render as part of the sphere
	*/
	for (int phi = 0; phi <= 180; phi = phi + interval)
	{
		for (int theta = 0; theta <= 360; theta = theta + interval)
		{
			t = interval*(PI/180.f);
			//Point 1
			x = r*(cos(theta*(PI/180.f))*sin(phi*(PI/180.f)));
			y = r*(sin(theta*(PI/180.f))*sin(phi*(PI/180.f)));
			z = r*cos(phi*(PI/180.f));
			verts.push_back(Vec3f(x,y,z));


			//Point 2
			x = r*(cos(theta*(PI/180.f)+t)*sin(phi*(PI/180.f)));
			y = r*(sin(theta*(PI/180.f)+t)*sin(phi*(PI/180.f)));
			z = r*cos(phi*(PI/180.f));
			verts.push_back(Vec3f(x,y,z));


			//Point 3
			x = r*(cos(theta*(PI/180.f))*sin(phi*(PI/180.f)+t));
			y = r*(sin(theta*(PI/180.f))*sin(phi*(PI/180.f)+t));
			z = r*cos(phi*(PI/180.f)+t);
			verts.push_back(Vec3f(x,y,z));


			//Point 4
			x = r*(cos(theta*(PI/180.f)+t)*sin(phi*(PI/180.f)+t));
			y = r*(sin(theta*(PI/180.f)+t)*sin(phi*(PI/180.f)+t));
			z = r*cos(phi*(PI/180.f)+t);
			verts.push_back(Vec3f(x,y,z));
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, vertBufferID);
	glBufferData(GL_ARRAY_BUFFER,
               sizeof(Vec3f) * verts.size(), // byte size of Vec3f, 4 of them
               verts.data(),      // pointer (Vec3f*) to contents of verts
               GL_STATIC_DRAW);   // Usage pattern of GPU buffer
}

void loadLineGeometryToGPU() {
	// Draws the line
	// draws lines from the following points

	std::string file( "track.txt" );
	
	std::vector<Vec3f> points;
	
	loadVec3fFromFile( points, file);
	for( auto & v : points )
	{
		points.push_back(v);
	}
	
	//subdivide line using chaikin method until smooth
	//have to call it on intial points once due to my bad programming
	subDivision(points);
	for (unsigned i=0; i < 4; i++)
	{
		subDivision(curvePoints);
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, line_vertBufferID);
	glBufferData(GL_ARRAY_BUFFER,
               sizeof(Vec3f) * (sizeof(points)*curvePoints.size()), // byte size of Vec3f, Change to fit amount of points rendered each time
               curvePoints.data(),      // pointer (Vec3f*) to contents of verts
               GL_STATIC_DRAW);   // Usage pattern of GPU buffer
}


std::vector<Vec3f> position(float s)
{
	std::vector<Vec3f> p;
	p.push_back(Vec3f(curvePoints[s].x(), curvePoints[s].y(), curvePoints[s].z()));
	return p;
}

//curvePoints[i] = p.[i]
//objectPos = bT currently global
//float ds = DS global updated value
//float iT = i

Vec3f ArcLengthParameterization(Vec3f bT, std::vector<Vec3f> p, int &i, float dS)
{
	float magnitudeBT = p[(i+1) % p.size()].distance(bT);
	if (magnitudeBT > dS)
	{
		bT = bT + dS*((p[(i+1) % p.size()] - bT) / magnitudeBT);
		return bT;
	}
	else
	{
		float dSPrime = magnitudeBT;
		i = (i+1) % p.size();
		float magnitudePi = p[(i+1) % p.size()].distance(p[i]);
		while(dSPrime + magnitudePi < dS)
		{
			dSPrime = dSPrime + magnitudePi;
			i = (i+1) % p.size();
		}
		bT = ((dS - dSPrime)*(p[(i+1) % p.size()] - p[i])/magnitudePi) + bT;
		return bT;
	}
}

void subDivision(std::vector<Vec3f> points)
{
	std::vector<Vec3f> subPoints;
	curvePoints.clear();
	int numPoints = points.size();
	for (unsigned i=0; i < points.size(); i++)
	{
		subPoints.push_back(points[i]);
		subPoints.push_back((points[i] + (points[(i+1) % numPoints])) / 2);
	}
	for (unsigned j=0; j < subPoints.size(); j++)
	{
		curvePoints.push_back((subPoints[j] + (subPoints[(j+1) % (numPoints*2)])) / 2);
	}
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

  camera = Camera(Vec3f{0, 0, 15}, Vec3f{0, 0, -1}, Vec3f{0, 1, 0});

  // SETUP SHADERS, BUFFERS, VAOs

  generateIDs();
  setupVAO();
  loadQuadGeometryToGPU();
  loadLineGeometryToGPU();

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
      glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "CPSC 587 bead on a wire", NULL, NULL);
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
  
  DS = 0.1;
  float t = 0; //initial time value

  //gets the highest startingPoint on the curve
  startingPoint();
  
  //Initial position of the object on the highest part of the curve thanks to startingPoint()
  objectPos = curvePoints[iT];
  
  while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
         !glfwWindowShouldClose(window)) {
    if (g_play) {
      t += dt;
      animateQuad(t);
    }

    displayFunc();
    moveCamera();

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
