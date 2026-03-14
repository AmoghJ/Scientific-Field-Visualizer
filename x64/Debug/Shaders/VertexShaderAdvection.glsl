#version 430
layout(location = 0) in vec4 aPos;
layout(location = 1) in vec4 aVel;
layout(location = 2) in float aAge;
layout(location = 3) in vec3 aTrailPos;

uniform mat4 uMVP;
uniform int uTrailLength;

out vec3 velocity_vs;
out float age_vs;
out float trailPos_vs;
out vec3 pos_vs;

void main() {
    velocity_vs = aVel.xyz;
    age_vs = aAge;
    pos_vs = aTrailPos;

    //gl_Position = uMVP * vec4(aPos.xyz, 1.0);

    int trailIdx = gl_VertexID % uTrailLength;
    trailPos_vs = float(trailIdx) / float(uTrailLength - 1);

    gl_Position = uMVP * vec4(aTrailPos, 1.0);
}