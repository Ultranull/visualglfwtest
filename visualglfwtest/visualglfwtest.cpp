// visualglfwtest.cpp : Defines the entry point for the console application.
//
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
	//glEnable(GL_CULL_FACE);

	Resource::addTexture("test", "textures/blank.png");
	Resource::addTexture("plane","textures/uvtemplate.bmp");
	Resource::addShader("model", "shaders/modelL.vert", "shaders/modelL.frag");
	Resource::addShader("simple", "shaders/simple.vert", "shaders/simple.frag");
	ShaderProgram prog = Resource::getShader("model");
	ShaderProgram simple = Resource::getShader("simple");
	Camera cam;

	Mesh model("models/suzanne.obj");
	model.setTexture("test");

	Mesh lamp("models/sphere.obj");
	
	Mesh plane("models/plane.obj");
	plane.setTexture("plane");


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
	do {

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

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		prog.bind();
		mat4 M = mat4(1.f);
		mat4 V = cam.V();
		mat4 P = cam.P();
		prog.setUniformMat4("model", M);
		prog.setUniformMat4("view", V);
		prog.setUniformMat4("projection", P);

		prog.setUniformVec3("viewPos", cam.getPosition());
		prog.setUniformVec3("material.ambient", vec3(0));
		prog.setUniformVec3("material.specular", vec3(0.3));
		prog.setUniformf("material.shininess", 5);

		prog.setUniformVec3("lights[0].position", lightPos);
		prog.setUniformVec3("lights[0].ambient", vec3(0));
		prog.setUniformVec3("lights[0].diffuse", vec3(0,0,1));
		prog.setUniformVec3("lights[0].specular", vec3(0));
		prog.setUniformf("lights[0].linear", 0);
		prog.setUniformf("lights[0].quadratic", 1./20.);

		prog.setUniformVec3("lights[1].position", vec3(0,0,-6));
		prog.setUniformVec3("lights[1].ambient", vec3(0));
		prog.setUniformVec3("lights[1].diffuse", vec3(1,0,0));
		prog.setUniformVec3("lights[1].specular", vec3(0));
		prog.setUniformf("lights[1].linear", 0);
		prog.setUniformf("lights[1].quadratic", 1./ 20.);

		prog.setUniformVec3("lights[2].position", vec3(0, 0, 6));
		prog.setUniformVec3("lights[2].ambient", vec3(0));
		prog.setUniformVec3("lights[2].diffuse", vec3(0, 1, 0));
		prog.setUniformVec3("lights[2].specular", vec3(0));
		prog.setUniformf("lights[2].linear", 0);
		prog.setUniformf("lights[2].quadratic", 1./ 20.);

		prog.setUniformVec3("dirlight.direction", vec3(0, -1, 0));
		prog.setUniformVec3("dirlight.ambient", vec3(0.05));
		prog.setUniformVec3("dirlight.diffuse", vec3(.001));
		prog.setUniformVec3("dirlight.specular", vec3(0.2));


		model.render(cam,prog);
		glUseProgram(0);

		prog.bind();
		M = translate(vec3(cos(ticks) * 5, 1, sin(ticks) * 5))* rotate(-ticks, vec3(0, 1, 0));
		V = cam.V();
		P = cam.P();
		prog.setUniformMat4("model", M);
		prog.setUniformMat4("view", V);
		prog.setUniformMat4("projection", P);
		model.render(cam, prog);
		glUseProgram(0);


		prog.bind();
		M = translate(vec3(0, -1, 0));
		V = cam.V();
		P = cam.P();
		prog.setUniformMat4("model", M);
		prog.setUniformMat4("view", V);
		prog.setUniformMat4("projection", P);
		plane.render(cam, prog);
		glUseProgram(0);


		simple.bind();
		mat4 lampMVP = cam.P()*cam.V()*translate(lightPos)*scale(vec3(.5f, .5f, .5f));
		simple.setUniformMat4("MVP", lampMVP);
		simple.setUniformVec3("color", vec3(0,0,1));
		lamp.render(cam, simple);
		glUseProgram(0);

		simple.bind();
		lampMVP = cam.P()*cam.V()*translate(vec3(0,0,-6))*scale(vec3(.5f, .5f, .5f));
		simple.setUniformMat4("MVP", lampMVP);
		simple.setUniformVec3("color", vec3(1, 0, 0));
		lamp.render(cam, simple);
		glUseProgram(0);

		simple.bind();
		lampMVP = cam.P()*cam.V()*translate(vec3(0, 0, 6))*scale(vec3(.5f, .5f, .5f));
		simple.setUniformMat4("MVP", lampMVP);
		simple.setUniformVec3("color", vec3(0, 1, 0));
		lamp.render(cam, simple);
		glUseProgram(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
		viewportinit(window);
		ticks += 1.*delta;
		Sleep(1000 / dfps);
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);


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
