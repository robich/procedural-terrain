#include "camera.h"

Camera::Camera(AppParams* app_params) : _cam_up(START_CAM_UP), _cam_pos(START_CAM_POS), 
_cam_dir(START_CAM_DIR), _window_params(app_params->window_params), _dir_matrix(mat4::Identity())
{
	_cam_up = START_CAM_UP;
	_cam_pos = START_CAM_POS;
	_cam_dir = START_CAM_DIR;

	_water_params = app_params->water_params;


	compute_view_matrix();
	compute_projection_matrix();
}

void Camera::mouse_button(int button, int action) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		int x_i, y_i;
		glfwGetMousePos(&x_i, &y_i);
		vec2 p = transform_screen_coords(x_i, y_i);
		_anchor_pos = vec3(p.x(), p.y(), 0.0f);
		// Store the current state of the model matrix.
		_old_cam_dir = vec4(_cam_dir[0], _cam_dir[1], _cam_dir[2], 1.0f);
	}
}

void Camera::mouse_pos(float x, float y) {
	if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		vec2 p = transform_screen_coords(x, y);

		float x_diff = -DPHI * (p.x() - _anchor_pos[0]);
		float y_diff = -DPHI * (p.y() - _anchor_pos[1]);

		mat4 rot_x = Affine3f(AngleAxisf(x_diff, vec3(0, 1, 0))).matrix();
		mat4 rot_y = Affine3f(AngleAxisf(y_diff, vec3(_old_cam_dir[2], 0, -_old_cam_dir[0]))).matrix();

		_dir_matrix = rot_x * rot_y;

		_cam_dir = (_dir_matrix * _old_cam_dir).head<3>();

		compute_view_matrix();
	}
}

void Camera::compute_view_matrix(){
	vec3 cam_center = _cam_pos + _cam_dir;
	_view_matrix = lookAt(_cam_pos, cam_center, _cam_up);
}

void Camera::compute_projection_matrix(){
	if (_window_params->width != 0 && _window_params->height != 0) {
		_projection_matrix = Eigen::perspective(45.0f, _window_params->width / (float)_window_params->height, NEAR, FAR);
	}
}

vec2 Camera::transform_screen_coords(int x, int y) {
	return vec2(2.0f * (float)x / _window_params->width - 1.0f,
		1.0f - 2.0f * (float)y / _window_params->height);
}

void Camera::keyboard(int key, int action) {

	glfwAction = action;

	if (action == GLFW_RELEASE)
		pressed_keys[key] = false;
	if (action == GLFW_PRESS)
		pressed_keys[key] = true;
}

mat4 Camera::get_view_matrix() {
	return _view_matrix;
}

mat4 Camera::get_projection_matrix() {
	return _projection_matrix;
}

mat4 Camera::get_view_matrix_mirrored() {
	vec3 cam_pos_mirrored = vec3(_cam_pos[0], 2 * _water_params->height -_cam_pos[1], _cam_pos[2]);
	vec3 cam_dir_mirrored = vec3(_cam_dir[0], -_cam_dir[1], _cam_dir[2]);

	vec3 cam_center_mirrored = cam_pos_mirrored + cam_dir_mirrored;

	return lookAt(cam_pos_mirrored, cam_center_mirrored, _cam_up);
}

