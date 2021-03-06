#include "stdafx.h"
#include "model.h"
#include "math/vec3.h"
#include "tiny_obj_loader.h"

using namespace std;
using namespace tinyobj;

namespace nif
{
	model::vertex::vertex(const vec3 &pos) :
		pos(pos)
	{
	}

	model::model(const device &device, const std::string &filename)
	{
		vector<shape_t> shapes;
		vector<material_t> materials;

		string err;
		bool ret = LoadObj(shapes, materials, err, filename.c_str());

		vector<vertex> vertices;
		vertices.reserve(shapes[0].mesh.positions.size() / 3);
		for (size_t i = 0; i < shapes[0].mesh.positions.size(); i += 3) {
			vertices.push_back({ vec3(shapes[0].mesh.positions[i], shapes[0].mesh.positions[i + 1], shapes[0].mesh.positions[i + 2]) });
		}

		vector<unsigned int> indices;
		indices.reserve(shapes[0].mesh.indices.size());
		for (size_t i = 0; i < shapes[0].mesh.indices.size(); i++) {
			indices.push_back({ shapes[0].mesh.indices[i] });
		}

		meshes_.push_back(mesh<vertex>(device, vertices, indices));
	}

	const std::vector<mesh<model::vertex>>& model::meshes() const
	{
		return meshes_;
	}
}
