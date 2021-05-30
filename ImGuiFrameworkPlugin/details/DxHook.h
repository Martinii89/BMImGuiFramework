#pragma once

namespace imgui_framework::plugins
{
	class PluginWindow;
}

namespace imgui_framework::rendering
{
	class Renderer;
}


namespace imgui_framework::details
{
	class DxHook
	{
	public:
		void Hook();
		void UnHook() const;
		DxHook();
		~DxHook();

		void ShutDown();
		void AddRenderer(std::unique_ptr<rendering::Renderer> renderer);
		[[nodiscard]] bool RendererAvailable(const std::string& render_id) const;
		[[nodiscard]] bool AddWindow(const std::shared_ptr<plugins::PluginWindow>& window);
		[[nodiscard]] bool RemoveWindow(const std::shared_ptr<plugins::PluginWindow>& window);

		std::condition_variable shutdown_done_condition;
		std::mutex shutdown_mutex;
	private:
		bool shutdown_requested = false;
		bool shutdown_done = false;
		bool isInitialized = false;
		ID3D11Device* pDevice_ = nullptr;
		ID3D11DeviceContext* pDeviceContext_ = nullptr;
		HWND window_ = nullptr;

		std::vector<std::unique_ptr<rendering::Renderer>> initialized_renderers_;
		std::vector<std::unique_ptr<rendering::Renderer>> unitialized_renderers_;

		bool shouldBlockFurtherInput = false;

		[[nodiscard]] rendering::Renderer* GetRenderer(const std::string& render_id) const;

		// dx hook stuff
		[[nodiscard]] bool GetWindowDeviceContext(IDXGISwapChain* pSwapChain);
		void OnPresent(IDXGISwapChain* pSwapChain);
		LRESULT OnWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	};
}
