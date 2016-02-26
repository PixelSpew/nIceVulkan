#pragma once
#include "vkwrap/buffer.h"
#include <vector>

namespace nif {
	template <typename T>
	class mesh {
	public:
		mesh(const device &device, const std::vector<T> &vertices, const std::vector<uint32_t> &indices);
		const buffer<T>& vertex_buffer() const;
		const buffer<uint32_t>& index_buffer() const;
		uint32_t index_count() const;

	private:
		buffer<T> vertex_buffer_;
		buffer<uint32_t> index_buffer_;
		uint32_t index_count_;
	};

	template<typename T>
	mesh<T>::mesh(const device &device, const std::vector<T> &vertices, const std::vector<uint32_t> &indices)
		: vertex_buffer_(device, vk::BufferUsageFlagBits::eVertexBuffer, vertices),
		index_buffer_(device, vk::BufferUsageFlagBits::eIndexBuffer, indices),
		index_count_(static_cast<uint32_t>(indices.size())) {
	}

	template<typename T>
	inline const buffer<T>& mesh<T>::vertex_buffer() const {
		return vertex_buffer_;
	}

	template<typename T>
	inline const buffer<uint32_t>& mesh<T>::index_buffer() const {
		return index_buffer_;
	}

	template<typename T>
	inline uint32_t mesh<T>::index_count() const {
		return index_count_;
	}
}
