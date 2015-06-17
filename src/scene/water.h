#pragma once

#include "icg_common.h"
#include "../app/constants.h"
#include "../app/params.h"

class Water {

private:
	GLuint _vao;          ///< vertex array object
	GLuint _vbo;
	GLuint _pid;          ///< GLSL shader program ID
	GLuint _num_indices;  ///< number of vertices to render

	GLuint* _tex_mirror;
	GLuint* _tex_height;
	GLuint* _tex_depth;
	GLuint _tex_normal;

	GridParams* _grid_params;
	WaterParams* _water_params;
	ShadingParams* _shading_params;

	mat4 model = mat4::Identity();

public:
	void init(AppParams* app_params);
	
	void setMirrorTexture(GLuint* tex_mirror);
	void setHeightTexture(GLuint* tex_height);
	void setDepthTexture(GLuint* tex_depth);
	void setNormalTexture(GLuint* tex_normal);

	void draw(const mat4& view, const mat4& projection);
	void cleanup();
};