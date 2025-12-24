#pragma once

// ########################################################
//                          Platform Globels
// ########################################################
static bool running = true;

// ########################################################
//                          Platform Functions
// ########################################################
bool platfrom_create_window(int width, int height, char* title);
void platform_update_window();
void* platform_load_gl_function(char* functionName);
void platform_Swap_Buffer();

// ########################################################
//                          Windows Platform
// ########################################################