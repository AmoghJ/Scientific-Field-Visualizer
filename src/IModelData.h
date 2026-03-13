#pragma once

#include <vector>
#include <glm/glm.hpp>

class IModelData {
public:
	IModelData() {}
	virtual ~IModelData() = default;

	std::vector<glm::vec3> vertexPos;
	std::vector<glm::vec3> vertexNormal;
	std::vector<float> scalarField;

	virtual void renderGUI() = 0;
};
