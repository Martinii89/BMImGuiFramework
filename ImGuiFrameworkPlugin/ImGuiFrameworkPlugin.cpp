#include "pch.h"
#include "ImGuiFrameworkPlugin.h"
#include "bakkesmod/wrappers/PluginManagerWrapper.h"
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
	InitKiero();
}

void ImGuiFrameworkPlugin::onUnload()
{
	for (auto& [plugin_ptr, on_host_unload] : Pimpl()->registered_plugins)
	{
		if (on_host_unload)
		{
			on_host_unload();
		}
	}
}

void ImGuiFrameworkPlugin::InitKiero()
{
	Pimpl()->dx_hook_.Hook();

	cvarManager->registerNotifier("kiero_unhook", [this](...)
	{
		std::thread t([this]
		{
			Pimpl()->dx_hook_.ShutDown();
		});
		t.detach();
	}, "", 0);
}


IMGUI_FRAMEWORKAPI std::unique_ptr<imgui_framework::plugins::PluginHostWrapper> ImGuiFrameworkPlugin::RegisterPlugin(BakkesModPlugin* plugin, std::function<void()> on_host_unload)
{
	auto loaded_plugin = GetLoadedPlugin(plugin);
	if (!loaded_plugin)
	{
		throw std::exception("Failed to find the loaded plugin from the core");
	}

	LOG("Registering {}", loaded_plugin->_details->className);
	auto host_wrapper = std::make_unique<imgui_framework::plugins::PluginHostWrapper>(singleton, loaded_plugin);
	Pimpl()->registered_plugins.push_back({loaded_plugin, std::move(on_host_unload)});
	return std::move(host_wrapper);
}

bool ImGuiFrameworkPlugin::AddWindow(const imgui_framework::plugins::PluginWindowPtr& new_window, BakkesMod::Plugin::LoadedPlugin* owner)
{
	LOG("ImGuiFrameworkPlugin::AddWindow");
	LOG("Checking interface version");
	if (new_window->InterfaceVersion() != imgui_framework::plugins::kPluginWindowInterfaceVersion)
	{
		LOG("Interface version mismatch (was {}, should be {}", new_window->InterfaceVersion(), imgui_framework::plugins::kPluginWindowInterfaceVersion);
		return false;
	}
	auto required_backed = new_window->GetRendererVersion();
	LOG("Window {} requested backend: {}", new_window->GetWindowName(), required_backed);

	// if dx succeeds and winmng fails. This could go bad..
	if (Pimpl()->dx_hook_.AddWindow(new_window) && Pimpl()->window_manager_.AddWindow(new_window, owner))
	{
		return true;
	}
	return false;
}

bool ImGuiFrameworkPlugin::RemoveWindow(const imgui_framework::plugins::PluginWindowPtr& window, BakkesMod::Plugin::LoadedPlugin* owner)
{
	// if dx succeeds and winmng fails. This could go bad..
	if (Pimpl()->dx_hook_.RemoveWindow(window) && Pimpl()->window_manager_.RemoveWindow(window, owner))
	{
		return true;
	}
	LOG("Failed removing the window");
	return false;
}

void ImGuiFrameworkPlugin::UnregisterPlugin(imgui_framework::plugins::PluginHostWrapper* host_wrapper)
{
	auto host_owner = host_wrapper->GetOwner();
	auto it = std::find_if(Pimpl()->registered_plugins.begin(), Pimpl()->registered_plugins.end(),
		[host_owner](imgui_framework::details::RegisteredPlugin& registered_plugin)
		{
			return registered_plugin.plugin_ptr == host_owner;
		});
	if (it != Pimpl()->registered_plugins.end())
	{
		LOG("Removing registered plugin");
		Pimpl()->registered_plugins.erase(it);
	}
	else
	{
		LOG("{}: Failed to find the registered plugin", __FUNCTION__);
	}
}


BakkesMod::Plugin::LoadedPlugin* ImGuiFrameworkPlugin::GetLoadedPlugin(BakkesModPlugin* plugin) const
{
	auto plugin_manager = gameWrapper->GetPluginManager();
	auto& plugins = *plugin_manager.GetLoadedPlugins();
	for (auto& loaded_plugin : plugins)
	{
		if (loaded_plugin->_plugin.get() == plugin)
		{
			return loaded_plugin.get();
		}
	}
	return nullptr;
}
