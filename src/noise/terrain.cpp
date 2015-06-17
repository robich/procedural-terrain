#include "terrain.h"

#define INIT(in, out) int in = 0; int out = 1;
#define SWAP(in, out) in = 1 - in; out = 1 - out;
#define SWAP_TEXTURE(tex, buffer, index) tex = buffer[in].getTexture(index);
#define CLEAR_BUFFERS(buffer) buffer[0].clear(); buffer[1].clear();
#define SET_MODE(mode) _quad.setShaders(mode);_quad.genVertexArray();

Terrain::Terrain()
	{
		glGenTextures(1, &_tex_height);
		glBindTexture(GL_TEXTURE_2D, _tex_height);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glGenTextures(1, &_tex_dirt);
		glBindTexture(GL_TEXTURE_2D, _tex_dirt);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glGenTextures(1, &_tex_grass);
		glBindTexture(GL_TEXTURE_2D, _tex_grass);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glGenTextures(1, &_tex_sand);
		glBindTexture(GL_TEXTURE_2D, _tex_sand);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glGenTextures(1, &_tex_snow);
		glBindTexture(GL_TEXTURE_2D, _tex_snow);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		
	}

void Terrain::init(AppParams* app_params) {

	_theme_params = app_params->theme_params;
	_noise_params = app_params->noise_params;
	_snow_params = app_params->snow_params;
	_sand_params = app_params->sand_params;
	_grass_params = app_params->grass_params;

	_snowbuffer[0].init(3);
	_snowbuffer[0].genTextures();
	_snowbuffer[0].setFormat(GL_RG32F, GL_RED, GL_FLOAT);
	
	_snowbuffer[1].init(3);
	_snowbuffer[1].genTextures();
	_snowbuffer[1].setFormat(GL_RG32F, GL_RED, GL_FLOAT);

	_sandbuffer[0].init(3);
	_sandbuffer[0].genTextures();
	_sandbuffer[0].setFormat(GL_RG32F, GL_RED, GL_FLOAT);

	_sandbuffer[1].init(3);
	_sandbuffer[1].genTextures();
	_sandbuffer[1].setFormat(GL_RG32F, GL_RED, GL_FLOAT);

	_grassbuffer[0].init(2);
	_grassbuffer[0].genTextures();
	_grassbuffer[0].setFormat(GL_RG32F, GL_RED, GL_FLOAT);

	_grassbuffer[1].init(2);
	_grassbuffer[1].genTextures();
	_grassbuffer[1].setFormat(GL_RG32F, GL_RED, GL_FLOAT);
	
	_noisebuffer[0].init(1);
	_noisebuffer[0].genTextures();
	_noisebuffer[0].setFormat(GL_R32F, GL_RED, GL_FLOAT);
	
	_noisebuffer[1].init(1);
	_noisebuffer[1].genTextures();
	_noisebuffer[1].setFormat(GL_R32F, GL_RED, GL_FLOAT);

	_dirtbuffer[0].init(BUFFER_DIRT_COUNT);
	_dirtbuffer[0].genTextures();
	_dirtbuffer[0].setFormat(GL_R32F, GL_RED, GL_FLOAT);

	_dirtbuffer[1].init(BUFFER_DIRT_COUNT);
	_dirtbuffer[1].genTextures();
	_dirtbuffer[1].setFormat(GL_R32F, GL_RED, GL_FLOAT);
	
	_copybuffer.resize(_noise_params->resolution, _noise_params->resolution);
	
	resize();
	check_error_gl();
	_quad.init(app_params);
}

