#pragma once
#include "KieroSDK.h"

#include <functional>
#include <memory>

class ImGuiFrameworkPlugin;

namespace imgui_framework::plugins
{
	class PluginWindow;
	using PluginWindowPtr = std::shared_ptr<PluginWindow>;
	using HostPtr = std::weak_ptr<ImGuiFrameworkPlugin>;
	

	class IMGUI_FRAMEWORKAPI PluginHostWrapper
	{
	public:
		// just so you can default construct your object. It's not gonna be usable for anything.
		// If you use this, you can't use delayed dll loading.
		PluginHostWrapper() noexcept;
		PluginHostWrapper(HostPtr host_ptr, BakkesMod::Plugin::LoadedPlugin* owner);

		[[nodiscard]] bool HostIsValid() const;
		[[nodiscard]] bool AddWindow(const PluginWindowPtr& new_window);
		[[nodiscard]] bool RemoveWindow(const PluginWindowPtr& window);
		[[nodiscard]] BakkesMod::Plugin::LoadedPlugin* GetOwner() const;
		
		// MAYBE..
		// The host can create these windows. But the plugin is responsible for managing these once created.
		// The core will call the create_function and pass the returned pointer to AddWindow once created.
		void RegisterCreatableWindow(std::function<PluginWindowPtr()> create_function, std::function<void(PluginWindowPtr)> on_created = nullptr);

	private:
		// http://oliora.github.io/2015/12/29/pimpl-and-rule-of-zero.html
		class Impl;
		[[nodiscard]] const Impl* Pimpl() const { return pImpl.get(); }
		Impl* Pimpl() { return pImpl.get(); }
		std::unique_ptr<Impl, void (*)(Impl *)> pImpl;
	};
}


