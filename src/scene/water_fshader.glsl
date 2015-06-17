#version 430 core

in vec2 uv;

out vec4 color;

layout (location = 0) uniform sampler2D tex_mirror;
layout (location = 1) uniform sampler2D tex_height;
layout (location = 2) uniform sampler2D tex_depth;
layout (location = 3) uniform sampler2D tex_normal;

uniform vec3 shading_light_pos;
uniform vec3 shading_Id;

uniform float far;
uniform float near;

uniform int grid_enable;

uniform int water_enable;
uniform vec3 water_color;
uniform	float water_height;
uniform	float water_transparency;
uniform	float water_depth_alpha_factor;
uniform float water_depth_color_factor;
uniform float water_reflection_factor;

uniform float time;

uniform float water_waves_speed;
uniform float water_waves_tile_factor;
uniform float water_waves_amplitude;

const float time_factor = 0.05;

void main() {
    
	if (grid_enable == 0 || water_enable == 0) {
		discard;
	}

	float height = texture(tex_height, uv)[0];
	if (height > water_height) {
		discard;
	}

	// Get the size of the screen (same as size of tex_mirror)
    ivec2 texture_size = textureSize(tex_mirror, 0);

	// Get screen-wide uv coordinates
    float _u = gl_FragCoord[0] / texture_size[0];
    float _v_inv = 1-gl_FragCoord[1] / texture_size[1];
	float _v = gl_FragCoord[1] / texture_size[1];
	vec2 uv_screen = gl_FragCoord.xy / texture_size.xy;
	vec2 uv_screen_inv = vec2(0, 1) + vec2(1, -1) * uv_screen;

	// Compute the depth of the water

	float grid_z_b = texture(tex_depth, vec2(_u, _v))[0];
	float grid_z_n = 2.0 * grid_z_b - 1.0;
	float grid_z_e = 2.0 * near * far / (far + near - grid_z_n * (far - near));

	float water_z_b = gl_FragCoord.z;
	float water_z_n = 2.0 * water_z_b - 1.0;
	float water_z_e = 2.0 * near * far / (far + near - water_z_n * (far - near));

	float depth = grid_z_e - water_z_e;

	// Check if we need to take it into account
	if (depth <= 0) {
		discard;
	}
    
	// Compute ambiant color according to depth & mirror color
	vec3 texture_color = water_color / (1 + depth * water_depth_color_factor);
    

	// Compute alpha
	float alpha = water_transparency * (1 + water_depth_alpha_factor * depth);
    
    // Compute ambiant & diffuse color
   

	vec3 normal = vec3(texture(tex_normal,  uv * vec2(water_waves_tile_factor)  + vec2(time * water_waves_speed))).rgb;

	vec3 mirror_color = texture(tex_mirror, uv_screen_inv + vec2(water_waves_amplitude*normal[0], water_waves_amplitude*normal[1])/texture_size).rgb;
	vec3 ambiant_color = vec3(mix(texture_color, mirror_color, vec3(water_reflection_factor)));

	vec3 diffuse_color =  water_color * (dot(normalize(normal), normalize(shading_light_pos)));

	// Put everything together
	color = vec4(0.8 * ambiant_color + 0.2 * diffuse_color, alpha);
}