#pragma once
#include "Plugins/PluginWindow.h"

namespace imgui_framework::details
{
	struct RegisteredPlugin;

	class WindowManagerGUI: public plugins::PluginWindow
	{
	public:
		explicit WindowManagerGUI(std::vector<RegisteredPlugin>& registered_plugins);

		[[nodiscard]] int InterfaceVersion() const noexcept override;
		[[nodiscard]] std::string GetRendererVersion() const noexcept override;
		[[nodiscard]] std::string GetWindowName() const noexcept override;
		[[nodiscard]] bool GetIsOpen() const noexcept override;
		[[nodiscard]] bool GetShouldBlockInput() const noexcept override;
		[[nodiscard]] bool GetIsActiveOverlay() const noexcept override;
		void SetImGuiContext(ImGuiContext* ctx) override;
		void Render() override;
		void Open() override;
		void Close() override;
	private:
		bool open_ = true;
		std::string menu_name_ = "Window Manager";
		std::vector<RegisteredPlugin>& registered_plugins_; 
	};

}

