#pragma once

#include "icg_common.h"

#ifdef WITH_OPENCV
#include "opencv2/core/core.hpp"       ///< cv::Mat
#include "opencv2/highgui/highgui.hpp" ///< cv::imShow
#include "opencv2/contrib/contrib.hpp" ///< cvtConvert
#include "opencv2/imgproc/types_c.h"   ///< CV_BGRA2RGBA
#endif

class Generalbuffer{

private:
	bool _rb = false;
	GLuint _width;
	GLuint _height;
	GLuint _fbo;

	GLuint _count;
	GLuint** _tex;
	GLuint _depth_rb;

	GLint _internal_format;
	GLenum _format;
	GLenum _type;

public:
	Generalbuffer();

	void init(GLuint count);

	void setTexture(GLuint index, GLuint* texture);
	void genTextures(bool use_interpolation = false);

	void setFormat(GLint internal_format, GLenum format, GLenum type);
	void setSize(GLuint width, GLuint height);
	void genTextureImages();

	void genRenderbuffer();
	void genFramebuffer(const GLenum buffers[], GLuint count);

	int init_texture(bool use_interpolation = false);
	void set_texture(GLuint* texture, bool use_interpolation = false);

	void bind(const GLenum buffers[], GLuint count);
	void unbind();
	void clear();
	void cleanup();

	GLuint* getTexture(GLuint index);
};