
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

float w_width = 800.0f;
float w_height = 600.0f;
float aspectRatio = static_cast<float>(w_width/w_height);
void frame_buffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	w_width = width;
	w_height = height;
	aspectRatio = static_cast<float>(width) / height;
}


//Background colors
float backgroundColor = 0.1f;
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

bool isCameraLocked = false;

vec3 lightPos = vec3(1.2f, 1.0f, 2.0f);
float blinn = 1.0f;
float kShininess = 0.1f;
bool gammaEnabled = false;
vec3 pointLightPositions[] = {
	vec3(0.7f,0.2f,2.0f),
	vec3(2.3f,-3.3f,-4.0f),
	vec3(-4.0f,2.0f,-12.0f),
	vec3(0.0f,0.0f,-3.0f)
};

bool spaceKeyPreviouslyPressed = false;
void processInput(GLFWwindow* window) {
	//setting up the default camera positions.

	//getting escape input.
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		cout << "mouse enabeld" << endl;
	}

	bool spaceKeyPressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
	if (spaceKeyPressed && !spaceKeyPreviouslyPressed) {
		// The Space key was just pressed
		// Perform actions here
		isCameraLocked = !isCameraLocked;
		if (!isCameraLocked) {
			cout << "yeah" << isCameraLocked <<endl;
		}
		else {
			cout << "Camera Switched"<< isCameraLocked << endl;
		}
	
		// Update the state of the button
		spaceKeyPreviouslyPressed = true;
	}
	else if (!spaceKeyPressed) {
		// The Space key is not currently pressed
		// Reset the state of the button
		spaceKeyPreviouslyPressed = false;
	}

	float currentSpeed = static_cast<float>(cameraSpeed * deltaTime);
	if (!isCameraLocked) {
		return;
	}

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

	if (!isCameraLocked) {
		return;
	}
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

	//==============initializng with the opengl.
	glfwInit();
	//checking version controls
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWidnowHint(GLFW_POENGL_FORAWRD_COMPAT, GL_TRUE);
	//GLFW creating windows so easiser than the actual DIRECTX12 windows HOLY.
	GLFWwindow* window = glfwCreateWindow(w_width, w_height, "Advanced Lighting", nullptr, nullptr);
	if (window == NULL) {
		cout << "Failed to create GLFW window" << endl;
		// killing windows screen
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR);
	//setting up the window size.
	//glfwSetWindowSizeCallback(window, window_resize_callback);
	glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);
	
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Failed to intilaize GALD" << endl;
		glfwTerminate();
		return -1;
	}

	//==============initializng with the opengl.

	stbi_set_flip_vertically_on_load(true);
	glEnable(GL_DEPTH_TEST);
	//setting up the ViewPort set the location of the lower left corner of the window.
	//glViewport(0, 0, 800, 600);
	//keep calling back.
	//Light Shader program
	Shader blinnLightShader("Shaders/blinn_phongShader.vert", "Shaders/blinn_phongShader.frag");
	//Creating our shader program
	//Shader Model
	//Shader modelShader("Shaders/model_Shader.vert", "Shaders/model_Shader.frag");
	//Shader planeShader("Shaders/model_Shader.vs", "Shaders/model_Shader.fs");
	//Our Model
    Model ourModel("Resources/backpack/backpack.obj");
	Model plane("Resources/Plane/untitled.obj");


	// lighting info
   // -------------
	glm::vec3 lightPositions[] = {
		glm::vec3(-3.0f, 0.5f, 0.0f),
		glm::vec3(-1.0f, 0.5f, 0.0f),
		glm::vec3(1.0f, 0.5f, 0.0f),
		glm::vec3(3.0f, 0.5f, 0.0f)
	};
	glm::vec3 lightColors[] = {
		glm::vec3(0.25),
		glm::vec3(0.50),
		glm::vec3(0.75),
		glm::vec3(1.00)
	};

	//blinn light.
	blinnLightShader.use();
	blinnLightShader.setInt("texture1", 0);

	//vec3 lightPos(0.0f, 0.0f, 0.0f);

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
		glClearColor(backgroundColor, backgroundColor, backgroundColor, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //aso clear the dpeth buffer now!

	

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		
		ImGui::NewFrame();
		//creating menu bar.
		ImGui::BeginMainMenuBar();

		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Open", "Ctrl+O")) {
				// Handle open action
			}
			if (ImGui::MenuItem("Save", "Ctrl+S")) {
				// Handle save action
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Exit", "Alt+F4")) {
				// Handle exit action
				glfwSetWindowShouldClose(window, true);
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit")) {
			// Add menu items for editing actions

			ImGui::EndMenu();
		}

		// Add more menus as needed

		ImGui::EndMainMenuBar();

		//Properties panel
		ImGui::SetNextWindowPos(ImVec2(0, 20));
		ImVec2 size = { 150,w_height };
		ImGui::SetNextWindowSize(size, 0);
		ImGui::Begin("Properties", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		double fps = 1.0f / static_cast<float>(deltaTime);
		ImGui::Text("FPS: %.2f", fps);

		if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen)) {
			if (ImGui::Button("Open...")) {

			}
		}
		if (ImGui::CollapsingHeader("Mesh-Properties", ImGuiTreeNodeFlags_DefaultOpen)) {
			if (ImGui::Button("Mesh Type")) {

			}
			ImGui::SliderFloat("Background:", &backgroundColor, 0.1f, 1);
			ImGui::SliderFloat("Blinn Lights:", &blinn, 0.1f, 1);
			ImGui::SliderFloat("kShininess", &kShininess, 0.1f, 64);
			ImGui::Checkbox("Gamma", &gammaEnabled);
			
		}
		ImGui::End();

		ImGui::Render();
	
		blinnLightShader.use();

		//3D model view
		mat4 model = mat4(1.0f);
		mat4 view = mat4(1.0f);
		mat4 projection = mat4(1.0f);
		const float radius = 10.0f;
		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;
		// THIS IS CORE COMPONENTS OF THE LIGHTSHADER
		projection = perspective(radians(fovAngle), aspectRatio, 0.1f, 100.0f);
		view = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		blinnLightShader.setMat4("projection", projection);
		blinnLightShader.setMat4("view", view);

		//render the moedl
		//mat4 model = mat4(1.0f);
	
	
		model = translate(model, vec3(0.0f, -1.5f, 0.0f)); // translate it down so it's at the center of the scene.
		model = scale(model, vec3(1, 1, 1)); // it's a bit too big for our scene, so scale it down.
		blinnLightShader.setMat4("model", model);
		ourModel.Draw(blinnLightShader);

		model = translate(model, vec3(0.0f,-1.0f, 0.0f)); // translate it down so it's at the center of the scene.
		model = scale(model, vec3(2, 1, 2)); // it's a bit too big for our scene, so scale it down.
		blinnLightShader.setMat4("model", model);
		plane.Draw(blinnLightShader);

		blinnLightShader.use();
		blinnLightShader.setMat4("projection", projection);
		blinnLightShader.setMat4("view", view);
		//set llight uniforms;
		glUniform3fv(glGetUniformLocation(blinnLightShader.ID, "lightPositions"), 4, &lightPositions[0][0]);
		glUniform3fv(glGetUniformLocation(blinnLightShader.ID, "lightColors"), 4, &lightColors[0][0]);

		blinnLightShader.setVec3("viewPos", cameraPos.x, cameraPos.y,cameraPos.z);
		blinnLightShader.setInt("gamma", gammaEnabled);

		
		blinnLightShader.setInt("blinn", blinn);
		blinnLightShader.setFloat("kShininess", kShininess);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents();
		//Logics
		if (!isCameraLocked) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	//once completed delet shaders

	glfwTerminate();
	return 0;
}
