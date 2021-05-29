#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "ImGuiFrameworkPlugin.h"

#include "version.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);


class ExamplePlugin: public BakkesMod::Plugin::BakkesModPlugin
{

	ImGuiFrameworkPlugin* host_plugin_ = nullptr;

	//Boilerplate
	virtual void onLoad();
	virtual void onUnload();

	void InitHost();
	bool ValidateHostPluginLoaded();
	void FreeHostResources();

	std::unique_ptr<imgui_framework::plugins::PluginHostWrapper> host_wrapper_;
	std::shared_ptr<imgui_framework::plugins::PluginWindow> addon_window_;


};

