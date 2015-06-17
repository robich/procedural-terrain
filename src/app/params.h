#pragma once

#include "constants.h"

typedef struct ThemeParams {
	ThemeType theme_type;
};

typedef struct WindowParams {
	int width;
	int height;
};

typedef struct GridParams {
	bool enable;
	int length;
	int width;
	int resolution;

	void setup(GLuint pid){
		glUniform1i(glGetUniformLocation(pid, "grid_enable"), enable);
		glUniform1i(glGetUniformLocation(pid, "grid_length"), length);
		glUniform1i(glGetUniformLocation(pid, "grid_width"), width);
		glUniform1i(glGetUniformLocation(pid, "grid_resolution"), resolution);
	}
};

typedef struct NoiseParams {
	NoiseType noise_type;
	FractalType fractal_type;

	Effect noise_effect;
	Effect fractal_effect;

	int resolution;
	int width;
	int height;
	float amplitude;
	float offset;

	float H;
	int lacunarity;
	int octaves;

	float seed;

	NoiseParams copy() {
		return{
			noise_type,
			fractal_type,
			noise_effect,
			fractal_effect,
			resolution,
			width,
			height,
			amplitude,
			offset,
			H,
			lacunarity,
			octaves,
			seed
		};
	}

	void setup_copy(GLuint pid) {
		glUniform1f(glGetUniformLocation(pid, "fractal_offset"), offset);
		glUniform1f(glGetUniformLocation(pid, "fractal_amplitude"), amplitude);
		glUniform1i(glGetUniformLocation(pid, "fractal_type"), fractal_type);
		glUniform1i(glGetUniformLocation(pid, "fractal_effect"), fractal_effect);
	}

	void setup_draw(GLuint pid) {
		glUniform1i(glGetUniformLocation(pid, "noise_type"), noise_type);
		glUniform1i(glGetUniformLocation(pid, "noise_effect"), noise_effect);

		glUniform1f(glGetUniformLocation(pid, "noise_width"), width);
		glUniform1f(glGetUniformLocation(pid, "noise_height"), height);

		glUniform1f(glGetUniformLocation(pid, "noise_seed"), seed);

		glUniform1i(glGetUniformLocation(pid, "fractal_type"), fractal_type);
	}
};

static NoiseParams FLAT_NOISE = {
	NO_NOISE,
	FBM,
	NO_EFFECT,
	NO_EFFECT,
	1,
	1,
	0,
	1,
	0,
	0,
	0,
	0
};

typedef struct GrassParams {
	bool enable;

	float max_height;
	float min_height;
	float max_slope;
	float min_angle;

	int time_grow;
	int time_smooth;

	void setup(GLuint pid) {
		glUseProgram(pid);

		glUniform1i(glGetUniformLocation(pid, "grass_enable"), enable);

		glUniform1f(glGetUniformLocation(pid, "grass_max_height"), max_height);
		glUniform1f(glGetUniformLocation(pid, "grass_min_height"), min_height);
		glUniform1f(glGetUniformLocation(pid, "grass_max_slope"), max_slope);
		glUniform1f(glGetUniformLocation(pid, "grass_min_angle"), min_angle);

		glUniform1f(glGetUniformLocation(pid, "grass_time_grow"), time_grow);
		glUniform1f(glGetUniformLocation(pid, "grass_time_smooth"), time_smooth);
	}
};

typedef struct SandParams {
	bool enable;

	float amount;
	float max_amount;
	float max_slope;
	float min_height;
	float max_height;
	float threshold;

	int slide_time;
	int melt_time;
	int smooth_time;

	void setup(GLuint pid) {
		glUseProgram(pid);

		glUniform1i(glGetUniformLocation(pid, "sand_enable"), enable);

		glUniform1f(glGetUniformLocation(pid, "sand_amount"), amount);
		glUniform1f(glGetUniformLocation(pid, "sand_max_slope"), max_slope);
		glUniform1f(glGetUniformLocation(pid, "sand_min_height"), min_height);
		glUniform1f(glGetUniformLocation(pid, "sand_max_height"), max_height);
		glUniform1f(glGetUniformLocation(pid, "sand_max_amount"), max_amount);
		glUniform1f(glGetUniformLocation(pid, "sand_threshold"), threshold);

		glUniform1i(glGetUniformLocation(pid, "sand_slide_time"), slide_time);
		glUniform1i(glGetUniformLocation(pid, "sand_melt_time"), melt_time);
		glUniform1i(glGetUniformLocation(pid, "sand_smooth_time"), smooth_time);
	}
};


