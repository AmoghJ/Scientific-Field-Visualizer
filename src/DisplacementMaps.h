#pragma once

#include <functional>
#include <string>
#include "glm/glm.hpp"

using DispFunc = std::function<glm::vec3(glm::vec3)>;

extern std::string dispFuncLabels[8];
extern DispFunc dispFunctionsArray[8];


extern DispFunc twist;

// Ripple from center — expanding rings
extern DispFunc ripple;

// Noise-like curl — swirling displacement
extern DispFunc curl;

// Pinch toward axis — sucked into Y axis
extern DispFunc pinch;

// Shockwave ring — displacement peaks at a specific radius
extern DispFunc shockwave;

// Squish and stretch — compress Y, expand XZ
extern DispFunc squish;

// Twisted spike — spiral outward from top
extern DispFunc spike;

// Breathing — pulsate along surface normal direction
extern DispFunc breathe;
