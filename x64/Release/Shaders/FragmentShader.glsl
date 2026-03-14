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


vec3 plasma(float t) {
    t = clamp(t, 0.0, 1.0);
    vec3 c0 = vec3(0.0504, 0.0298, 0.5280);
    vec3 c1 = vec3(2.8776, 0.6138, 1.6276);
    vec3 c2 = vec3(-2.9938, 2.7779, -6.9460);
    vec3 c3 = vec3(0.8718, -3.9772, 12.1463);
    vec3 c4 = vec3(0.5734, 2.4537, -8.8154);
    vec3 c5 = vec3(-0.3882, -0.8796, 2.2711);
    return clamp(c0 + t*(c1 + t*(c2 + t*(c3 + t*(c4 + t*c5)))), 0.0, 1.0);
}

vec3 inferno(float t) {
    t = clamp(t, 0.0, 1.0);
    vec3 c0 = vec3(0.0002, 0.0016, 0.0139);
    vec3 c1 = vec3(0.6226, 0.2285, 1.0587);
    vec3 c2 = vec3(0.5266, 0.8138, -1.5834);
    vec3 c3 = vec3(0.8481, -1.7473, 1.4370);
    vec3 c4 = vec3(-0.5775, 1.1384, -0.1920);
    vec3 c5 = vec3(0.0000, 0.0000, 0.0000);
    return clamp(c0 + t*(c1 + t*(c2 + t*(c3 + t*(c4 + t*c5)))), 0.0, 1.0);
}


vec3 hot(float t) {
    t = clamp(t, 0.0, 1.0);
    return clamp(vec3(
        t * 3.0,
        t * 3.0 - 1.0,
        t * 3.0 - 2.0
    ), 0.0, 1.0);
}


vec3 coolwarm(float t) {
    t = clamp(t, 0.0, 1.0);
    vec3 cold = vec3(0.085, 0.532, 0.201) * (1.0 - t);
    vec3 warm = vec3(0.865, 0.865, 0.865) * t;
    vec3 mid  = vec3(0.085, 0.532, 0.865) * (1.0 - abs(t * 2.0 - 1.0));
    return clamp(vec3(
        mix(0.230, 0.706, t),
        mix(0.299, 0.016, t),
        mix(0.754, 0.150, t)
    ), 0.0, 1.0);
}


vec3 rainbow(float t) {
    t = clamp(t, 0.0, 1.0);
    float r = abs(t * 6.0 - 3.0) - 1.0;
    float g = 2.0 - abs(t * 6.0 - 2.0);
    float b = 2.0 - abs(t * 6.0 - 4.0);
    return clamp(vec3(r, g, b), 0.0, 1.0);
}


vec3 magma(float t) {
    t = clamp(t, 0.0, 1.0);
    vec3 c0 = vec3(0.0016, 0.0002, 0.0139);
    vec3 c1 = vec3(0.7276, 0.1759, 1.0507);
    vec3 c2 = vec3(0.2898, 0.9988, -1.3966);
    vec3 c3 = vec3(0.7277, -1.8416, 1.3925);
    vec3 c4 = vec3(-0.5757, 1.0951, -0.3077);
    vec3 c5 = vec3(0.0000, 0.0000, 0.0000);
    return clamp(c0 + t*(c1 + t*(c2 + t*(c3 + t*(c4 + t*c5)))), 0.0, 1.0);
}

vec3 applyColormap(float t) {
    if (uColorMap == 0) return viridis(t);
    if (uColorMap == 1) return jet(t);
    if (uColorMap == 2) return plasma(t);
    if (uColorMap == 3) return inferno(t);
    if (uColorMap == 4) return hot(t);
    if (uColorMap == 5) return coolwarm(t);
    if (uColorMap == 6) return rainbow(t);
    if (uColorMap == 7) return magma(t);
    return jet(t);  // default
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