void Camera::move(NAVIGATION_MODE navmode, Grid* grid) {
	mat4 translation = mat4::Identity();

	float current_time = glfwGetTime();

	float time_diff_inertia_ad = 1;
	float time_diff_inertia_ws = 1;
	float time_diff_inertia_updo = 1;

	float time_diff;
	if (last_time == 0) {
		time_diff = 0;
	}
	else {
		time_diff = current_time - last_time;
	}

	//std::cout << "current_time: " << current_time << ", last_time: " << last_time << ", time_diff: " << time_diff << std::endl;

	last_time = current_time;

	if (navmode == INERTIA) {
		if (pressed_keys[87]) { // W
			last_key = 87;
			last_time_pressed_ws = glfwGetTime();
		}
		if (pressed_keys[83]) {// S
			last_key = 83;
			last_time_pressed_ws = glfwGetTime();
		}
		if (pressed_keys[68]) {// D
			last_key = 68;
			last_time_pressed_ad = glfwGetTime();
		}
		if (pressed_keys[65]) {// A
			last_key = 65;
			last_time_pressed_ad = glfwGetTime();
		}
		if (navmode != FPS) {
			if (pressed_keys[32]) {// SPACE or SHIF
				last_key = 32;
				last_time_pressed_updo = glfwGetTime();
			}
			if (pressed_keys[287]){ // SHIFT
				last_key = 287;
				last_time_pressed_updo = glfwGetTime();
			}
		}

		time_diff_inertia_ws = 1 - (current_time - last_time_pressed_ws);
		time_diff_inertia_ad = 1 - (current_time - last_time_pressed_ad);
		time_diff_inertia_updo = 1 - (current_time - last_time_pressed_updo);

		if (time_diff_inertia_ws < 0){
			time_diff_inertia_ws = 0;
		}

		if (time_diff_inertia_ad < 0){
			time_diff_inertia_ad = 0;
		}

		if (time_diff_inertia_updo < 0){
			time_diff_inertia_updo = 0;
		}


		//std::cout << "last_time_pressed_ws: " << last_time_pressed_ws << ", time_diff_inertia_ws : " << time_diff_inertia_ws << std::endl;

		if (time_diff == 0)	time_diff = 1;

		vec3 forward = 10 * DX * vec3(_cam_dir[0], 0.0f, _cam_dir[2]) * time_diff + 5 * DX * vec3(_cam_dir[0], 0.0f, _cam_dir[2]) * time_diff_inertia_ws;
		vec3 right = 10 * DX * vec3(-_cam_dir[2], 0.0f, _cam_dir[0]) * time_diff + 5 * DX * vec3(-_cam_dir[2], 0.0f, _cam_dir[0]) * time_diff_inertia_ad;
		vec3 up = 10 * DX * vec3(0.0f, 1.0f, 0.0f) * time_diff + 5 * DX * vec3(0.0f, 1.0f, 0.0f) * time_diff_inertia_updo;

		//std::cout << "Camera: (" << _cam_pos.x() << ", " << _cam_pos.z() << ") Height: " << height << std::endl;

		if (time_diff_inertia_ws != 0) {
			if (last_key == 87) {// W
				translation *= Eigen::Affine3f(Eigen::Translation3f(forward)).matrix();
			}
			if (last_key == 83) {// S
				translation *= Eigen::Affine3f(Eigen::Translation3f(-forward)).matrix();
			}
		}

		if (time_diff_inertia_ad != 0) {
			if (last_key == 65) { // A
				translation *= Eigen::Affine3f(Eigen::Translation3f(-right)).matrix();
			}
			if (last_key == 68) { // D
				translation *= Eigen::Affine3f(Eigen::Translation3f(right)).matrix();
			}
		}
		if (navmode != FPS) {
			if (time_diff_inertia_updo != 0) {
				if (last_key == 32) {// SPACE
					translation *= Eigen::Affine3f(Eigen::Translation3f(up)).matrix();
				}
				if (last_key == 287) {// SHIFT
					translation *= Eigen::Affine3f(Eigen::Translation3f(-up)).matrix();
				}
			}
		}
	}
	else {
		// for navmode == FREE mainly

		vec3 forward = 80 * DX * vec3(_cam_dir[0], 0.0f, _cam_dir[2]) * time_diff;
		vec3 right = 80 * DX * vec3(-_cam_dir[2], 0.0f, _cam_dir[0]) * time_diff;
		vec3 up = 80 * DX * vec3(0.0f, 1.0f, 0.0f) * time_diff;

		//float height = grid->get_height(_cam_pos.x(), _cam_pos.y());
		float height = 1;

		//std::cout << "Camera: (" << _cam_pos.x() << ", " << _cam_pos.z() << ") Height: " << height << std::endl;

		if (pressed_keys[65]) // A
			translation *= Eigen::Affine3f(Eigen::Translation3f(-right)).matrix();
		if (pressed_keys[87]) // W
			translation *= Eigen::Affine3f(Eigen::Translation3f(forward)).matrix();
		if (pressed_keys[83]) // S
			translation *= Eigen::Affine3f(Eigen::Translation3f(-forward)).matrix();
		if (pressed_keys[68]) // D
			translation *= Eigen::Affine3f(Eigen::Translation3f(right)).matrix();
		if (pressed_keys[32] && navmode != FPS) // SPACE
			translation *= Eigen::Affine3f(Eigen::Translation3f(up)).matrix();
		if (pressed_keys[287] && navmode != FPS) // SHIFT
			translation *= Eigen::Affine3f(Eigen::Translation3f(-up)).matrix();

	}
	

	_cam_pos = (translation * vec4(_cam_pos[0], _cam_pos[1], _cam_pos[2], 1.0f)).head<3>();

	if (navmode == FPS) {
		float height = grid->get_height(_cam_pos.x(), _cam_pos.z());
		_cam_pos.y() = height + 0.2;
	}

	compute_view_matrix();
}

void Camera::set_height_texture(GLuint* tex_height) {
	_tex_height = tex_height;
}