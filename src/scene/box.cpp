#include "box.h"

void Box::init(AppParams* app_params){
	
	_water_params = app_params->water_params;
	_grid_params = app_params->grid_params;
	_noise_params = app_params->noise_params;

	///--- Compile the shaders
	_pid = opengp::load_shaders("scene/box_vshader.glsl", "scene/box_fshader.glsl");
	if (!_pid) exit(EXIT_FAILURE);
	glUseProgram(_pid);

	//--- Vertex one vertex Array
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	//--- Vertex coordinates
	{
		const GLfloat faces[] = {
			LFB, LBB, LFT, LBT, // Left
			RFB, RBB, RFT, RBT, // Right 
			LFB, RFB, LFT, RFT, // Front
			LBB, RBB, LBT, RBT, // Back
			LFB, RFB, LBB, RBB  // Bottom
		};

		// position buffer
		glGenBuffers(1, &_vbo_pos);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo_pos);
		glBufferData(GL_ARRAY_BUFFER, sizeof(faces), faces, GL_STATIC_DRAW);

		// position shader attribute
		GLuint vertex_pos_id = glGetAttribLocation(_pid, "vertex_pos");
		cout << vertex_pos_id << endl;
		glEnableVertexAttribArray(vertex_pos_id);
		glVertexAttribPointer(vertex_pos_id, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
	}

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	///--- to avoid the current object being polluted
	glBindVertexArray(0);
	glUseProgram(0);
	check_error_gl();
}

void Box::cleanup(){
	glDeleteBuffers(1, &_vbo_tex);
	glDeleteBuffers(1, &_vbo_pos);
	glDeleteVertexArrays(1, &_vao);
	glDeleteProgram(_pid);
}

void Box::setHeightTexture(GLuint* tex_height) {
	_tex_height = tex_height;
}

void Box::setWaterDepthTexture(GLuint* tex_water_depth) {
	_tex_water_depth = tex_water_depth;
}

void Box::draw(const mat4& view, const mat4& projection){
	glUseProgram(_pid);
	glBindVertexArray(_vao);

	// Send Uniforms
	_water_params->setup(_pid);
	_grid_params->setup(_pid);
	_noise_params->setup_copy(_pid);

	glUniform1f(glGetUniformLocation(_pid, "FAR"), FAR);
	glUniform1f(glGetUniformLocation(_pid, "NEAR"), NEAR);

	// Pass textures

	glUniform1i(glGetUniformLocation(_pid, "tex_height") , 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, *_tex_height);

	glUniform1i(glGetUniformLocation(_pid, "tex_water_depth"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, *_tex_water_depth);

	// Setup MVP
	mat4 MVP = projection*view*model;
	GLuint MVP_id = glGetUniformLocation(_pid, "mvp");
	glUniformMatrix4fv(MVP_id, 1, GL_FALSE, MVP.data());

	// Draw
	for (GLuint i = 0; i < 20; i += 4){
		glDrawArrays(GL_TRIANGLE_STRIP, i, 4);
	}

	glBindVertexArray(0);
	glUseProgram(0);
}