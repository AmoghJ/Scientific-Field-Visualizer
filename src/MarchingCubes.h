#pragma once

#include <memory>
#include <functional>
#include "IModelData.h"
#include <string>
#include <map>

class Container;

using GeneratorFunction = std::function<float(glm::vec3)>;

class MarchingCubes : public IModelData {

	float cellSize = 1.0f;
	float isolevel = 0.5f;

	std::string generatorLabels[2];
	enum Generators {
		Torus, Gyroid
	} currentGenerator;

	std::map<Generators, GeneratorFunction> generators;

	void updateMarchingCube();
	void updateScalarField(); //Helper to only update scalar field

public:
	MarchingCubes(Container* cont);
	~MarchingCubes();

	void renderGUI() override;
};