void Terrain::renderFractal() {

	GLuint* tex_height;

	const GLuint TEX_HEIGHT_INDEX = 0;

	CLEAR_BUFFERS(_noisebuffer);
	SET_MODE(NOISE_MODE);

	NoiseParams noise_params_tmp = _noise_params->copy();
	noise_params_tmp.amplitude = 1;

	// Two variables to swap buffers
	INIT(in, out);
	SWAP_TEXTURE(tex_height, _noisebuffer, TEX_HEIGHT_INDEX);

	if (_noise_params->fractal_type == MULTIFRACTAL) {
		_noisebuffer[out].bind(BUFFER_ATTACHMENT_0, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			_quad.drawNoise(&FLAT_NOISE, 1, tex_height);
		_noisebuffer[out].unbind();

		SWAP(in, out);
		SWAP_TEXTURE(tex_height, _noisebuffer, TEX_HEIGHT_INDEX);
	}

	for (int i = 0; i < _noise_params->octaves; i++) {

		_noisebuffer[out].bind(BUFFER_ATTACHMENT_0, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			_quad.drawNoise(&noise_params_tmp, noise_params_tmp.amplitude, tex_height);
		_noisebuffer[out].unbind();

		noise_params_tmp.height *= _noise_params->lacunarity;
		noise_params_tmp.width *= _noise_params->lacunarity;
		noise_params_tmp.amplitude *= pow(_noise_params->lacunarity, -_noise_params->H);

		// Swap input & output textures
		SWAP(in, out);
		SWAP_TEXTURE(tex_height, _noisebuffer, TEX_HEIGHT_INDEX);
	}

	copyNoise(tex_height, &_tex_height, _noise_params->amplitude, _noise_params->offset);
}

void Terrain::resize() {
	_snowbuffer[0].setSize(_noise_params->resolution, _noise_params->resolution);
	_snowbuffer[0].genTextureImages();
	_snowbuffer[0].genFramebuffer(BUFFER_ATTACHMENT_2, 3);
	_snowbuffer[0].clear();

	_snowbuffer[1].setSize(_noise_params->resolution, _noise_params->resolution);
	_snowbuffer[1].genTextureImages();
	_snowbuffer[1].genFramebuffer(BUFFER_ATTACHMENT_2, 3);
	_snowbuffer[1].clear();

	_sandbuffer[0].setSize(_noise_params->resolution, _noise_params->resolution);
	_sandbuffer[0].genTextureImages();
	_sandbuffer[0].genFramebuffer(BUFFER_ATTACHMENT_2, 3);
	_sandbuffer[0].clear();

	_sandbuffer[1].setSize(_noise_params->resolution, _noise_params->resolution);
	_sandbuffer[1].genTextureImages();
	_sandbuffer[1].genFramebuffer(BUFFER_ATTACHMENT_2, 3);
	_sandbuffer[1].clear();

	_grassbuffer[0].setSize(_noise_params->resolution, _noise_params->resolution);
	_grassbuffer[0].genTextureImages();
	_grassbuffer[0].genFramebuffer(BUFFER_ATTACHMENT_1, 2);
	_grassbuffer[0].clear();

	_grassbuffer[1].setSize(_noise_params->resolution, _noise_params->resolution);
	_grassbuffer[1].genTextureImages();
	_grassbuffer[1].genFramebuffer(BUFFER_ATTACHMENT_1, 2);
	_grassbuffer[1].clear();

	_noisebuffer[0].setSize(_noise_params->resolution, _noise_params->resolution);
	_noisebuffer[0].genTextureImages();
	_noisebuffer[0].genFramebuffer(BUFFER_ATTACHMENT_0, 1);
	_noisebuffer[0].clear();

	_noisebuffer[1].setSize(_noise_params->resolution, _noise_params->resolution);
	_noisebuffer[1].genTextureImages();
	_noisebuffer[1].genFramebuffer(BUFFER_ATTACHMENT_0, 1);
	_noisebuffer[1].clear();

}

void Terrain::addSnow() {
	GLuint* tex_snow;
	GLuint* tex_height;
	GLuint* tex_pos;

	const GLuint TEX_HEIGHT_INDEX = 0;
	const GLuint TEX_SNOW_INDEX = 1;
	const GLuint TEX_POS_INDEX = 2;

	CLEAR_BUFFERS(_snowbuffer);
	SET_MODE(SNOW_MODE);

	INIT(in, out);

	tex_height = &_tex_height;
	SWAP_TEXTURE(tex_snow, _snowbuffer, TEX_SNOW_INDEX);
	SWAP_TEXTURE(tex_pos, _snowbuffer, TEX_POS_INDEX);

	if (_snow_params->enable) {
		_snowbuffer[out].bind(BUFFER_ATTACHMENT_2, 3);
			glClear(GL_COLOR_BUFFER_BIT);
			_quad.fall(tex_height, tex_snow, tex_pos);
		_snowbuffer[out].unbind();

		SWAP(in, out);
		SWAP_TEXTURE(tex_height, _snowbuffer, TEX_HEIGHT_INDEX);
		SWAP_TEXTURE(tex_snow, _snowbuffer, TEX_SNOW_INDEX);
		SWAP_TEXTURE(tex_pos, _snowbuffer, TEX_POS_INDEX);
		
		for (int t = 0; t < _snow_params->slide_time; t++) {

			_snowbuffer[out].bind(BUFFER_ATTACHMENT_2, 3);
			glClear(GL_COLOR_BUFFER_BIT);
			_quad.slide(tex_height, tex_snow, tex_pos);
			_snowbuffer[out].unbind();

			SWAP(in, out);
			SWAP_TEXTURE(tex_height, _snowbuffer, TEX_HEIGHT_INDEX);
			SWAP_TEXTURE(tex_snow, _snowbuffer, TEX_SNOW_INDEX);
			SWAP_TEXTURE(tex_pos, _snowbuffer, TEX_POS_INDEX);
		}
		
		for (int t = 0; t < _snow_params->melt_time; t++) {

			_snowbuffer[out].bind(BUFFER_ATTACHMENT_2, 3);
			glClear(GL_COLOR_BUFFER_BIT);
			_quad.melt(tex_height, tex_snow, tex_pos);
			_snowbuffer[out].unbind();

			SWAP(in, out);
			SWAP_TEXTURE(tex_height, _snowbuffer, TEX_HEIGHT_INDEX);
			SWAP_TEXTURE(tex_snow, _snowbuffer, TEX_SNOW_INDEX);
			SWAP_TEXTURE(tex_pos, _snowbuffer, TEX_POS_INDEX);
		}

		for (int t = 0; t < _snow_params->smooth_time; t++) {

			_snowbuffer[out].bind(BUFFER_ATTACHMENT_2, 3);
				glClear(GL_COLOR_BUFFER_BIT);
				_quad.smooth(tex_height, tex_snow, tex_pos);
			_snowbuffer[out].unbind();

			SWAP(in, out);
			SWAP_TEXTURE(tex_height, _snowbuffer, TEX_HEIGHT_INDEX);
			SWAP_TEXTURE(tex_snow, _snowbuffer, TEX_SNOW_INDEX);
			SWAP_TEXTURE(tex_pos, _snowbuffer, TEX_POS_INDEX);
		}
	}

	copyTexture(tex_snow, &_tex_snow);
}

void Terrain::addSand() {
	GLuint* tex_sand;
	GLuint* tex_height;
	GLuint* tex_pos;

	const GLuint TEX_HEIGHT_INDEX = 0;
	const GLuint TEX_SAND_INDEX = 1;
	const GLuint TEX_POS_INDEX = 2;

	CLEAR_BUFFERS(_sandbuffer);
	SET_MODE(SAND_MODE);

	INIT(in, out);

	tex_height = &_tex_height;
	SWAP_TEXTURE(tex_sand, _sandbuffer, TEX_SAND_INDEX);
	SWAP_TEXTURE(tex_pos, _sandbuffer, TEX_POS_INDEX);

	if (_sand_params->enable) {
		_sandbuffer[out].bind(BUFFER_ATTACHMENT_2, 3);
		glClear(GL_COLOR_BUFFER_BIT);
		_quad.drawSand(tex_height, tex_sand, tex_pos, FALL);
		_sandbuffer[out].unbind();

		SWAP(in, out);
		SWAP_TEXTURE(tex_height, _sandbuffer, TEX_HEIGHT_INDEX);
		SWAP_TEXTURE(tex_sand, _sandbuffer, TEX_SAND_INDEX);
		SWAP_TEXTURE(tex_pos, _sandbuffer, TEX_POS_INDEX);
		
		for (int t = 0; t < _sand_params->slide_time; t++) {
			
			_sandbuffer[out].bind(BUFFER_ATTACHMENT_2, 3);
				glClear(GL_COLOR_BUFFER_BIT);
				_quad.drawSand(tex_height, tex_sand, tex_pos, SLIDE);
			_sandbuffer[out].unbind();

			SWAP(in, out);
			SWAP_TEXTURE(tex_height, _sandbuffer, TEX_HEIGHT_INDEX);
			SWAP_TEXTURE(tex_sand, _sandbuffer, TEX_SAND_INDEX);
			SWAP_TEXTURE(tex_pos, _sandbuffer, TEX_POS_INDEX);
		}
		
		for (int t = 0; t < _sand_params->melt_time; t++) {

			_sandbuffer[out].bind(BUFFER_ATTACHMENT_2, 3);
			glClear(GL_COLOR_BUFFER_BIT);
			_quad.drawSand(tex_height, tex_sand, tex_pos, MELT);
			_sandbuffer[out].unbind();

			SWAP(in, out);
			SWAP_TEXTURE(tex_height, _sandbuffer, TEX_HEIGHT_INDEX);
			SWAP_TEXTURE(tex_sand, _sandbuffer, TEX_SAND_INDEX);
			SWAP_TEXTURE(tex_pos, _sandbuffer, TEX_POS_INDEX);
		}
		
		for (int t = 0; t < _sand_params->smooth_time; t++) {

			_sandbuffer[out].bind(BUFFER_ATTACHMENT_2, 3);
			glClear(GL_COLOR_BUFFER_BIT);
			_quad.drawSand(tex_height, tex_sand, tex_pos, SMOOTH);
			_sandbuffer[out].unbind();

			SWAP(in, out);
			SWAP_TEXTURE(tex_height, _sandbuffer, TEX_HEIGHT_INDEX);
			SWAP_TEXTURE(tex_sand, _sandbuffer, TEX_SAND_INDEX);
			SWAP_TEXTURE(tex_pos, _sandbuffer, TEX_POS_INDEX);
		}
	}

	copyTexture(tex_sand, &_tex_sand);
}

void Terrain::addGrass() {
	GLuint* tex_grass;
	GLuint* tex_height;

	const GLuint TEX_HEIGHT_INDEX = 0;
	const GLuint TEX_GRASS_INDEX = 1;

	const int ACTION_INIT = 0;
	const int ACTION_SLIDE = 1;
	const int ACTION_SMOOTH = 2;

	CLEAR_BUFFERS(_grassbuffer);
	SET_MODE(GRASS_MODE);

	INIT(in, out);

	tex_height = &_tex_height;
	SWAP_TEXTURE(tex_grass, _grassbuffer, TEX_GRASS_INDEX);

	if (_grass_params->enable) {

		_grassbuffer[out].bind(BUFFER_ATTACHMENT_1, 2);
			glClear(GL_COLOR_BUFFER_BIT);
			_quad.drawGrass(tex_height, tex_grass, ACTION_INIT);
		_grassbuffer[out].unbind();

		SWAP(in, out);
		SWAP_TEXTURE(tex_height, _grassbuffer, TEX_HEIGHT_INDEX);
		SWAP_TEXTURE(tex_grass, _grassbuffer, TEX_GRASS_INDEX);
		
		for (int t = 0; t < _grass_params->time_grow; t++) {

			_grassbuffer[out].bind(BUFFER_ATTACHMENT_1, 2);
				glClear(GL_COLOR_BUFFER_BIT);
				_quad.drawGrass(tex_height, tex_grass, ACTION_SLIDE);
			_grassbuffer[out].unbind();

			SWAP(in, out);
			SWAP_TEXTURE(tex_height, _grassbuffer, TEX_HEIGHT_INDEX);
			SWAP_TEXTURE(tex_grass, _grassbuffer, TEX_GRASS_INDEX);
		}

		for (int t = 0; t < _grass_params->time_smooth; t++) {

			_grassbuffer[out].bind(BUFFER_ATTACHMENT_1, 2);
			glClear(GL_COLOR_BUFFER_BIT);
			_quad.drawGrass(tex_height, tex_grass, ACTION_SMOOTH);
			_grassbuffer[out].unbind();

			SWAP(in, out);
			SWAP_TEXTURE(tex_height, _grassbuffer, TEX_HEIGHT_INDEX);
			SWAP_TEXTURE(tex_grass, _grassbuffer, TEX_GRASS_INDEX);
		}
	}

	copyTexture(tex_grass, &_tex_grass);
}

GLuint* Terrain::getHeightTexture() {
	return &_tex_height;
}

GLuint* Terrain::getDirtTexture() {
	return &_tex_dirt;
}

GLuint* Terrain::getSandTexture() {
	return &_tex_sand;
}

GLuint* Terrain::getSnowTexture() {
	return &_tex_snow;
}

GLuint* Terrain::getGrassTexture() {
	return &_tex_grass;
}
void Terrain::copyTexture(GLuint* src, GLuint* dst) {
	_copybuffer.resize(_noise_params->resolution, _noise_params->resolution);
	_copybuffer.init(dst);

	_quad.setShaders(COPY_MODE);
	_quad.genVertexArray();
	_copybuffer.bind();
		glClear(GL_COLOR_BUFFER_BIT);
		 _quad.drawTexture(src);
	_copybuffer.unbind();
}

void Terrain::copyNoise(GLuint* src, GLuint* dst, float amplitude, float offset) {
	///--- Render random noise on quad in the framebuffer
	_copybuffer.resize(_noise_params->resolution, _noise_params->resolution);
	_copybuffer.init(dst);
	_copybuffer.clear();

	_quad.setShaders(COPY_MODE);
	_quad.genVertexArray();

	_copybuffer.bind();
		glClear(GL_COLOR_BUFFER_BIT);
		_quad.drawTexture(src, amplitude, offset);
	_copybuffer.unbind();
}