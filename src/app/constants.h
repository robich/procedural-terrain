#pragma once

const int WIDTH = 1600;
const int HEIGHT = 1000;

const string IO_HEADER_STRING = "# --- HEADER: terrain data --- #";
const string IO_HEADER_CREDITS = "# --- Credits: Alain Milliet, Julien Perrenoud, Robin Genolet --- #";
const string IO_PATH_TO_SAVED_TERRAINS = "../saved_terrains/";

const string IO_PATH_TO_SKYBOXES = "textures/skybox/";

const int DRAW_NOISE = 0;
const int COPY_NOISE = 1;

const vec3 START_CAM_UP = vec3(0.0f, 1.0f, 0.0f);
const vec3 START_CAM_DIR = vec3(0.0f, -1.0f, -2.0f).normalized();
const vec3 START_CAM_POS = vec3(0.0f, 2.0f, 4.0f);

const float MAX_Y_CAM_DIR = 0.9;

const float DX = 0.01f;
const float DPHI = 1.0f;

const float NEAR = 0.1f;
const float FAR = 40.0f;

const int FALL = 0;
const int SLIDE = 1;
const int MELT = 2;
const int SMOOTH = 3;

const int NOISE_MODE = 0;
const int SNOW_MODE = 1;
const int DIRT_MODE = 2;
const int GRASS_MODE = 3;
const int COPY_MODE = 4;
const int SAND_MODE = 5;

const int NORMAL = 0;
const int ONLY_REFLECT = 1;
const int ILLUMINATE = 2;

const GLuint TYPE_ATTACHMENT_0 = 0;
const GLenum BUFFER_ATTACHMENT_0[] = { 
	GL_COLOR_ATTACHMENT0 
};

const GLuint TYPE_ATTACHMENT_1 = 1;
const GLenum BUFFER_ATTACHMENT_1[] = {
	GL_COLOR_ATTACHMENT0,
	GL_COLOR_ATTACHMENT1
};

const GLuint TYPE_ATTACHMENT_2 = 3;
const GLenum BUFFER_ATTACHMENT_2[] = {
	GL_COLOR_ATTACHMENT0,
	GL_COLOR_ATTACHMENT1,
	GL_COLOR_ATTACHMENT2
};

const GLuint TYPE_ATTACHMENT_3 = 3;
const GLenum BUFFER_ATTACHMENT_3[] = {
	GL_COLOR_ATTACHMENT0,
	GL_COLOR_ATTACHMENT1,
	GL_COLOR_ATTACHMENT2,
	GL_COLOR_ATTACHMENT3
};

const GLuint TYPE_ATTACHMENT_DEPTH = 4;
const GLenum BUFFER_ATTACHMENT_DEPTH[] = {
	GL_DEPTH_ATTACHMENT
};

const int BUFFER_DIRT_COUNT = 6;
const GLenum BUFFER_DIRT[] = {
	GL_COLOR_ATTACHMENT0,
	GL_COLOR_ATTACHMENT1,
	GL_COLOR_ATTACHMENT2,
	GL_COLOR_ATTACHMENT3,
	GL_COLOR_ATTACHMENT4,
	GL_COLOR_ATTACHMENT5,
};

typedef enum {
	FREE,
	FPS,
	BEZIER,
	CAMERA_PATH,
	INERTIA
} NAVIGATION_MODE;

typedef enum {
	NO_THEME,
	SUNNY,
	NIGHT,
	SUN_SET
} ThemeType;

typedef enum {
	NO_SKY,
	BLUE,
	CLOUDS,
	SUNSET,
	SEA,
	LAKE,
	NIGHTSKY,
} SkyboxType;

const string SKYBOX_DIR[] = {
	"",
	"blue/",
	"clouds/",
	"sunset/",
	"sea/",
	"lake/",
	"night/"
};

typedef enum {
	NO_NOISE,
	RANDOM_NOISE,
	PERLIN_NOISE,
	WORLEY_NOISE
} NoiseType;

typedef enum {
	FBM,
	MULTIFRACTAL
} FractalType;

typedef enum {
	NO_EFFECT,
	ABSOLUTE_VALUE,
	CLAMP_EXTREMAS,
	DISCRETIZE
} Effect;

typedef enum {
	NO_TEXTURE,
	SHADES,
	TEXTURE
} TextureType;