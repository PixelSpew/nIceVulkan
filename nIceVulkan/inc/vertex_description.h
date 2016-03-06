#pragma once
#include "vulkan/vk_cpp.h"
#include "math/vec4.h"
#include <vector>
#include <map>
#include <typeindex>

namespace nif
{
	struct attrib_descriptions : std::vector<vk::VertexInputAttributeDescription>
	{
		attrib_descriptions add(std::type_index type) const
		{
			static const std::map<std::type_index, vk::Format> formats = {
				{ typeid(vec4), vk::Format::eR32G32B32A32Sfloat },
				{ typeid(vec3), vk::Format::eR32G32B32Sfloat },
				{ typeid(vec2), vk::Format::eR32G32Sfloat },
				{ typeid(float), vk::Format::eR32Sfloat }
			};

			attrib_descriptions ret = *this;
			ret.push_back(vk::VertexInputAttributeDescription(static_cast<uint32_t>(size()), 0, (*formats.find(type)).second, 0));
			return ret;
		}
	};

	template<typename T, typename ...Mems>
	class vertex_description;

	template<typename T>
	class vertex_description<T>
	{
	public:
		vertex_description() = delete;

		static const attrib_descriptions attrib_descs_copy()
		{
			return attrib_descriptions();
		}
	};

	template<typename T, typename This, typename ...Rest>
	class vertex_description<T, This, Rest...>
	{
	public:
		vertex_description() = delete;

		static const vk::PipelineVertexInputStateCreateInfo& value()
		{
			static const vk::PipelineVertexInputStateCreateInfo ret(
				0,
				static_cast<uint32_t>(bind_descs().size()),
				bind_descs().data(),
				static_cast<uint32_t>(attrib_descs().size()),
				attrib_descs().data());

			return ret;
		}

	private:
		static const std::vector<vk::VertexInputBindingDescription>& bind_descs()
		{
			static const std::vector<vk::VertexInputBindingDescription> ret = {
				vk::VertexInputBindingDescription(0, sizeof(T), vk::VertexInputRate::eVertex)
			};

			return ret;
		}

		static const attrib_descriptions& attrib_descs()
		{
			static const attrib_descriptions ret =
				vertex_description<T, Rest...>::attrib_descs_copy().add(typeid(This));

			return ret;
		}

		static const attrib_descriptions attrib_descs_copy()
		{
			return vertex_description<T, Rest...>::attrib_descs_copy().add(typeid(This))
		}
	};
}
