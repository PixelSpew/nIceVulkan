#pragma once
#include "gpu_memory.h"
#include <vector>
#include <memory>

namespace nif
{
	class ibuffer
	{
		ibuffer(const ibuffer&) = delete;

	public:
		vk::Buffer handle() const;
		size_t size() const;

	protected:
		ibuffer(const device &device, const vk::BufferUsageFlags flags, const void* data, const size_t size);
		~ibuffer();

	private:
		const device &device_;
		vk::Buffer handle_;
		std::unique_ptr<gpu_memory> gpumem_;
		const size_t size_;
	};

	template<typename T>
	class buffer : public ibuffer
	{
	public:
		buffer(const device &device, const vk::BufferUsageFlags flags, const std::vector<T> &data)
			: ibuffer(device, flags, data.data(), data.size() * sizeof(T)) {}
		~buffer() {}
	};

	template<typename T>
	class vertex_buffer : public buffer<T>
	{
	public:
		vertex_buffer(const device &device, const vk::BufferUsageFlags flags, const std::vector<T> &data)
			: buffer<T>(device, flags, data)
		{
		}

		const std::vector<vk::VertexInputBindingDescription>& bind_descs() const
		{
			return T::binding_descriptions();
		}

		const std::vector<vk::VertexInputAttributeDescription>& attrib_descs() const
		{
			return T::attribute_descriptions();
		}

		const vk::PipelineVertexInputStateCreateInfo& pipeline_info() const
		{
			return T::pipeline_info();
		}

	private:
		vk::PipelineVertexInputStateCreateInfo pipeline_info_;
		std::vector<vk::VertexInputBindingDescription> bind_descs_;
		std::vector<vk::VertexInputAttributeDescription> attrib_descs_;
	};
}
