#pragma once

#include <memory>
#include "IModelData.h"
#include <string>
#include <map>

struct IMarchingCubesGenerator {

};

struct TorusGenerator : public IMarchingCubesGenerator {

};

struct GyroidGenerator : public IMarchingCubesGenerator {

};

class MarchingCubes : public IModelData {

	float cellSize = 1.0f;

	std::string generatorLabels[2];
	enum Generators {
		Torus, Gyroid
	} currentGenerator;

	IMarchingCubesGenerator* selectedGenerator;
	std::map<Generators, std::unique_ptr<IMarchingCubesGenerator>> generators;

public:
	MarchingCubes();
	~MarchingCubes();

	void renderGUI() override;
};