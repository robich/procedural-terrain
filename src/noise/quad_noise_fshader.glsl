#version 330 core

in vec2 uv;
out vec3 color;

uniform int noise_type;
uniform int fractal_type;

uniform int noise_effect;
uniform int fractal_effect;

uniform float noise_width;
uniform float noise_height;

uniform float noise_amplitude;
uniform float noise_offset;
uniform float fractal_amplitude;
uniform float fractal_offset;

uniform float noise_seed;

uniform int draw_or_copy;
uniform sampler2D in_texture;
uniform int is_texture;

const int DRAW = 0;
const int COPY = 1;

const int NO_NOISE = 0;
const int RANDOM_NOISE = 1;
const int PERLIN_NOISE = 2;
const int WORLEY_NOISE = 3;

const int NONE = 0;
const int ABSOLUTE_VALUE = 1;
const int CLAMP_EXTREMAS = 2;
const int DISCRETIZE = 3;

const int FBM = 0;
const int MULTIFRACTAL = 1;

float f(float t) {
	return 6 * pow(t, 5) - 15 * pow(t, 4) + 10 * pow(t, 3);
}

float random(vec2 xy, float seed) {
	return fract(sin(dot(vec2(xy[0] + seed,xy[1] - seed), vec2(12.9898,78.233))) * 43758.5453);
}

float clampToInterval(float value, float current_min, float current_max, float expected_min, float expected_max) {
	return (value - current_min) / (current_max - current_min) * (expected_max - expected_min) + expected_min;
}

void main() {

	float noise = 0;
	
	if (draw_or_copy == DRAW) {

		// --- Compute Noise --- //

		if (noise_type == NO_NOISE) {

			noise = 0.0;

		} else if (noise_type == RANDOM_NOISE) {

			float x = uv[0] * noise_width;
			float y = uv[1] * noise_height;

			x = floor(x);
			y = floor(y);

			// Pseudorandom
			noise = 2 * random(vec2(x, y), noise_seed) - 1;

		} else if (noise_type == PERLIN_NOISE) {

			float x = uv[0] * noise_width;
			float y = uv[1] * noise_height;
	
			// Coordinate inside the cell
			float x_in_cell = fract(x);
			float y_in_cell = fract(y);

			// Coordinate of the cell (bottom left corner)
			float x_of_cell = x - x_in_cell;
			float y_of_cell = y - y_in_cell;

			// Get corner coordinates
			vec2 bottom_left = vec2(x_of_cell, y_of_cell);
			vec2 bottom_right = vec2(x_of_cell + 1, y_of_cell);
			vec2 top_left = vec2(x_of_cell, y_of_cell + 1);
			vec2 top_right = vec2(x_of_cell + 1, y_of_cell + 1);

			vec2 g_s = vec2(random(bottom_left, noise_seed) - 0.5, random(bottom_left, 1-noise_seed) - 0.5);
			vec2 g_t = vec2(random(bottom_right, noise_seed) - 0.5, random(bottom_right, 1-noise_seed) - 0.5);
			vec2 g_u = vec2(random(top_left, noise_seed) - 0.5, random(top_left, 1-noise_seed) - 0.5);
			vec2 g_v = vec2(random(top_right, noise_seed) - 0.5, random(top_right, 1-noise_seed) - 0.5);

			vec2 a = vec2(x_in_cell, y_in_cell);
			vec2 b = vec2(x_in_cell - 1, y_in_cell);
			vec2 c = vec2(x_in_cell, y_in_cell - 1);
			vec2 d = vec2(x_in_cell - 1, y_in_cell - 1);

			float s = dot(g_s, a);
			float t = dot(g_t, b);
			float u = dot(g_u, c);
			float v = dot(g_v, d);

			// Mixing
			float f_x = f(x_in_cell);
			float f_y = f(y_in_cell);

			float s_t = (1 - f_x) * s + f_x * t;
			float u_v = (1 - f_x) * u + f_x * v;

			noise = (1 - f_y) * s_t + f_y * u_v;
	
			// Trying to put it to ~1 of height
			noise *= 4;

		} else if (noise_type == WORLEY_NOISE) {
			float x = uv[0] * noise_width;
			float y = uv[1] * noise_height;

			// Coordinate inside the cell
			float x_in_cell = fract(x);
			float y_in_cell = fract(y);

			// Coordinate of the cell (bottom left corner)
			float x_of_cell = x - x_in_cell;
			float y_of_cell = y - y_in_cell;

			// Get corner coordinates
			vec2 bottom_left = vec2(x_of_cell, y_of_cell);
			vec2 bottom_right = vec2(x_of_cell + 1, y_of_cell);
			vec2 top_left = vec2(x_of_cell, y_of_cell + 1);
			vec2 top_right = vec2(x_of_cell + 1, y_of_cell + 1);

			vec2 g_s = vec2(random(bottom_left, noise_seed) - 0.5, random(bottom_left, 1-noise_seed) - 0.5);
			vec2 g_t = vec2(random(bottom_right, noise_seed) - 0.5, random(bottom_right, 1-noise_seed) - 0.5);
			vec2 g_u = vec2(random(top_left, noise_seed) - 0.5, random(top_left, 1-noise_seed) - 0.5);
			vec2 g_v = vec2(random(top_right, noise_seed) - 0.5, random(top_right, 1-noise_seed) - 0.5);

			vec2 a = vec2(x_in_cell, y_in_cell);
			vec2 b = vec2(x_in_cell - 1, y_in_cell);
			vec2 c = vec2(x_in_cell, y_in_cell - 1);
			vec2 d = vec2(x_in_cell - 1, y_in_cell - 1);

			float s = length(g_s - a);
			float t = length(g_t - b);
			float u = length(g_u - c);
			float v = length(g_v - d);

			// Mixing
			s = min(s, t);
			u = min(u, v);
			noise = min(s, u);
	
		} else {
			// Unknown, no noise
			noise = 0;
		}

		// --- Compute Effect --- //

		if (noise_effect == ABSOLUTE_VALUE) {
			if (noise < 0) {
				noise *= (-1);
			}
		} else if (noise_effect == CLAMP_EXTREMAS) {
			// Min/max = 0/1
			if (noise > 1) {
				noise = 1;
			} else if (noise < 0) {
				noise = 0;
			}
		} else if (noise_effect == DISCRETIZE) {
			// Step = 0.1
			float step = 0.1;
		
			noise = (floor(noise/step) + 0.5) * step;
		}

		noise = (noise_amplitude * noise) + noise_offset;

		// --- Add input --- //

		float input_noise = texture(in_texture, uv)[0];

		if (fractal_type == FBM) {
			noise += input_noise;
		} else if (fractal_type == MULTIFRACTAL) {
			noise *= input_noise;
		}

	} else if (draw_or_copy == COPY) {

		// --- Take input --- // 
		noise = texture(in_texture, uv)[0];
		noise = (fractal_amplitude * noise) + fractal_offset;

		// --- Fractal Effect --- //

		if (fractal_effect == ABSOLUTE_VALUE) {
			if (noise < 0) {
				noise *= (-1);
			}
		} else if (fractal_effect == CLAMP_EXTREMAS) {
			// Min/max = 0/1
			if (noise > 1) {
				noise = 1;
			} else if (noise < 0) {
				noise = 0;
			}
		} else if (fractal_effect == DISCRETIZE) {
			// Step = 0.1
			float step = 0.1;
		
			noise =  (floor(noise/step) + 0.5) * step;
		}
	}

	color = vec3(noise, noise, noise);
}