#version 430
layout(location = 0) in vec4 aPos;
layout(location = 1) in vec4 aNormal;
layout(location = 2) in float aScalar;
layout(location = 3) in vec4 aDisp;

uniform mat4 uMVP;
uniform float uDisplacementScale;

out vec3 normal;
out float scalar;
out vec3 vBary;

void main() {
    normal = aNormal.xyz;
    scalar = aScalar;

    int id = gl_VertexID % 3;
    if (id == 0)      vBary = vec3(1, 0, 0);
    else if (id == 1) vBary = vec3(0, 1, 0);
    else              vBary = vec3(0, 0, 1);

    vec3 displaced = aPos.xyz + aDisp.xyz * uDisplacementScale;
    gl_Position = uMVP * vec4(displaced, 1.0);
}