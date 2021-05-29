#pragma once
#include <string>

struct ImGuiContext;

namespace imgui_framework::plugins
{
	constexpr int kPluginWindowInterfaceVersion = 1;

	class PluginWindow
	{
	public:
		virtual ~PluginWindow() = default;

		// Interface info
		[[nodiscard]] virtual int InterfaceVersion() const noexcept = 0;
		[[nodiscard]] virtual std::string GetRendererVersion() const noexcept = 0;

		// Getters
		[[nodiscard]] virtual std::string GetWindowName() const noexcept = 0;
		[[nodiscard]] virtual bool GetIsOpen() const noexcept = 0;
		[[nodiscard]] virtual bool GetShouldBlockInput() const noexcept = 0;
		[[nodiscard]] virtual bool GetIsActiveOverlay() const noexcept = 0;

		// Actions
		virtual void SetImGuiContext(ImGuiContext* ctx) = 0;
		virtual void Render() = 0;
		virtual void Open() = 0;
		virtual void Close() = 0;
	};
}
