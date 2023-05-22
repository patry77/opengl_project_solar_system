#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stb_image.h>

#include "Shader.h"
#include "Sphere.h"
#include "Camera.h"

#include <cstdlib>
#include <iostream>
#include <vector>
#include <map>
#include <wtypes.h>
#include <ctime>
#include <filesystem>
#include <string>
#define _USE_MATH_DEFINES
#include <math.h>

#define TAU (M_PI * 2.0)


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
unsigned int loadTexture(char const * path);
unsigned int loadCubemap(std::vector<std::string> faces);



GLfloat deltaTime = 0.0f;	
GLfloat lastFrame = 0.0f;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool onRotate = false;
bool onFreeCam = true;
bool SkyBoxExtra = false;
float SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600;

glm::vec3 point = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 PlanetPos = glm::vec3(0.0f, 0.0f, 0.0f);
GLfloat lastX = (GLfloat)(SCREEN_WIDTH / 2.0);
GLfloat lastY = (GLfloat)(SCREEN_HEIGHT / 2.0);
float PlanetSpeed = .1f;
int PlanetView = 0;

bool keys[1024];
GLfloat SceneRotateY = 0.0f;
GLfloat SceneRotateX = 0.0f;
bool onPlanet = false;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
	{
		//camera.Position = PlanetPos;
		onPlanet = true;
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

bool firstMouse = true;
GLfloat xoff = 0.0f, yoff = 0.0f;


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = (GLfloat)xpos;
		lastY = (GLfloat)ypos;
		firstMouse = false;
	}

	GLfloat xoffset = (GLfloat)(xpos - lastX);
	GLfloat yoffset = (GLfloat)(lastY - ypos);
	xoff = xoffset;
	yoff = yoff;

	lastX = (GLfloat)xpos;
	lastY = (GLfloat)ypos;
	if (onRotate)
	{
		SceneRotateY += yoffset * 0.1f;
		SceneRotateX += xoffset * 0.1f;
	}
	camera.ProcessMouseMovement(xoffset, yoffset);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (yoffset == 1)
		camera.ProcessKeyboard(SCROLL_FORWARD, deltaTime);
	else
	{
		camera.ProcessKeyboard(SCROLL_BACKWARD, deltaTime);
	}
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (onFreeCam && !camera.FreeCam)
	{
		if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
			onRotate = true;
		else onRotate = false;
	}
}

