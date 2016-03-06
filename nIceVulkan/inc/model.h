#pragma once
#include "mesh.h"
#include "math/vec3.h"
#include "vertex_description.h"

namespace nif
{
	class model
	{
	public:
		struct vertex
		{
			vec3 pos;

			using description = vertex_description<vertex, vec3>;
		};

		model(const device &device, const std::string &filename);
		const std::vector<mesh<vertex>>& meshes() const;

	private:
		std::vector<mesh<vertex>> meshes_;
	};
}
