#version 430

in vec3 normal;
in float scalar;
in vec3 vBary;

out vec4 FragColor;

uniform int uColorMap;
uniform bool uWireframe;

uniform float uRangeMin;
uniform float uRangeMax;

uniform bool uIsoline;
uniform float uIsolineValue;

vec3 viridis(float t) {
    t = clamp(t, 0.0, 1.0);
    vec3 c0 = vec3(0.0280, 0.0130, 0.0841);
    vec3 c1 = vec3(0.1986, 0.5388, 2.4495);
    vec3 c2 = vec3(0.9728, -1.2664, -1.0280);
    vec3 c3 = vec3(-0.4593, 1.4192, 0.7516);
    vec3 c4 = vec3(0.0283, -0.5150, -0.2530);
    vec3 c5 = vec3(0.0000, 0.0000, 0.0000);
    return clamp(c0 + t*(c1 + t*(c2 + t*(c3 + t*(c4 + t*c5)))), 0.0, 1.0);
}

vec3 jet(float t) {
    t = clamp(t, 0.0, 1.0);
    float r = clamp(1.5 - abs(4.0 * t - 3.0), 0.0, 1.0);
    float g = clamp(1.5 - abs(4.0 * t - 2.0), 0.0, 1.0);
    float b = clamp(1.5 - abs(4.0 * t - 1.0), 0.0, 1.0);
    return vec3(r, g, b);
}

vec3 turbo(float t) {
    t = clamp(t, 0.0, 1.0);
    vec3 c0 = vec3(0.1140, 0.0628, 0.5629);
    vec3 c1 = vec3(4.7130, 2.1669, -0.0990);
    vec3 c2 = vec3(-22.0840, -4.5069, 4.3652);
    vec3 c3 = vec3(40.5980, 5.8073, -8.4520);
    vec3 c4 = vec3(-35.0640, -5.5530, 6.9110);
    vec3 c5 = vec3(11.5520, 1.9560, -1.9450);
    return clamp(c0 + t*(c1 + t*(c2 + t*(c3 + t*(c4 + t*c5)))), 0.0, 1.0);
}

vec3 applyColormap(float t) {
    if (uColorMap == 0) return viridis(t);
    if (uColorMap == 1) return jet(t);
    if (uColorMap == 2) return turbo(t);
    return viridis(t);  // default
}

void main() {

    vec3 sunDir = normalize(vec3(-1.0f, 1.0f, -1.0f));

    float remapped = clamp((scalar - uRangeMin) / (uRangeMax - uRangeMin), 0.0, 1.0);
    vec3 color = applyColormap(remapped);
    float diff = clamp(dot(-normal, sunDir), 0.0, 1.0);
    vec3 lit = color * (0.15 + 0.85 * diff);

   if (uWireframe) {
        float edge = min(min(vBary.x, vBary.y), vBary.z);
        float line = 1.0 - smoothstep(0.0, fwidth(edge) * 0.8f, edge);
        lit = mix(lit, lit * vec3(0.2), line);
   }

    if(uIsoline) {
        float dist = abs(scalar - uIsolineValue);
        float fw = fwidth(scalar);
        float line = 1.0 - smoothstep(0.0, fw * 1.2f, dist);

        float lum = dot(color, vec3(0.299, 0.587, 0.114));  // perceptual luminance
        vec3 isolineColor = lum > 0.5 ? vec3(0.0) : vec3(1.0);
        lit = mix(lit, isolineColor, line);
    }

    FragColor = vec4(lit, 1.0);
}