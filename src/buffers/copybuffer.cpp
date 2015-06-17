#include "copybuffer.h"


Copybuffer::Copybuffer(){
	_tex = (GLuint*)malloc(sizeof(GLuint));
}

///--- Warning: ovverrides viewport!!
void Copybuffer::bind() {
	glViewport(0, 0, _width, _height);
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
	const GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, buffers);
}

void Copybuffer::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Copybuffer::clear() {
	glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
	glClearColor(.0, .0, .0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glClearColor(.937, .937, .937, 1.0);
}

void Copybuffer::init(GLuint* tex_out) {

	_tex = tex_out;

	glGenTextures(1, _tex);
	glBindTexture(GL_TEXTURE_2D, *_tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F,
		_width, _height, 0,
		GL_RED, GL_FLOAT, NULL); ///< how to load from buffer

	///--- Tie it all together
	{
		glGenFramebuffers(1, &_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER_EXT, _fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 /*location = 0*/, GL_TEXTURE_2D, *_tex, 0 /*level*/);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cerr << "!!!ERROR: Framebuffer not OK :(" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0); ///< avoid pollution
	}
}

void Copybuffer::resize(float width, float height) {
	_width = width;
	_height = height;
}

void Copybuffer::cleanup() {
	glDeleteTextures(1, _tex);
	glBindFramebuffer(GL_FRAMEBUFFER, 0 /*UNBIND*/);
	glDeleteFramebuffers(1, &_fbo);
}

