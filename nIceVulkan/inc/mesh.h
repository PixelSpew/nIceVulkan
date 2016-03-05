#pragma once
#include "vkwrap/buffer.h"

namespace nif
{
	template <typename T>
	class mesh
	{
	public:
		mesh(const device &device, const std::vector<T> &vertices, const std::vector<uint32_t> &indices) :
			vertex_buffer_(device, vk::BufferUsageFlagBits::eVertexBuffer, vertices),
			index_buffer_(device, vk::BufferUsageFlagBits::eIndexBuffer, indices),
			index_count_(static_cast<uint32_t>(indices.size()))
		{
		}

		mesh(mesh &&old) :
			vertex_buffer_(move(old.vertex_buffer_)),
			index_buffer_(move(old.index_buffer_)),
			index_count_(old.index_count_)
		{
		}

		const buffer<T>& vertex_buffer() const
		{
			return vertex_buffer_;
		}

		const buffer<uint32_t>& index_buffer() const
		{
			return index_buffer_;
		}

		uint32_t index_count() const
		{
			return index_count_;
		}


	private:
		buffer<T> vertex_buffer_;
		buffer<uint32_t> index_buffer_;
		uint32_t index_count_;
	};
}
