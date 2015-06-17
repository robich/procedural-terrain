#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <istream>
#include <ostream>
#include <iterator>
#include <sstream>
#include <algorithm>
#include <ctime>

#include "icg_common.h"
#include "../app/constants.h"
#include "../camera/bezier.h"
#include "../app/params.h"

void writeFile(string file_name, AppParams* app_params, Bezier* bezier);
void loadFromFile(string file_name, AppParams* app_params, Bezier* bezier);

bool save_screenshot(string file_name, int width, int height);
string get_unique_name();