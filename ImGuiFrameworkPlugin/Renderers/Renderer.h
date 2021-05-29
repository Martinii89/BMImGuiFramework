#pragma once
#include <string>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;

constexpr int kRenderInterfaceVersion = 1;

namespace imgui_framework::plugins
{
	class PluginWindow;
}

namespace imgui_framework::rendering
{
	class Renderer
	{
	public:
		virtual ~Renderer() = default;

		[[nodiscard]] virtual bool AddWindow(std::shared_ptr<imgui_framework::plugins::PluginWindow> window) = 0;
		[[nodiscard]] virtual bool RemoveWindow(std::shared_ptr<imgui_framework::plugins::PluginWindow> window) = 0;

		[[nodiscard]] virtual std::string GetRendererVersion() const noexcept = 0;
		[[nodiscard]] virtual int GetInterfaceVersion() const noexcept = 0;

		[[nodiscard]] virtual bool CaptureInput() const noexcept = 0;
		[[nodiscard]] virtual bool InitRenderer(HWND window, ID3D11Device* device, ID3D11DeviceContext* context) = 0;
		[[nodiscard]] virtual bool OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
		virtual void ShutDown() = 0;
		virtual void Render(IDXGISwapChain* pSwapChain) = 0;
	};
}
