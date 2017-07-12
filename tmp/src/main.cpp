#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>

#include "Camera.h"
#include "DiffuseShader.h"
#include "LineShader.h"
#include "Matrix3f.h"

#include "Image.h"
#include "PNGDecoder.h"
#include "Texture.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshManager.h"
#include "TextureManager.h"
#include "ShaderManager.h"

static void error_callback(int error, const char* description) {
	fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}


int main() {
	GLFWwindow* window;

	glfwSetErrorCallback(error_callback);

	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	window = glfwCreateWindow(640, 480, "glcheck", NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	MeshManager meshManager;
	TextureManager textureManager;
	ShaderManager shaderManager;

	Texture* texture = textureManager.loadTexture("image.png");

	Shader* shader = shaderManager.getShader("diffuse");
	shader->init();

	Material* material = new Material(texture, shader);
	const Mesh* mesh = meshManager.getBoxMesh(material);

	Camera camera;

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	float ratio = width / (float) height;
	camera.init(1.0f, 1000.0f, 50.0f, ratio);

	//..
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND); // 現在ObjはBLEND無し
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//..

	float head = 0.0f;

	while (!glfwWindowShouldClose(window)) {
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Matrix4f mat;

		Vector4f pos(0.0f, -3.0f, -10.0f, 1.0f);

		head += 0.01f;

		mat.setIdentity();
		mat.setRotationY(head);
		mat.setColumn(3, pos);

		const Matrix4f& cameraInvMat = camera.getInvMat();
		const Matrix4f& projectionMat = camera.getProjectionMat();

		Matrix4f modelViewMat;
		modelViewMat.mul(cameraInvMat, mat);

		Matrix4f modelViewProjectionMat;
		modelViewProjectionMat.mul(projectionMat, modelViewMat);

		//Vector4f color(1.0f, 1.0f, 0.0f, 1.0f);
		//shader->setColor(color);

		mesh->draw(modelViewMat,
				   modelViewProjectionMat);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	glfwTerminate();
	
	exit(EXIT_SUCCESS);
}
