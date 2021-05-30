#include "pch.h"
#include "DefaultRenderer.h"

#include "IMGUI/imgui.h"
#include "IMGUI/imgui_impl_dx11.h"
#include "IMGUI/imgui_impl_win32.h"
#include "Plugins/PluginWindow.h"

using namespace imgui_framework::rendering;

DefaultRenderer::~DefaultRenderer()
{
	DefaultRenderer::ShutDown();
}

std::string DefaultRenderer::GetRendererVersion() const noexcept
{
	return IMGUI_VERSION;
}

int DefaultRenderer::GetInterfaceVersion() const noexcept
{
	return kRenderInterfaceVersion;
}

bool DefaultRenderer::CaptureInput() const noexcept
{
	return shouldCaptureInput;
}

bool DefaultRenderer::InitRenderer(HWND window, ID3D11Device* device, ID3D11DeviceContext* context)
{
	pDevice_ = device;
	pDeviceContext_ = context;

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = 1.0f / 60.0f;
	//io.MouseDrawCursor = false;
	//io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
	//io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	//io.ConfigViewportsNoDecoration = false;
	//io.ConfigViewportsNoAutoMerge = true;    
	//io.ConfigViewportsNoTaskBarIcon = true;  
	//io.ConfigViewportsNoDecoration;   
	io.ConfigViewportsNoDefaultParent = true;
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(device, context);
	initialized_ = true;

	return true;
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


bool DefaultRenderer::OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam) || shouldCaptureInput)
	{
		return true;
	}
	return false;
}

bool DefaultRenderer::AddWindow(WindowPtr window)
{
	windows_add_queue_.push_back(std::move(window));
	return true;
}

bool DefaultRenderer::RemoveWindow(WindowPtr window)
{
	auto it = std::find(windows_.begin(), windows_.end(), window);
	if (it != windows_.end())
	{
		windows_.erase(it);
		return true;
	}
	return false;
}

void DefaultRenderer::ShutDown()
{
	if (initialized_)
	{
		LOG("DefaultRenderer::ShutDown()");
		initialized_ = false;
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
		if (pMainRenderTargetView_)
		{
			pMainRenderTargetView_->Release();
		}
	}
}


void DefaultRenderer::Render(IDXGISwapChain* pSwapChain)
{
	pSwapChain_ = pSwapChain;
	if (!initialized_) return;


	// Pre render
	CreateRenderTarget();
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	for (const auto it = windows_add_queue_.begin(); it != windows_add_queue_.end();)
	{
		(*it)->SetImGuiContext(ImGui::GetCurrentContext());
		windows_.push_back(std::move(*it));
		windows_add_queue_.erase(it);
	}


	// Draw the imgui windows
	for (auto& window : windows_)
	{
		if (window->GetIsOpen())
		{
			window->Render();
		}
	}

	// Check io capture
	shouldCaptureInput = ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;

	// Render the imgui windows
	ImGui::Render();
	pDeviceContext_->OMSetRenderTargets(1, &pMainRenderTargetView_, nullptr);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

void DefaultRenderer::CreateRenderTarget()
{
	if (pMainRenderTargetView_)
	{
		pMainRenderTargetView_->Release();
		pMainRenderTargetView_ = nullptr;
	}
	ID3D11Texture2D* pBackBuffer;
	pSwapChain_->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	pDevice_->CreateRenderTargetView(pBackBuffer, nullptr, &pMainRenderTargetView_);
	pBackBuffer->Release();
}
