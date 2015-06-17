#pragma once

#include "icg_common.h"
#include "../app/constants.h"
#include "../app/params.h"


class Quad {

private:
	GLuint _vao; ///< vertex array object
	GLuint _pid; ///< GLSL shader program ID 
	GLuint _vbo; ///< memory buffer
	GLuint _tex; ///< Texture ID

	SnowParams* _snow_params;
	GrassParams* _grass_params;
	SandParams* _sand_params;
	GridParams* _grid_params;
	NoiseParams* _noise_params;

public:
	void init(AppParams* app_params);
	void setShaders(int mode);
	void genVertexArray();

	void drawNoise(NoiseParams* noise_params, float noise_amplitude, GLuint* in_texture = nullptr);
	void drawSand(GLuint* tex_noise, GLuint* tex_sand, GLuint* tex_pos, int mode);
	void drawSnow(GLuint* tex_noise, GLuint* tex_snow, GLuint* tex_pos, int mode);
	void drawGrass(GLuint* tex_noise, GLuint* tex_grass, int mode);
	void drawTexture(GLuint* texture, float amplitude = 1, float offset = 0);

	// Snow
	void fall(GLuint* tex_noise, GLuint* tex_snow, GLuint* tex_pos);
	void slide(GLuint* tex_noise, GLuint* tex_snow, GLuint* tex_pos);
	void melt(GLuint* tex_noise, GLuint* tex_snow, GLuint* tex_pos);
	void smooth(GLuint* tex_noise, GLuint* tex_snow, GLuint* tex_pos);

	void cleanup();
};
