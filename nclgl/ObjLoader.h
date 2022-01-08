#pragma once
#include <vector>
#include "OGLRenderer.h"

class ObjLoader {
public:
	ObjLoader() {}
	~ObjLoader(){}
	bool loadOBJ(
		const char* path,
		std::vector<Vector3>& out_vertices,
		std::vector<Vector2>& out_uvs,
		Vector3& size
	);
};