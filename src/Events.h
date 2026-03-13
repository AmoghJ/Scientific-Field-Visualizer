#pragma once

#include "glm/glm.hpp"

struct CameraMoveEvent {
	glm::vec3 position;
	glm::mat4 mvp;
};

struct MouseDragEvent {
	float dX, dY;
};