#include "pch.h"
#include "ExamplePlugin.h"

#include "bakkesmod/wrappers/PluginManagerWrapper.h"
#include "TestWindow.h"
#include <delayimp.h>


#pragma comment(lib, "ImGuiFrameworkPlugin.lib")
#pragma comment(lib, "delayimp")

BAKKESMOD_PLUGIN(ExamplePlugin, "Example plugin", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;


bool ModuleAvailable(LPCSTR szDll)
{
	auto hr = LoadLibraryA(szDll);

	if (hr != nullptr)
	{
		FreeLibrary(hr);
		return true;
	}
	return false;
}

void ExamplePlugin::onLoad()
{
	_globalCvarManager = cvarManager;

	if (!ModuleAvailable("ImGuiFrameworkPlugin.dll"))
	{
		LOG("host not available");
		return;
	}
	LOG("host available");

	if (ValidateHostPluginLoaded())
	{
		InitHost();
	}
	else
	{
		LOG("Host plugin is not loaded");
	}
}

void ExamplePlugin::onUnload()
{
	FreeHostResources();
}

void ExamplePlugin::InitHost()
{
	if (!host_plugin_)
	{
		LOG("Host plugin not verified. aborting init");
		return;
	}

	host_wrapper_ = host_plugin_->RegisterPlugin(this, [this]{FreeHostResources();});
	if (host_wrapper_ && host_wrapper_->HostIsValid())
	{
		addon_window_ = std::make_shared<TestWindow>();
		if (host_wrapper_->AddWindow(addon_window_))
		{
			LOG("Added window");
		}else
		{
			LOG("Failed to add window");
		}
	}
}

bool ExamplePlugin::ValidateHostPluginLoaded()
{
	auto& loaded_plugins = *gameWrapper->GetPluginManager().GetLoadedPlugins();
	for (auto& loaded_plugin : loaded_plugins)
	{
		if (const auto kiero_test = dynamic_cast<ImGuiFrameworkPlugin*>(loaded_plugin->_plugin.get()))
		{
			host_plugin_ = kiero_test;
			return true;
		}
	}
	return false;
}

void ExamplePlugin::FreeHostResources()
{
	LOG("On host unloader");
	host_wrapper_.reset();
	auto TestReturn = __FUnloadDelayLoadedDLL2("ImGuiFrameworkPlugin.dll");

	if (TestReturn)
		LOG("DLL was unloaded");
	else
		LOG("DLL was not unloaded");
}
