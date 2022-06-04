#version 450 core

layout (set = 0, binding = 0) uniform GlobalUniforms {
    mat4 u_ProjectionMatrix;
    mat4 u_ViewMatrix;
};

layout (location = 0) in vec3 in_vertex_position;
layout (location = 1) in vec3 in_position;
layout (location = 2) in vec4 in_color;

layout (location = 0) out vec4 vs_color;

void main() {
    vs_color = in_color;

    vec4 clip_pos = u_ViewMatrix * vec4(in_position, 1.0) + vec4(in_vertex_position, 0.0);
    gl_Position = u_ProjectionMatrix * clip_pos;
}
