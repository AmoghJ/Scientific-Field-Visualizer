#version 430
layout(location = 0) in vec4 aPos;
layout(location = 1) in vec4 aVel;
layout(location = 2) in float aAge;

uniform mat4 uMVP;

out vec3 velocity_vs;
out float age_vs;

void main() {
    velocity_vs = aVel.xyz;
    age_vs = aAge;

    gl_Position = uMVP * vec4(aPos.xyz, 1.0);
}