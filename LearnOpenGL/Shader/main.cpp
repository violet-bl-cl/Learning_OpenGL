#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
using namespace std;
//core functions
void frame_buffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

void frame_buffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}
void processInput(GLFWwindow* window) {
	//getting escape input.
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		cout << "wtf" << endl;
	}
}





int main(void) {

	glfwInit();
	//checking version controls
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWidnowHint(GLFW_POENGL_FORAWRD_COMPAT, GL_TRUE);

	//GLFW creating windows so easiser than the actual DIRECTX12 windows HOLY.
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		cout << "Failed to create GLFW window" << endl;
		// killing windows screen
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Failed to intilaize GALD" << endl;
		glfwTerminate();
		return -1;
	}

	//setting up the ViewPort set the location of the lower left corner of the window.
	//glViewport(0, 0, 800, 600);
	//keep calling back.
	glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);
	//Creating our shader program
	
	Shader ourShader("Shaders/shader.vs", "Shaders/shader.fs");
	//generating vertex array object
	float vertices[] = {
	-0.5f,-0.5f,0.0f, //left
	0.5f,-0.5f,0.0f, //right
	0.0f,0.5f,0.0f //top
	//second triangle
	,0.5f,-0.5f,0.0f, // bottom right
	-0.5f,-0.5f,0.0f, //bottom left
	-0.5f,0.5f,0.0f  // top left
	};

	//different vertices
	float eboVertices[] = {
		//Positions		//colors
	0.5f, -0.5f,0.0f, 1.0f,0.0f,0.0f,  // top right
	-0.5f,-0.5f,0.0f, 0.0f,1.0f,0.0f, // bottom right
	0.0f,  0.5f,0.0f, 0.0f,0.0f,1.0f// bottom left
	};
	//unsigned int indices[] = {
	//0, 1, 3,   // first triangle
	//1, 2, 3    // second triangle
	//};



	//unsigned int EBO;
	unsigned int VAO;
	unsigned int VBO;
	// 0. create vertex object
	//Bind the vertex array object first, then bind and set vertex buffer(s). then configure vertex attributes(s).
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
//	glGenBuffers(1, &EBO);
	// VERTEX BUFFER OBJECTS
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(eboVertices), eboVertices, GL_STATIC_DRAW);


//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	//3. then set our vertex attributes pointers

	//Vertext ARRAY Objects
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//add vertex object :)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//it will create buffer and bind the buffer.
	//now insert the buffer data into VBO.
	//setting up the shader 
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//getting the maximum number of the vertex attributes
	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	cout << "Maximum nr of vertex attributes supported: " << nrAttributes << endl;

	//check if the windows is closing
	while (!glfwWindowShouldClose(window)) {
		//input handling
		processInput(window);

		//rendering commans here
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//Updating Feature
		//float timeValue = glfwGetTime();
		//float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
		//int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
		//
		//glUseProgram(shaderProgram);
		//changing the colors
		//glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
		ourShader.use();
		ourShader.setFloat("someUniform", 1.0f);
		glBindVertexArray(VAO);
		//This is for triangles with the VBO
		glDrawArrays(GL_TRIANGLES, 0, 3);


	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//check and call events and swap the buffers.
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	//once completed delet shaders
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &EBO);
	glfwTerminate();
	return 0;
}
