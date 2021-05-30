#pragma once
#include "Plugins/PluginWindow.h"

class TestWindow: public imgui_framework::plugins::PluginWindow
{
public:
	[[nodiscard]] int InterfaceVersion() const noexcept override;
	[[nodiscard]] std::string GetRendererVersion() const noexcept override;
	[[nodiscard]] std::string GetWindowName() const noexcept override;
	[[nodiscard]] bool GetIsOpen() const noexcept override;
	[[nodiscard]] bool GetShouldBlockInput() const noexcept override;
	[[nodiscard]] bool GetIsActiveOverlay() const noexcept override;
	void Render() override;
	void Open() override;
	void Close() override;

	void SetImGuiContext(ImGuiContext* ctx) override;
protected:
	bool open_ = true;
};


class DemoWindow : public TestWindow
{
public:
	[[nodiscard]] std::string GetWindowName() const noexcept override;
	void Render() override;
};