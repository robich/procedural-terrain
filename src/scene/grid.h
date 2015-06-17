#pragma once

#include "icg_common.h"
#include "../app/constants.h"
#include "../app/params.h"

using namespace std;

class Grid {

	string TEX_PATH = "textures/terrains/mountain/";
	string TEX_EXT = ".tga";

	const int GRASS = 0;
	const int SAND = 1;
	const int ROCK = 2;
	const int SNOW = 3;
	const int ROCK_UNDERWATER = 4;

	const int TEXTURES_COUNT = 5;

	const char* TEX_PATHS[5];
	string TEX_NAMES[5];

private:
	NoiseParams* _noise_params;
	GridParams* _grid_params;
	SnowParams* _snow_params;
	GrassParams* _grass_params;
	TextureParams* _texture_params;
	ShadingParams* _shading_params;
	WaterParams* _water_params;
	SandParams* _sand_params;

	mat4 model = mat4::Identity();

protected:
    GLuint _vao;          ///< vertex array object
    GLuint _vbo_position; ///< memory buffer for positions
    GLuint _vbo_index;    ///< memory buffer for indice
	GLuint _vbo;
    GLuint _pid;          ///< GLSL shader program ID
   
	GLuint* _tex_height; 
	GLuint* _tex_snow;
	GLuint* _tex_grass;
	GLuint* _tex_shadow;
	GLuint* _tex_sand;

	vector<GLuint> _texs;     

    GLuint _num_indices;  ///< number of vertices to render
    
public:    
	void init(AppParams* app_params);

	void setTexHeight(GLuint* tex_height);
	void setTexSand(GLuint* tex_sand);
	void setTexGrass(GLuint* tex_grass);
	void setTexSnow(GLuint* tex_snow);
	void setTexShadow(GLuint* tex_shadow);

	void draw(const mat4& view, const mat4& projection, const mat4& light_view, const mat4& light_projection, int mode);
	void cleanup();

	

	int get_vertex_index(int i, int j);

	float get_height(float x, float y);
};
