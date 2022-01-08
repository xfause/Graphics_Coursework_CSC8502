#pragma once
#include <vector>
#include "../nclgl/Mesh.h"
#include "../nclgl/OGLRenderer.h"
#include "../nclgl/HeightMap.h"
#include "VertexFormat.h"

#define M_PI 3.1415926535897932384626433832795

typedef std::vector<std::vector<unsigned int> > matrice;

typedef enum LODLEVELS {
	FULLLOD = 0,
	PARTIALLOD = 1,
	LOWLOD = 2,
	VERYLOWLOD = 3,
	INVISIBLE = 4
};

class Tree
{
public:
	Tree();

	Tree(HeightMap* hm);

	~Tree();

	void DrawTree(int lodLevel);

	void DrawLeaves(int lodLevel);

	void CreateTree();

	Vector3 GetStartPosition();

	static int SelectTreeLod(Vector3 cameraPosition, Vector3 treePosition);

private:
	int _detailLevel;
	int _noLeavesPerBranchCircle = 1;
	int _leafSize = 4.5f;

	int _noTrunkCircles = 4;
	int _trunkLevelHeight = 40;
	int _trunkRadius = 20;

	float _smoothCircles = 5;

	int _maxBranchLevel = 5;

	float _noBranchCircles = 4;

	float _maxBranchLength = 150;
	float _minBranchLength = 10;

	float _maxBranchRadius = 15;
	float _minBranchRadius = 0.2f;

	float _branchRandomness = 4;

	int _lodLevels = 4;


	HeightMap* _hm;

	float _resolution;

	Vector3 vecStartPosition;

	std::vector<unsigned int> _tree_vao, _tree_vbo, _tree_ibo, _tree_indices;
	matrice _branch_vao, _branch_vbo, _branch_ibo, _branch_indices;
	std::vector<unsigned int> _leaves_vao, _leaves_vbo, _leaves_ibo, _leaves_indices;

	std::vector<unsigned int> _noBranches;
	unsigned int _noLeaves;

	void CreateBranch(Vector3 startCirclePosition, Vector3 parentRotation, float branchLength, float branchSize, int level);
	void CreateLeaves(Vector3 branchStartCirclePosition, float branchLength, float branchSize, float rotateAngle, float branchCircle);

	void SelectResolution(int lodLevel);

	Vector3 GetRotationVector(Vector3 vecRotation);
	Vector3 GetOppositeRotation(Vector3 vecRotation);
};

