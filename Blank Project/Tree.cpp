#include "Tree.h"
#include "Grass.h"

Tree::Tree()
{
}

Tree::Tree(HeightMap* hm)
{
	_hm = hm;

	std::vector<unsigned int> tempVector = std::vector<unsigned int>(4);

	_tree_vao = tempVector;
	_tree_vbo = tempVector;
	_tree_ibo = tempVector;
	_tree_indices = tempVector;

	for (size_t i = 0; i < _lodLevels; i++)
	{
		_branch_vao.push_back(std::vector<unsigned int>());
		_branch_vbo.push_back(std::vector<unsigned int>());
		_branch_ibo.push_back(std::vector<unsigned int>());
		_branch_indices.push_back(std::vector<unsigned int>());



		_noBranches.push_back(0);

	}

	_noLeaves = 0;
}

Tree::~Tree()
{
}

void Tree::SelectResolution(int lodLevel)
{
	switch (lodLevel) {
	case FULLLOD:
		_resolution = 0.2f;
		break;

	case PARTIALLOD:
		_resolution = 0.6f;
		break;

	case LOWLOD:
		_resolution = 0.8f;
		break;

	case VERYLOWLOD:
		_resolution = 1.0f;
		break;
	}
}

void Tree::DrawTree(int lodLevel)
{
	if (lodLevel != INVISIBLE) {
		glBindVertexArray(_tree_vao[lodLevel]);
		glDrawElements(GL_TRIANGLES, _tree_indices[lodLevel], GL_UNSIGNED_INT, 0);

		for (int i = 0; i < _noBranches[lodLevel]; i++) {
			glBindVertexArray(_branch_vao[lodLevel][i]);
			glDrawElements(GL_TRIANGLES, _branch_indices[lodLevel][i], GL_UNSIGNED_INT, 0);
		}
	}

}

void Tree::DrawLeaves(int lodLevel)
{
	if (lodLevel != INVISIBLE) {
		for (int i = 0; i < _noLeaves / (lodLevel + 1); i++) {
			glBindVertexArray(_leaves_vao[i]);
			glDrawElements(GL_TRIANGLES, _leaves_indices[i], GL_UNSIGNED_INT, 0);
		}
	}
}

Vector3 Tree::GetOppositeRotation(Vector3 vecRotation) {
	int coordX = 0, coordY = 0, coordZ = 0;
	if (vecRotation.x != 0) {
		coordX = vecRotation.x == -1 ? 1 : -1;
	}

	if (vecRotation.y != 0) {
		coordY = vecRotation.y == -1 ? 1 : -1;
	}

	if (vecRotation.z != 0) {
		coordZ = vecRotation.z == -1 ? 1 : -1;
	}

	return Vector3(coordX, coordY, coordZ);
}

Vector3 Tree::GetRotationVector(Vector3 vecRotation) {
	Vector3 vecNewRotation;

	int nRotation = rand() % 6;
	int nCoordX = rand() % 3 - 1;
	int nCoordY = rand() % 3 - 1;
	int nCoordZ = rand() % 3 - 1;
	vecNewRotation = Vector3(nCoordX, nCoordY, nCoordZ);
	while (vecNewRotation == vecRotation ||
		vecNewRotation == GetOppositeRotation(vecRotation) ||
		vecNewRotation == Vector3(0, 0, 0) ||
		vecNewRotation == Vector3(1, 1, 1)
		) {
		int nCoordX = rand() % 3 - 1;
		int nCoordY = rand() % 3 - 1;
		int nCoordZ = rand() % 3 - 1;
		vecNewRotation = Vector3(nCoordX, nCoordY, nCoordZ);
	}

	return vecNewRotation;
}

