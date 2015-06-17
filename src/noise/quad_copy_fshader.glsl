#version 330 core

in vec2 uv;
out vec3 color;

uniform sampler2D tex;

uniform float amplitude;
uniform float offset;

void main() {
	color = texture(tex, uv).xyz * vec3(amplitude) + vec3(offset);
}