#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace std;
using namespace glm;
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
	glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Failed to intilaize GALD" << endl;
		glfwTerminate();
		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	//setting up the ViewPort set the location of the lower left corner of the window.
	//glViewport(0, 0, 800, 600);
	//keep calling back.
	//Creating our shader program
	
	Shader ourShader("Shaders/shader.vs", "Shaders/shader.fs");

	//texture coordinates
	float texCoords[] = {
		0.0f,0.0f, // lower-left corner
		1.0f,0.0f, // lower-right corner
		0.5f,1.0f //top-center corner
	};

	//generating vertex array object
	float vertices[] = {
   -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	//different vertices
	float eboVertices[] = {
		//Positions		//colors // texutre corrds
	0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
	 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
	-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
	-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
	};
	unsigned int indices[] = {
	0, 1, 3,   // first triangle
	1, 2, 3    // second triangle
	};

	//cube positions
	vec3 cubePositions[] = {
		vec3(0.0f,  0.0f,  0.0f),
		vec3(2.0f,  5.0f, -15.0f),
		vec3(-1.5f, -2.2f, -2.5f),
		vec3(-3.8f, -2.0f, -12.3f),
		vec3(2.4f, -0.4f, -3.5f),
		vec3(-1.7f,  3.0f, -7.5f),
		vec3(1.3f, -2.0f, -2.5f),
		vec3(1.5f,  2.0f, -2.5f),
		vec3(1.5f,  0.2f, -1.5f),
		vec3(-1.3f,  1.0f, -1.5f)
	};


	//unsigned int EBO;
	unsigned int VAO;
	unsigned int VBO;
	// 0. create vertex object
	//Bind the vertex array object first, then bind and set vertex buffer(s). then configure vertex attributes(s).
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	//glGenBuffers(1, &EBO);
	// VERTEX BUFFER OBJECTS
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	//3. then set our vertex attributes pointers

	//Vertext ARRAY Objects
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//add vertex object :)
	//Array index, paramters, size of the index.
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(6 * sizeof(float)));
	//glEnableVertexAttribArray(2);
	//it will create buffer and bind the buffer.
	//now insert the buffer data into VBO.
	//setting up the shader 
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//getting the maximum number of the vertex attributes
	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	cout << "Maximum nr of vertex attributes supported: " << nrAttributes << endl;

	//applying texture map
	

	//float borderColor[] = { 1.0f,1.0f,0.0f, 1.0f };
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	//using stb_image implementation.
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	//generating the texture
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	unsigned char* data = stbi_load("Images/container.jpg", &width, &height, &nrChannels, 0);
	//generating MimMap.
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		cout << "Failed to load texture " << endl;
	}
	stbi_image_free(data);
	unsigned int texture2;
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	//its transparent image add GL_RGBA
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	unsigned char* data2 = stbi_load("Images/awesomeface.png", &width, &height, &nrChannels, 0);
	if (data2) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		cout << "Failed to load texture " << endl;
	}


	stbi_image_free(data2);

	ourShader.use(); // dont forget to activate teh shader before setting uniforms!
	//glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);
	ourShader.setInt("texture1", 0); // or with shader class
	ourShader.setInt("texture2", 1); // or with shader class
	//check if the windows is closing

	// Graphics Language Mathematics.
	//cout << "printNOW!" << endl;
	//vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
	//mat4 trans = mat4(1.0f);
	//trans = translate(trans, vec3(1.0f, 1.0f, 0.0f));
	//vec = trans * vec;
	//cout << vec.x << vec.y << vec.z << endl;
	// another math task.
	//mat4 trans = mat4(1.0f);
	//trans = rotate(trans, radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
	//trans = scale(trans, vec3(0.5f, 0.5f, 0.5f));

	//Coordinate Systems NDC (normalized device coordinates)
	//Local space (or Object space)
	//World space
	//View space (or Eye space)
	//Clip space
	//Screen space.
	// Model MATRIX , Projection Matrix.

	//making orthographic view projection
	//ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f);
	//making perspective projection
	//mat4 proj = perspective(radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);



	
	while (!glfwWindowShouldClose(window)) {
		//input handling
		processInput(window);

		//rendering commans here
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //aso clear the dpeth buffer now!

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		ourShader.use();
		//3D model view
		mat4 model = mat4(1.0f);
		mat4 view = mat4(1.0f);
		mat4 projection = mat4(1.0f);

		//model = rotate(model, radians(-55.0f), vec3(1.0f, 0.0f, 0.0f));
		model = rotate(model, (float)glfwGetTime() * radians(50.0f), vec3(0.5f, 1.0f, 0.0f));
		view = translate(view, vec3(0.0f, 0.0f, -3.0f));
		projection = perspective(radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
		//Updating Feature
		int modelLoc = glGetUniformLocation(ourShader.ID, "model");
		int viewLoc = glGetUniformLocation(ourShader.ID, "view");

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
		//ourShader.setFloat("someUniform", 1.0f); order is very important
		
		
		//This is for triangles with the VBO
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		//redner two objects.

		glBindVertexArray(VAO);
		for (unsigned int i = 0; i < 10; i++) {
			mat4 model = mat4(1.0f);
			model = translate(model, cubePositions[i]);
			float angle = 20.0f * i * (float)glfwGetTime();
			model = rotate(model, radians(angle), vec3(1.0f, 0.3f, 0.5f));
			ourShader.setMat4("model", model);
			ourShader.setMat4("projection", projection);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//glDrawArrays(GL_TRIANGLES, 0, 36);
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
