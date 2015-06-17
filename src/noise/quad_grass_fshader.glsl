#version 430 core

in vec2 uv;

layout (location = 0) out vec4 tex_out_height;
layout (location = 1) out vec4 tex_out_grass;

// Input textures
layout (location = 0) uniform sampler2D tex_in_height;
layout (location = 1) uniform sampler2D tex_in_grass;

uniform int grass_enable;

uniform float grass_max_slope;
uniform float grass_min_angle;
uniform float grass_max_height;
uniform float grass_min_height;

uniform int time;

uniform int action;

uniform float DX;
uniform float DY;
uniform float DZ;

const int ACTION_INIT = 0;
const int ACTION_SLIDE = 1;
const int ACTION_SMOOTH = 2;

float random(vec2 xy, float seed) {
	return fract(sin(dot(vec2(xy[0] + seed, xy[1] - seed), vec2(12.9898,78.233))) * 43758.5453);
}

void main() {

	float height = texture(tex_in_height, uv)[0];
	float grass = texture(tex_in_grass, uv)[0];

	if ( action == ACTION_INIT ) {
		vec2 uv_1 = uv + vec2(DX, 0);
		vec2 uv_2 = uv + vec2(-DX, 0);
		
		vec2 uv_3 = uv + vec2(0, DY);
		vec2 uv_4 = uv + vec2(0, -DY);

		float height_1 = texture(tex_in_height, uv_1).x;
		float height_2 = texture(tex_in_height, uv_2).x;
		
		float height_3 = texture(tex_in_height, uv_3).x;
		float height_4 = texture(tex_in_height, uv_4).x;

		float slope12 = (height_1 - height_2) / (2 * DX);
		float slope34 = (height_3 - height_4) / (2 * DX);
		
		float angle = asin(sqrt((slope12 * slope12) + (slope34 * slope34)) / sqrt(1 + (slope12 * slope12) + (slope34 * slope34)));

		if (height > grass_min_height && height < grass_max_height) {
			if (angle < grass_min_angle) {	
				grass = 1;
			}
			else {
				grass = 0;
			}
		}
	} 
	else if ( action == ACTION_SLIDE ) {		
		if ( grass != 1 ) {
			for(int i = -1; i <= 1; i++) {
				for(int j = -1; j <= 1; j++) {
					if (i != 0 || j != 0) { 
						vec2 uv_ij = uv + vec2(i, j) * vec2(DX, DY);
						float slope;

						float height_ij = texture(tex_in_height, uv_ij).x;
						float grass_ij = texture(tex_in_grass, uv_ij).x;
						if(abs(i) + abs(j) == 1) {
							slope = (height - height_ij) / DX;
						} else {
							slope = (height - height_ij) / DZ;
						}

						float diff = 1 - ((grass_max_slope - slope) / grass_max_slope);
						float rand = 2 * random(uv, time) - 1;
						
						if (height > grass_min_height && grass_ij == 1 && abs(slope) < grass_max_slope){
							//if (rand > diff) {
								grass= 1;
							//}
						}
					}
				}
			}
		}
	} else if (action == ACTION_SMOOTH) {
		float sum;
		for(int i = -1; i <= 1; i++) {
			for(int j = -1; j <= 1; j++) {
					vec2 uv_ij = uv + vec2(i, j) * vec2(DX, DY);
					float grass_ij = texture(tex_in_grass, uv_ij)[0];

					sum += grass_ij;
			}
		}
		grass = sum / 9.0;
	}
	tex_out_height = vec4(height, 0, 0, 1);
	tex_out_grass = vec4(grass, 0, 0, 1);
}