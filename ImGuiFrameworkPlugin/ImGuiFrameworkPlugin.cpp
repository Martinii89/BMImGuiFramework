#include "pch.h"
#include "ImGuiFrameworkPlugin.h"
#include "details/FrameworkImpl.h"

#include "version.h"

namespace imgui_framework
{
	constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);
}


BAKKESMOD_PLUGIN(ImGuiFrameworkPlugin, "Multi-version ImguiFramework for bakkesmod", imgui_framework::plugin_version, PLUGINTYPE_THREADED | PLUGINTYPE_THREADEDUNLOAD)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

ImGuiFrameworkPlugin::ImGuiFrameworkPlugin(): pImpl(new imgui_framework::Impl(this), [](imgui_framework::Impl* impl) { delete impl; }) { }

void ImGuiFrameworkPlugin::onLoad()
{
	_globalCvarManager = cvarManager;
	Pimpl()->cv_ = cvarManager;
	Pimpl()->gw_ = gameWrapper;
	Pimpl()->weak_main_ = singleton;
	Pimpl()->OnLoad();
}

void ImGuiFrameworkPlugin::onUnload()
{
	Pimpl()->OnUnload();
}


IMGUI_FRAMEWORKAPI std::unique_ptr<imgui_framework::plugins::PluginHostWrapper> ImGuiFrameworkPlugin::RegisterPlugin(BakkesModPlugin* plugin, std::function<void()> on_host_unload)
{
	return Pimpl()->RegisterPlugin(plugin, std::move(on_host_unload));
}

bool ImGuiFrameworkPlugin::AddWindow(const imgui_framework::plugins::PluginWindowPtr& new_window, BakkesMod::Plugin::LoadedPlugin* owner)
{
	return Pimpl()->AddWindow(new_window, owner);
}

bool ImGuiFrameworkPlugin::RemoveWindow(const imgui_framework::plugins::PluginWindowPtr& window, BakkesMod::Plugin::LoadedPlugin* owner)
{
	return Pimpl()->RemoveWindow(window, owner);
}

void ImGuiFrameworkPlugin::UnregisterPlugin(imgui_framework::plugins::PluginHostWrapper* host_wrapper)
{
	Pimpl()->UnregisterPlugin(host_wrapper);
}
