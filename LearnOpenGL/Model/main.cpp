
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <filesystem>
#include "Shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Model.h"

using namespace std;
using namespace glm;
//core functions
void frame_buffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void processInput(GLFWwindow* window);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);

unsigned int loadTexture(char const* path);

void frame_buffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}
//Camera default postions.
vec3 cameraPos = vec3(0.0f, 0.0f, 3.0f);
vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);
const float cameraSpeed = 2.5f;

float deltaTime = 0.0f; // time between current frame and last frame.
float lastFrame = 0.0f; //Time of last frame. 

float lastX = 400, lastY = 300;
bool firstMouse = true;
float yawAngle = -90.0f;
float pitchAngle = 0.0f;
float fovAngle = 45.0f;

vec3 lightPos = vec3(1.2f, 1.0f, 2.0f);

vec3 pointLightPositions[] = {
	vec3(0.7f,0.2f,2.0f),
	vec3(2.3f,-3.3f,-4.0f),
	vec3(-4.0f,2.0f,-12.0f),
	vec3(0.0f,0.0f,-3.0f)
};


void processInput(GLFWwindow* window) {
	//setting up the default camera positions.

	//getting escape input.
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		cout << "mouse enabeld" << endl;
	}

	float currentSpeed = static_cast<float>(cameraSpeed * deltaTime);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		cameraPos += currentSpeed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		cameraPos -= currentSpeed * cameraFront;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		cameraPos -= normalize(cross(cameraFront, cameraUp)) * currentSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		cameraPos += normalize(cross(cameraFront, cameraUp)) * currentSpeed;
	}

}
void mouse_callback(GLFWwindow* window, double xPos, double yPos) {

	if (firstMouse) {
		lastY = yPos;
		lastX = xPos;
		firstMouse = false;
	}


	float xOffset = xPos - lastX;
	float yOffset = lastY - yPos; //reversed since y-coordinates range from bottom to top.
	lastX = xPos;
	lastY = yPos;

	const float senstivity = 0.1f;
	xOffset *= senstivity;
	yOffset *= senstivity;

	yawAngle += xOffset;
	pitchAngle += yOffset;

	if (pitchAngle > 89.0f) {
		pitchAngle = 89.0f;
	}
	if (pitchAngle < -89.0f) {
		pitchAngle = -89.0f;
	}
	//get the callbacks
	vec3 direction;
	direction.x = cos(radians(yawAngle)) * cos(radians(pitchAngle));
	direction.y = sin(radians(pitchAngle));
	direction.z = sin(radians(yawAngle)) * cos(radians(pitchAngle));
	cameraFront = normalize(direction);

}

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset) {
	fovAngle -= (float)yOffset;
	if (fovAngle < 1.0f) {
		fovAngle = 1.0f;
	}
	if (fovAngle > 45.0f) {
		fovAngle = 45.0f;
	}
}
unsigned int loadTexture(char const* path) {

	unsigned int textureID;
	int width, height, nrChannels;
	glGenTextures(1, &textureID);
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
	GLenum format;
	switch (nrChannels) {
	case 1: {
		format = GL_RED;
		cout << "RED\n" << endl;
	}
	case 3: {
		format = GL_RGB;
		cout << "RGB\n" << endl;
	}
	case 4: {
		format = GL_RGBA;
		cout << "RGBA\n" << endl;
	}
	}

	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//generating MimMap.
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		cout << "Failed to load texture " << endl;
	}
	stbi_image_free(data);
	return textureID;
}

int main(void) {

	glfwInit();
	//checking version controls
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWidnowHint(GLFW_POENGL_FORAWRD_COMPAT, GL_TRUE);

	//GLFW creating windows so easiser than the actual DIRECTX12 windows HOLY.
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
	if (window == NULL) {
		cout << "Failed to create GLFW window" << endl;
		// killing windows screen
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Failed to intilaize GALD" << endl;
		glfwTerminate();
		return -1;
	}
	stbi_set_flip_vertically_on_load(true);
	glEnable(GL_DEPTH_TEST);
	//setting up the ViewPort set the location of the lower left corner of the window.
	//glViewport(0, 0, 800, 600);
	//keep calling back.
	//Creating our shader program
	//Shader Model
	Shader modelShader("Shaders/model_Shader.vs", "Shaders/model_Shader.fs");
	//Our Model
	Model ourModel("Resources/backpack/backpack.obj");


	//initiating UI IMGUI.
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsLight();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
	while (!glfwWindowShouldClose(window)) {

		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		//input handling
		processInput(window);
		//rendering commans 
		//adding window
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //aso clear the dpeth buffer now!

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin("SkyEngine1.0");
		ImGui::Text("Additional Preference settings");
		ImGui::Text("Create");
		ImGui::Text("Polygon Counts: 0");
		ImGui::End();

		ImGui::Begin("Output");
		ImGui::Text("Console: loaded succesfully.");
		ImGui::End();

		ImGui::Render();
	
		modelShader.use();

		//3D model view
		mat4 model = mat4(1.0f);
		mat4 view = mat4(1.0f);
		mat4 projection = mat4(1.0f);
		const float radius = 10.0f;
		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;
		// THIS IS CORE COMPONENTS OF THE LIGHTSHADER
		projection = perspective(radians(fovAngle), 800.0f / 600.0f, 0.1f, 100.0f);
		view = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		modelShader.setMat4("projection", projection);
		modelShader.setMat4("view", view);

		//render the moedl
		//mat4 model = mat4(1.0f);
	


		model = translate(model, vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene.
		model = scale(model, vec3(0.1f, 0.1f, 0.1f)); // it's a bit too big for our scene, so scale it down.
		modelShader.setMat4("model", model);
		ourModel.Draw(modelShader);
		

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	//once completed delet shaders

	glfwTerminate();
	return 0;
}
