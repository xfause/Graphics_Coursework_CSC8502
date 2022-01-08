#pragma once
#include <vector>
#include "../nclgl/Mesh.h";

class BetterWater:public Mesh {
public:
	std::vector<float> init_plane(int size, float width, float height);
	std::vector<GLuint> init_indices(int size);

	Mesh* load_object(std::vector<float>& vertices, std::vector<GLuint>& indices);
};