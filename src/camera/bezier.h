#pragma once

#include "icg_common.h"
#include "../app/constants.h"
#include "bezier_curve.h";
#include "../camera/point.h"

class Bezier {

private:
	bool unproject(int mouse_x, int mouse_y, vec3 &p, const mat4 &view, const mat4 &projection);
	void render_selection(const mat4 &view, const mat4 &projection);
	vec2 transform_xy_screen(int x, int y);

	mat4 model = mat4::Identity();

	GLuint _pid_bezier;
	GLuint _pid_point;
	GLuint _pid_point_selection;

	BezierCurve cam_pos_curve;
	BezierCurve cam_look_curve;

	std::vector<ControlPoint> cam_pos_points;
	std::vector<ControlPoint> cam_look_points;
	int selected_point;

	int width;
	int height;
	int travel_time;
	int start_time;

public:
	void init(int width, int height, int travel_time);
	void cleanup();
	void set_start_time(int start_time);
	int get_start_time();
	void set_travel_time(int travel_time);
	int get_travel_time();
	void sample_points(vec3 &sample1, vec3 &sample2);
	void pos_curve_draw(const mat4& view, const mat4& projection);
	void cam_look_draw(const mat4& view, const mat4& projection);
	void draw_cam_pos_points(const mat4& view, const mat4& projection);
	void draw_cam_look_points(const mat4& view, const mat4& projection);
	void selection_button(int button, int action, const mat4 &view, const mat4 &projection);
	mat4 get_view_matrix(const vec3 &eye, const vec3 &center, const vec3 &up);
	mat4 get_view_matrix_mirrored(const vec3 &eye, const vec3 &center, const vec3 &up);
	void print_control_points();

	/* For IO */
	void set_cam_pos_points(std::vector<ControlPoint> cam_pos_points);
	void set_cam_look_points(std::vector<ControlPoint> cam_look_points);
	std::vector<ControlPoint> get_cam_pos_points();
	std::vector<ControlPoint> get_cam_look_points();
};