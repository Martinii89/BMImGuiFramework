#pragma once
#include "Renderer.h"

struct ID3D11RenderTargetView;

namespace imgui_framework::rendering
{
	class DefaultRenderer : public Renderer
	{
	public:
		using WindowPtr = std::shared_ptr<plugins::PluginWindow>;

		~DefaultRenderer() override;
		[[nodiscard]] std::string GetRendererVersion() const noexcept override;
		[[nodiscard]] int GetInterfaceVersion() const noexcept override;
		[[nodiscard]] bool CaptureInput() const noexcept override;
		[[nodiscard]] bool InitRenderer(HWND window, ID3D11Device* device, ID3D11DeviceContext* context) override;
		[[nodiscard]] bool OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
		[[nodiscard]] bool AddWindow(WindowPtr window) override;
		[[nodiscard]] bool RemoveWindow(WindowPtr window) override;

		void ShutDown() override;

		void Render(IDXGISwapChain* pSwapChain) override;

	private:
		ID3D11Device* pDevice_ = nullptr;
		ID3D11DeviceContext* pDeviceContext_ = nullptr;
		IDXGISwapChain* pSwapChain_ = nullptr;
		ID3D11RenderTargetView* pMainRenderTargetView_ = nullptr;
		bool initialized_ = false;
		bool shouldCaptureInput = false;

		std::vector<WindowPtr> windows_;
		std::vector<WindowPtr> windows_add_queue_;


		void CreateRenderTarget();
	public:
	};
}
