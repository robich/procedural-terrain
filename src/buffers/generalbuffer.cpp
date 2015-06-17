#include "generalbuffer.h"

Generalbuffer::Generalbuffer(){
	_tex = (GLuint**)malloc(sizeof(GLuint*));
}

void Generalbuffer::init(GLuint count) {
	_count = count;
	_tex = (GLuint**)calloc(_count, sizeof(GLuint*));
	for (int i = 0; i < count; i++) {
		_tex[i] = (GLuint*)malloc(sizeof(GLuint));
	}
}

void Generalbuffer::setTexture(GLuint index, GLuint* texture){
	_tex[index] = texture;
}

void Generalbuffer::genTextures(bool use_interpolation) {
	for (int i = 0; i < _count; i++) {
		glGenTextures(1, _tex[i]);
		glBindTexture(GL_TEXTURE_2D, *_tex[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		if (use_interpolation) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		} else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
	}
}

void Generalbuffer::setSize(GLuint width, GLuint height) {
	_width = width;
	_height = height;
}

void Generalbuffer::setFormat(GLint internal_format, GLenum format, GLenum type){
	_internal_format = internal_format;
	_format = format;
	_type = type;
}

void Generalbuffer::genTextureImages(){
	for (int i = 0; i < _count; i++) {
		glBindTexture(GL_TEXTURE_2D, *_tex[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, _internal_format,
			_width, _height, 0,
			_format, _type, NULL);
	}
}

void Generalbuffer::genRenderbuffer(){
	_rb = true;
	glGenRenderbuffers(1, &_depth_rb);
	glBindRenderbuffer(GL_RENDERBUFFER, _depth_rb);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, _width, _height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void Generalbuffer::genFramebuffer(const GLenum buffers[], GLuint count) {
	glGenFramebuffers(1, &_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER_EXT, _fbo);

	for (int i = 0; i < count; i++) {
		glBindTexture(GL_TEXTURE_2D, *_tex[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, buffers[i], GL_TEXTURE_2D, *_tex[i], 0);
	}
	if (_rb) glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depth_rb);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cerr << "!!!ERROR: Framebuffer not OK :(" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0); ///< avoid pollution
}


///--- Warning: ovverrides viewport!!
void Generalbuffer::bind(const GLenum buffers[], GLuint count) {
	glViewport(0, 0, _width, _height);
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
	
	glDrawBuffers(count, buffers);
}

void Generalbuffer::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Generalbuffer::clear() {
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
	glClearColor(.0, .0, .0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(.937, .937, .937, 1.0);
}

void Generalbuffer::cleanup() {
	for (int i = 0; i < _count; i++) {
		glDeleteTextures(1, _tex[i]);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0 /*UNBIND*/);
	glDeleteFramebuffers(1, &_fbo);
}

GLuint* Generalbuffer::getTexture(GLuint index) {
	return _tex[index];
}
