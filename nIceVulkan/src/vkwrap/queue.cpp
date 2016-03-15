#include "vkwrap/queue.h"
#include "util/shortcuts.h"

using namespace std;

namespace nif
{
	queue::queue()
	{
	}

	queue::queue(const vk::Queue queue) :
		handle_(queue)
	{
	}

	queue::~queue()
	{
	}

	void queue::present(const vk::PresentInfoKHR &presentInfo) const
	{
		vk_try(handle_.presentKHR(&presentInfo));
	}

	void queue::submit(const vector<vk::SubmitInfo> &submitInfos) const
	{
		vk_try(handle_.submit(
			static_cast<uint32_t>(submitInfos.size()),
			submitInfos.data(),
			nullptr));
	}

	void queue::wait_idle() const
	{
		vk_try(handle_.waitIdle());
	}
}