#pragma once
#include "../nclgl/Mesh.h"
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/HeightMap.h"
#include "VertexFormat.h"

class Grass {

public:
	Grass();
	~Grass();

	Grass(int grassCount, float grassWidth, float grassHeight, int coordX, int coordZ, int grassRadius, HeightMap* hm);

	void CreateGrass();
	void CreateGrassBlade();
	void CreateHighDetailGrass();

	void DrawGrass(int lodLevel);

	Vector3 GetStartPosition();

	static int SelectGrassLod(Vector3 cameraPosition, Vector3 treePosition, float grassRadius);

	float Distance(const Vector3& point1, const Vector3& point2)
	{
		float distance = sqrt((point1.x - point2.x) * (point1.x - point2.x) +
			(point1.y - point2.y) * (point1.y - point2.y) +
			(point1.z - point2.z) * (point1.z - point2.z));
		return distance;
	}

	Vector4 V4xM4(Vector4 a, Matrix4 b) {
		std::vector<float> vals;
		for (int i = 0; i < 16; i++) {
			vals.push_back(b.values[i]);
		}
		return Vector4(
			a.x * vals[0] + a.y * vals[4] + a.z * vals[8] + a.w * vals[12],
			a.x * vals[1] + a.y * vals[5] + a.z * vals[9] + a.w * vals[13],
			a.x * vals[2] + a.y * vals[6] + a.z * vals[10] + a.w * vals[14],
			a.x * vals[3] + a.y * vals[7] + a.z * vals[11] + a.w * vals[15]

		);
	}

protected:
	std::vector<Vector3> _vecPositions;
	float _grassWidth, _grassHeight;
	std::vector<unsigned int> _grass_vao, _grass_vbo, _grass_ibo, _grass_indices;
	int _indicesCount;
	int _grassCount;
	float _nGrassSize;
	float _grassRadius;
	GLuint _shaderProgram;
	int _lodLevels = 3;
	int _coordX = 0;
	int _coordZ = 0;
	int _coordY = 0;
	int _terrainScaleFactor = 0;
	int _grass;
	HeightMap* _hm;

	Vector3 _startPosition;

	std::vector<int> _grassIndices;
	void CreateFullBlade(Vector3 bladePosition, std::vector<VertexFormat>& vertices, int lod);
	void CreatePartialBlade(Vector3 bladePosition, std::vector<VertexFormat>& vertices, int lod);

	void ComputePartialBladeIndices(std::vector<unsigned int>& indices, int lodLevel);
	void ComputeFullBladeIndices(std::vector<unsigned int>& indices, int lodLevel);
};