void Tree::CreateLeaves(Vector3 branchStartCirclePosition, float branchLength, float branchSize, float rotateAngle, float branchCircle) {

	//for (int l = 0; l < _lodLevels; l++) {
	std::vector<VertexFormat> vecLeavesVertices;

	float fStartX = branchStartCirclePosition.x;
	float fStartY = branchStartCirclePosition.y;
	float fStartZ = branchStartCirclePosition.z;
	float fLeavesDistances = branchLength / _noLeavesPerBranchCircle;

	std::vector<unsigned int> indices;
	float rotateStep = 0;
	for (int i = 0; i < _noLeavesPerBranchCircle * 4; i += 4)
	{
		float randX = rand(); //% 360;
		float randZ = rand(); //% 360;
		float fPozX = fStartX + branchSize * 3 * cos(randX);
		float fPozZ = fStartZ + branchSize * 3 * sin(randZ);

		Matrix4 trans1, trans2, trans3, trans4;
		//trans1 = trans;
		Vector3 vecRotation;

		rotateStep = rand() % 360;
		vecRotation = Vector3(1, 1, 1);
		trans1 = Matrix4::Rotation(rotateStep, vecRotation);
		Vector4 vRotation1 = trans1 * Vector4(0, 0, 0, 1);

		rotateStep = rand() % 360;
		vecRotation = GetRotationVector(Vector3(0, 0, 0));
		trans2 = trans1* Matrix4::Rotation(rotateStep, vecRotation);
		Vector4 vRotation2 = trans1 * Vector4(_leafSize, 0, 0, 1);

		rotateStep = rand() % 360;
		vecRotation = GetRotationVector(Vector3(0, 0, 0));
		trans3 = trans1 * Matrix4::Rotation(rotateStep, vecRotation);
		Vector4 vRotation3 = trans1 * Vector4(_leafSize, _leafSize, 0, 1);

		rotateStep = rand() % 360;
		vecRotation = GetRotationVector(Vector3(0, 0, 0));
		trans4 = trans1 * Matrix4::Rotation(rotateStep, vecRotation);
		Vector4 vRotation4 = trans1 * Vector4(0, _leafSize, 0, 1);

		VertexFormat vertex1 = VertexFormat(fPozX + vRotation1.x, fStartY + vRotation1.y, fPozZ + vRotation1.z, 0, 1, 0, 1, 1);
		VertexFormat vertex2 = VertexFormat(fPozX + vRotation2.x + _leafSize, fStartY + vRotation2.y, fPozZ + vRotation2.z, 0, 1, 0, 0, 1);
		VertexFormat vertex3 = VertexFormat(fPozX + vRotation3.x + _leafSize, fStartY + vRotation3.y + _leafSize, fPozZ + vRotation3.z, 0, 1, 0, 0, 0);
		VertexFormat vertex4 = VertexFormat(fPozX + vRotation4.x, fStartY + vRotation4.y + _leafSize, fPozZ + vRotation4.z, 0, 1, 0, 1, 0);

		vertex1.branch_startX = fStartX;
		vertex1.branch_startY = fStartY;
		vertex1.branch_startZ = fStartZ;

		vertex2.branch_startX = fStartX;
		vertex2.branch_startY = fStartY;
		vertex2.branch_startZ = fStartZ;

		vertex3.branch_startX = fStartX;
		vertex3.branch_startY = fStartY;
		vertex3.branch_startZ = fStartZ;

		vertex4.branch_startX = fStartX;
		vertex4.branch_startY = fStartY;
		vertex4.branch_startZ = fStartZ;

		vertex1.branchCircle = branchCircle;
		vertex2.branchCircle = branchCircle;
		vertex3.branchCircle = branchCircle;
		vertex4.branchCircle = branchCircle;

		vecLeavesVertices.push_back(vertex1);
		vecLeavesVertices.push_back(vertex2);
		vecLeavesVertices.push_back(vertex3);
		vecLeavesVertices.push_back(vertex4);

		indices.push_back(i + 0); indices.push_back(i + 1); indices.push_back(i + 2);
		indices.push_back(i + 2); indices.push_back(i + 3);  indices.push_back(i + 0);
	}

	unsigned int vao, vbo, ibo;
	//vertex array object 
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//vertex buffer object
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vecLeavesVertices.size() * sizeof(VertexFormat), &vecLeavesVertices[0], GL_STATIC_DRAW);

	//index buffer object
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)0);						//trimite pozitii pe pipe 0
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(float) * 3));		//trimite normale pe pipe 1
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(float) * 3));	//trimite texcoords pe pipe 2
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(float) * 8));	// trimite pozitia de start a crengii
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(float) * 11));	// trimite detalii creanga
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(float) * 14));	// trimite detalii creanga
	_leaves_vao.push_back(vao);
	_leaves_vbo.push_back(vbo);
	_leaves_ibo.push_back(ibo);

	_leaves_indices.push_back(indices.size());

	_noLeaves++;
	//}
}

