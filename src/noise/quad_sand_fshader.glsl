#version 430 core

in vec2 uv;

layout (location = 0) out vec4 tex_out_height;
layout (location = 1) out vec4 tex_out_sand;
layout (location = 2) out vec4 tex_out_pos;

// Input textures
layout (location = 0) uniform sampler2D tex_in_height;
layout (location = 1) uniform sampler2D tex_in_sand;
layout (location = 2) uniform sampler2D tex_in_pos;

uniform int sand_enable;

uniform float sand_amount;
uniform float sand_max_slope;
uniform float sand_min_height;
uniform float sand_max_height;
uniform float sand_max_amount;
uniform float sand_threshold;

uniform int sand_slide_time;
uniform int sand_melt_time;
uniform int sand_smooth_time;

uniform float DX;
uniform float DY;
uniform float DZ;

uniform int action;

const int ACTION_FALL = 0;
const int ACTION_SLIDE = 1;
const int ACTION_MELT = 2;
const int ACTION_SMOOTH = 3;

void main() {

	float height = texture(tex_in_height, uv)[0];
	
	if (action == ACTION_FALL) {

		float height = texture(tex_in_height, uv)[0];
		float sand;

		float height_R = texture(tex_in_height, uv + vec2(DX, 0)).x;
		float height_L = texture(tex_in_height, uv - vec2(DX, 0)).x;
		float height_T = texture(tex_in_height, uv + vec2(0, DY)).x;
		float height_B = texture(tex_in_height, uv - vec2(0, DY)).x;

		float slope_LR = (height_L - height_R) / (2 * DX);
		float slope_BT = (height_B - height_T) / (2 * DX);

		float a = slope_LR * slope_LR + slope_BT * slope_BT;
		
		float angle = abs(asin(sqrt(a) / sqrt(1 + a)));

		if (height > sand_min_height && height < sand_max_height && angle < 1.32) {
			sand = 1;
		} else {
			sand = 0.0;
		}

		tex_out_sand = vec4(sand, 0, 0, 1);
	}
	
	else if (action == ACTION_SLIDE) {
		
		float height = texture(tex_in_height, uv)[0];
		float sand = texture(tex_in_sand, uv)[0];
		float level = height + sand;
			
		float min_level = level;
		float min_i = 0;
		float min_j = 0;
		for(int i = -1; i <= 1; i++) {
			for(int j = -1; j <= 1; j++) {
				if (i != 0 || j != 0) { 
					vec2 uv_ij = uv + vec2(i, j) * vec2(DX, DY);

					vec4 height_ij = texture(tex_in_height, uv_ij);
					vec4 sand_ij = texture(tex_in_sand, uv_ij);
					vec4 pos_ij = texture(tex_in_pos, uv_ij);

					float level_ij = height_ij[0] + sand_ij[0];

					if (pos_ij[0] == -i && pos_ij[1] == -j) {
						sand += sand_ij[1];
					}

					if (level_ij < min_level) {
						min_i = i;
						min_j = j;
						min_level = level_ij;
					}
				}
			}
		}

		float next_sand;
		if (min_i != 0 && min_j != 0){

			float slope = (level - min_level) / DZ;
			if (slope > sand_max_slope) {
				float diff = (level - min_level) - DZ * sand_max_slope;
				float extra = min(diff, sand);
			
				sand = sand - extra;
				next_sand = extra;
			}
		} else if (min_i != 0 || min_j != 0){

			float slope = (level - min_level) / DX;
			if (slope > sand_max_slope) {
				float diff = (level - min_level) - DX * sand_max_slope;
				float extra = min(diff, sand);
				sand -= extra;
				next_sand = extra;
			}
		}

		sand = min(sand, sand_amount * sand_max_amount);
		sand = max(sand, 0);

		tex_out_height = vec4(height, 0, 0, 1);
		tex_out_sand = vec4(sand, next_sand, 0, 1);
		tex_out_pos = vec4(min_i, min_j, 0, 1);
	}
	
	else if (action == ACTION_MELT) {
		float height = texture(tex_in_height, uv)[0];
		float sand = texture(tex_in_sand, uv)[0];

		if (sand > sand_threshold) {
			int count = 0;
			for(int i = -1; i <= 1; i++) {
				for(int j = -1; j <= 1; j++) {
					if (i != 0 || j != 0) { 
						vec2 uv_ij = uv + vec2(i, j) * vec2(DX, DY);
						float sand_ij = texture(tex_in_sand, uv_ij)[0];

						if (sand_ij > sand_threshold){
							count++;
						}	
					}
				}
			}
			if (count < 6) {
				sand = 0;
			} else {
				sand = 1;
			}
		} else {
			sand = 0;
		}

		tex_out_height = vec4(height, 0, 0, 1);
		tex_out_sand = vec4(sand, 0, 0, 1);
	}

	else if (action == ACTION_SMOOTH) {
		float height = texture(tex_in_height, uv)[0];
		float sand = texture(tex_in_sand, uv)[0];

			
		float sand_sum = 8 * sand;
		float height_sum =  height;

		for(int i = -1; i <= 1; i++) {
			for(int j = -1; j <= 1; j++) {
				if (i != 0 || j != 0) { 
					vec2 uv_ij = uv + vec2(i, j) * vec2(DX, DY);
					float height_ij = texture(tex_in_height, uv_ij)[0];
					float sand_ij = texture(tex_in_sand, uv_ij)[0];

					if (abs(i) + abs(j) == 1) {
						sand_sum += 2 * sand_ij;
						height_sum += height_ij;
					} else {
						sand_sum += sand_ij;
						height_sum += height_ij;
					}
				}
			}
		}

		sand = sand_sum / 20.0;
		height = height_sum / 9.0;

		tex_out_height = vec4(height, 0, 0, 1);
		tex_out_sand = vec4(sand, 0, 0, 1);
	}
}