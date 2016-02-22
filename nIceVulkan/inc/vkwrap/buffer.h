#pragma once
#include "vkwrap/gpu_memory.h"
#include <memory>

namespace nif
{
	class ibuffer
	{
	public:
		ibuffer(const ibuffer&) = delete;
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
	};
}
