#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h> //include gald to get all the required opengl headers.
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;
class Shader
{
public:
	//the program ID
	unsigned int ID;
	//Constructor  reads and build the shader
	Shader(const char* vertexPath, const char* fragmentPath);
	// use /activate the shader.
	void use();
	//Utility unifrom functions.
	void setBool(const string& name, bool value) const;
	void setInt(const string& name, int value) const;
	void setFloat(const string& name, float value) const;
};


#endif
