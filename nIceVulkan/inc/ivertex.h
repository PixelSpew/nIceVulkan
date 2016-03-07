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
	class ivertex;

	template<typename T>
	class ivertex<T>
	{
	public:
		static const attrib_descriptions attrib_descs()
		{
			return attrib_descriptions();
		}
	};

	template<typename T, typename This, typename ...Rest>
	class ivertex<T, This, Rest...>
	{
	public:
		static const vk::PipelineVertexInputStateCreateInfo description()
		{
			static const auto bindDescs = bind_descs();
			static const auto attribDescs = attrib_descs();

			static const vk::PipelineVertexInputStateCreateInfo ret(
				0,
				static_cast<uint32_t>(bindDescs.size()),
				bindDescs.data(),
				static_cast<uint32_t>(attribDescs.size()),
				attribDescs.data());

			return ret;
		}

	private:
		static const std::vector<vk::VertexInputBindingDescription> bind_descs()
		{
			return { vk::VertexInputBindingDescription(0, sizeof(T), vk::VertexInputRate::eVertex) };
		}

		static const attrib_descriptions attrib_descs()
		{
			return ivertex<T, Rest...>::attrib_descs().add(typeid(This));
		}
	};
}
