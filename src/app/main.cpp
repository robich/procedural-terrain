#include "main.h"

// --- Params --- // 

WindowParams window_params{ WIDTH, HEIGHT };
ThemeParams theme_params;
NoiseParams noise_params;
GrassParams grass_params;
SandParams sand_params;
SnowParams snow_params;
ShadingParams shading_params;
WaterParams water_params;
TextureParams texture_params;
GridParams grid_params;

AppParams app_params{
	&theme_params,
	&window_params,
	&grid_params,
	&noise_params,
	&grass_params,
	&sand_params,
	&snow_params,
	&texture_params,
	&shading_params,
	&water_params,
};

// --- Scene Objects --- // 

Grid grid;
Water water;
Box box;
Sky sky;

// --- Buffers --- // 

Generalbuffer fb_mirror;
Generalbuffer fb_water_depth;
Generalbuffer fb_shadow;

// --- Textures --- //

GLuint* _tex_height;
GLuint* _tex_snow;
GLuint* _tex_dirt;
GLuint* _tex_grass;
GLuint* _tex_sand;

GLuint* _tex_mirror;
GLuint* _tex_water_depth;
GLuint* _tex_shadow;

GLuint* _tex_normal_map;

// --- Camera --- //
Camera camera(&app_params);
NAVIGATION_MODE navmode;
Bezier bezier;

// --- Other --- //

bool ant_tw_entering_value = false;

Terrain terrain;

void initBuffers() {
	
	fb_mirror.init(1);
	fb_mirror.genTextures();
	fb_mirror.setFormat(GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE);
	
	fb_mirror.setSize(WIDTH, HEIGHT);
	fb_mirror.genTextureImages();
	fb_mirror.genRenderbuffer();
	fb_mirror.genFramebuffer(BUFFER_ATTACHMENT_0, 1);
	
	fb_water_depth.init(1);
	fb_water_depth.genTextures();
	fb_water_depth.setFormat(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);

	fb_water_depth.setSize(WIDTH, HEIGHT);
	fb_water_depth.genTextureImages();
	fb_water_depth.genFramebuffer(BUFFER_ATTACHMENT_DEPTH, 1);

	fb_shadow.init(1);
	fb_shadow.genTextures(true);
	fb_shadow.setFormat(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT);

	fb_shadow.setSize(WIDTH, HEIGHT);
	fb_shadow.genTextureImages();
	fb_shadow.genFramebuffer(BUFFER_ATTACHMENT_DEPTH, 1);

	check_error_gl();
}

// Gets called when the windows is resized.
void resize_callback(int width, int height) {
	
	window_params.width = width;
	window_params.height = height;
	/*
	fb_mirror.setSize(width, height);
	fb_mirror.genTextureImages();
	fb_mirror.wrap(BUFFER_ATTACHMENT_0, 1);

	fb_water_depth.setSize(width, height);
	fb_water_depth.genTextureImages();
	fb_water_depth.wrap(BUFFER_ATTACHMENT_0, 1);*/

	camera.compute_projection_matrix();

	glViewport(0, 0, width, height);
}

void compute_height_map() {
	terrain.resize();
	terrain.renderFractal();
	terrain.addGrass();
	terrain.addSnow();
	terrain.addSand();
	sky.loadSky();
}

void init(){

    // Sets background color.
    glClearColor(/*gray*/ .937,.937,.937, /*solid*/1.0);
	
    // Enable depth test.
    glEnable(GL_DEPTH_TEST);

	// Possible to specify the travel time in the last argument. Note: the bigger the slower.
	bezier.init(WIDTH, HEIGHT, 20);

	initParams();

	terrain.init(&app_params);
	
	initBuffers();
	initTextures();
	initSceneObjects();

	navmode = FREE;
	
	compute_height_map();
	
	resize_callback(WIDTH, HEIGHT);

#ifdef WITH_ANTTWEAKBAR
	initAntTwBar(&app_params, &sky, &bezier);
#endif
	check_error_gl();
}

