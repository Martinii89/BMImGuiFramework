#include "pch.h"
#include "PluginHostWrapper.h"

#include "ImGuiFrameworkPlugin.h"


class ImGuiFrameworkPlugin;

class imgui_framework::plugins::PluginHostWrapper::Impl
{
public:
	explicit Impl(PluginHostWrapper* main, HostPtr host_ptr, BakkesMod::Plugin::LoadedPlugin* owner): p_main_(main), p_owner(owner), host_ptr_(std::move(host_ptr)) {}

	Impl(const Impl& other) = delete;
	Impl(Impl&& other) noexcept = delete;
	Impl& operator=(const Impl& other) = delete;
	Impl& operator=(Impl&& other) noexcept = delete;

	~Impl();

	PluginHostWrapper* p_main_{nullptr}; // back pointer
	BakkesMod::Plugin::LoadedPlugin* p_owner = nullptr;
	HostPtr host_ptr_{};
	std::vector<PluginWindowPtr> active_windows_;
};

imgui_framework::plugins::PluginHostWrapper::Impl::~Impl()
{
	auto host = host_ptr_.lock();
	if (!host)
	{
		LOG("AddWindow: Host be ded");
		return;
	}
	LOG("~Impl: Unregistering {} windows", active_windows_.size());
	for (auto& window : active_windows_)
	{
		if (!host->RemoveWindow(window, p_owner))
		{
			LOG("Failed to remove the window on the host");
		}
	}
	host->UnregisterPlugin(p_main_);
}

imgui_framework::plugins::PluginHostWrapper::PluginHostWrapper() noexcept: PluginHostWrapper(HostPtr(), nullptr) {}
imgui_framework::plugins::PluginHostWrapper::PluginHostWrapper(HostPtr host_ptr, BakkesMod::Plugin::LoadedPlugin* owner): pImpl(new Impl(this, std::move(host_ptr), owner), [](Impl* impl) { delete impl; }) {}

bool imgui_framework::plugins::PluginHostWrapper::HostIsValid() const
{
	if (Pimpl()->host_ptr_.lock())
	{
		return true;
	}
	return false;
}

bool imgui_framework::plugins::PluginHostWrapper::AddWindow(const PluginWindowPtr& new_window)
{
	auto host = Pimpl()->host_ptr_.lock();
	if (!host)
	{
		LOG("AddWindow: Host be ded");
		return false;
	}
	LOG("Got the host");
	if (!host->AddWindow(new_window, Pimpl()->p_owner))
	{
		LOG("Failed to add the window to the host");
		return false;
	}
	LOG("window added");
	Pimpl()->active_windows_.emplace_back(new_window);
	LOG("window added to active list in pluginhostwrapper");
	return true;
}

bool imgui_framework::plugins::PluginHostWrapper::RemoveWindow(const PluginWindowPtr& window)
{
	LOG("PluginHostWrapper::RemoveWindow");
	auto host = Pimpl()->host_ptr_.lock();
	if (!host)
	{
		LOG("RemoveWindow: Host be ded");
		return false;
	}

	auto& active_windows = Pimpl()->active_windows_;
	const auto it = std::find_if(
		active_windows.begin(),
		active_windows.end(),
		[this, window](const PluginWindowPtr& active_window)
		{
			return active_window == window;
		});

	if (it == active_windows.end())
	{
		LOG("no window with that id");
		return false;
	}

	if (!host->RemoveWindow(*it, Pimpl()->p_owner))
	{
		LOG("Failed to remove the window on the host");
		return false;
	}
	active_windows.erase(it);
	return true;
}

BakkesMod::Plugin::LoadedPlugin* imgui_framework::plugins::PluginHostWrapper::GetOwner() const
{
	return Pimpl()->p_owner;
}



//auto imgui_framework::plugins::PluginHostWrapper::GetRandomUID()
//{
//	static auto start = 0ull;
//	return start++;
//	//std::random_device rd;
//	//std::mt19937 gen(rd());
//	//std::uniform_int_distribution<> dis(0ull, std::numeric_limits<unsigned long long>::max());
//	//return dis(gen);
//}
