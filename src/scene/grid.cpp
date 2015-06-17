#include "grid.h"

using namespace std;


	int Grid::get_vertex_index(int i, int j) {
		return i + j * _grid_params->resolution * _grid_params->length;
	}

	void Grid::init(AppParams* app_params){

		_noise_params = app_params->noise_params;
		_grid_params = app_params->grid_params;
		_texture_params = app_params->texture_params;
		_water_params = app_params->water_params;
		_shading_params = app_params->shading_params;
		_snow_params = app_params->snow_params;
		_grass_params = app_params->grass_params;
		_sand_params = app_params->sand_params;

		TEX_PATHS[0] = "textures/terrains/active/tex_grass.tga";
		TEX_PATHS[1] = "textures/terrains/active/tex_sand.tga";
		TEX_PATHS[2] = "textures/terrains/active/tex_rock.tga";
		TEX_PATHS[3] = "textures/terrains/active/tex_snow.tga";
		TEX_PATHS[4] = "textures/terrains/active/tex_rock_underwater.tga";

		TEX_NAMES[0] = "tex_grass_";
		TEX_NAMES[1] = "tex_sand_";
		TEX_NAMES[2] = "tex_rock_";
		TEX_NAMES[3] = "tex_snow_";
		TEX_NAMES[4] = "tex_rock_underwater_";

		// Compile the shaders
		_pid = opengp::load_shaders("scene/grid_vshader.glsl", "scene/grid_fshader.glsl");
		if (!_pid) exit(EXIT_FAILURE);
		glUseProgram(_pid);

		// Vertex one vertex Array
		glGenVertexArrays(1, &_vao);
		glBindVertexArray(_vao);

		// Vertex coordinates and indices
		{
			std::vector<GLfloat> vertices;
			std::vector<GLuint> indices;

			// Grid dimension
			int grid_length = _grid_params->resolution * _grid_params->length;
			int grid_width = _grid_params->resolution * _grid_params->width;

			cout << grid_length << endl;
			cout << grid_width << endl;

			// Put vertex positions
			for (int i = 0; i < grid_length; i++) {
				for (int j = 0; j < grid_width; j++) {
					float x = -0.5 + float(i) / (grid_length - 1);
					float y = -0.5 + float(j) / (grid_width - 1);
					vertices.push_back(x);
					vertices.push_back(y);
				}
			}

			for (int i = 0; i < grid_length - 1; i++) {
				for (int j = 0; j < grid_width - 1; j++) {
					// triangle (i,j), (i+1, j), (i, j+1)
					indices.push_back(get_vertex_index(i, j));
					indices.push_back(get_vertex_index(i + 1, j));
					indices.push_back(get_vertex_index(i, j + 1));

					indices.push_back(get_vertex_index(i + 1, j));
					indices.push_back(get_vertex_index(i, j + 1));
					indices.push_back(get_vertex_index(i + 1, j + 1));
				}
			}

			_num_indices = indices.size();

			// position buffer
			glGenBuffers(1, &_vbo_position);
			glBindBuffer(GL_ARRAY_BUFFER, _vbo_position);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

			// vertex indices
			glGenBuffers(1, &_vbo_index);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vbo_index);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

			// position shader attribute
			GLuint vertex_pos_id = glGetAttribLocation(_pid, "vertex_pos");
			glEnableVertexAttribArray(vertex_pos_id);
			glVertexAttribPointer(vertex_pos_id, 2, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
		}

		// --- Create Textures ---

		{
			for (int i = 0; i < TEXTURES_COUNT; i++){

				GLuint tex;
				const char* full_name = (TEX_PATH + TEX_NAMES[i] + TEX_EXT).data();
				glGenTextures(1, &tex);
				glBindTexture(GL_TEXTURE_2D, tex);
				glfwLoadTexture2D(TEX_PATHS[i], 0);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				GLuint tex_id = glGetUniformLocation(_pid, TEX_NAMES[i].data());
				glUniform1i(tex_id, 11 + i);
				glActiveTexture(GL_TEXTURE10 + i);
				glBindTexture(GL_TEXTURE_2D, tex);

				_texs.push_back(tex);
			}
		}

		// to avoid the current object being polluted
		glBindVertexArray(0);
		check_error_gl();
	}

	void Grid::setTexHeight(GLuint* tex_height){
		_tex_height = tex_height;
	}

	void Grid::setTexSnow(GLuint* tex_snow){
		_tex_snow = tex_snow;
	}

	void Grid::setTexShadow(GLuint* tex_shadow){
		_tex_shadow = tex_shadow;
	}

	void Grid::setTexGrass(GLuint* tex_grass){
		_tex_grass = tex_grass;
	}

	void Grid::setTexSand(GLuint* tex_sand){
		_tex_sand = tex_sand;
	}

	void Grid::cleanup(){
		glDeleteBuffers(1, &_vbo_position);
		glDeleteBuffers(1, &_vbo_index);
		glDeleteVertexArrays(1, &_vao);
		glDeleteProgram(_pid);
	}

	float Grid::get_height(float x, float y) {
		float hl = _grid_params->length / 2;
		float hw = _grid_params->width / 2;

		//std::cout << "x : " << x << ", " << "	y : " << y << "   hl : " << hl << "   hw : " << hw << std::endl;

		if (x > hw || x < -hw || y > hl || y < -hl) {
			return 1.5f;
		}

		x += hw;
		y += hl;

		int px_x = _noise_params->resolution * x / _grid_params->width;
		int px_y = _noise_params->resolution * y / _grid_params->length;

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, *_tex_height);

		GLfloat* data = (GLfloat*)calloc(_noise_params->resolution * _noise_params->resolution, sizeof(GLfloat));
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, data);

		GLfloat retval = data[px_y * _noise_params->resolution + px_x];

		delete[] data;
		return retval;
	}

	void Grid::draw(const mat4& view, const mat4& projection, const mat4& light_view, const mat4& light_projection, int mode){
		glUseProgram(_pid);
		glBindVertexArray(_vao);

		_grid_params->setup(_pid);
		_shading_params->setup(_pid);
		_snow_params->setup(_pid);
		_water_params->setup(_pid);
		_grass_params->setup(_pid);
		_texture_params->setup(_pid);
		_sand_params->setup(_pid);

		glUniform1f(glGetUniformLocation(_pid, "DX"), 1.0 / _noise_params->resolution);
		glUniform1f(glGetUniformLocation(_pid, "DY"), 1.0 / _noise_params->resolution);

		glUniform1i(glGetUniformLocation(_pid, "mode"), mode);

		// Bind textures
		glUniform1i(glGetUniformLocation(_pid, "tex_height"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, *_tex_height);

		glUniform1i(glGetUniformLocation(_pid, "tex_grass"), 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, *_tex_grass);

		glUniform1i(glGetUniformLocation(_pid, "tex_sand"), 2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, *_tex_sand);

		glUniform1i(glGetUniformLocation(_pid, "tex_snow"), 3);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, *_tex_snow);

		glUniform1i(glGetUniformLocation(_pid, "tex_shadow"), 4);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, *_tex_shadow);

		for (int i = 0; i < TEXTURES_COUNT; i++){

			GLuint tex_id = glGetUniformLocation(_pid, TEX_NAMES[i].data());
			glUniform1i(tex_id, 10 + i);
			glActiveTexture(GL_TEXTURE10 + i);
			glBindTexture(GL_TEXTURE_2D, _texs[i]);
		}

		// Setup MVPs

		mat4 MVP = projection*view*model;
		GLuint MVP_id = glGetUniformLocation(_pid, "mvp");
		glUniformMatrix4fv(MVP_id, 1, GL_FALSE, MVP.data());

		mat4 MVP_light = light_projection * light_view * model;

		GLuint MVP_shadow_id = glGetUniformLocation(_pid, "mvp_light");
		glUniformMatrix4fv(MVP_shadow_id, 1, GL_FALSE, MVP_light.data());

		// Draw
		glDrawElements(GL_TRIANGLES, _num_indices, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glUseProgram(0);
	}
