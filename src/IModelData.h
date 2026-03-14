#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "Component.h"
#include "ScalarMaps.h"
#include "DisplacementMaps.h"

class IModelData : public Component {
public:
	IModelData(Container* cont) : Component(cont) {}
	virtual ~IModelData() = default;

	std::vector<glm::vec4> vertexPos;
	std::vector<glm::vec4> vertexNormal;
	std::vector<float> scalarField;
	std::vector<glm::vec4> displacement;

	int selectedScalarFunction = 0;
	int selectedDispFunction = 0;

	virtual void init() = 0;
	virtual void renderGUI() = 0;
};
