#pragma once

#include <vector>
#include "glm/glm.hpp"
#include "gl/glew.h"

struct CameraMoveEvent {
	glm::vec3 position;
	glm::mat4 mvp;
};

struct ViewerResizeEvent {
	int width, height;
};

struct LeftMouseDragEvent {
	float dX, dY;
};

struct RightMouseDragEvent {
	float dX, dY;
};

struct MouseScrollEvent {
	float scrollY;
};

struct MeshData {
	std::vector<glm::vec4>* pos;
	std::vector<glm::vec4>* normals;
	std::vector<float>* scalarField;
	std::vector<glm::vec4>* displacement;
};

struct GetMeshData {
	GLuint posVbo, normalVbo, scalarVbo, dispVbo;
};

struct GetAdvectionData {
	GLuint posVbo, velVbo, ageVbo, trailVbo;
	int trailLength;
};

struct MeshVertSizeUpdate {
	int numVertices;
};

struct AdvectTrailSizeUpdate {
	int numVertices;
};

struct ChangedLoader {
	int loaderIndex;
};