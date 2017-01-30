/**
 * Author:	Andrew Robert Owens
 * Email:	arowens [at] ucalgary.ca
 * Date:	January, 2017
 * Course:	CPSC 587/687 Fundamental of Computer Animation
 * Organization: University of Calgary
 *
 * Copyright (c) 2017 - Please give credit to the author.
 *
 * File:	ShaderTools.h
 */

#ifndef SHADER_TOOLS_H
#define SHADER_TOOLS_H

#include "glad/glad.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

/* Shader Program Helper
        This function will attempt to create a shader program that has
        the Vertex and Fragement shader code that you pass in. If successfully
        compiled and linked to the program, the unique program ID given by
        OpenGL will be returned. This ID will be >1 if successful, or 0 (an
        invalid ID) if any of the above fails.

        Most of the Code below is for checking if the shaders compiled and
        linked correctly.
*/
GLuint CreateShaderProgram(const std::string &vsSource,
                           const std::string &fsSource);
GLuint CreateShaderProgram(const std::string &vsSource,
                           const std::string &gsSource,
                           const std::string &fsSource);

bool checkCompileStatus(GLint shaderID);
bool checkLinkStatus(GLint programID);

std::string loadShaderStringfromFile(const std::string &filePath);

#endif // SHADER_TOOLS_H
