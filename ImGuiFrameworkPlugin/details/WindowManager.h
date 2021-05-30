#pragma once

#include "WindowManagerGUI.h"
#include "Plugins/PluginHostWrapper.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"

namespace imgui_framework::details
{
	//struct LoadedWindow
	//{
	//	std::string name;
	//	std::string backend;
	//	BakkesMod::Plugin::LoadedPlugin* owner;
	//	plugins::PluginWindowPtr window;
	//};

	class WindowManager
	{
	public:
		explicit WindowManager(std::vector<RegisteredPlugin>& plugin_list);
		std::shared_ptr<WindowManagerGUI> gui_window_;
	private:
		std::vector<RegisteredPlugin>& plugin_list_;
	};
}
