#version 450 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 instance_position;

layout (location = 0) out vec4 vs_color;

void main() {
    vs_color = vec4(1);
    gl_Position = /*global_uniforms.view_projection * push_constants.transform **/ vec4(in_position + instance_position, 1.0);
}
