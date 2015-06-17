#pragma once

#include "icg_common.h"
#include "../buffers/copybuffer.h"
#include "../buffers/generalbuffer.h"
#include "quad.h"
#include "../app/constants.h"
#include "../app/params.h"


class Terrain {

private:
	Generalbuffer _snowbuffer[2];
	Generalbuffer _grassbuffer[2];
	Generalbuffer _noisebuffer[2];
	Generalbuffer _dirtbuffer[2];
	Generalbuffer _sandbuffer[2];

	Copybuffer _copybuffer;

	GLuint _tex_height;
	GLuint _tex_snow;
	GLuint _tex_sand; 
	GLuint _tex_dirt;
	GLuint _tex_water;
	GLuint _tex_grass;

	Quad _quad;

	ThemeParams* _theme_params;
	NoiseParams* _noise_params;
	SnowParams* _snow_params;
	GrassParams* _grass_params;
	SandParams* _sand_params;

public:
	Terrain();

	void init(AppParams* app_params);

	void copyTexture(GLuint* src, GLuint* dst);
	void copyNoise(GLuint* src, GLuint* dst, float amplitude, float offset);

	void renderFractal();
	void erode();
	void addSnow();
	void addGrass();
	void addSand();

	GLuint* getHeightTexture();
	GLuint* getDirtTexture();
	GLuint* getSnowTexture();
	GLuint* getSandTexture();
	GLuint* getWaterTexture();
	GLuint* getGrassTexture();

	void resize();
};