// Gets called for every frame.
void display(){
	
	opengp::update_title_fps("Mountains");

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 view_matrix;
	mat4 view_matrix_mirrored;
	
	switch(navmode) {
	case BEZIER:
	{
		vec3 cam_pos(2.0f, 2.0f, 2.0f);
		vec3 cam_look(0.0f, 0.0f, 0.0f);
		vec3 cam_up(0.0f, 1.0f, 0.0f);

		bezier.sample_points(cam_pos, cam_look);

		vec3 cam_pos_mirrored = vec3(cam_pos[0], 2*water_params.height-cam_pos[1], cam_pos[2]);
		vec3 cam_look_mirrored = vec3(cam_look[0], 2 * water_params.height - cam_look[1], cam_look[2]);

		view_matrix = Eigen::lookAt(cam_pos, cam_look, cam_up);

		view_matrix_mirrored = Eigen::lookAt(cam_pos_mirrored, cam_look_mirrored, cam_up);
	}
			break;
		case FREE:
		case INERTIA:
		case FPS:
		case CAMERA_PATH:
			camera.move(navmode, &grid);
			view_matrix = camera.get_view_matrix();
			view_matrix_mirrored = camera.get_view_matrix_mirrored();
			break;
	}
	
	mat4 projection_matrix = camera.get_projection_matrix();
	mat4 light_view_matrix = shading_params.get_view_matrix();
	mat4 light_projection_matrix = shading_params.get_projection_matrix(); 
	
	// 1. Shadow

	fb_shadow.bind(BUFFER_ATTACHMENT_0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		grid.draw(view_matrix, projection_matrix, light_view_matrix, light_projection_matrix, ILLUMINATE);
	fb_shadow.unbind();

	// 2. Water Reflects

	fb_mirror.bind(BUFFER_ATTACHMENT_0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		grid.draw(view_matrix_mirrored, projection_matrix, light_view_matrix, light_projection_matrix, ONLY_REFLECT);
		sky.draw(view_matrix_mirrored, projection_matrix);
	fb_mirror.unbind();

	check_error_gl();
	
	// 3. Water depth
	
	fb_water_depth.bind(BUFFER_ATTACHMENT_0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//box.draw(view_matrix, projection_matrix);
		grid.draw(view_matrix, projection_matrix, light_view_matrix, light_projection_matrix, NORMAL);
	fb_water_depth.unbind();
	
	// 4. Scene

	grid.draw(view_matrix, projection_matrix, light_view_matrix, light_projection_matrix, NORMAL);
	water.draw(view_matrix, projection_matrix);
	box.draw(view_matrix, projection_matrix);
	sky.draw(view_matrix, projection_matrix);
	
	if (navmode == CAMERA_PATH) {

		// Display points
		bezier.draw_cam_pos_points(view_matrix, projection_matrix);
		bezier.draw_cam_look_points(view_matrix, projection_matrix);

		// Display bezier curves
		bezier.pos_curve_draw(view_matrix, projection_matrix);
		bezier.cam_look_draw(view_matrix, projection_matrix);
	}
	
#ifdef WITH_ANTTWEAKBAR
	TwDraw();
#endif
}

void cleanup(){
#ifdef WITH_ANTTWEAKBAR
    TwTerminate();
#endif
	bezier.cleanup();
}

int main(int, char**){
    glfwInitWindowSize(window_params.width, window_params.height);
    glfwCreateWindow("Terrain");
    glfwDisplayFunc(display);

	glfwSetWindowSizeCallback(OnWindowSize);
	glfwSetMouseButtonCallback(OnMouseButton);
	glfwSetMousePosCallback(OnMousePos);
	glfwSetMouseWheelCallback(OnMouseWheel);
	glfwSetKeyCallback(OnKey);
	glfwSetCharCallback(OnChar);

    init();
    glfwMainLoop();
    cleanup();
    return EXIT_SUCCESS;
}

// --- Helpers --- //

void initParams() {

	loadFromFile(IO_PATH_TO_SAVED_TERRAINS + "DEFAULT.terrain", &app_params, &bezier);

}

void initSceneObjects() {
	
	box.init(&app_params);
	box.setHeightTexture(_tex_height);
	box.setWaterDepthTexture(_tex_water_depth);

	camera.set_height_texture(_tex_height);

	grid.init(&app_params);
	grid.setTexHeight(_tex_height);
	grid.setTexSnow(_tex_snow);
	grid.setTexShadow(_tex_shadow);
	grid.setTexGrass(_tex_grass);
	grid.setTexSand(_tex_sand);

	water.init(&app_params);
	water.setHeightTexture(_tex_height);
	water.setMirrorTexture(_tex_mirror);
	water.setDepthTexture(_tex_water_depth);
	
	sky.init(&app_params);
	
	check_error_gl();
}

void initTextures() {
	
	_tex_height = terrain.getHeightTexture();
	_tex_dirt = terrain.getDirtTexture();
	_tex_snow = terrain.getSnowTexture();
	_tex_grass = terrain.getGrassTexture();
	_tex_sand = terrain.getSandTexture();

	_tex_shadow = fb_shadow.getTexture(0);
	_tex_mirror = fb_mirror.getTexture(0);
	_tex_water_depth = fb_water_depth.getTexture(0);
}


// --- Callbacks --- //

// Callback function called by GLFW when a mouse button is clicked
void GLFWCALL OnMouseButton(int glfwButton, int glfwAction)
{
#ifdef WITH_ANTTWEAKBAR
	if (!TwEventMouseButtonGLFW(glfwButton, glfwAction))   // Send event to AntTweakBar
	{
#endif
		// Event not handled by AntTweakBar, we handle it ourselves

		if (navmode == BEZIER) {
			if (glfwAction != GLFW_RELEASE) return; ///< only act on release
			std::cerr << "[Warning] No mouse interaction in Bezier mode.\n" << std::flush;
			return;
		}
		
		if (navmode == CAMERA_PATH) {
			camera.mouse_button(glfwButton, glfwAction);
			bezier.selection_button(glfwButton, glfwAction, camera.get_view_matrix(), camera.get_projection_matrix());
		}
		else {
			// FREE mode or default
			camera.mouse_button(glfwButton, glfwAction);
		}
#ifdef WITH_ANTTWEAKBAR
	}
#endif
}


// Callback function called by GLFW when mouse has moved
void GLFWCALL OnMousePos(int mouseX, int mouseY)
{
#ifdef WITH_ANTTWEAKBAR
	if (!TwEventMousePosGLFW(mouseX, mouseY))  // Send event to AntTweakBar
	{
#endif
		if (navmode == BEZIER) {
			return;
		}

		camera.mouse_pos(mouseX, mouseY);
#ifdef WITH_ANTTWEAKBAR
	}
#endif
}


// Callback function called by GLFW on mouse wheel event
void GLFWCALL OnMouseWheel(int pos)
{
#ifdef WITH_ANTTWEAKBAR
	if (!TwEventMouseWheelGLFW(pos))   // Send event to AntTweakBar
	{
#endif
		// Nothing for the moment
#ifdef WITH_ANTTWEAKBAR
	}
#endif
}


// Callback function called by GLFW on key event
void GLFWCALL OnKey(int glfwKey, int glfwAction)
{
#ifdef WITH_ANTTWEAKBAR
	// The following lines fix the unwanted behavior where bezier mode is loaded when typing a '2' in any field of the AntTwBar.
	if (TwKeyTest(2, 0) || TwKeyTest(1, 0)) {
		ant_tw_entering_value = true;
	}
	if (ant_tw_entering_value && glfwKey != 294 /* enter */ && glfwKey != 295 /* delete */) {
		ant_tw_entering_value = false;
		return;
	}
	if (!TwEventKeyGLFW(glfwKey, glfwAction))  // Send event to AntTweakBar
	{
#endif

		if (glfwKey == GLFW_KEY_ESC && glfwAction == GLFW_PRESS) // Want to quit?
			glfwCloseWindow();
		else
		{
			camera.keyboard(glfwKey, glfwAction);

			if (glfwAction != GLFW_RELEASE) return; ///< only act on release

			bool key_used = false;


			switch (glfwKey){
			case '1':
				navmode = FREE;
				std::cout << "[Info] Running in free mode\n" << std::flush;
				twBarVisible(true);
				key_used = true;
				break;
			case '2':
				navmode = FPS;
				std::cout << "[Info] Running in FPS mode\n" << std::flush;
				twBarVisible(true);
				key_used = true;
				break;
			case '3':
				navmode = BEZIER;
				std::cout << "[Info] Running in bezier mode\n" << std::flush;
				bezier.set_start_time(glfwGetTime());
				twBarVisible(false);
				key_used = true;
				break;
			case '4':
				navmode = CAMERA_PATH;
				std::cout << "[Info] Running in camera path editing mode\n" << std::flush;
				twBarVisible(false);
				key_used = true;
				break;
			case '5':
				navmode = INERTIA;
				std::cout << "[Info] Running in camera path editing mode\n" << std::flush;
				key_used = true;
				break;
			case 87 /* w */:
				if (navmode == BEZIER) {
					unsigned int new_value = bezier.get_travel_time() - 1;
					if (new_value < 1)
						new_value = 1;
					bezier.set_travel_time(new_value);
					std::cout << "[Info] Travel time changed to  '" << new_value << "'\n" << std::flush;
				}
				key_used = true;
				break;
			case 83 /* s */:
				if (navmode == BEZIER) {
					unsigned int new_value = bezier.get_travel_time() + 1;
					if (new_value == UINT_MAX)
						new_value--;
					bezier.set_travel_time(new_value);
					std::cout << "[Info] Travel time changed to  '" << new_value << "'\n" << std::flush;
				}
				key_used = true;
				break;
			case 269 /* F12 */:
				if (save_screenshot("../screenshots/" + get_unique_name() + ".tga", WIDTH, HEIGHT))
					std::cout << "[Info] Screenshot saved successfully\n" << std::flush;
				else
					std::cerr << "[Error] An error occured while trying to save the screenshot\n" << std::flush;

				break;
			default:
				/* Too verbose
				if (!key_used)
					std::cerr << "[Warning] No actions attached to key '" << glfwKey << "'\n" << std::flush;*/
				break;
			}
		}
#ifdef WITH_ANTTWEAKBAR
	}
#endif
}


// Callback function called by GLFW on char event
void GLFWCALL OnChar(int glfwChar, int glfwAction)
{
#ifdef WITH_ANTTWEAKBAR
	if (!TwEventCharGLFW(glfwChar, glfwAction))    // Send event to AntTweakBar
	{
#endif
		// Nothing for the moment
#ifdef WITH_ANTTWEAKBAR
	}
#endif
}


// Callback function called by GLFW when window size changes
void GLFWCALL OnWindowSize(int width, int height)
{
#ifdef WITH_ANTTWEAKBAR
	// Send the new window size to AntTweakBar
	TwWindowSize(width, height);
#endif

	resize_callback(width, height);
}