void Tree::CreateBranch(Vector3 startCirclePosition, Vector3 parentRotation, float branchLength, float branchSize, int level) {

	std::vector<Vector3> vecCirclePositions;

	Vector3 middleCirclePosition;
	float posX = startCirclePosition.x;
	float posZ = startCirclePosition.z;
	float posY = startCirclePosition.y;

	float rotateStep = 50 + rand() % 16;
	float fRotateAngle = rotateStep;
	bool changeRotation = true;

	Matrix4 trans;
	trans = Matrix4::Rotation(rotateStep, parentRotation);

	for (int l = 0; l < _lodLevels; l++) {
		std::vector<VertexFormat> vecLevelVertices;
		std::vector<int> vecLevelIndices;

		SelectResolution(l);
		int verticesPerLevel = (2 * M_PI) / _resolution;

		float crtLevelDistance = branchLength / _noBranchCircles;
		float crtLevelRadius = branchSize;
		float nextLevelRadius = branchSize / 2;

		float smoothRadiusStep = (crtLevelRadius - nextLevelRadius) / _smoothCircles;
		float branchRadiusStep = (_maxBranchRadius - _minBranchRadius) / _maxBranchLevel;

		for (int i = 0; i < _noBranchCircles; i++) {

			if (i >= _noBranchCircles - _smoothCircles) {
				crtLevelRadius -= smoothRadiusStep;
			}

			Vector4 vecVertexCenter = trans * Vector4(0, 0 + crtLevelDistance * i, 0, 1);

			VertexFormat centerLevelVertex = VertexFormat(posX + vecVertexCenter.x, posY + vecVertexCenter.y, posZ + vecVertexCenter.z, 0, 1, 0, 0, 0);
			centerLevelVertex.branch_startX = posX;
			centerLevelVertex.branch_startY = posY;
			centerLevelVertex.branch_startZ = posZ;

			centerLevelVertex.branchCircle = i;
			centerLevelVertex.branchRadius = crtLevelRadius;
			centerLevelVertex.branchLevel = level;

			centerLevelVertex.branchVertex = 1.0f;
			centerLevelVertex.branchLength = branchLength;


			Vector3 vecCenterLevel = Vector3(centerLevelVertex.coordX, centerLevelVertex.coordY, centerLevelVertex.coordZ);
			vecCirclePositions.push_back(vecCenterLevel);

			if (level == _maxBranchLevel && i < _noBranchCircles - 1) {
				CreateLeaves(vecCenterLevel, branchLength, branchSize, rotateStep, i);
			}
			vecLevelVertices.push_back(centerLevelVertex);


			for (int j = 0; j < verticesPerLevel; j++) {
				float randOffsetX = 0, randOffsetY = 0, randOffsetZ = 0;
				int nLevelRadius = (int)crtLevelRadius;
				if (nLevelRadius < 1) nLevelRadius++;

				randOffsetX = (1 + (rand() % (int)nLevelRadius)) / _branchRandomness;
				randOffsetY = 0;
				randOffsetZ = (1 + (rand() % (int)nLevelRadius)) / _branchRandomness;

				float coordX = crtLevelRadius * cos(j) + randOffsetX;
				float coordZ = crtLevelRadius * sin(j) + randOffsetZ;
				float coordY = crtLevelDistance * i + randOffsetY;

				Vector4 vecVertex = trans * Vector4(coordX, coordY, coordZ, 1);

				VertexFormat tempVertex = VertexFormat(posX + vecVertex.x, posY + vecVertex.y, posZ + vecVertex.z, 0, 1, 0, 0, 0);

				tempVertex.branch_startX = posX;
				tempVertex.branch_startY = posY;
				tempVertex.branch_startZ = posZ;

				tempVertex.branchCircle = i;
				tempVertex.branchRadius = crtLevelRadius;
				tempVertex.branchLevel = level;

				tempVertex.branchVertex = 1.0f;
				tempVertex.branchLength = branchLength;


				vecLevelVertices.push_back(tempVertex);
			}
		}

		for (int i = 0; i < _noBranchCircles; i++) {
			int crtCenterIndex = i * verticesPerLevel + i;
			for (int j = 1; j < verticesPerLevel - 1; j++) {
				vecLevelIndices.push_back(crtCenterIndex);
				vecLevelIndices.push_back(crtCenterIndex + j);
				vecLevelIndices.push_back(crtCenterIndex + j + 1);
			}

		}

		for (int i = 0; i < _noBranchCircles - 1; i++) {
			int level1CenterIndex = i * verticesPerLevel + i;
			int level2CenterIndex = (i + 1) * verticesPerLevel + (i + 1);
			for (int j = 1; j < verticesPerLevel; j += 2) {

				vecLevelIndices.push_back(level1CenterIndex + j); //11
				vecLevelIndices.push_back(level1CenterIndex + j + 1); //12
				vecLevelIndices.push_back(level2CenterIndex + j + 1); //22

				vecLevelIndices.push_back(level2CenterIndex + j + 1); //22
				vecLevelIndices.push_back(level2CenterIndex + j); //21
				vecLevelIndices.push_back(level1CenterIndex + j); //11
			}
		}

		for (int i = 0; i < _noBranchCircles; i += 2) {
			int level1CenterIndex = i * verticesPerLevel + i;
			int level2CenterIndex = (i + 1) * verticesPerLevel + (i + 1);
			for (int j = 1; j < verticesPerLevel; j += 2) {

				vecLevelVertices[level1CenterIndex + j].texcoord_x = 0;
				vecLevelVertices[level1CenterIndex + j].texcoord_y = 0;

				vecLevelVertices[level1CenterIndex + j + 1].texcoord_x = 1;
				vecLevelVertices[level1CenterIndex + j + 1].texcoord_y = 0;

				vecLevelVertices[level2CenterIndex + j + 1].texcoord_x = 1;
				vecLevelVertices[level2CenterIndex + j + 1].texcoord_y = 1;

				vecLevelVertices[level2CenterIndex + j].texcoord_x = 0;
				vecLevelVertices[level2CenterIndex + j].texcoord_y = 1;
			}
		}

		//vertex array object 
		unsigned int vao, vbo, ibo;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		//vertex buffer object 
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vecLevelVertices.size() * sizeof(VertexFormat), &vecLevelVertices[0], GL_STATIC_DRAW);

		//index buffer object 
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, vecLevelIndices.size() * sizeof(unsigned int), &vecLevelIndices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)0);					// trimite pozitii pe pipe 0
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(float) * 3));	// trimite normale pe pipe 1
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(float) * 6));	// trimite texcoord pe pipe 2
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(float) * 8));	// trimite pozitia de start a crengii
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(float) * 11));	// trimite detalii creanga
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(float) * 14));	// trimite detalii creanga

		_branch_vao[l].push_back(vao);
		_branch_vbo[l].push_back(vbo);
		_branch_ibo[l].push_back(ibo);

		_branch_indices[l].push_back(vecLevelIndices.size());

		_noBranches[l] ++;
	}

	float nCircleLevel, newBranchSize, newBranchLength;
	float branchLengthStep = (_maxBranchLength - _minBranchLength) / (float)_maxBranchLevel;

	int nNewLevel;
	Vector3 vecNewRotation;

	if (level < _maxBranchLevel) {
		int nNewBranches = abs(rand() % (level + 2));
		if (level == _maxBranchLevel - 1) {
			nNewBranches = 2;
		}
		for (int i = 0; i < nNewBranches; i++) {
			nCircleLevel = abs(rand() % ((int)_noBranchCircles - 2)) + 1;
			vecNewRotation = GetRotationVector(parentRotation);

			newBranchLength = branchLength - branchLengthStep;
			CreateBranch(vecCirclePositions[nCircleLevel], vecNewRotation, newBranchLength, branchSize / 2, level + 1);
		}

		newBranchLength = branchLength - branchLengthStep;
		vecNewRotation = GetRotationVector(GetOppositeRotation(parentRotation));

		CreateBranch(vecCirclePositions[_noBranchCircles - 1], vecNewRotation, newBranchLength, branchSize / 2, level + 1);

		vecNewRotation = GetOppositeRotation(vecNewRotation);

		CreateBranch(vecCirclePositions[_noBranchCircles - 1], vecNewRotation, newBranchLength, branchSize / 2, level + 1);
	}
}

