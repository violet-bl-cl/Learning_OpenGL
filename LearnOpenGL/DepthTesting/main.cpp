
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
GLFWwindow* currentWindow;
//core functions
void frame_buffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void processInput(GLFWwindow* window);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);

unsigned int loadTexture(char const* path);
// unity like systems
int init();
void panelUI();
void start();
void update();
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
float blinn;
float kShininess;
float outliner;
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
			cout << "Camera::LOCKED::" << isCameraLocked <<endl;
		}
		else {
			cout << "Camera::UNLOCKED::"<< isCameraLocked << endl;
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
int init() {
	//==============initializng with the opengl.
	glfwInit();
	//checking version controls
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWidnowHint(GLFW_POENGL_FORAWRD_COMPAT, GL_TRUE);
	//GLFW creating windows so easiser than the actual DIRECTX12 windows HOLY.
	currentWindow = glfwCreateWindow(w_width, w_height, "Depth testing and Stencil testing", nullptr, nullptr);
	if (currentWindow == NULL) {
		cout << "Failed to create GLFW window" << endl;
		// killing windows screen
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(currentWindow);
	glfwSetInputMode(currentWindow, GLFW_CURSOR, GLFW_CURSOR);
	glfwSetFramebufferSizeCallback(currentWindow, frame_buffer_size_callback);

	glfwSetCursorPosCallback(currentWindow, mouse_callback);
	glfwSetScrollCallback(currentWindow, scroll_callback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Failed to intilaize GALD" << endl;
		glfwTerminate();
		return -1;
	}

	//==============initializng with the opengl.

	stbi_set_flip_vertically_on_load(true);
	glEnable(GL_DEPTH_TEST);
	//glDepthMask(GL_FALSE);
	glDepthFunc(GL_LESS);
	//configure global opengl state
	//glEnable(GL_STENCIL_TEST);
	//glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	int stencilBits;
	glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_STENCIL, GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE, &stencilBits);
	if (stencilBits != NULL) {
		cout << "stencil size in bits" << stencilBits << endl;
	}
	glStencilOp(
		GL_KEEP,	// stencil action stencil test fails
		GL_REPLACE,	// stencil action if stencil test passes but depth test fails.
		GL_REPLACE);// stencil action if both pass (fragment shader will also run)
	//for 3d
	//glEnable(GL_CULL_FACE);

}
void panelUI() {
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
			glfwSetWindowShouldClose(currentWindow, true);
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
		ImGui::SliderFloat("Background:", &backgroundColor, 0, 1);
		ImGui::SliderFloat("Blinn Lights:", &blinn, 0, 1);
		ImGui::SliderFloat("kShininess", &kShininess, 0, 32);
		ImGui::SliderFloat("outliner", &outliner, 1.0f, 2.0f);

	}
	ImGui::End();

	ImGui::Render();
}

int main(void) {
	init();
	// Light Shader program
	Shader blinnLightShader("Shaders/model_Shader.vert", "Shaders/model_Shader.frag");
	// Stencil test
	Shader shaderSingleColor("Shaders/shaderSingleColor.vert", "Shaders/shaderSingleColor.frag");
	Model ourModel("Resources/backpack/backpack.obj");
	Model plane("Resources/Plane/untitled.obj");

	// blinn light.
	blinnLightShader.use();
	blinnLightShader.setInt("texture1", 0);

	// initiating UI IMGUI.
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	ImGui_ImplGlfw_InitForOpenGL(currentWindow, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	while (!glfwWindowShouldClose(currentWindow)) {
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Input handling
		processInput(currentWindow);

		// Rendering commands
		glClearColor(backgroundColor, backgroundColor, backgroundColor, 1.0f);
		glClearStencil(0); // clearing stencils.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Also clear the depth buffer and stencil buffer

		// Displaying UI elements
		panelUI();

		mat4 model = mat4(1.0f);
		mat4 view = mat4(1.0f);
		mat4 projection = mat4(1.0f);
		const float radius = 10.0f;
		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;
		projection = perspective(radians(fovAngle), aspectRatio, 0.1f, 100.0f);
		view = lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		// Initiating outlineShader
		shaderSingleColor.use();
		shaderSingleColor.setMat4("view", view);
		shaderSingleColor.setMat4("projection", projection);

		// 3D model view
		blinnLightShader.use();
		blinnLightShader.setMat4("view", view);
		blinnLightShader.setMat4("projection", projection);
	
		// Set outlineShader uniforms
		// Draw floor as normal but don't write the floor to the stencil buffer, we only care about the containers.
		glDisable(GL_STENCIL_TEST);

		glStencilMask(0x00); // Disable writing to the stencil buffer

		model = translate(model, vec3(0.0f, -1.5f, 0.0f)); // Translate it down so it's at the center of the scene.
		model = scale(model, vec3(1, 1, 1)); // Scale it down.
		blinnLightShader.setMat4("model", model);
		plane.Draw(blinnLightShader);
		// 1st render pass: draw objects as normal, writing to the stencil buffer
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);


		model = translate(model, vec3(0.0f, 1.5f, 0.0f)); // Translate it down so it's at the center of the scene.
		model = scale(model, vec3(1.0f, 1.0f, 1.0f)); // Scale it down.
		blinnLightShader.setMat4("model", model);
		blinnLightShader.setFloat("kShininess", kShininess);
		blinnLightShader.setVec3("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);
		blinnLightShader.setVec3("lightPos", lightPos.x, lightPos.y, lightPos.z);
		blinnLightShader.setFloat("blinn", blinn);
		ourModel.Draw(blinnLightShader); // Outline of ourModel

		// Enable wireframe rendering mode
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		// 2nd render pass: now draw slightly scaled versions of the objects, this time disabling stencil writing.
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glDisable(GL_DEPTH_TEST);
		glStencilMask(0x00);
		shaderSingleColor.use();
		shaderSingleColor.setMat4("view", view);
		shaderSingleColor.setMat4("projection", projection);
		model = translate(model, vec3(0.0f, 0.0f, 0.0f)); // Translate it down so it's at the center of the scene.
		shaderSingleColor.setMat4("model", model);
		shaderSingleColor.setFloat("outliner", outliner);
		ourModel.Draw(shaderSingleColor);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glStencilMask(0xFF);
		glStencilFunc(GL_ALWAYS, 0, 0xFF);
		glEnable(GL_DEPTH_TEST);
		// Switch back to the default rendering mode
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glfwSwapBuffers(currentWindow);
		glfwPollEvents();

		// Logic
		if (!isCameraLocked) {
			glfwSetInputMode(currentWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else {
			glfwSetInputMode(currentWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}

	// Clean up
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}