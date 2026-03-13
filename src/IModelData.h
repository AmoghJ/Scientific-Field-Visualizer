#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "Component.h"
#include "ScalarMaps.h"

class IModelData : public Component {
public:
	IModelData(Container* cont) : Component(cont) {}
	virtual ~IModelData() = default;

	std::vector<glm::vec3> vertexPos;
	std::vector<glm::vec3> vertexNormal;
	std::vector<float> scalarField;

	int selectedFunction = 0;

	virtual void renderGUI() = 0;
};
