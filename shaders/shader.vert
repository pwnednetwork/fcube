#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 instanceOffset;
layout(location = 2) in float intensity;

layout(binding = 0) uniform UniformBufferObject {
    mat4 mvp;
} ubo;

layout(location = 0) out float fragIntensity;

void main() {
    gl_Position = ubo.mvp * vec4(inPosition + instanceOffset, 1.0);
    fragIntensity = intensity;
}
