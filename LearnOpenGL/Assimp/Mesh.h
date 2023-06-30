#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <sstream>
#include <vector>
#include "Shader.h"
using namespace std;
using namespace glm;
struct Vertex {
	vec3 Position;
	vec3 Normal;
	vec2 TexCoords;
};

struct Texture {
	unsigned int id;
	string type;
};

class Mesh
{
public:
	//mesh data
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;
	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
	void Draw(Shader &shader);
private:
	//render data
	unsigned int VAO, VBO, EBO;
	void setupMesh();
};

