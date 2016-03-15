#pragma once
#include "vulkan/vk_cpp.h"
#include <vector>

namespace nif
{
	class queue
	{
	public:
		queue();
		queue(const vk::Queue queue);
		~queue();

		void present(const vk::PresentInfoKHR &presentInfo) const;
		void submit(const std::vector<vk::SubmitInfo> &submitInfos) const;
		void wait_idle() const;

	private:
		vk::Queue handle_;
	};
}