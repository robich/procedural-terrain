#version 430 core

in vec2 uv;

layout (location = 0) out vec4 tex_out_height;
layout (location = 1) out vec4 tex_out_snow;
layout (location = 2) out vec4 tex_out_pos;

// Input textures
layout (location = 0) uniform sampler2D tex_in_height;
layout (location = 1) uniform sampler2D tex_in_snow;
layout (location = 2) uniform sampler2D tex_in_pos;

uniform int snow_enable;

uniform float snow_amount;
uniform float snow_max_slope;
uniform float snow_min_height;
uniform float snow_max_amount;
uniform float snow_threshold;

uniform int snow_slide_time;
uniform int snow_melt_time;
uniform int snow_smooth_time;

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

	if (height > snow_min_height) {
		if (action == ACTION_FALL) {
			float height = texture(tex_in_height, uv)[0];
			float diff = height - snow_min_height;

			float snow = min(diff, snow_amount);

			snow = min(snow, snow_amount * snow_max_amount);
			snow = max(snow, 0);
			
			tex_out_height = vec4(height, 0, 0, 1);
			tex_out_snow = vec4(snow, 0, 0, 1);
		}

		if (action == ACTION_SLIDE) {
		
			float height = texture(tex_in_height, uv)[0];
			float snow = texture(tex_in_snow, uv)[0];
			float level = height + snow;
			
			float min_level = level;
			float min_i = 0;
			float min_j = 0;
			for(int i = -1; i <= 1; i++) {
				for(int j = -1; j <= 1; j++) {
					if (i != 0 || j != 0) { 
						vec2 uv_ij = uv + vec2(i, j) * vec2(DX, DY);

						vec4 height_ij = texture(tex_in_height, uv_ij);
						vec4 snow_ij = texture(tex_in_snow, uv_ij);
						vec4 pos_ij = texture(tex_in_pos, uv_ij);

						float level_ij = height_ij[0] + snow_ij[0];

						if (pos_ij[0] == -i && pos_ij[1] == -j) {
							snow += snow_ij[1];
						}

						if (level_ij < min_level) {
							min_i = i;
							min_j = j;
							min_level = level_ij;
						}
					}
				}
			}

			float next_snow;
			if (min_i != 0 && min_j != 0){

				float slope = (level - min_level) / DZ;
				if (slope > snow_max_slope) {
					float diff = (level - min_level) - DZ * snow_max_slope;
					float extra = min(diff, snow);
			
					snow = snow - extra;
					next_snow = extra;
				}
			} else if (min_i != 0 || min_j != 0){

				float slope = (level - min_level) / DX;
				if (slope > snow_max_slope) {
					float diff = (level - min_level) - DX * snow_max_slope;
					float extra = min(diff, snow);
					snow -= extra;
					next_snow = extra;
				}
			}

			snow = min(snow, snow_amount * snow_max_amount);
			snow = max(snow, 0);

			tex_out_height = vec4(height, 0, 0, 1);
			tex_out_snow = vec4(snow, next_snow, 0, 1);
			tex_out_pos = vec4(min_i, min_j, 0, 1);
		}

		else if (action == ACTION_MELT) {
			float height = texture(tex_in_height, uv)[0];
			float snow = texture(tex_in_snow, uv)[0];

			if (snow > snow_threshold) {
				int count = 0;
				for(int i = -1; i <= 1; i++) {
					for(int j = -1; j <= 1; j++) {
						if (i != 0 || j != 0) { 
							vec2 uv_ij = uv + vec2(i, j) * vec2(DX, DY);
							float snow_ij = texture(tex_in_snow, uv_ij)[0];

							if (snow_ij > snow_threshold){
								count++;
							}	
						}
					}
				}
				if (count < 5) {
					snow = 0;
				} else {
					snow = 1;
				}
			} else {
				snow = 0;
			}

			tex_out_height = vec4(height, 0, 0, 1);
			tex_out_snow = vec4(snow, 0, 0, 1);
		}

		else if (action == ACTION_SMOOTH) {
			float height = texture(tex_in_height, uv)[0];
			float snow = texture(tex_in_snow, uv)[0];

			
			float sum = 8 * snow;

			for(int i = -1; i <= 1; i++) {
				for(int j = -1; j <= 1; j++) {
					if (i != 0 || j != 0) { 
						vec2 uv_ij = uv + vec2(i, j) * vec2(DX, DY);
						float height_ij = texture(tex_in_height, uv_ij)[0];
						float snow_ij = texture(tex_in_snow, uv_ij)[0];

						if (abs(i) + abs(j) == 1) {
							sum += 2 * snow_ij;
						} else {
							sum += snow_ij;
						}
					}
				}
			}

			snow = sum / 20.0;

			tex_out_height = vec4(height, 0, 0, 1);
			tex_out_snow = vec4(snow, 0, 0, 1);
		}
	} else {
		float height = texture(tex_in_height, uv)[0];
		float snow = 0.0; 
		float next_snow = 0.0;

		float min_i = 0.0;
		float min_j = 0.0;

		tex_out_height = vec4(height, 0, 0, 1);
		tex_out_snow = vec4(snow, next_snow, 0, 1);
		tex_out_pos = vec4(min_i, min_j, 0, 1);
	}
}