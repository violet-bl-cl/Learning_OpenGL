#include "Shader.h"
/// <summary>
/// setting up the shader. it will do it for u automatically.
/// </summary>
/// <param name="vertexPath"></param>
/// <param name="fragmentPath"></param>
Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	string vertexCode;
	string fragmentCode;
	ifstream vShaderFile;
	ifstream fShaderFile;

	//ensure ifstream objects can throw exceptions:s
	vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	try {
		//open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		stringstream vShaderStream;
		stringstream fShaderStream;
		//read file's buffer conents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		//close file handelrs.
		vShaderFile.close();
		fShaderFile.close();
		//convert stream into string.
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();

	}
	catch (ifstream::failure e) {
		cout << "Error:Shader:File_Not_Succesfully_Read" << endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	//2. compile shaders
	unsigned int vertex, fragment;
	int success;
	char infoLog[512];

	//vertex Shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	//print compile errors if any 
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		cout << "Error:Shader:Vertex:Compilation_Failed\n" << infoLog << endl;

	}

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	//print compile errors if any 
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		cout << "Error:Shader:Fragment:Compilation_Failed\n" << infoLog << endl;

	}
	//Shader Program
	
	Shader::ID = glCreateProgram();
	glAttachShader(Shader::ID, vertex);
	glAttachShader(Shader::ID, fragment);
	glLinkProgram(Shader::ID);
	//print linking errors if any
	glGetProgramiv(Shader::ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(Shader::ID, 512, NULL, infoLog);
		cout << "Error:Shader:Program:Linking_failed\n" << infoLog << endl;
	}
	//delete the shaders as they are linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}
void Shader::use() {
	glUseProgram(Shader::ID);
}
void Shader::setBool(const string& name, bool value) const {
	glUniform1i(glGetUniformLocation(Shader::ID, name.c_str()), (int)value);
}

void Shader::setInt(const string& name, int value) const {
	glUniform1i(glGetUniformLocation(Shader::ID, name.c_str()), value);
}

void Shader::setFloat(const string& name, float value) const {
	glUniform1i(glGetUniformLocation(Shader::ID, name.c_str()), value);
}
void Shader::setMat4(const string& name, mat4 &mat) const {
	glUniformMatrix4fv(glGetUniformLocation(Shader::ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);

//	glUniform1i(glGetUniformLocation(Shader::ID, name.c_str()), value);
}