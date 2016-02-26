#pragma once
#include "mesh.h"
#include "math/vec3.h"

namespace nif {
	class model {
	public:
		struct vertex {
			vec3 pos;

			static const vk::PipelineVertexInputStateCreateInfo& pipeline_info();
		};

		model(const device &device, const std::string &filename);
		const std::vector<mesh<vertex>>& meshes() const;

	private:
		std::vector<mesh<vertex>> meshes_;
	};
}
