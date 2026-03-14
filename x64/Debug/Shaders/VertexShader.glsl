#version 430
layout(location = 0) in vec4 aPos;
layout(location = 1) in vec4 aNormal;
layout(location = 2) in float aScalar;
layout(location = 3) in vec4 aDisp;

uniform mat4 uMVP;
uniform float uDisplacementScale;

out vec3 normal_vs;
out float scalar_vs;
out vec3 vBary_vs;
out vec3 worldPos_vs;

void main() {
    normal_vs = aNormal.xyz;
    scalar_vs = aScalar;

    int id = gl_VertexID % 3;
    if (id == 0)      vBary_vs = vec3(1, 0, 0);
    else if (id == 1) vBary_vs = vec3(0, 1, 0);
    else              vBary_vs = vec3(0, 0, 1);

    vec3 displaced = aPos.xyz + aDisp.xyz * uDisplacementScale;
    worldPos_vs = displaced;
    gl_Position = uMVP * vec4(displaced, 1.0);
}