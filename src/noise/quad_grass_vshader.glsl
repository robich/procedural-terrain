#version 330 core

in vec3 vertex_pos;
out vec2 uv;

uniform int noise_width;
uniform int noise_height;

void main() {
    gl_Position = vec4(vertex_pos, 1.0);

    uv = ((vertex_pos + vec2(1.0, 1.0)) * 0.5).xy;
}
