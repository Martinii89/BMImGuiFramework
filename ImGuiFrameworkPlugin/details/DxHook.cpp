#include "pch.h"
#include "DxHook.h"

#include "Plugins/PluginWindow.h"
#include "Renderers/DefaultRenderer.h"

using namespace imgui_framework::details;

typedef HRESULT (__stdcall* Present)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
typedef LRESULT (CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);

Present oPresent;
WNDPROC oWndProc;

std::function<void(IDXGISwapChain* pSwapChain)> PresentHook;
std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> MyWndProcHandler;


LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return MyWndProcHandler(hWnd, uMsg, wParam, lParam);
}


bool shutting_down_default_renderer = false;


HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
	static bool in_hook = false;
	if (!in_hook)
	{
		in_hook = true;
		PresentHook(pSwapChain);
		in_hook = false;
	}
	return oPresent(pSwapChain, SyncInterval, Flags);
}


void DxHook::Hook()
{
	PresentHook = [this](auto swap_chain) { OnPresent(swap_chain); };
	MyWndProcHandler = [this](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return OnWndProc(hWnd, uMsg, wParam, lParam); };
	if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
	{
		LOG("Binding hkPresent");
		kiero::bind(8, (void**)&oPresent, hkPresent);
	}
	else
	{
		LOG("Kiero init fail");
	}
}

void DxHook::UnHook() const
{
	LOG("unhooking present and wndproc");
	kiero::unbind(8);
	(WNDPROC)SetWindowLongPtr(window_, GWLP_WNDPROC, (LONG_PTR)oWndProc);
}

DxHook::DxHook()
{
	AddRenderer(std::make_unique<rendering::DefaultRenderer>());
}

DxHook::~DxHook()
{
	ShutDown();
}

void DxHook::ShutDown()
{
	if (!isInitialized) return;
	std::unique_lock lck(shutdown_mutex);
	LOG("Requesting shutdown");
	shutdown_requested = true;
	LOG("Waiting for shutdown_done");
	shutdown_done_condition.wait(lck, [this] { return shutdown_done; });
	LOG("shutdown_done");


	if (isInitialized)
	{
		LOG("unhook");
		UnHook();
	}
	if (pDevice_)
	{
		LOG("release device");
		pDevice_->Release();
	}
	if (pDeviceContext_)
	{
		LOG("release context");
		pDeviceContext_->Release();
	}
}

void DxHook::AddRenderer(std::unique_ptr<rendering::Renderer> renderer)
{
	if (RendererAvailable(renderer->GetRendererVersion()))
	{
		LOG("This render version already loaded");
		return;
	}
	unitialized_renderers_.push_back(std::move(renderer));
}

bool DxHook::RendererAvailable(const std::string& render_id) const
{
	return GetRenderer(render_id) != nullptr;
}


bool DxHook::AddWindow(const std::shared_ptr<imgui_framework::plugins::PluginWindow>& window)
{
	LOG("DxHook::AddWindow");
	auto* renderer = GetRenderer(window->GetRendererVersion());
	if (!renderer)
	{
		LOG("Renderer {} requested is not available", window->GetRendererVersion());
		return false;
	}
	LOG("Found the right render");
	if (!renderer->AddWindow(window))
	{
		LOG("Failed to add window to the renderer {}", renderer->GetRendererVersion());
		return false;
	}
	return true;
}

bool DxHook::RemoveWindow(const std::shared_ptr<imgui_framework::plugins::PluginWindow>& window)
{
	auto* renderer = GetRenderer(window->GetRendererVersion());
	if (!renderer)
	{
		LOG("failed to find render for window");
		return false;
	}
	if (!renderer->RemoveWindow(window))
	{
		LOG("Failed to remove the window from the renderer");
		return false;
	}
	return true;
}


imgui_framework::rendering::Renderer* DxHook::GetRenderer(const std::string& render_id) const
{
	LOG("Fetching the renderer for {}", render_id);
	auto render_iterator = std::find_if(
		unitialized_renderers_.begin(),
		unitialized_renderers_.end(),
		[render_id](auto& renderer)
		{
			return renderer->GetRendererVersion() == render_id;
		});
	if (render_iterator != unitialized_renderers_.end()) return &*render_iterator->get();

	auto initialized_render_iterator = std::find_if(
		initialized_renderers_.begin(),
		initialized_renderers_.end(),
		[render_id](auto& renderer)
		{
			return renderer->GetRendererVersion() == render_id;
		});
	if (initialized_render_iterator != initialized_renderers_.end()) return &*initialized_render_iterator->get();

	LOG("Failed to find the right renderer\nAvailable renderers:");
	for (auto& r : unitialized_renderers_)
	{
		LOG("\t{}", r->GetRendererVersion());
	}
	for (auto& r : initialized_renderers_)
	{
		LOG("\t{}", r->GetRendererVersion());
	}

	return nullptr;
}

void DxHook::OnPresent(IDXGISwapChain* pSwapChain)
{
	//LOG("OnPresent");
	if (shutdown_requested)
	{
		LOG("Shutting down in the render thread");
		std::lock_guard lck(shutdown_mutex);
		for (auto& renderer : initialized_renderers_)
		{
			LOG("Shutting down: {}", renderer->GetRendererVersion());
			renderer->ShutDown();
		}
		shutdown_done = true;
		shutdown_requested = false;
		LOG("notifying about shutdown done");
		shutdown_done_condition.notify_all();
		return;
	}
	if (!isInitialized)
	{
		if (!GetWindowDeviceContext(pSwapChain))
		{
			return;
		}
		oWndProc = (WNDPROC)SetWindowLongPtr(window_, GWLP_WNDPROC, (LONG_PTR)WndProc);
		isInitialized = true;
	}
	for (auto it = unitialized_renderers_.begin(); it != unitialized_renderers_.end();)
	{
		if ((*it)->InitRenderer(window_, pDevice_, pDeviceContext_))
		{
			LOG("Backend initialized: {}", (*it)->GetRendererVersion());
			initialized_renderers_.push_back(std::move(*it));
			it = unitialized_renderers_.erase(it);
		}
		else
		{
			++it;
		}
	}
	shouldBlockFurtherInput = false;
	for (auto& initialized_renderer : initialized_renderers_)
	{
		initialized_renderer->Render(pSwapChain);
	}
}

bool DxHook::GetWindowDeviceContext(IDXGISwapChain* pSwapChain)
{
	if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice_)))
	{
		pDevice_->GetImmediateContext(&pDeviceContext_);
		DXGI_SWAP_CHAIN_DESC sd;
		pSwapChain->GetDesc(&sd);
		window_ = sd.OutputWindow;
		return true;
	}
	return false;
}

LRESULT DxHook::OnWndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	for (auto it = initialized_renderers_.rbegin(); it != initialized_renderers_.rend(); ++it)
	{
		if ((*it)->OnWndProc(hWnd, uMsg, wParam, lParam))
		{
			return 1;
		}
	}

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}
