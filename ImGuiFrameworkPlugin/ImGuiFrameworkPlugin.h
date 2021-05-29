#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"

#include "Plugins/KieroSDK.h"


namespace imgui_framework {
	namespace details {
		class FrameworkImpl;
	}
	using Impl = details::FrameworkImpl;
	
}


class ImGuiFrameworkPlugin: public BakkesMod::Plugin::BakkesModPlugin
{
public:
	ImGuiFrameworkPlugin();
	// public kieroplugin interface
	
	// on_host_unload should free up any resources shared with the host
	// This would be the uniqe_ptr of the HostWrapper and any dll references if you're using delayed loading.
	IMGUI_FRAMEWORKAPI std::unique_ptr<imgui_framework::plugins::PluginHostWrapper> RegisterPlugin(BakkesModPlugin* plugin, std::function<void()> on_host_unload);

	[[nodiscard]] bool AddWindow(const imgui_framework::plugins::PluginWindowPtr& new_window, BakkesMod::Plugin::LoadedPlugin* owner);
	[[nodiscard]] bool RemoveWindow(const imgui_framework::plugins::PluginWindowPtr& window, BakkesMod::Plugin::LoadedPlugin* owner);
	void UnregisterPlugin(imgui_framework::plugins::PluginHostWrapper* host_wrapper);

private:
	//Boilerplate
	void onLoad() override;
	void onUnload() override;

	void InitKiero();
	BakkesMod::Plugin::LoadedPlugin* GetLoadedPlugin(BakkesModPlugin* plugin) const;

	// Using pimple here so users of this header don't have to deal with the internals. 
	// http://oliora.github.io/2015/12/29/pimpl-and-rule-of-zero.html
	[[nodiscard]] const imgui_framework::Impl* Pimpl() const { return pImpl.get(); }
	[[nodiscard]] imgui_framework::Impl* Pimpl() { return pImpl.get(); }
	std::unique_ptr<imgui_framework::Impl, void (*)(imgui_framework::Impl*)> pImpl;
};

