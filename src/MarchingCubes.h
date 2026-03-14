#pragma once

#include <memory>
#include <functional>
#include "IModelData.h"
#include <string>
#include <map>

#include "gl/glew.h"

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
	void updateMarchingCubeComputeShader();

	void updateScalarField(); //Helper to only update scalar field
	void updateDispField(); //Helper to only update displacement field

	GLuint computeProgram, counterSSBO;
	GLuint edgeTableVbo, triTableVbo;
	GLuint cellSizeLocation, gridSizeLocation,generatorFuncLocation, scalarFuncLocation, dispFuncLocation;

public:
	MarchingCubes(Container* cont);
	~MarchingCubes();

	void init() override;
	void renderGUI() override;
};