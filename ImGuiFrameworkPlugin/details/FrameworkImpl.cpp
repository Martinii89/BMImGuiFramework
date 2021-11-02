#include "pch.h"
#include "FrameworkImpl.h"

#include "ImGuiFrameworkPlugin.h"
#include "bakkesmod/wrappers/PluginManagerWrapper.h"

using namespace imgui_framework::details;

FrameworkImpl::FrameworkImpl(): window_manager_(registered_plugins) {}

void FrameworkImpl::OnLoad()
{
	InitKiero();
	const auto main = weak_main_.lock();
	const auto me = GetLoadedPlugin(main.get());
	// Add myself to the list so I also can register windows.
	registered_plugins.push_back({me, []{}});
	if (!me)
	{
		LOG("Failed to get my own loaded plugin info");
	}
	else if (AddWindow(window_manager_.gui_window_, me))
	{
		LOG("Registered the window manager");
		cv_->registerNotifier("imgui_framework_window_manager", [this](...)
		{
			window_manager_.gui_window_->Open();
		}, "Open the window manager", 0);
	}
	else
	{
		LOG("Failed to register the window manager");
	}
}

void FrameworkImpl::OnUnload()
{
	for (auto& [plugin_ptr, on_host_unload, _] : registered_plugins)
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
	registered_plugins.push_back({loaded_plugin, std::move(on_host_unload), {}});
	return std::move(host_wrapper);
}

bool FrameworkImpl::AddWindow(const plugins::PluginWindowPtr& new_window, BakkesMod::Plugin::LoadedPlugin* owner)
{
	LOG("ImGuiFrameworkPlugin::AddWindow");
	LOG("Checking interface version");
	if (new_window->InterfaceVersion() != plugins::kPluginWindowInterfaceVersion)
	{
		LOG("Interface version mismatch (was {}, should be {}", new_window->InterfaceVersion(), plugins::kPluginWindowInterfaceVersion);
		return false;
	}

	auto it = std::find_if(registered_plugins.begin(), registered_plugins.end(),
		[owner](RegisteredPlugin& reg)
		{
			return reg.plugin_ptr == owner;
		});
	if (it == registered_plugins.end())
	{
		LOG("Can't register a window to a unknown plugin");
		return false;
	}


	auto required_backed = new_window->GetRendererVersion();
	LOG("Window {} requested backend: {}", new_window->GetWindowName(), required_backed);

	if (dx_hook_.AddWindow(new_window))
	{
		it->windows.push_back(new_window);
		return true;
	}
	LOG("Failed to add windows dx_hook_.AddWindow return false");
	return false;
}

bool FrameworkImpl::RemoveWindow(const plugins::PluginWindowPtr& window, BakkesMod::Plugin::LoadedPlugin* owner)
{
	auto it = std::find_if(registered_plugins.begin(), registered_plugins.end(),
		[owner](RegisteredPlugin& reg)
		{
			return reg.plugin_ptr == owner;
		});
	if (it == registered_plugins.end())
	{
		LOG("Can't remove a window for a unknown plugin");
		return false;
	}
	auto window_it = std::find_if(it->windows.begin(), it->windows.end(),
		[window](plugins::PluginWindowPtr& win_ptr)
		{
			return win_ptr == window;
		});
	if (window_it == it->windows.end())
	{
		LOG("Window not in the list for this plugin. Aborting");
		return false;
	}
	if (dx_hook_.RemoveWindow(window))
	{
		it->windows.erase(window_it);
		return true;
	}
	LOG("Failed removing the window");
	return false;
}

void FrameworkImpl::UnregisterPlugin(plugins::PluginHostWrapper* host_wrapper)
{
	auto host_owner = host_wrapper->GetOwner();
	auto it = std::find_if(registered_plugins.begin(), registered_plugins.end(),
		[host_owner](RegisteredPlugin& registered_plugin)
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
