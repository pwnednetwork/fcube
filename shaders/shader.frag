#version 450

layout(location = 0) in float fragIntensity;
layout(location = 0) out vec4 outColor;

void main() {
    outColor = mix(vec4(0, 0, 1, 1), vec4(1, 0, 0, 1), clamp(fragIntensity, 0.0, 1.0));
}
