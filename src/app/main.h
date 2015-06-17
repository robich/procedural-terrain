#pragma once

#include "icg_common.h"
#include "constants.h"
#include "params.h"
#include "../camera/camera.h"
#include "../scene/grid.h"
#include "../camera/bezier.h"
#include "../noise/quad.h"
#include "../noise/terrain.h"
#include "../buffers/generalbuffer.h"
#include "../scene/water.h"
#include "../scene/sky.h"
#include "../tw/tweakbar.h"
#include "../scene/box.h"
#include "../tw/io.h"


using namespace std;

void GLFWCALL OnWindowSize(int width, int height);
void GLFWCALL OnChar(int glfwChar, int glfwAction);
void GLFWCALL OnKey(int glfwKey, int glfwAction);
void GLFWCALL OnMouseWheel(int pos);
void GLFWCALL OnMousePos(int mouseX, int mouseY);
void GLFWCALL OnMouseButton(int glfwButton, int glfwAction);

void compute_height_map();
void resize_callback(int width, int height);

void initTextures();
void initSceneObjects();
void initParams();
void initBuffers();
