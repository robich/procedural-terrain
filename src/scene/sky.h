#pragma once

#include "icg_common.h"
#include "../app/constants.h"
#include "../app/params.h"

class Sky {
	private:
		GLuint _tex_skybox[6];
		GLuint _vao;
		GLuint _vbo;
		GLuint _pid;

		mat4 model = mat4::Identity();

		TextureParams* _texture_params;
		
	public:
		void init(AppParams* app_params);
		void cleanup();
		void draw(const mat4& view, const mat4& projection);
		void loadSky();
};