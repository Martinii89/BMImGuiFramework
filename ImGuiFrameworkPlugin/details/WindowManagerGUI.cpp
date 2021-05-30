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
	const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;

	static ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;

	if (ImGui::BeginTable("PluginWindows", 2, flags))
	{
		// The first column will use the default _WidthStretch when ScrollX is Off and _WidthFixed when ScrollX is On
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
		ImGui::TableSetupColumn("Toggle", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 12.0f);
		ImGui::TableHeadersRow();

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		for (auto& [plugin_ptr, _, windows] : registered_plugins_)
		{
			ImGui::PushID(plugin_ptr);
			bool open = ImGui::TreeNodeEx(plugin_ptr->_details->pluginName, ImGuiTreeNodeFlags_SpanFullWidth);
			ImGui::TableNextColumn();
			ImGui::TextDisabled("--");
			ImGui::TableNextColumn();
			if (open)
			{
				for (auto& window : windows)
				{
					ImGui::PushID(window.get());
					ImGui::TreeNodeEx(window->GetWindowName().c_str(),
						ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth);
					ImGui::TableNextColumn();
					if (window->GetIsOpen())
					{
						if (ImGui::Button("Close"))
						{
							window->Close();
						}
					}
					else
					{
						if (ImGui::Button("Open"))
						{
							window->Open();
						}
					}
					ImGui::TableNextColumn();
					ImGui::PopID();
				}
				ImGui::TreePop();
			}
			ImGui::PopID();
		}
		ImGui::EndTable();
	}

	//	// Simple storage to output a dummy file-system.
	//	struct MyTreeNode
	//	{
	//		const char* Name;
	//		const char* Type;
	//		int Size;
	//		int ChildIdx;
	//		int ChildCount;

	//		static void DisplayNode(const MyTreeNode* node, const MyTreeNode* all_nodes)
	//		{
	//			ImGui::TableNextRow();
	//			ImGui::TableNextColumn();
	//			const bool is_folder = (node->ChildCount > 0);
	//			if (is_folder)
	//			{
	//				bool open = ImGui::TreeNodeEx(node->Name, ImGuiTreeNodeFlags_SpanFullWidth);
	//				ImGui::TableNextColumn();
	//				ImGui::TextDisabled("--");
	//				ImGui::TableNextColumn();
	//				ImGui::TextUnformatted(node->Type);
	//				if (open)
	//				{
	//					for (int child_n = 0; child_n < node->ChildCount; child_n++)
	//						DisplayNode(&all_nodes[node->ChildIdx + child_n], all_nodes);
	//					ImGui::TreePop();
	//				}
	//			}
	//			else
	//			{
	//				ImGui::TreeNodeEx(node->Name, ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth);
	//				ImGui::TableNextColumn();
	//				ImGui::Text("%d", node->Size);
	//				ImGui::TableNextColumn();
	//				ImGui::TextUnformatted(node->Type);
	//			}
	//		}
	//	};
	//	static const MyTreeNode nodes[] =
	//	{
	//		{"Root", "Folder", -1, 1, 3}, // 0
	//		{"Music", "Folder", -1, 4, 2}, // 1
	//		{"Textures", "Folder", -1, 6, 3}, // 2
	//		{"desktop.ini", "System file", 1024, -1, -1}, // 3
	//		{"File1_a.wav", "Audio file", 123000, -1, -1}, // 4
	//		{"File1_b.wav", "Audio file", 456000, -1, -1}, // 5
	//		{"Image001.png", "Image file", 203128, -1, -1}, // 6
	//		{"Copy of Image001.png", "Image file", 203256, -1, -1}, // 7
	//		{"Copy of Image001 (Final2).png", "Image file", 203512, -1, -1}, // 8
	//	};

	//	MyTreeNode::DisplayNode(&nodes[0], nodes);

	//	ImGui::EndTable();
	//}
	//ImGui::TreePop();

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
