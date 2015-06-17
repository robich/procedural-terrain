#pragma once

#include "icg_common.h"
#include "../app/constants.h"
#include "../camera/point.h"

const static Scalar H = .7;
const static Scalar R = 2;

class BezierCurve{
private:
	class Hull{
	public:
		Hull() {
			_p1 = _p2 = _p3 = _p4 = vec3::Zero();
		}

		vec3& p1(){ return _p1; }
		vec3& p2(){ return _p2; }
		vec3& p3(){ return _p3; }
		vec3& p4(){ return _p4; }

	private:
		vec3 _p1;
		vec3 _p2;
		vec3 _p3;
		vec3 _p4;
	};

private:
	std::vector<Hull> _hulls;                  ///< Bezier control points
	std::vector<vec3> _vertices; ///< Bezier evaluated points
	std::vector<Scalar> _param;  ///< Arc length parameterization
	GLuint _vao;                 ///< Vertex array objects
	GLuint _pid;          ///< GLSL program ID
	GLuint _vbo;

private:
	void bezier(Hull& p, int depth = 0){
		Hull l;
		Hull r;

		//--- endpoints
		l.p1() = p.p1();
		r.p4() = p.p4();

		//--- one-in
		l.p2() = (p.p1() + p.p2()) / 2.0;
		r.p3() = (p.p3() + p.p4()) / 2.0;

		//--- interior
		l.p3() = (l.p2() + (p.p2() + p.p3()) / 2.0) / 2.0;
		r.p2() = (r.p3() + (p.p2() + p.p3()) / 2.0) / 2.0;

		//--- middle
		l.p4() = (l.p3() + r.p2()) / 2.0;
		r.p1() = l.p4();

		//--- recursion v.s. draw
		if (depth<5){
			bezier(l, depth + 1);
			bezier(r, depth + 1);
		}
		else {
			_vertices.push_back(l.p1());
		}
		_vertices.push_back(p.p4());
	}

	void compute_parameterization(){
		_param.clear();;
		if (_vertices.empty()) {
			return;
		}
		_param.resize(_vertices.size(), 0);

		_param[0] = 0;
		for (int i = 1; i < _vertices.size(); i++){
			_param[i] = _param[i-1] + sqrt(((_vertices[i].x() - _vertices[i-1].x()) * (_vertices[i].x() - _vertices[i-1].x())) + ((_vertices[i].y() - _vertices[i-1].y()) * (_vertices[i].y() - _vertices[i-1].y())));
		}
	}
public:
	void init(GLuint pid){
		///--- Set the (compiled) shaders
		_pid = pid;

		/// Generate the vertex array
		glGenVertexArrays(ONE, &_vao);
		glBindVertexArray(_vao);

		/// Generate one buffer, put the resulting identifier in vertexbuffer
		glGenBuffers(ONE, &_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, _vbo);

		///--- to avoid the current object being polluted
		glBindVertexArray(0);
		glUseProgram(0);
	}
	void set_points(std::vector<ControlPoint> points) {
		_vertices.clear();
		_param.clear();

		// Test size (must be 1 + 3n for n curves)
		if (points.size() % 3 != 1) {
			// error
		}

		_hulls.clear();
		for (int index = 1; index < points.size(); index += 3){
			// Create new Hull
			Hull hull;
			// Initialize it
			hull.p1() = points[index - 1].position();
			hull.p2() = points[index].position();
			hull.p3() = points[index + 1].position();
			hull.p4() = points[index + 2].position();
			// Put it in the array
			_hulls.push_back(hull);
		}

		// Compute bezier & parametrization
		for (Hull hull : _hulls) {
			bezier(hull);
		}
		compute_parameterization();
	}
	static bool cmp(const Scalar &a, const Scalar &b){
		return (a<b);
	}

	void sample_point(double t, vec3 &sample) {
		if (_vertices.empty() || _param.empty()) {
			return;
		}

		if (_param.size() == 1 || _vertices.size() == 1){
			sample = _vertices[0];
			return;
		}

		double curve_length = _param[_param.size() - 1];
		double distance = t * curve_length;

		int min_index = 0;
		int max_index = 1;

		while (_param[max_index] < distance) {
			min_index++;
			max_index++;
		}

		// Compute how much we are (between 0 and 1) into the last segment
		double fraction_of_last_segment = (distance - _param[min_index]) / (_param[max_index] - _param[min_index]);

		// Weighted sum of two neighbouring vertices with respect to the fraction computed above
		sample = (1 - fraction_of_last_segment) * _vertices[min_index] + fraction_of_last_segment * _vertices[max_index];
	}

	void draw(const mat4& model, const mat4& view, const mat4& projection){
		if (_vertices.empty()) return;

		glUseProgram(_pid);
		glBindVertexArray(_vao);
		check_error_gl();

		///--- Vertex Attribute ID for Vertex Positions
		GLuint position = glGetAttribLocation(_pid, "position");
		glEnableVertexAttribArray(position);
		glVertexAttribPointer(position, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);

		///--- vertices
		// !!! TODO: This line causes the AntTweakBat to fuck up hard. !!!
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3)*_vertices.size(), &_vertices[0], GL_STATIC_DRAW);

		///--- setup view matrices        
		GLuint projection_id = glGetUniformLocation(_pid, "projection");
		glUniformMatrix4fv(projection_id, ONE, DONT_TRANSPOSE, projection.data());
		mat4 MV = view*model;
		GLuint model_view_id = glGetUniformLocation(_pid, "model_view");
		glUniformMatrix4fv(model_view_id, ONE, DONT_TRANSPOSE, MV.data());
		check_error_gl();

		glDrawArrays(GL_LINE_STRIP, 0, _vertices.size());
		glDisableVertexAttribArray(position);
		glBindVertexArray(0);
		glUseProgram(0);
		check_error_gl();
	}
};
