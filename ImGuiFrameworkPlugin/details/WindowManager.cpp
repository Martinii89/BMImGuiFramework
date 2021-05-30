#include "pch.h"
#include "WindowManager.h"

#include "Plugins/PluginWindow.h"

using namespace imgui_framework::details;

WindowManager::WindowManager(std::vector<RegisteredPlugin>& plugin_list): plugin_list_(plugin_list)
{
	gui_window_ = std::make_shared<WindowManagerGUI>(plugin_list);
}

//bool WindowManager::AddWindow(const plugins::PluginWindowPtr& window, BakkesMod::Plugin::LoadedPlugin* owner)
//{
//	LOG("Adding window {} from {}", window->GetWindowName(), owner->_details->pluginName);
//	loaded_windows_.push_back({window->GetWindowName(), window->GetRendererVersion(), owner, window});
//	return true;
//}
//
//bool WindowManager::RemoveWindow(const plugins::PluginWindowPtr& window, BakkesMod::Plugin::LoadedPlugin* owner)
//{
//	LOG("Removing window {} from {}", window->GetWindowName(), owner->_details->pluginName);
//	const auto it = std::find_if(
//		loaded_windows_.begin(),
//		loaded_windows_.end(),
//		[window, owner](const LoadedWindow& l)
//		{
//			return l.window == window && l.owner == owner;
//		});
//	if (it != loaded_windows_.end())
//	{
//		loaded_windows_.erase(it);
//		return true;
//	}
//	LOG("Remove failed. Cound't find it in the loaded list");
//	return false;
//}
