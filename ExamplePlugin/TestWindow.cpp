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
	ImGui::Begin("Addon window", &open_);
	ImGui::Text("Just a addon window");
	ImGui::End();
	
}

void TestWindow::Open()
{
	LOG("{}", __FUNCTION__);
	open_ = true;
}

void TestWindow::Close()
{
	LOG("{}", __FUNCTION__);
	open_ = false;
}

void TestWindow::SetImGuiContext(ImGuiContext* ctx)
{
	ImGui::SetCurrentContext(ctx);
}

std::string DemoWindow::GetWindowName() const noexcept
{
	return "DemoWindow";
}

void DemoWindow::Render()
{
	ImGui::ShowDemoWindow(&open_);
}
