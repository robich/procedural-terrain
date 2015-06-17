#pragma once

#include <AntTweakBar.h>
#include <iostream>
#include <fstream>
#include <string>
#include <istream>
#include <ostream>
#include <iterator>
#include <sstream>
#include <algorithm>

#include "icg_common.h"
#include "../camera/camera.h"
#include "../app/main.h"
#include "../scene/grid.h"
#include "../app/params.h"
#include "../noise/quad.h"
#include "../noise/terrain.h"
#include "../scene/water.h"
#include "../scene/box.h"
#include "../app/constants.h"
#include "../tw/io.h"


void initAntTwBar(AppParams* app_params, Sky* sky, Bezier* bezier);
void twBarVisible(bool b);
void refreshTwBar();
void load_theme(ThemeParams* theme_params);

// --- Value Callbacks --- //

void TW_CALL setIntParamCallback(const void* value, void* clientData);
void TW_CALL setFloatParamCallback(const void* value, void* clientData);
void TW_CALL setBoolParamCallback(const void* value, void* clientData);
void TW_CALL getIntParamCallback(void* value, void* clientData);
void TW_CALL getFloatParamCallback(void* value, void* clientData);
void TW_CALL getBoolParamCallback(void* value, void* clientData);

// --- Save Functions --- //

void TW_CALL CopyStdStringToClient(std::string& destinationClientString, const std::string& sourceLibraryString);
void TW_CALL SaveCB(void * clientData);
void TW_CALL LoadCB(void * clientData);