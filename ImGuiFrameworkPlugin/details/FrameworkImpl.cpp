#include "pch.h"
#include "FrameworkImpl.h"
#include "bakkesmod/wrappers/PluginManagerWrapper.h"

using namespace imgui_framework::details;

void FrameworkImpl::OnLoad()
{
	InitKiero();
}

void FrameworkImpl::OnUnload()
{
	for (auto& [plugin_ptr, on_host_unload] : registered_plugins)
	{
		if (on_host_unload)
		{
			on_host_unload();
		}
	}
}

std::unique_ptr<imgui_framework::plugins::PluginHostWrapper> FrameworkImpl::RegisterPlugin(BakkesMod::Plugin::BakkesModPlugin* plugin,
                                                                                           std::function<void()> on_host_unload)
{
	auto loaded_plugin = GetLoadedPlugin(plugin);
	if (!loaded_plugin)
	{
		throw std::exception("Failed to find the loaded plugin from the core");
	}

	LOG("Registering {}", loaded_plugin->_details->className);
	auto main = weak_main_.lock();
	auto host_wrapper = std::make_unique<plugins::PluginHostWrapper>(main, loaded_plugin);
	registered_plugins.push_back({loaded_plugin, std::move(on_host_unload)});
	return std::move(host_wrapper);
}

bool FrameworkImpl::AddWindow(const plugins::PluginWindowPtr& new_window, BakkesMod::Plugin::LoadedPlugin* owner)
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
	if (dx_hook_.AddWindow(new_window) && window_manager_.AddWindow(new_window, owner))
	{
		return true;
	}
	return false;
}

bool FrameworkImpl::RemoveWindow(const plugins::PluginWindowPtr& window, BakkesMod::Plugin::LoadedPlugin* owner)
{
	// if dx succeeds and winmng fails. This could go bad..
	if (dx_hook_.RemoveWindow(window) && window_manager_.RemoveWindow(window, owner))
	{
		return true;
	}
	LOG("Failed removing the window");
	return false;
}

void FrameworkImpl::UnregisterPlugin(plugins::PluginHostWrapper* host_wrapper)
{
	auto host_owner = host_wrapper->GetOwner();
	auto it = std::find_if(registered_plugins.begin(), registered_plugins.end(),
		[host_owner](imgui_framework::details::RegisteredPlugin& registered_plugin)
		{
			return registered_plugin.plugin_ptr == host_owner;
		});
	if (it != registered_plugins.end())
	{
		LOG("Removing registered plugin");
		registered_plugins.erase(it);
	}
	else
	{
		LOG("{}: Failed to find the registered plugin", __FUNCTION__);
	}
}

void FrameworkImpl::InitKiero()
{
	dx_hook_.Hook();

	cv_->registerNotifier("kiero_unhook", [this](...)
	{
		std::thread t([this]
		{
			dx_hook_.ShutDown();
		});
		t.detach();
	}, "", 0);
}

BakkesMod::Plugin::LoadedPlugin* FrameworkImpl::GetLoadedPlugin(BakkesMod::Plugin::BakkesModPlugin* plugin) const
{
	auto plugin_manager = gw_->GetPluginManager();
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
