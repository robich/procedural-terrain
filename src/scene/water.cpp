#include "water.h"

void Water::init(AppParams* app_params){

	_water_params = app_params->water_params;
	_grid_params = app_params->grid_params;
	_shading_params = app_params->shading_params;

	///--- Compile the shaders
	_pid = opengp::load_shaders("scene/water_vshader.glsl", "scene/water_fshader.glsl");
	if (!_pid) exit(EXIT_FAILURE);
	glUseProgram(_pid);

	///--- Vertex one vertex Array
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	///--- Vertex coordinates
	{
		const GLfloat vpoint[] = {
			-1.0f, -1.0f,
			+1.0f, -1.0f,
			-1.0f, +1.0f,
			+1.0f, +1.0f
		};

		///--- Buffer
		glGenBuffers(1, &_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vpoint), vpoint, GL_STATIC_DRAW);

		///--- Attribute
		GLuint vertex_pos_id = glGetAttribLocation(_pid, "vertex_pos");
		glEnableVertexAttribArray(vertex_pos_id);
		glVertexAttribPointer(vertex_pos_id, 2, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
	}

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	///--- to avoid the current object being polluted
	glBindVertexArray(0);
	glUseProgram(0);

	glGenTextures(1, &_tex_normal);
	glBindTexture(GL_TEXTURE_2D, _tex_normal);
	glfwLoadTexture2D("textures/water/tex_normal_map_water.tga", 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	
	check_error_gl();
}

void Water::cleanup(){
	glDeleteBuffers(1, &_vbo);
	glDeleteVertexArrays(1, &_vao);
	glDeleteProgram(_pid);
}

void Water::setMirrorTexture(GLuint* tex_mirror){
	_tex_mirror = tex_mirror;
}

void Water::setHeightTexture(GLuint* tex_height) {
	_tex_height = tex_height;
}

void Water::setDepthTexture(GLuint* tex_depth) {
	_tex_depth = tex_depth;
}

void Water::draw(const mat4& view, const mat4& projection){
	glUseProgram(_pid);
	glBindVertexArray(_vao);

	_water_params->setup(_pid);
	_grid_params->setup(_pid);
	_shading_params->setup(_pid);

	glUniform1f(glGetUniformLocation(_pid, "near"), NEAR);
	glUniform1f(glGetUniformLocation(_pid, "far"), FAR);

	glUniform1f(glGetUniformLocation(_pid, "time"), glfwGetTime());

	// Pass textures

	glUniform1i(glGetUniformLocation(_pid, "tex_height"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, *_tex_height);

	glUniform1i(glGetUniformLocation(_pid, "tex_mirror"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, *_tex_mirror);

	glUniform1i(glGetUniformLocation(_pid, "tex_depth"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, *_tex_depth);
	
	glUniform1i(glGetUniformLocation(_pid, "tex_normal"), 3);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, _tex_normal);

	// Setup MVP
	mat4 MVP = projection*view*model;
	GLuint MVP_id = glGetUniformLocation(_pid, "mvp");
	glUniformMatrix4fv(MVP_id, 1, GL_FALSE, MVP.data());

	// Draw
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
	glUseProgram(0);
}