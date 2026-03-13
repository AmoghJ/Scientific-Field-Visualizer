#pragma once

#include "Component.h"
#include "GL/glew.h"
#include <string>

#include "FileMonitor.h"

class Container;

/*
* The opengl viewer component that manages rendering and opengl states.
* It recieves events from the model component and control component to
* appropriate display the geometry.
* 
* It is also responsible for compiling shaders, setting uniforms, and exposing
* the various visualization options.
*/
class OpenGLViewer : public Component {
public:
	OpenGLViewer(Container* cont);
	~OpenGLViewer();

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
	std::string fragShaderName = "FragmentShader.glsl";
	std::string vertShaderName = "VertexShader.glsl";
	FileMonitor vertMonitor, fragMonitor;
	bool reloadShader = true;

	void updateShader();

	GLuint vao, vbo, ebo;
};