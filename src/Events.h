#pragma once

#include <vector>
#include "glm/glm.hpp"

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
	std::vector<glm::vec3>* pos;
	std::vector<glm::vec3>* normals;
	std::vector<float>* scalarField;
};