#include "pch.h"
#include "WindowManagerGUI.h"

#include "FrameworkImpl.h"

imgui_framework::details::WindowManagerGUI::WindowManagerGUI(std::vector<RegisteredPlugin>& registered_plugins): registered_plugins_(registered_plugins) {}

int imgui_framework::details::WindowManagerGUI::InterfaceVersion() const noexcept
{
	return plugins::kPluginWindowInterfaceVersion;
}

std::string imgui_framework::details::WindowManagerGUI::GetRendererVersion() const noexcept
{
	return IMGUI_VERSION;
}

std::string imgui_framework::details::WindowManagerGUI::GetWindowName() const noexcept
{
	return menu_name_;
}

bool imgui_framework::details::WindowManagerGUI::GetIsOpen() const noexcept
{
	return open_;
}

bool imgui_framework::details::WindowManagerGUI::GetShouldBlockInput() const noexcept
{
	return true;
}

bool imgui_framework::details::WindowManagerGUI::GetIsActiveOverlay() const noexcept
{
	return true;
}

void imgui_framework::details::WindowManagerGUI::SetImGuiContext(ImGuiContext* ctx)
{
	// no need. global contex is already set
}

void imgui_framework::details::WindowManagerGUI::Render()
{
	ImGui::Begin("Window Manager", &open_);

	for (auto& [plugin_ptr, _, windows]: registered_plugins_)
	{
		ImGui::PushID(plugin_ptr);
		ImGui::Text("%s", plugin_ptr->_details->pluginName);
		ImGui::Indent(25);
		for(auto& window: windows)
		{
			ImGui::PushID(window.get());
			ImGui::Text("%s", window->GetWindowName().c_str());
			ImGui::SameLine();
			if (ImGui::Button("Open"))
			{
				window->Open();
			}
			ImGui::SameLine();
			if (ImGui::Button("Close"))
			{
				window->Close();
			}
			ImGui::PopID();
		}
		ImGui::Unindent(25);
		ImGui::PopID();
	}
	ImGui::End();
}

void imgui_framework::details::WindowManagerGUI::Open()
{
	open_ = true;
}

void imgui_framework::details::WindowManagerGUI::Close()
{
	open_ = false;
}