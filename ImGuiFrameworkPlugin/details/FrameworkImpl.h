#pragma once
#include "DxHook.h"
#include "WindowManager.h"

class ImGuiFrameworkPlugin;

namespace imgui_framework::details
{
	struct RegisteredPlugin
	{
		BakkesMod::Plugin::LoadedPlugin* plugin_ptr;
		std::function<void()> on_host_unload;
	};

	class FrameworkImpl
	{
	public:
		explicit FrameworkImpl(ImGuiFrameworkPlugin* main): p_main(main) {}
		FrameworkImpl(const FrameworkImpl& other) = delete;
		FrameworkImpl(FrameworkImpl&& other) noexcept = delete;
		FrameworkImpl& operator=(const FrameworkImpl& other) = delete;
		FrameworkImpl& operator=(FrameworkImpl&& other) noexcept = delete;
		~FrameworkImpl() = default;

		void OnLoad();
		void OnUnload();

		std::unique_ptr<plugins::PluginHostWrapper> RegisterPlugin(BakkesMod::Plugin::BakkesModPlugin* plugin, std::function<void()> on_host_unload);
		[[nodiscard]] bool AddWindow(const plugins::PluginWindowPtr& new_window, BakkesMod::Plugin::LoadedPlugin* owner);
		[[nodiscard]] bool RemoveWindow(const plugins::PluginWindowPtr& window, BakkesMod::Plugin::LoadedPlugin* owner);
		void UnregisterPlugin(plugins::PluginHostWrapper* host_wrapper);
		void InitKiero();

		std::shared_ptr<GameWrapper> gw_;
		std::shared_ptr<CVarManagerWrapper> cv_;
		std::weak_ptr<ImGuiFrameworkPlugin> weak_main_; // Weak ptr to avoid circular shared/unique references 


	private:
		BakkesMod::Plugin::LoadedPlugin* GetLoadedPlugin(BakkesMod::Plugin::BakkesModPlugin* plugin) const;
		std::vector<RegisteredPlugin> registered_plugins;
		ImGuiFrameworkPlugin* p_main = nullptr; // back pointer
		DxHook dx_hook_;
		// not sure about this class. But I should have some store of all added windows somewhere.
		// dxHook doesn't seem like the right place for it.
		WindowManager window_manager_;
	};
}
