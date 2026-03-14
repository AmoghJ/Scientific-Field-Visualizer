#pragma once

#include "Component.h"
#include "GL/glew.h"
#include <string>
#include <glm/glm.hpp>

#include "FileMonitor.h"

class Container;

class AdvectionViewer : public Component {
public:

	glm::vec3 backgroundColor;

	AdvectionViewer(Container* cont);
	~AdvectionViewer();

	void init();
	void update();

	/* Ensure that the render function runs on the main thread for opengl context */
	void render();

	/* Returns the opengl texture id that is being rendered to */
	GLuint getTextureID();

private:
	int width, height;

	GLuint renderTex, renderFbo, rbo;
	GLuint shader;

	//All shaders are in Shaders/ folder at the root of .exe
	std::string shaderDir = "Shaders";
	std::string fragShaderName = "FragmentShaderAdvection.glsl";
	std::string vertShaderName = "VertexShaderAdvection.glsl";
	FileMonitor vertMonitor, fragMonitor;
	bool reloadShader = true;

	void updateShader();
	void updateViewportSize(int width, int height);

	GLuint vao, vbo, velVbo, ageVbo;

	int numVertices = 0;

	GLuint mvpLocation;
	glm::mat4 mvp; //Local copy of mvp matrix - update when camera sends event

	float lastScrollY = 0.0f; //TODO: Need better workaround, right now doing dirty way to save time
};