void Tree::CreateTree()
{
	std::vector<VertexFormat> vecLevelVertices;
	std::vector<int> vecLevelIndices;
	Vector3 topCircleCenter;

	int fTerrainWidth = _hm->GetHeightMapSize().x;
	int fTerrainHeight = _hm->GetHeightMapSize().z;

	int posX = rand() % (fTerrainWidth - 40) + 20;
	int posZ = rand() % (fTerrainHeight - 40) + 20;
	float posY = _hm->GetCoordHeight(posX / 16, posZ / 16) - 10;

	posX += abs(rand() % (16));
	posZ += abs(rand() % (16));

	for (int l = 0; l < _lodLevels; l++) {
		vecLevelVertices.clear();
		vecLevelIndices.clear();

		SelectResolution(0);

		int verticesPerLevel = (2 * M_PI) / _resolution;

		vecStartPosition = Vector3(posX, posY, posZ);
		float trunkRadius = _trunkRadius;
		float smoothTrunkStep = (_trunkRadius - _maxBranchRadius) / _smoothCircles;
		for (int i = 0; i < _noTrunkCircles; i++) {
			VertexFormat centerLevelVertex = VertexFormat(posX, posY + _trunkLevelHeight * i, posZ, 0, 1, 0, 0, 0);

			if (i == _noTrunkCircles - 1) {
				topCircleCenter = Vector3(posX, posY + _trunkLevelHeight * i, posZ);
			}

			if (i >= _noTrunkCircles - _smoothCircles) {
				trunkRadius -= smoothTrunkStep;
			}

			vecLevelVertices.push_back(centerLevelVertex);

			for (int j = 0; j < verticesPerLevel; j++) {
				int randOffsetX = 0, randOffsetY = 0, randOffsetZ = 0;

				randOffsetX = (rand() % (_noTrunkCircles - i)) * ((_noTrunkCircles - i) / 2);
				randOffsetY = rand() % (_trunkLevelHeight / 2);
				randOffsetZ = (rand() % (_noTrunkCircles - i)) * ((_noTrunkCircles - i) / 2);

				VertexFormat tempVertex = VertexFormat(posX + trunkRadius * cos(j) + randOffsetX,
					posY + _trunkLevelHeight * i + randOffsetY,
					posZ + trunkRadius * sin(j) + randOffsetZ,
					0, 1, 0, 0, 0);
				vecLevelVertices.push_back(tempVertex);
			}
		}

		for (int i = 0; i < _noTrunkCircles; i++) {
			int crtCenterIndex = i * verticesPerLevel + i;
			for (int j = 1; j < verticesPerLevel - 1; j++) {
				vecLevelIndices.push_back(crtCenterIndex);
				vecLevelIndices.push_back(crtCenterIndex + j);
				vecLevelIndices.push_back(crtCenterIndex + j + 1);
			}
		}

		for (int i = 0; i < _noTrunkCircles - 1; i++) {
			int level1CenterIndex = i * verticesPerLevel + i;
			int level2CenterIndex = (i + 1) * verticesPerLevel + (i + 1);
			for (int j = 1; j < verticesPerLevel; j += 2) {

				vecLevelIndices.push_back(level1CenterIndex + j); //11
				vecLevelIndices.push_back(level1CenterIndex + j + 1); //12
				vecLevelIndices.push_back(level2CenterIndex + j + 1); //22

				vecLevelIndices.push_back(level2CenterIndex + j + 1); //22
				vecLevelIndices.push_back(level2CenterIndex + j); //21
				vecLevelIndices.push_back(level1CenterIndex + j); //11
			}
		}

		for (int i = 0; i < _noTrunkCircles; i += 2) {
			int level1CenterIndex = i * verticesPerLevel + i;
			int level2CenterIndex = (i + 1) * verticesPerLevel + (i + 1);
			for (int j = 1; j < verticesPerLevel; j += 2) {

				vecLevelVertices[level1CenterIndex + j].texcoord_x = 0;
				vecLevelVertices[level1CenterIndex + j].texcoord_y = 0;

				vecLevelVertices[level1CenterIndex + j + 1].texcoord_x = 1;
				vecLevelVertices[level1CenterIndex + j + 1].texcoord_y = 0;

				vecLevelVertices[level2CenterIndex + j + 1].texcoord_x = 1;
				vecLevelVertices[level2CenterIndex + j + 1].texcoord_y = 1;

				vecLevelVertices[level2CenterIndex + j].texcoord_x = 0;
				vecLevelVertices[level2CenterIndex + j].texcoord_y = 1;
			}
		}

		// vertex array object
		unsigned int vao, vbo, ibo;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		// vertex buffer object
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vecLevelVertices.size() * sizeof(VertexFormat), &vecLevelVertices[0], GL_STATIC_DRAW);

		// index buffer object 
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, vecLevelIndices.size() * sizeof(unsigned int), &vecLevelIndices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)0);						// trimite pozitii pe pipe 0
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(float) * 3));		// trimite normale pe pipe 1
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(float) * 3));	// trimite texcoord pe pipe 2

		_tree_vao[l] = vao;
		_tree_vbo[l] = vbo;
		_tree_ibo[l] = ibo;
		_tree_indices[l] = vecLevelIndices.size();
	}

	int nBranches = rand() % 2 + 2;

	for (int i = 0; i < nBranches; i++) {

		int nRotationX = rand() % 3 - 1;
		int nRotationY = rand() % 3 - 1;
		int nRotationZ = rand() % 3 - 1;

		int nRotation = rand() % 6;
		CreateBranch(topCircleCenter, Vector3(nRotationX, nRotationY, nRotationZ), _maxBranchLength, _maxBranchRadius, 1);
	}
}

Vector3 Tree::GetStartPosition()
{
	return vecStartPosition;
}

int Tree::SelectTreeLod(Vector3 cameraPostion, Vector3 treePosition) {
	Grass tool = Grass();
	float distance = tool.Distance(cameraPostion, treePosition);
	//std::cout << distance << std::endl;
	if (distance < 1000) {
		return FULLLOD;
	}
	else if (distance < 2000) {
		return PARTIALLOD;
	}
	else if (distance < 2500) {
		return LOWLOD;
	}
	else if (distance < 3000) {
		return VERYLOWLOD;
	}
	else {
		return INVISIBLE;
	}
}