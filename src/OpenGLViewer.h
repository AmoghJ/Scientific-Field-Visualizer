#pragma once

#include "Component.h"
#include "GL/glew.h"
#include <string>
#include <glm/glm.hpp>

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

	glm::vec3 backgroundColor;

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
	std::string geomShaderName = "GeometryShader.glsl";
	FileMonitor vertMonitor, fragMonitor;
	bool reloadShader = true;

	void updateShader();
	void updateViewportSize(int width, int height);

	GLuint vao, vbo, nVbo, sVbo, dVbo;

	int numVertices = 0;

	GLuint mvpLocation;
	glm::mat4 mvp; //Local copy of mvp matrix - update when camera sends event

	float lastScrollY = 0.0f; //TODO: Need better workaround, right now doing dirty way to save time

	GLuint colorMapSelectorLocation;
	std::string transferFuncLabels[8] = { "Viridis", "Jet", "Plasma", 
					"Inferno", "Hot", "CoolWarm", "Rainbow", "Magma"};
	enum TransferFunctions {
		Viridis, Jet, Plasma, Inferno, Hot, CoolWarm, Rainbow, Magma
	} currentTransferFunc;

	bool wireframe = true;
	GLuint wireframeToggleLocation;

	float scalarRange[2] = { 0.0f, 1.0f };
	GLuint rangeMinLocation, rangeMaxLocation;

	bool showIsoline = false;
	float isolineValue = 0.85f;
	GLuint isolineBoolLocation, isolineValueLocation;

	float dispScale = 0.0f;
	GLuint displacementScaleLocation;
};