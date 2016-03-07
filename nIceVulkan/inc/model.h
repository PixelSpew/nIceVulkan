#pragma once
#include "mesh.h"
#include "math/vec3.h"
#include "ivertex.h"

namespace nif
{
	class model
	{
	public:
		struct vertex : ivertex<vertex, vec3>
		{
			vertex(const vec3 &pos);

			vec3 pos;
		};

		model(const device &device, const std::string &filename);
		const std::vector<mesh<vertex>>& meshes() const;

	private:
		std::vector<mesh<vertex>> meshes_;
	};
}
