#version 330 core

uniform mat4 mvp;
uniform mat4 mvp_light;

uniform sampler2D tex_height;
uniform sampler2D tex_shadow;

uniform int grid_enable;
uniform int grid_length;
uniform int grid_width;

uniform int mode;

in vec2 vertex_pos;

out vec2 uv;
out vec4 pos_to_light;

const int NORMAL = 0;
const int ONLY_REFLECT = 1;
const int ILLUMINATE = 2;

void main() {
    uv = vertex_pos + vec2(0.5, 0.5);

	float height = texture(tex_height, uv).rgb[0];

	vec3 vertex_pos_3d;
	if (grid_enable == 1) {
		vertex_pos_3d = vec3(vertex_pos.x * grid_length, height, vertex_pos.y * grid_width);
	} else {
		vertex_pos_3d = vec3(vertex_pos.x * grid_length, 0, vertex_pos.y * grid_width);
	}

	if (mode == ILLUMINATE) {
		gl_Position = mvp_light * vec4(vertex_pos_3d, 1.0);
	} else {
		gl_Position = mvp * vec4(vertex_pos_3d, 1.0);
		pos_to_light = mvp_light * vec4(vertex_pos_3d, 1.0);
	}
}