int main() {
	PlanetView = 0;
	onFreeCam = false;
	camera.FreeCam = true;
	/* glfw */
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	

	
	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Kucek Kuzera Lugowski", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);



	/* glad */
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}



	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/* SHADERS */
	Shader SimpleShader("simpleVS.vs", "simpleFS.fs");
	Shader SkyboxShader("skybox.vs", "skybox.fs");
	/* SHADERS */

	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	/* skybox gen. */
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


	/* orbita */
	std::vector<float> orbVert;
	GLfloat xx;
	GLfloat zz;
	float angl;
	for (int i = 0; i < 2000; i++)
	{
		angl = (float)(M_PI / 2 - i * (M_PI / 1000));
		xx = sin(angl) * 100.0f;
		zz = cos(angl) * 100.0f;
		orbVert.push_back(xx);
		orbVert.push_back(0.0f);
		orbVert.push_back(zz);

	}
	

	/* vao-vbo*/
	GLuint VBO_t, VAO_t;
	glGenVertexArrays(1, &VAO_t);
	glGenBuffers(1, &VBO_t);
	glBindVertexArray(VAO_t);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_t);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*orbVert.size(), orbVert.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);





	unsigned int texture_earth = loadTexture("resources/planets/earth2k.jpg");
	unsigned int t_sun = loadTexture("resources/planets/2k_sun.jpg");
	unsigned int texture_moon = loadTexture("resources/planets/2k_moon.jpg");
	unsigned int texture_mercury = loadTexture("resources/planets/2k_mercury.jpg");
	unsigned int texture_venus = loadTexture("resources/planets/2k_mercury.jpg");
	unsigned int texture_mars = loadTexture("resources/planets/2k_mars.jpg");
	unsigned int texture_jupiter = loadTexture("resources/planets/2k_jupiter.jpg");
	unsigned int texture_saturn = loadTexture("resources/planets/2k_saturn.jpg");
	unsigned int texture_uranus = loadTexture("resources/planets/2k_uranus.jpg");
	unsigned int texture_neptune = loadTexture("resources/planets/2k_neptune.jpg");
	unsigned int texture_saturn_ring = loadTexture("resources/planets/r.jpg");
	unsigned int texture_earth_clouds = loadTexture("resources/planets/2k_earth_clouds.jpg");
	


	Sphere Sun(100.0f, 36 * 5, 18 * 5);
	Sphere Mercury(10.0f, 36, 18);
	Sphere Venus(12.0f, 36, 18);
	Sphere Earth(11.8f, 36, 18);
	Sphere Mars(8.0f, 36, 18);
	Sphere Jupiter(40.0f, 36, 18);
	Sphere Saturn(37.0f, 36, 18);
	Sphere Uranus(30.0f, 36, 18);
	Sphere Neptune(30.0f, 36, 19);
	Sphere Moon(5.5f, 36, 18);



	/* te pierwsze sa startowe te drugie po wcisnieciu "e" (tekstury skyboxa)*/
	std::vector<std::string> faces
	{
		"resources/skybox/starfield/starfield_rt.tga",
		"resources/skybox/starfield/starfield_lf.tga",
		"resources/skybox/starfield/starfield_up.tga",
		"resources/skybox/starfield/starfield_dn.tga",
		"resources/skybox/starfield/starfield_ft.tga",
		"resources/skybox/starfield/starfield_bk.tga",
	};
	std::vector<std::string> faces_extra
	{
		"resources/skybox/blue/bkg1_right.png",
		"resources/skybox/blue/bkg1_left.png",
		"resources/skybox/blue/bkg1_top.png",
		"resources/skybox/blue/bkg1_bot.png",
		"resources/skybox/blue/bkg1_front.png",
		"resources/skybox/blue/bkg1_back.png",
	};

	unsigned int cubemapTexture = loadCubemap(faces);
	unsigned int cubemapTextureExtra = loadCubemap(faces_extra);
	GLfloat camX = 10.0f;
	GLfloat camZ = 10.0f;
	
	camera.Position = glm::vec3(0.0f, 250.0f, -450.0f);
	camera.Yaw = 90.0f;
	camera.Pitch = -40.0f;
	glm::mat4 view;
	glm::vec3 PlanetsPositions[9];
	while (!glfwWindowShouldClose(window))
	{
		
		GLfloat currentFrame = (GLfloat)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;


		processInput(window); // input

		if (!onFreeCam)
		{
			SceneRotateY = 0.0f;
			SceneRotateX = 0.0f;
		}
		if (camera.FreeCam || PlanetView > 0)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		// render
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		SimpleShader.Use();

		glm::mat4 model = glm::mat4(1.0f);
	
		double viewX;
		double viewZ;
		glm::vec3 viewPos;
		glm::vec3 lightPosition = glm::vec3(100.0f, 36 * 5, 18 * 5);
		glm::vec3 lightPositionRelativeToCamera = lightPosition - camera.Position;
		SimpleShader.Use();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 10000.0f);
		SimpleShader.setMat4("model", model);
		SimpleShader.setMat4("view", view);
		SimpleShader.setMat4("projection", projection);
		// Set lighting uniforms
		SimpleShader.setVec3("lightPosition", lightPosition);
		SimpleShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f)); // Set the light color to white
		SimpleShader.setVec3("viewPosition", viewPos); // Set the viewer position
		SimpleShader.setFloat("ambientStrength", 0.7f); // Set the ambient strength
		SimpleShader.setFloat("specularStrength", 1.0f); // Set the specular strength
		SimpleShader.setFloat("shininess", 32.0f); // Set the shininess

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, t_sun);

		/* slonce */
		glm::mat4 model_sun;
		model_sun = glm::rotate(model_sun, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.f));
		model_sun = glm::rotate(model_sun, (GLfloat)glfwGetTime() * glm::radians(23.5f) * 0.25f, glm::vec3(0.0f, 0.0f, 1.f));
		model_sun = glm::translate(model_sun, point);
		SimpleShader.setMat4("model", model_sun);

		Sun.Draw();


		/* merkury*/
		glm::mat4 model_mercury;
		double xx = sin(glfwGetTime() * PlanetSpeed) * 100.0f *2.0f *1.3f;
		double zz = cos(glfwGetTime() * PlanetSpeed) * 100.0f *2.0f *1.3f;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_mercury);
		model_mercury = glm::translate(model_mercury, point);
		model_mercury = glm::rotate(model_mercury, glm::radians(SceneRotateY), glm::vec3(1.0f, 0.0f, 0.0f));
		model_mercury = glm::rotate(model_mercury, glm::radians(SceneRotateX), glm::vec3(0.0f, 0.0f, 1.0f));
		model_mercury = glm::translate(model_mercury, glm::vec3(xx, 0.0f, zz));
		PlanetsPositions[0] = glm::vec3(xx, 0.0f, zz);
		model_mercury = glm::rotate(model_mercury, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.f));
		model_mercury = glm::rotate(model_mercury, (GLfloat)glfwGetTime() * glm::radians(-90.0f) * 0.05f, glm::vec3(0.0f, 0.0f, 1.f));
		SimpleShader.setMat4("model", model_mercury);
		Mercury.Draw();
		

		/* wenus */
		glm::mat4 model_venus;
		xx = sin(glfwGetTime() * PlanetSpeed *0.75f) * 100.0f * 3.0f *1.3f;
		zz = cos(glfwGetTime() * PlanetSpeed * 0.75f) * 100.0f * 3.0f *1.3f;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_venus);
		model_venus = glm::translate(model_venus, point);
		model_venus = glm::rotate(model_venus, glm::radians(SceneRotateY), glm::vec3(1.0f, 0.0f, 0.0f));
		model_venus = glm::rotate(model_venus, glm::radians(SceneRotateX), glm::vec3(0.0f, 0.0f, 1.0f));
		model_venus = glm::translate(model_venus, glm::vec3(xx, 0.0f, zz));
		PlanetsPositions[1] = glm::vec3(xx, 0.0f, zz);
		model_venus = glm::rotate(model_venus, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.f));
		model_venus = glm::rotate(model_venus, glm::radians(-132.5f), glm::vec3(0.0f, 1.0f, 0.f));
		model_venus = glm::rotate(model_venus, (GLfloat)glfwGetTime() * glm::radians(-132.5f) * 0.012f, glm::vec3(0.0f, 0.0f, 1.f));
		SimpleShader.setMat4("model", model_venus);
		Venus.Draw();
	

		/* ziemia */
		glm::mat4 model_earth;
		xx = sin(glfwGetTime() * PlanetSpeed * 0.55f) * 100.0f * 4.0f *1.3f;
		zz = cos(glfwGetTime() * PlanetSpeed * 0.55f) * 100.0f * 4.0f *1.3f;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_earth);
		model_earth = glm::translate(model_earth, point);
		model_earth = glm::rotate(model_earth, glm::radians(SceneRotateY), glm::vec3(1.0f, 0.0f, 0.0f));
		model_earth = glm::rotate(model_earth, glm::radians(SceneRotateX), glm::vec3(0.0f, 0.0f, 1.0f));
		model_earth = glm::translate(model_earth, glm::vec3(xx, 0.0f, zz));
		glm::vec3 EarthPoint = glm::vec3(xx, 0.0f, zz);
		PlanetsPositions[2] = glm::vec3(xx, 0.0f, zz);
		model_earth = glm::rotate(model_earth, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.f));
		model_earth = glm::rotate(model_earth, glm::radians(-33.25f), glm::vec3(0.0f, 1.0f, 0.f));
		model_earth = glm::rotate(model_earth, (GLfloat)glfwGetTime() * glm::radians(-33.25f) * 2.0f, glm::vec3(0.0f, 0.0f, 1.f));
		camera.LookAtPos = glm::vec3(model_earth[3][0], model_earth[3][1], model_earth[3][2]);
		SimpleShader.setMat4("model", model_earth);
		Earth.Draw();  
		
		
		


		/* mars */
		glm::mat4 model_mars;
		xx = sin(glfwGetTime() * PlanetSpeed * 0.35f) * 100.0f * 5.0f *1.3f;
		zz = cos(glfwGetTime() * PlanetSpeed * 0.35f) * 100.0f * 5.0f *1.3f;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_mars);
		model_mars = glm::translate(model_mars, point);
		model_mars = glm::rotate(model_mars, glm::radians(SceneRotateY), glm::vec3(1.0f, 0.0f, 0.0f));
		model_mars = glm::rotate(model_mars, glm::radians(SceneRotateX), glm::vec3(0.0f, 0.0f, 1.0f));
		model_mars = glm::translate(model_mars, glm::vec3(xx, 0.0f, zz));
		PlanetsPositions[3] = glm::vec3(xx, 0.0f, zz);
		model_mars = glm::rotate(model_mars, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.f));
		model_mars = glm::rotate(model_mars, glm::radians(-32.4f), glm::vec3(0.0f, 1.0f, 0.f));
		model_mars = glm::rotate(model_mars, (GLfloat)glfwGetTime() * glm::radians(-32.4f) * 2.1f, glm::vec3(0.0f, 0.0f, 1.f));
		SimpleShader.setMat4("model", model_mars);
		Mars.Draw();
	

		/* jowisz */
		glm::mat4 model_jupiter;
		xx = sin(glfwGetTime() * PlanetSpeed * 0.2f) * 100.0f * 6.0f *1.3f;
		zz = cos(glfwGetTime() * PlanetSpeed * 0.2f) * 100.0f * 6.0f *1.3f;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_jupiter);
		model_jupiter = glm::translate(model_jupiter, point);
		model_jupiter = glm::rotate(model_jupiter, glm::radians(SceneRotateY), glm::vec3(1.0f, 0.0f, 0.0f));
		model_jupiter = glm::rotate(model_jupiter, glm::radians(SceneRotateX), glm::vec3(0.0f, 0.0f, 1.0f));
		model_jupiter = glm::translate(model_jupiter, glm::vec3(xx, 0.0f, zz));
		PlanetsPositions[4] = glm::vec3(xx, 0.0f, zz);
		model_jupiter = glm::rotate(model_jupiter, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.f));
		model_jupiter = glm::rotate(model_jupiter, glm::radians(-23.5f), glm::vec3(0.0f, 1.0f, 0.f));
		model_jupiter = glm::rotate(model_jupiter, (GLfloat)glfwGetTime() * glm::radians(-23.5f) * 4.5f, glm::vec3(0.0f, 0.0f, 1.f));
		SimpleShader.setMat4("model", model_jupiter);
		Jupiter.Draw();
	

		/* saturn */
		glm::mat4 model_saturn;
		xx = sin(glfwGetTime() * PlanetSpeed * 0.15f) * 100.0f * 7.0f *1.3f;
		zz = cos(glfwGetTime() * PlanetSpeed * 0.15f) * 100.0f * 7.0f *1.3f;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_saturn);
		model_saturn = glm::translate(model_saturn, point);
		model_saturn = glm::rotate(model_saturn, glm::radians(SceneRotateY), glm::vec3(1.0f, 0.0f, 0.0f));
		model_saturn = glm::rotate(model_saturn, glm::radians(SceneRotateX), glm::vec3(0.0f, 0.0f, 1.0f));
		model_saturn = glm::translate(model_saturn, glm::vec3(xx, 0.0f, zz));
		glm::vec3 SatrunPoint = glm::vec3(xx, 0.0f, zz);
		PlanetsPositions[5] = glm::vec3(xx, 0.0f, zz);
		model_saturn = glm::rotate(model_saturn, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.f));
		model_saturn = glm::rotate(model_saturn, glm::radians(-34.7f), glm::vec3(0.0f, 1.0f, 0.f));
		model_saturn = glm::rotate(model_saturn, (GLfloat)glfwGetTime() * glm::radians(-34.7f) * 4.48f, glm::vec3(0.0f, 0.0f, 1.f));
		SimpleShader.setMat4("model", model_saturn);
		Saturn.Draw();
	

		/* uran */
		glm::mat4 model_uranus;
		xx = sin(glfwGetTime() * PlanetSpeed * 0.1f) * 100.0f * 8.0f *1.3f;
		zz = cos(glfwGetTime() * PlanetSpeed * 0.1f) * 100.0f * 8.0f *1.3f;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_uranus);
		model_uranus = glm::translate(model_uranus, point);
		model_uranus = glm::rotate(model_uranus, glm::radians(SceneRotateY), glm::vec3(1.0f, 0.0f, 0.0f));
		model_uranus = glm::rotate(model_uranus, glm::radians(SceneRotateX), glm::vec3(0.0f, 0.0f, 1.0f));
		model_uranus = glm::translate(model_uranus, glm::vec3(xx, 0.0f, zz));
		PlanetsPositions[6] = glm::vec3(xx, 0.0f, zz);
		model_uranus = glm::rotate(model_uranus, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.f));
		model_uranus = glm::rotate(model_uranus, glm::radians(-99.0f), glm::vec3(0.0f, 1.0f, 0.f));
		model_uranus = glm::rotate(model_uranus, (GLfloat)glfwGetTime() * glm::radians(-99.0f) * 4.5f, glm::vec3(0.0f, 0.0f, 1.f));
		SimpleShader.setMat4("model", model_uranus);
		Uranus.Draw();


		/* neptun */
		glm::mat4 model_neptune;
		xx = sin(glfwGetTime() * PlanetSpeed * 0.08f) * 100.0f * 9.0f *1.3f;
		zz = cos(glfwGetTime() * PlanetSpeed * 0.08f) * 100.0f * 9.0f *1.3f;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_neptune);

		model_neptune = glm::translate(model_neptune, point);
		model_neptune = glm::rotate(model_neptune, glm::radians(SceneRotateY), glm::vec3(1.0f, 0.0f, 0.0f));
		model_neptune = glm::rotate(model_neptune, glm::radians(SceneRotateX), glm::vec3(0.0f, 0.0f, 1.0f));
		model_neptune = glm::translate(model_neptune, glm::vec3(xx, 0.0f, zz));
		PlanetsPositions[7] = glm::vec3(xx, 0.0f, zz);
		model_neptune = glm::rotate(model_neptune, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.f));
		model_neptune = glm::rotate(model_neptune, glm::radians(-30.2f), glm::vec3(0.0f, 1.0f, 0.f));
		model_neptune = glm::rotate(model_neptune, (GLfloat)glfwGetTime() * glm::radians(-30.2f) * 4.0f, glm::vec3(0.0f, 0.0f, 1.f));
		
		SimpleShader.setMat4("model", model_neptune);
		Neptune.Draw();
		

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_venus);




		/* draw skybox */
		glDepthFunc(GL_LEQUAL);  
		SkyboxShader.Use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		SkyboxShader.setMat4("view", view);
		SkyboxShader.setMat4("projection", projection);
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		if (SkyBoxExtra)
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureExtra);
		else
			glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);  
	


		switch (PlanetView)
		{
		case 0:
			view = camera.GetViewMatrix();
			break;
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO_t);
	glDeleteBuffers(1, &VBO_t);
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		SkyBoxExtra = true;

} 

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

unsigned int loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;

	for (unsigned int i = 0; i < faces.size(); i++)
	{
		
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);

		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}
