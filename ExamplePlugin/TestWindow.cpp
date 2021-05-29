#include "pch.h"
#include "TestWindow.h"
#include "IMGUI/imgui.h"

int TestWindow::InterfaceVersion() const noexcept { return imgui_framework::plugins::kPluginWindowInterfaceVersion; }

std::string TestWindow::GetRendererVersion() const noexcept
{
	return IMGUI_VERSION;
}

std::string TestWindow::GetWindowName() const noexcept
{
	return "AddonTest";
}

bool TestWindow::GetIsOpen() const noexcept
{
	return open_;
}

bool TestWindow::GetShouldBlockInput() const noexcept
{
	return false;
}

bool TestWindow::GetIsActiveOverlay() const noexcept
{
	return false;
}

void TestWindow::Render()
{
	if (!open_) return;

	ImGui::Begin("Addon window", &open_);
	ImGui::Text("Just a addon window");
	ImGui::End();

	ImGui::ShowDemoWindow();
}

void TestWindow::Open()
{
	open_ = true;
}

void TestWindow::Close()
{
	open_ = false;
}

void TestWindow::SetImGuiContext(ImGuiContext* ctx)
{
	ImGui::SetCurrentContext(ctx);
}