typedef struct SnowParams {
	bool enable;

	float amount;
	float max_amount;
	float max_slope;
	float min_height;
	float threshold;

	int slide_time;
	int melt_time;
	int smooth_time;

	void setup(GLuint pid) {
		glUseProgram(pid);

		glUniform1i(glGetUniformLocation(pid, "snow_enable"), enable);

		glUniform1f(glGetUniformLocation(pid, "snow_amount"), amount);
		glUniform1f(glGetUniformLocation(pid, "snow_max_slope"), max_slope);
		glUniform1f(glGetUniformLocation(pid, "snow_min_height"), min_height);
		glUniform1f(glGetUniformLocation(pid, "snow_max_amount"), max_amount);
		glUniform1f(glGetUniformLocation(pid, "snow_threshold"), threshold);

		glUniform1i(glGetUniformLocation(pid, "snow_slide_time"), slide_time);
		glUniform1i(glGetUniformLocation(pid, "snow_melt_time"), melt_time);
		glUniform1i(glGetUniformLocation(pid, "snow_smooth_time"), smooth_time);
	}
};

typedef struct ShadingParams {
	bool enable_phong;
	bool enable_shadow;

	vec3 light_pos;
	float near, far;
	vec3 Ia, Id;

	float shadow_intensity;

	void setup(GLuint pid) {
		glUseProgram(pid);

		glUniform1i(glGetUniformLocation(pid, "shading_enable_phong"), enable_phong);
		glUniform1i(glGetUniformLocation(pid, "shading_enable_shadow"), enable_shadow);
		glUniform3fv(glGetUniformLocation(pid, "shading_light_pos"), ONE, light_pos.data());
		glUniform3fv(glGetUniformLocation(pid, "shading_Ia"), ONE, Ia.data());
		glUniform3fv(glGetUniformLocation(pid, "shading_Id"), ONE, Id.data());
		glUniform1f(glGetUniformLocation(pid, "shading_shadow_intensity"), shadow_intensity);

		glUniform1f(glGetUniformLocation(pid, "shading_near"), near);
		glUniform1f(glGetUniformLocation(pid, "shading_far"), far);
	}

	mat4 get_view_matrix() {
		return lookAt(
			light_pos,
			vec3(0, 0, 0),
			vec3(0, 1, 0)
			);
	}

	mat4 get_projection_matrix() {
		return Eigen::ortho(-4.0f, 4.0f, -4.0f, 4.0f, near, far);
		//return Eigen::perspective(45.0f, float(WIDTH)/ HEIGHT, NEAR, FAR);
	}
};

typedef struct WaterParams {
	/* Water*/
	bool enable;
	float height;
	float transparency;
	float depth_alpha_factor;
	float depth_color_factor;
	float reflection_factor;
	vec3 color;

	/* Waves */
	float waves_speed;
	float waves_tile_factor;
	float waves_amplitude;

	void setup(GLuint pid) {
		glUseProgram(pid);

		/* Water */
		glUniform1i(glGetUniformLocation(pid, "water_enable"), enable);
		glUniform1f(glGetUniformLocation(pid, "water_height"), height);
		glUniform1f(glGetUniformLocation(pid, "water_transparency"), transparency);
		glUniform3f(glGetUniformLocation(pid, "water_color"), color[0], color[1], color[2]);
		glUniform1f(glGetUniformLocation(pid, "water_depth_alpha_factor"), depth_alpha_factor);
		glUniform1f(glGetUniformLocation(pid, "water_depth_color_factor"), depth_color_factor);
		glUniform1f(glGetUniformLocation(pid, "water_reflection_factor"), reflection_factor);

		/* Waves */
		glUniform1f(glGetUniformLocation(pid, "water_waves_speed"), waves_speed);
		glUniform1f(glGetUniformLocation(pid, "water_waves_tile_factor"), waves_tile_factor);
		glUniform1f(glGetUniformLocation(pid, "water_waves_amplitude"), waves_amplitude);
	}
};

typedef struct TextureParams {
	TextureType texture_type;
	SkyboxType skybox_type;

	void setup(GLuint pid) {
		glUseProgram(pid);

		glUniform1i(glGetUniformLocation(pid, "texture_type"), texture_type);
	}
};

typedef struct AppParams {
	ThemeParams* theme_params;
	WindowParams* window_params;
	GridParams* grid_params;
	NoiseParams* noise_params;
	GrassParams* grass_params;
	SandParams* sand_params;
	SnowParams* snow_params;
	TextureParams* texture_params;
	ShadingParams* shading_params;
	WaterParams* water_params;
};