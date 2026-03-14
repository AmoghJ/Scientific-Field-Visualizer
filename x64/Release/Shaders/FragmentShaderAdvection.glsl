#version 430

in vec3 velocity_vs;
in float age_vs;
in float trailPos_vs;
in vec3 pos_vs;

out vec4 FragColor;

float hash(vec3 p) {
    return fract(sin(dot(p, vec3(127.1, 311.7, 74.7))) * 43758.5453);
}

float smoothNoise(vec3 p) {
    vec3 i = floor(p);
    vec3 f = fract(p);
    vec3 u = f * f * (3.0 - 2.0 * f);

    float a = hash(i + vec3(0,0,0));
    float b = hash(i + vec3(1,0,0));
    float c = hash(i + vec3(0,1,0));
    float d = hash(i + vec3(1,1,0));
    float e = hash(i + vec3(0,0,1));
    float f2= hash(i + vec3(1,0,1));
    float g = hash(i + vec3(0,1,1));
    float h = hash(i + vec3(1,1,1));

    return mix(mix(mix(a,b,u.x), mix(c,d,u.x), u.y),
               mix(mix(e,f2,u.x), mix(g,h,u.x), u.y), u.z);
}

void main() {

    //float tAge = age_vs / 5.0f;  // Max age is hardcoded for now
    //float tVel = length(velocity_vs) / 4.0f;

    //vec3 color = mix(vec3(1.0, 0.5, 0.0), vec3(0.0, 0.5, 1.0), tVel);
    //float alpha = 1.0 - tAge;  // fade out as particle ages

    float alpha = trailPos_vs;
    vec3 color  = mix(vec3(1.0, 0.5, 0.0), vec3(0.0, 0.5, 1.0), smoothNoise(pos_vs * 0.05f));

    FragColor = vec4(color, alpha);
}