#pragma once

#include <functional>
#include <string>
#include "glm/glm.hpp"

using ScalarFunc = std::function<float(glm::vec3)>;

extern std::string scalarFuncLabels[7];
extern ScalarFunc functionsArray[7];

// Sine ripples from center
extern ScalarFunc sinRipples;

// XYZ product bands
extern ScalarFunc productBands;

// Twisted bands
extern ScalarFunc twistedBands;

// Concentric shells
extern ScalarFunc concShells;

// Diagonal gradient with turbulence
extern ScalarFunc diagonalGrad;

// Polar angle
extern ScalarFunc polarAngle;

// Checkerboard 3D
extern ScalarFunc checkboard;
