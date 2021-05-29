#pragma once

#ifdef _IMGUI_FRAMEWORK
#define IMGUI_FRAMEWORKAPI __declspec(dllexport)
#else
#define IMGUI_FRAMEWORKAPI __declspec(dllimport)
#endif

#include "PluginHostWrapper.h"
#include "PluginWindow.h"