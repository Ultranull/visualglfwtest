#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <stdlib.h>

#include <windows.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <SOIL.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"

#include "Resource.h"
#include "Input.h"
#include "Mesh.h"
#include "Camera.h"
#include "ShaderProgram.h"
#include "Renderer.h"

using namespace glm;
using namespace std;

GLFWwindow * initWindow(int, int, const char *);
void viewportinit(GLFWwindow *window);

int main() {

	GLFWwindow *window = initWindow(700, 600, "test");
	if (window == nullptr)return-1;
	viewportinit(window);

	glClearColor(0.0f, 0.0f, 0.f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	Resource::addTexture("blank", "textures/blank.png");

	Resource::addTexture("barrel"  , "textures/big_diffus.png");
	Resource::addTexture("barrelNM", "textures/big_normal.png");
	Resource::addTexture("barrelSP", "textures/big_specular.png");

	Resource::addTexture("floor",  "textures/rustytiles02_diff.png");
	Resource::addTexture("floorNM","textures/rustytiles02_norm.png");
	Resource::addTexture("floorSM","textures/rustytiles02_spec.png");

	Resource::addTexture("wall",   "textures/stonetiles_003_diff.png");
	Resource::addTexture("wallNM", "textures/stonetiles_003_norm.png");
	Resource::addTexture("wallSM", "textures/stonetiles_003_disp.png");

	Resource::addShader("T", "shaders/T.vert", "shaders/T.frag");
	Resource::addShader("TLN-simple", "shaders/TLN-simple.vert", "shaders/TLN-simple.frag");
	Resource::addShader("TLN", "shaders/TLN.vert", "shaders/TLN.frag","shaders/calcTBN.geom");
	Resource::addShader("TLNS", "shaders/TLN.vert", "shaders/TLNS.frag","shaders/calcTBN.geom");
	Resource::addShader("simple", "shaders/simple.vert", "shaders/simple.frag");

	Resource::addMesh("barrel", "models/big_wood_barrel.obj");
	Resource::addMesh("lamp", "models/sphere.obj");
	Resource::addMesh("plane", "models/plane.obj");
	Camera cam;

	vec3 lightPos=cam.getPosition();
	Input::addInput(GLFW_KEY_SPACE, [&lightPos, &cam](int action,int mod) {
		if (action == GLFW_PRESS)
			lightPos = cam.getPosition();
	});

	double lastFrame = glfwGetTime();
	int frames = 0;
	double lastFPS = glfwGetTime();

	int dfps = 90;

	float ticks = 0;


	
	Renderer::addLight("red",   PointLight(vec3(0, 0,-6), 0, 1. / 20., vec3(0), vec3(1, 0, 0), vec3(1, 0, 0)));
	Renderer::addLight("green", PointLight(vec3(0, 0, 6), 0, 1. / 20., vec3(0), vec3(0, 1, 0), vec3(0, 1, 0)));
	Renderer::addLight("blue",  PointLight(vec3(6, 0, 0), 0, 1. / 20., vec3(0), vec3(0, 0, 1), vec3(0, 0, 1)));
	Renderer::addLight("white", PointLight(lightPos, 0, 1. / 50., vec3(0), vec3(1), vec3(1)));
	Mesh plane = Resource::getMesh("plane"),
		 lamp = Resource::getMesh("lamp"),
		 barrel = Resource::getMesh("barrel");
	const int shlen = 5;
	string sh[shlen] = {"simple","T","TLN-simple","TLN","TLNS" };
	int ind = 0;
	Input::addInput(GLFW_KEY_RIGHT_BRACKET, [&ind] (int act,int mod){
		if (act == GLFW_PRESS)ind++;
	});
	Input::addInput(GLFW_KEY_LEFT_BRACKET, [&ind](int act, int mod) {
		if (act == GLFW_PRESS)ind--;
	});

	do {

		if (ind >= shlen)ind = 0;
		if (ind < 0)ind = shlen-1;

		double currenttime = glfwGetTime();
		frames++;
		if (currenttime - lastFPS >= 1) {
			printf("%d -- %d\n", frames, dfps);
			frames = 0;
			lastFPS += 1.0;
		}
		double delta = currenttime - lastFrame;
		lastFrame = currenttime;

		cam.orbit(window, delta, vec3(0));
		//cam.apply(window, delta);
		Renderer::setCamera(cam);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Renderer::editLight("white", PointLight(lightPos, 0, 1. / 50., vec3(0), vec3(1), vec3(1)));


		Renderer::useShader(Resource::getShader(sh[ind]), Renderer::LIGHTING_RENDER);
		Renderer::useMaterial(Material(vec3(1), vec3(0), 1));
		Renderer::useTexture(    Resource::getTexture("barrel"));
		Renderer::useNormalMap(  Resource::getTexture("barrelNM"));
		Renderer::useSpecularMap(Resource::getTexture("barrelSP"));
		if (ind == 0)
			Renderer::useMaterial(Material(vec3(.4f,0,0), vec3(0), 1));
		Renderer::renderMesh(GL_TRIANGLES, barrel, rotate(ticks, vec3(0, 1, 0))*
												   scale(vec3(1.f/64.f))*
												   rotate(radians(30.f), vec3(1, 0, 0)));

		Renderer::useMaterial(Material(vec3(1), vec3(.003), 50));
		Renderer::useTexture(Resource::getTexture("floor"));
		Renderer::useNormalMap(Resource::getTexture("floorNM"));
		Renderer::useSpecularMap(Resource::getTexture("floorSM"));
		if (ind == 0)
			Renderer::useMaterial(Material(vec3(0, .4f, 0), vec3(0), 1));
		Renderer::renderMesh(GL_TRIANGLES, plane, translate(vec3(0, -1, 0)));
		Renderer::useTexture(    Resource::getTexture("wall"));
		Renderer::useNormalMap(  Resource::getTexture("wallNM"));
		Renderer::useSpecularMap(Resource::getTexture("wallSM"));
		if (ind == 0)
			Renderer::useMaterial(Material(vec3(.3f, .3f, 1), vec3(0), 1));
		Renderer::renderMesh(GL_TRIANGLES, plane, translate(vec3( 0,  9, -10))*rotate(radians( 90.f), vec3(1, 0, 0)));
		Renderer::renderMesh(GL_TRIANGLES, plane, translate(vec3( 0,  9,  10))*rotate(radians(-90.f), vec3(1, 0, 0)));
		Renderer::renderMesh(GL_TRIANGLES, plane, translate(vec3(-10,  9,  0))*
												  rotate(radians(-90.f), vec3(0, 0, 1))*
												  rotate(radians(-90.f), vec3(0, 1, 0)));
		Renderer::renderMesh(GL_TRIANGLES, plane, translate(vec3( 10,  9,  0))*
												  rotate(radians( 90.f), vec3(0, 0, 1))*
												  rotate(radians( 90.f), vec3(0, 1, 0)));



		Renderer::useShader(Resource::getShader("simple"));
		Renderer::useMaterial(Material(vec3(0, 0, 1), vec3(0), 0));
		Renderer::renderMesh(GL_TRIANGLES, lamp, translate(vec3(6, 0, 0))*scale(vec3(.5f, .5f, .5f)));
	

		Renderer::useMaterial(Material(vec3(1, 0, 0), vec3(0), 0));
		Renderer::renderMesh(GL_TRIANGLES, lamp, translate(vec3(0, 0, -6))*scale(vec3(.5f, .5f, .5f)));


		Renderer::useMaterial(Material(vec3(0, 1, 0), vec3(0), 0));
		Renderer::renderMesh(GL_TRIANGLES, lamp, translate(vec3(0, 0, 6))*scale(vec3(.5f, .5f, .5f)));

		Renderer::useMaterial(Material(vec3(1), vec3(0), 0));
		Renderer::renderMesh(GL_TRIANGLES, lamp, translate(lightPos)*scale(vec3(.5f, .5f, .5f)));


		glfwSwapBuffers(window);
		glfwPollEvents();
		viewportinit(window);
		ticks += 1.*delta;
		Sleep(1000 / dfps);
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	Resource::cleanup();
	glfwTerminate();
	/*
		printf("Press enter to continue...");
		getchar();*/
	return 0;
}
void viewportinit(GLFWwindow *window) {
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
}
GLFWwindow * initWindow(int w, int h, const char *title) {
	GLFWwindow *window;
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize glfw\n");
		getchar();
		exit(-1);
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	/*glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);*/

	window = glfwCreateWindow(w, h, title, NULL, NULL);
	if (window == nullptr) {
		fprintf(stderr, "Failed to init window\n");
		getchar();
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(window);
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to load glew\n");
		getchar();
		glfwTerminate();
		exit(-1);
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetKeyCallback(window, Input::callback);
	return window;
}
