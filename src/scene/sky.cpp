#include "sky.h"


void Sky::init(AppParams* app_params){

	_texture_params = app_params->texture_params;

	_pid = opengp::load_shaders("scene/sky_vshader.glsl", "scene/sky_fshader.glsl");
	if (!_pid) exit(EXIT_FAILURE);
	glUseProgram(_pid);

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	{
		float t = 10.0;
		float skybox_vertices[] =
		{    // x, y, z, x, y, z, x, y, z, x, y, z
			-t, -t, t, -t, t, t, t, -t, t, t, t, t,			//  (back)
			-t, t, -t, -t, -t, -t, t, t, -t, t, -t, -t,		// (front)
			-t, -t, -t, -t, -t, t, t, -t, -t, t, -t, t, 	//  (bottom)
			t, t, t, -t, t, t, t, t, -t, -t, t, -t,			//  (top)
			-t, t, t, -t, -t, t, -t, t, -t, -t, -t, -t,		//  (left)
			t, -t, t, t, t, t, t, -t, -t, t, t, -t			// (right)
		};

		glGenBuffers(1, &_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), skybox_vertices, GL_STATIC_DRAW);

		GLuint vertex_pos_id = glGetAttribLocation(_pid, "vertex_pos");
		glEnableVertexAttribArray(vertex_pos_id);
		glVertexAttribPointer(vertex_pos_id, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
	}

	{
		const GLfloat vtexcoord[] = {
			0.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,

			1.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 1.0f,
			0.0f, 0.0f,

			0.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,

			0.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 0.0f,
			1.0f, 1.0f,

			1.0f, 1.0f,
			1.0f, 0.0f,
			0.0f, 1.0f,
			0.0f, 0.0f,

			0.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 0.0f,
			1.0f, 1.0f
		};

		glGenBuffers(1, &_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vtexcoord), vtexcoord, GL_STATIC_DRAW);

		GLuint tex_pos_id = glGetAttribLocation(_pid, "tex_pos");
		glEnableVertexAttribArray(tex_pos_id);
		glVertexAttribPointer(tex_pos_id, 2, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
	}

	loadSky();

	check_error_gl();
	glBindVertexArray(0);
	glUseProgram(0);
}

void Sky::loadSky() {
	
	const string path = IO_PATH_TO_SKYBOXES + SKYBOX_DIR[_texture_params->skybox_type];

	string faces[6];

	faces[0] = path + "XN.tga";
	faces[1] = path + "XP.tga";
	faces[2] = path + "YN.tga";
	faces[3] = path + "YP.tga";
	faces[4] = path + "ZN.tga";
	faces[5] = path + "ZP.tga";


	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glGenTextures(6, _tex_skybox);

	for (GLuint i = 0; i < 6; i++){
		glBindTexture(GL_TEXTURE_2D, _tex_skybox[i]);
		glfwLoadTexture2D(faces[i].data(), 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Sky::cleanup(){
	glDeleteBuffers(1, &_vbo);
	glDeleteVertexArrays(1, &_vao);
	glDeleteProgram(_pid);
}

void Sky::draw(const mat4& view, const mat4& projection)
{
	glUseProgram(_pid);
	glBindVertexArray(_vao);

	glDepthMask(GL_FALSE);

	// Setup MVP
	mat4 MVP = projection*view*model;
	GLuint MVP_id = glGetUniformLocation(_pid, "mvp");
	glUniformMatrix4fv(MVP_id, 1, GL_FALSE, MVP.data());

	for (GLuint i = 0; i < 6; i++){
		GLuint tex_skybox_id = glGetUniformLocation(_pid, "tex_skybox");
		glUniform1i(tex_skybox_id, 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _tex_skybox[i]);
		glDrawArrays(GL_TRIANGLE_STRIP, i * 4, 4);
	}

	glDepthMask(GL_TRUE);

	glBindVertexArray(0);
	glUseProgram(0);
}