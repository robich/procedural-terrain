#version 430 core

in vec3 fragment_pos;
in vec2 uv;

out vec4 color;

layout (location = 0) uniform sampler2D tex_height;
layout (location = 1) uniform sampler2D tex_water_depth;

uniform int grid_enable;

uniform int water_enable;
uniform vec3 water_color;
uniform	float water_height;
uniform	float water_transparency;
uniform	float water_depth_alpha_factor;
uniform float water_depth_color_factor;
uniform float water_reflection_factor;

uniform float NEAR;
uniform float FAR;

void main() {

	if (grid_enable == 0) {
		if (fragment_pos[1] > 0) {
			discard;
		} else {
			color = vec4(0.1, 0.1, 0.1, 1.0);
		}
	} else {
		float grid_height = texture(tex_height, uv)[0];
		
		// Check if we need to take it into account
		if (fragment_pos[1] < grid_height) {
			// Solid base
			color = vec4(0.1, 0.1, 0.1, 1.0);
		} else if (fragment_pos[1] < water_height && water_enable == 1) {
			// Get the size of the screen (same as size of tex_mirror)
			ivec2 texture_size = textureSize(tex_water_depth, 0);

			// Get screen-wide uv coordinates
			float _u = gl_FragCoord[0] / texture_size[0];
			float _v = gl_FragCoord[1] / texture_size[1];

			// Compute the depth of the water

			float grid_z_b = texture(tex_water_depth, vec2(_u, _v))[0];
			float grid_z_n = 2.0 * grid_z_b - 1.0;
			float grid_z_e = 2.0 * NEAR * FAR / (FAR + NEAR - grid_z_n * (FAR - NEAR));

			float water_z_b = gl_FragCoord.z;
			float water_z_n = 2.0 * water_z_b - 1.0;
			float water_z_e = 2.0 * NEAR * FAR / (FAR + NEAR - water_z_n * (FAR - NEAR));

			float depth = grid_z_e - water_z_e;
    
			// Compute color & alpha
			vec3 color_tot = water_color / (1 + depth * water_depth_color_factor);
			float alpha = water_transparency * (1 + water_depth_alpha_factor * depth);

			// Put everything together
			color = vec4(color_tot, alpha);
		} else {
			// Air
			discard;
		}
	}
}