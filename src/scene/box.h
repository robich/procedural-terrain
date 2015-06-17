#pragma once

#include "icg_common.h"
#include "../app/constants.h"
#include "../app/params.h"

// Vertices [Left/Right] [Back/Front] [Bottom/Top]
#define LFB -1, -1, -1
#define LFT -1, +1, -1
#define LBB -1, -1, +1
#define LBT -1, +1, +1
#define RFB +1, -1, -1
#define RFT +1, +1, -1
#define RBB +1, -1, +1
#define RBT +1, +1, +1

class Box {

private:
	mat4 model = mat4::Identity();
	WaterParams* _water_params;
	GridParams* _grid_params;
	NoiseParams* _noise_params;

protected:
	GLuint _vao;          ///< vertex array object
	GLuint _vbo_pos;
	GLuint _vbo_tex;
	GLuint _pid;          ///< GLSL shader program ID
	GLuint _num_indices;  ///< number of vertices to render

	GLuint* _tex_height;
	GLuint* _tex_water_depth;

public:
	void init(AppParams* app_params);
	void draw(const mat4& view, const mat4& projection);
	void cleanup();

	void setHeightTexture(GLuint* tex_height);
	void setWaterDepthTexture(GLuint* tex_water_depth);
};