#pragma once

#include <math.h>

#include "icg_common.h"
#include "../app/constants.h"
#include "../scene/grid.h";

using namespace Eigen;

class Camera {

private:

	// Helpers
	vec2 transform_screen_coords(int x, int y);

	GLuint* _tex_height;
	vec3 _cam_up;
	vec3 _cam_pos;
	vec3 _cam_dir;

	vec4 _old_cam_pos;
	vec4 _old_cam_dir;

	mat4 _projection_matrix;
	mat4 _view_matrix;

	mat4 _dir_matrix;

	vec3 _anchor_pos;

	std::map<int, bool> pressed_keys;

	WindowParams* _window_params;
	WaterParams* _water_params;

	float last_time;
	float last_time_pressed_ws;
	float last_time_pressed_ad;
	float last_time_pressed_updo;
	float last_key;
	int glfwAction;

public:
	const vec3 START_CAM_UP = vec3(0.0f, 1.0f, 0.0f);
	const vec3 START_CAM_DIR = vec3(0.0f, -1.0f, -2.0f).normalized();
	const vec3 START_CAM_POS = vec3(0.0f, 2.0f, 4.0f);

	// Constructor
	Camera(AppParams* app_params);

	// Getters
	mat4 get_view_matrix();
	mat4 get_projection_matrix();
	mat4 get_view_matrix_mirrored();

	// (Re)Setters
	void compute_view_matrix();
	void compute_projection_matrix();
	void set_height_texture(GLuint* tex_height);

	// User Interaction
	void mouse_button(int button, int action);
	void mouse_pos(float x, float y);
	void keyboard(int key, int action);
	void move(NAVIGATION_MODE navmode, Grid* grid);
};
