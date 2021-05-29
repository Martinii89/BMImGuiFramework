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

	std::vector<RegisteredPlugin> registered_plugins;
	
	ImGuiFrameworkPlugin* p_main = nullptr; // back pointer
	DxHook dx_hook_;
	// not sure about this class. But I should have some store of all added windows somewhere.
	// dxHook doesn't seem like the right place for it.
	WindowManager window_manager_; 
	};
}
