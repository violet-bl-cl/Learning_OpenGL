
#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>
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

bool drawMe = false;
float ambientValues = 0.0f;

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
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR);
	glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
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

	Shader lightShader("Shaders/basic_AllLightShader.vs", "Shaders/basic_AllLightShader.fs");
	Shader cubeLightShader("Shaders/cube_Shader.vs", "Shaders/cube_Shader.fs");


	//generating vertex array object
	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
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

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(VAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	unsigned int lightCubeVAO;
	glGenVertexArrays(1, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//getting the maximum number of the vertex attributes
	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	cout << "Maximum nr of vertex attributes supported: " << nrAttributes << endl;

	unsigned int texture = loadTexture("Images/container2.png");
	unsigned int texture1 = loadTexture("Images/container2_specular.png");

	lightShader.use();
	lightShader.setInt("material.diffuse", 0);
	lightShader.setInt("material.specular", 1);

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
		ImGui::Text("Additional Prefrence settings");
		ImGui::Text("Create");
		ImGui::Text("Polygon Counts: 0");
		ImGui::End();

		ImGui::Begin("Output");
		ImGui::Text("Console: loaded succesfully.");
		ImGui::Checkbox("Draw Triangle", &drawMe);
		ImGui::SliderFloat("Ambient Light", &ambientValues, 0.0f, 1.0f);
		ImGui::End();

		ImGui::Render();
	
		if (drawMe) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		
		}
		else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR);
		}

		lightShader.use();

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
		lightShader.setMat4("projection", projection);
		lightShader.setMat4("view", view);
		lightShader.setVec3("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);
		lightShader.setFloat("material.shininess", 32.0f);
		// THIS IS CORE COMPONENTS OF THE LIGHTSHADER
		// YOU PUT UR SETTINGS ALL HERE.
		//direction lights
		lightShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		lightShader.setVec3("dirLight.ambient", ambientValues, ambientValues, ambientValues);
		lightShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		lightShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
		//point lights.
		lightShader.setVec3("pointLights[0].position", pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		lightShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
		lightShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
		lightShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		lightShader.setFloat("pointLights[0].constant", 1.0f);
		lightShader.setFloat("pointLights[0].linear", 0.09f);
		lightShader.setFloat("pointLights[0].quadratic", 0.032f);
		//point 1 light
		lightShader.setVec3("pointLights[1].position", pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
		lightShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
		lightShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
		lightShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		lightShader.setFloat("pointLights[1].constant", 1.0f);
		lightShader.setFloat("pointLights[1].linear", 0.09f);
		lightShader.setFloat("pointLights[1].quadratic", 0.032f);
		//point 2 light
		lightShader.setVec3("pointLights[2].position", pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
		lightShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
		lightShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
		lightShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
		lightShader.setFloat("pointLights[2].constant", 1.0f);
		lightShader.setFloat("pointLights[2].linear", 0.09f);
		lightShader.setFloat("pointLights[2].quadratic", 0.032f);
		//point 3 light
		lightShader.setVec3("pointLights[3].position", pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
		lightShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
		lightShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
		lightShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
		lightShader.setFloat("pointLights[3].constant", 1.0f);
		lightShader.setFloat("pointLights[3].linear", 0.09f);
		lightShader.setFloat("pointLights[3].quadratic", 0.032f);
		//spotLights 
		lightShader.setVec3("spotLight.position", cameraPos.x, cameraPos.y, cameraPos.z);
		lightShader.setVec3("spotLight.direction", 0, 0, -1);
		lightShader.setVec3("spotLight.position", cameraPos.x, cameraPos.y, cameraPos.z);
		lightShader.setVec3("spotLight.ambient", 0.05f, 0.05f, 0.05f);
		lightShader.setVec3("spotLight.diffuse", 0.8f, 0.8f, 0.8f);
		lightShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		lightShader.setFloat("spotLight.constant", 1.0f);
		lightShader.setFloat("spotLight.linear", 0.09f);
		lightShader.setFloat("spotLight.quadratic", 0.032f);
		//cutoff.
		lightShader.setFloat("spotLight.cutOff", cos(radians(12.5f)));
		lightShader.setFloat("spotLight.outerCutOff", cos(radians(15.0f)));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture1);
		//creating textured map objects.
		for (unsigned int i = 0; i < 10; i++) {

			model = mat4(1.0f);
			model = translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			model = rotate(model, radians(angle), vec3(1.0f, 0.3f, 0.5f));
			lightShader.setMat4("model", model);
			glBindVertexArray(VAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		//Light Objects
		cubeLightShader.use();
		cubeLightShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		cubeLightShader.setMat4("projection", projection);
		cubeLightShader.setMat4("view", view);
		glBindVertexArray(lightCubeVAO);
		for (unsigned int i = 0; i < 4; i++) {
			model = mat4(1.0f);
			model = translate(model, pointLightPositions[i]);
			model = scale(model, vec3(0.2f)); // a smaller cube
			cubeLightShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	//once completed delet shaders
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &EBO);
	glfwTerminate();
	return 0;
}
