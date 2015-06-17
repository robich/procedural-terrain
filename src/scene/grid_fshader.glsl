#version 430 core

// --- Inputs --- //

in vec2 uv;
in vec4 pos_to_light;

// --- Outputs --- //

out vec4 color;

// --- Textures --- //

layout(location = 0) uniform sampler2D tex_height;
layout(location = 1) uniform sampler2D tex_grass;
layout(location = 2) uniform sampler2D tex_sand;
layout(location = 3) uniform sampler2D tex_snow;
layout(location = 4) uniform sampler2D tex_shadow;

layout(location = 10) uniform sampler2D tex_grass_;
layout(location = 11) uniform sampler2D tex_sand_;
layout(location = 12) uniform sampler2D tex_rock_;
layout(location = 13) uniform sampler2D tex_snow_;
layout(location = 14) uniform sampler2D tex_rock_underwater_;

// --- Uniforms --- //

// light_params
uniform int shading_enable_phong;
uniform int shading_enable_shadow;
uniform vec3 shading_light_pos;
uniform vec3 shading_Ia, shading_Id;
uniform float shading_shadow_intensity;

// texture_params

uniform int texture_type;

uniform float snow_threshold;
uniform float sand_threshold;

uniform float sand_min_height;
uniform float sand_max_height;
uniform float grass_max_height;

uniform float sand_max_slope;
uniform float grass_max_slope;

uniform int grass_h_transition;
uniform int grass_s_transition;
uniform int sand_h_transition;
uniform int sand_s_transition;

uniform float water_height;

uniform float DX;
uniform float DY;

uniform float shading_far;
uniform float shading_near;

const int NONE = 0;
const int SHADES = 1;
const int TEXTURE = 2;


uniform int mode;

const int NORMAL = 0;
const int ONLY_REFLECT = 1;
const int ILLUMINATE = 2;

const vec2 poisson_disk[4] = vec2[](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);

void main() {

	float height = texture(tex_height, uv).rgb[0];
	float grass = texture(tex_grass, uv)[0];
	float sand = texture(tex_sand, uv)[0];
	float snow = texture(tex_snow, uv)[0];

	if (mode == ONLY_REFLECT && height < water_height) {
		discard;
	}

	vec3 derivative_x = vec3(1, 0, 1000*texture(tex_height, uv + vec2(DX, 0))[0] - 1000*texture(tex_height, uv - vec2(DX, 0)*2*DX)[0]);
	vec3 derivative_y = vec3(0, 1, 1000*texture(tex_height, uv + vec2(0, DX))[0] - 1000*texture(tex_height, uv - vec2(0, DX)*2*DY)[0]);
	vec3 normal = normalize(cross(derivative_x, derivative_y));

	vec3 ambient;
	vec3 diffuse;

	if (texture_type == TEXTURE) {
	
		vec3 color_rock = texture(tex_rock_, uv * vec2(20)).rgb;
		vec3 color_sand = texture(tex_sand_, uv * vec2(80)).rgb;
		vec3 color_snow = texture(tex_snow_, uv * vec2(20)).rgb;
		vec3 color_grass = texture(tex_grass_, uv * vec2(20)).rgb;
		vec3 color_rock_underwater = texture(tex_rock_underwater_, uv * vec2(20)).rgb;

		if (grass > 0) {
			ambient = color_grass * grass + (1-grass) * color_rock;
		} else {
   			ambient = color_rock;
		} 

		if (sand > 0) {
			ambient = color_sand * sand + (1 - sand) * ambient;
		}

		if (snow > 0) {
			ambient = color_snow * snow + (1-snow) * ambient;
		} 
	} else if (texture_type == NONE) {
		ambient = vec3(1, 1, 1);
	} else {
		if (snow < snow_threshold){
			ambient = vec3(height + 0.5);
		} else {
			ambient = vec3(snow + 0.5, 0, 0);
		}
	}

	if (shading_enable_phong != 0) {
		ambient *= shading_Ia;
		diffuse = shading_Id * dot(normal, normalize(shading_light_pos));
	}

	vec3 color_unshadowed = vec3(ambient + diffuse);

	
	if (shading_enable_shadow != 0 && mode != ILLUMINATE) {

		float NEAR = shading_near;
		float FAR = shading_far;

		vec2 uv_light = vec2((pos_to_light.x + 1) / 2, (pos_to_light.y + 1) / 2);
		float bias = 0.005;
		float visibility = 1.0;

		float grid_z_b = pos_to_light.z;
		float grid_depth = (grid_z_b * (FAR - NEAR) + FAR + NEAR)/(2.0);

		vec2 tex_shadow_size = textureSize(tex_shadow, 0);

		int count = 0;

		// Poisson sampling
		for (int i = 0; i < 4; i++){

			vec2 uv_light_i = uv_light + poisson_disk[i] / (tex_shadow_size);
		
			float shadow_z_b = texture(tex_shadow, uv_light_i)[0];
			float shadow_z_n = 2.0 * shadow_z_b - 1.0;
			float shadow_depth = (shadow_z_n * (FAR - NEAR) + FAR + NEAR)/(2.0);

			float diff = grid_depth - shadow_depth;

			if ( diff  >  bias ){
				count ++;
			}
		}

		if (count > 1){
			visibility = visibility - (visibility - shading_shadow_intensity);
		}
		
		color = vec4(vec3(visibility) * color_unshadowed, 1.0);
	} else {
		color = vec4(color_unshadowed, 1.0);
	}
}