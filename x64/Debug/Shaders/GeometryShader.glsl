#version 430
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 normal_vs[];
in float scalar_vs[];
in vec3 vBary_vs[];
in vec3 worldPos_vs[];

out vec3 normal;
out float scalar;
out vec3 vBary;


void main() {
    vec3 v0 = worldPos_vs[0];
    vec3 v1 = worldPos_vs[1];
    vec3 v2 = worldPos_vs[2];

    vec3 faceNormal = normalize(cross(v1 - v0, v2 - v0));

    for (int i = 0; i < 3; i++) {
        gl_Position = gl_in[i].gl_Position;
        normal      = faceNormal;
        scalar      = scalar_vs[i];
        vBary       = vBary_vs[i];
        EmitVertex();
    }
    EndPrimitive();
}