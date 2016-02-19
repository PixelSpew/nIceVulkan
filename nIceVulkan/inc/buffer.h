#pragma once
#include "device.h"
#include <vector>
#include <memory>

namespace nif
{
	class ibuffer
	{
	public:
		std::vector<vk::VertexInputBindingDescription>& bind_descs();
		std::vector<vk::VertexInputAttributeDescription>& attrib_descs();
		vk::PipelineVertexInputStateCreateInfo& pipeline_info();

		static vk::Bool32 getMemoryType(uint32_t typeBits, vk::MemoryPropertyFlags properties, uint32_t typeIndex);

	protected:
		ibuffer(const device &device, const vk::BufferUsageFlags flags, const void* data, const size_t size);
		~ibuffer();

	private:
		ibuffer(const ibuffer&);

		const device &device_;
		vk::Buffer handle_;
		gpu_memory gpumem_;
		vk::PipelineVertexInputStateCreateInfo pipeline_info_;
		std::vector<vk::VertexInputBindingDescription> bind_descs_;
		std::vector<vk::VertexInputAttributeDescription> attrib_descs_;
	};

	template<typename T>
	class buffer : public ibuffer
	{
	public:
		buffer(const device &device, const vk::BufferUsageFlags flags, const std::vector<T> &data)
			: ibuffer(device, flags, data.data(), data.size() * sizeof(T)) {}
		~buffer() {}
	};
}
