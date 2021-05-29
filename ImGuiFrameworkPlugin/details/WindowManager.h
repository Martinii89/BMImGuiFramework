#pragma once

#include "Plugins/PluginHostWrapper.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"

namespace imgui_framework::details
{
	struct LoadedWindow
	{
		std::string name;
		std::string backend;
		BakkesMod::Plugin::LoadedPlugin* owner;
		plugins::PluginWindowPtr window;
	};

	class WindowManager
	{
	public:
		bool AddWindow(const plugins::PluginWindowPtr& window, BakkesMod::Plugin::LoadedPlugin* owner);
		bool RemoveWindow(const plugins::PluginWindowPtr& window, BakkesMod::Plugin::LoadedPlugin* owner);

	private:
		std::vector<LoadedWindow> loaded_windows_;
	};
}
