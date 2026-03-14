#version 430

in vec3 velocity_vs;
in float age_vs;

out vec4 FragColor;

void main() {

    float tAge = age_vs / 5.0f;  // Max age is hardcoded for now
    float tVel = length(velocity_vs) / 4.0f;

    vec3 color = mix(vec3(1.0, 0.5, 0.0), vec3(0.0, 0.5, 1.0), tVel);
    float alpha = 1.0;// - tAge;  // fade out as particle ages

    FragColor = vec4(color, alpha);
}