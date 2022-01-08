#include "Grass.h"

#define M_PI 3.1415926535897932384626433832795

typedef enum LODLEVELS {
	FULLLOD = 0,
	PARTIALLOD = 1,
	LOWLOD = 2,
	VERYLOWLOD = 3,
	INVISIBLE = 4
};

Grass::Grass() {}

Grass::~Grass() {
}

Grass::Grass(int grassCount, float grassWidth, float grassHeight, int coordX, int coordZ, int grassRadius, HeightMap* hm) {
	_grassCount = grassCount;
	_grass = 0;
	_grassWidth = grassWidth;
	_grassHeight = grassHeight;
	_grassRadius = grassRadius;
	_coordX = coordX;
	_coordZ = coordZ;
	_terrainScaleFactor = 16;
	_hm = hm;
	_coordY = _hm->GetCoordHeight(_coordX / _terrainScaleFactor, _coordZ / _terrainScaleFactor);

	_grassIndices = std::vector<int>(_lodLevels);
	for (int i = 0; i < _lodLevels; i++) {
		_grassIndices[i] = 0;
		_grass_vao.push_back(0);
		_grass_vbo.push_back(0);
		_grass_ibo.push_back(0);
		_grass_indices.push_back(0);
	}

	_startPosition = Vector3(_coordX, _coordY, _coordZ);
	CreateGrassBlade();
}

void Grass::ComputePartialBladeIndices(std::vector<unsigned int>& indices, int lodLevel) {
	int lodIndices = _grassIndices[lodLevel];
	switch (lodLevel) {
	case 0:
		indices.push_back(lodIndices + 0); indices.push_back(lodIndices + 1); indices.push_back(lodIndices + 2);
		indices.push_back(lodIndices + 1); indices.push_back(lodIndices + 3); indices.push_back(lodIndices + 2);
		indices.push_back(lodIndices + 2); indices.push_back(lodIndices + 3); indices.push_back(lodIndices + 4);
		indices.push_back(lodIndices + 3); indices.push_back(lodIndices + 5); indices.push_back(lodIndices + 4);
		indices.push_back(lodIndices + 4); indices.push_back(lodIndices + 5); indices.push_back(lodIndices + 6);
		_grassIndices[lodLevel] += 7;
		break;

	case 1:
		indices.push_back(lodIndices + 0); indices.push_back(lodIndices + 1); indices.push_back(lodIndices + 2);
		indices.push_back(lodIndices + 1); indices.push_back(lodIndices + 3); indices.push_back(lodIndices + 2);
		indices.push_back(lodIndices + 2); indices.push_back(lodIndices + 3); indices.push_back(lodIndices + 4);
		_grassIndices[lodLevel] += 5;
		break;

	case 2:
		indices.push_back(lodIndices + 0); indices.push_back(lodIndices + 1); indices.push_back(lodIndices + 2);
		_grassIndices[lodLevel] += 3;
	}
}

void Grass::ComputeFullBladeIndices(std::vector<unsigned int>& indices, int lodLevel) {
	int lodIndices = _grassIndices[lodLevel];
	switch (lodLevel) {
	case 0:
		indices.push_back(lodIndices + 0); indices.push_back(lodIndices + 1); indices.push_back(lodIndices + 2);
		indices.push_back(lodIndices + 1); indices.push_back(lodIndices + 3); indices.push_back(lodIndices + 2);
		indices.push_back(lodIndices + 2); indices.push_back(lodIndices + 3); indices.push_back(lodIndices + 4);
		indices.push_back(lodIndices + 3); indices.push_back(lodIndices + 5); indices.push_back(lodIndices + 4);
		indices.push_back(lodIndices + 4); indices.push_back(lodIndices + 5); indices.push_back(lodIndices + 6);
		_grassIndices[lodLevel] += 7;
		break;
	case 1:
		indices.push_back(lodIndices + 0); indices.push_back(lodIndices + 1); indices.push_back(lodIndices + 2);
		indices.push_back(lodIndices + 1); indices.push_back(lodIndices + 3); indices.push_back(lodIndices + 2);
		indices.push_back(lodIndices + 2); indices.push_back(lodIndices + 3); indices.push_back(lodIndices + 4);
		_grassIndices[lodLevel] += 5;
		break;
	case 2:
		indices.push_back(lodIndices + 0); indices.push_back(lodIndices + 1); indices.push_back(lodIndices + 2);
		_grassIndices[lodLevel] += 3;
	}
}

void Grass::CreatePartialBlade(Vector3 bladePosition,
	std::vector<VertexFormat>& vertices, int lod)
{
	Vector3 svPosition = bladePosition;
	float fOffsetX1 = _grassWidth * 3.0f / 5.0f;
	float fOffsetX2 = _grassWidth * 3.0f / 2.0f;
	float fOffsetX3 = 3.5f * _grassWidth;

	float fHDist = _grassWidth;

	float fHeight1 = _grassHeight * 2.0f / 3.0f;
	float fHeight2 = _grassHeight;
	float fHeight3 = _grassHeight;

	Vector4 vertexPosition;
	Vector3 up = Vector3(0, 1, 0);

	float rotateAngle = rand() % 360;
	Matrix4 trans = Matrix4::Translation(svPosition) * Matrix4::Rotation(rotateAngle, up);
	Vector4 vRotation = Vector4(0, 0, 0, 0);

	float randMovement = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 2;
	if (lod < 1) {
		// 0
		vertexPosition = Vector4(-fHDist * 2, fHeight1, 0, 0);
		vRotation = trans * vertexPosition;
		vertexPosition += Vector4(vRotation.x, vRotation.y, vRotation.z, 0);
		vertexPosition += Vector4(svPosition.x, svPosition.y, svPosition.z, 0);

		float randGreen = ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 100.0f + 40.0f) / 255.0f);
		vertices.push_back(VertexFormat(vertexPosition.x, vertexPosition.y, vertexPosition.z, 0, randGreen, 0, 1, randMovement));
		// 1 
		vertexPosition = Vector4(0, fHeight1 * 2.0f / 3.0f, 0, 0);
		vRotation = trans * vertexPosition;
		vertexPosition += Vector4(vRotation.x, vRotation.y, vRotation.z, 0);
		vertexPosition += Vector4(svPosition.x, svPosition.y, svPosition.z, 0);

		randGreen = ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 100.0f + 40.0f) / 255.0f);
		vertices.push_back(VertexFormat(vertexPosition.x, vertexPosition.y, vertexPosition.z, 0, randGreen, 0, 1, randMovement));
	}

	if (lod < 2) {
		// 2
		vertexPosition = Vector4(0, 0, 0, 0);
		vRotation = trans * vertexPosition;
		vertexPosition += Vector4(vRotation.x, vRotation.y, vRotation.z, 0);
		vertexPosition += Vector4(svPosition.x, svPosition.y, svPosition.z, 0);

		float randGreen = ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 100.0f + 40.0f) / 255.0f);
		vertices.push_back(VertexFormat(vertexPosition.x, vertexPosition.y, vertexPosition.z, 0, randGreen, 0, 0, randMovement));

		// 3 
		vertexPosition = Vector4(fOffsetX1, 0, 0, 0);
		vRotation = trans * vertexPosition;
		vertexPosition += Vector4(vRotation.x, vRotation.y, vRotation.z, 0);
		vertexPosition += Vector4(svPosition.x, svPosition.y, svPosition.z, 0);

		randGreen = ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 100.0f + 40.0f) / 255.0f);
		vertices.push_back(VertexFormat(vertexPosition.x, vertexPosition.y, vertexPosition.z, 0, randGreen, 0, 0, randMovement));
	}
	if (lod < 3) {
		// 4 
		vertexPosition = Vector4(fOffsetX1, fHeight1, 0, 0);
		vRotation = trans * vertexPosition;
		vertexPosition += Vector4(vRotation.x, vRotation.y, vRotation.z, 0);
		vertexPosition += Vector4(svPosition.x, svPosition.y, svPosition.z, 0);

		float randGreen = ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 100.0f + 40.0f) / 255.0f);
		vertices.push_back(VertexFormat(vertexPosition.x, vertexPosition.y, vertexPosition.z, 0, randGreen, 0, 1, randMovement));
		// 5
		vertexPosition = Vector4(fOffsetX1 + fHDist, fHeight1, 0, 0);
		vRotation = trans * vertexPosition;
		vertexPosition += Vector4(vRotation.x, vRotation.y, vRotation.z, 0);
		vertexPosition += Vector4(svPosition.x, svPosition.y, svPosition.z, 0);

		randGreen = ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 100.0f + 40.0f) / 255.0f);
		vertices.push_back(VertexFormat(vertexPosition.x, vertexPosition.y, vertexPosition.z, 0, randGreen, 0, 1, randMovement));
		// 6
		vertexPosition = Vector4(fOffsetX2 + fHDist, fHeight2, 0, 0);
		vRotation = trans * vertexPosition;
		vertexPosition += Vector4(vRotation.x, vRotation.y, vRotation.z, 0);
		vertexPosition += Vector4(svPosition.x, svPosition.y, svPosition.z, 0);

		randGreen = ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 100.0f + 40.0f) / 255.0f);
		vertices.push_back(VertexFormat(vertexPosition.x, vertexPosition.y, vertexPosition.z, 0, randGreen, 0, 1, randMovement));
	}
}

void Grass::CreateFullBlade(Vector3 bladePosition,
	std::vector<VertexFormat>& vertices, int lod)
{
	Vector3 svPosition = bladePosition;
	float fOffsetX1 = _grassWidth * 3.0f / 5.0f;
	float fOffsetX2 = _grassWidth * 3.0f / 2.0f;
	float fOffsetX3 = 3.5f * _grassWidth;

	float fHDist = _grassWidth;

	float fHeight1 = _grassHeight / 3.0f;
	float fHeight2 = _grassHeight * 2.0f / 3.0f;
	float fHeight3 = _grassHeight;

	Vector4 vertexPosition;
	Vector3 up = Vector3(0, 1, 0);

	float rotateAngle = rand() % 360;
	Matrix4 trans =  Matrix4::Translation(svPosition) * Matrix4::Rotation(rotateAngle, up);
	Vector4 vRotation = Vector4(0, 0, 0, 0);

	float randMovement = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 2;
	if (lod < 1) {
		// 0
		vertexPosition = Vector4(0, 0, 0, 0);
		vRotation = trans * vertexPosition;
		vertexPosition += Vector4(vRotation.x, vRotation.y, vRotation.z, 0);
		vertexPosition += Vector4(svPosition.x, svPosition.y, svPosition.z, 0);

		float randGreen = ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 100.0f + 40.0f) / 255.0f);

		vertices.push_back(VertexFormat(vertexPosition.x, vertexPosition.y, vertexPosition.z, 0, randGreen, 0, 0, randMovement));

		// 1 
		vertexPosition = Vector4(fHDist, 0, 0, 0);
		vRotation = trans * vertexPosition;
		vertexPosition += Vector4(vRotation.x, vRotation.y, vRotation.z, 0);
		vertexPosition += Vector4(svPosition.x, svPosition.y, svPosition.z, 0);

		randGreen = ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 100.0f + 40.0f) / 255.0f);
		vertices.push_back(VertexFormat(vertexPosition.x, vertexPosition.y, vertexPosition.z, 0, randGreen, 0, 0, randMovement));

	}
	if (lod < 2) {
		// 2
		vertexPosition = Vector4(fOffsetX1, fHeight1, 0, 0);
		vRotation = trans * vertexPosition;
		vertexPosition += Vector4(vRotation.x, vRotation.y, vRotation.z, 0);
		vertexPosition += Vector4(svPosition.x, svPosition.y, svPosition.z, 0);

		float randGreen = ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 100.0f + 40.0f) / 255.0f);
		vertices.push_back(VertexFormat(vertexPosition.x, vertexPosition.y, vertexPosition.z, 0, randGreen, 0, 1, randMovement));


		// 3 
		vertexPosition = Vector4(fOffsetX1 + fHDist, fHeight1, 0, 0);
		vRotation = trans * vertexPosition;
		vertexPosition += Vector4(vRotation.x, vRotation.y, vRotation.z, 0);
		vertexPosition += Vector4(svPosition.x, svPosition.y, svPosition.z, 0);

		randGreen = ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 100.0f + 40.0f) / 255.0f);
		vertices.push_back(VertexFormat(vertexPosition.x, vertexPosition.y, vertexPosition.z, 0, randGreen, 0, 1, randMovement));
	}
	if (lod < 3) {
		// 4 
		vertexPosition = Vector4(fOffsetX2, fHeight2, 0, 0);
		vRotation = trans * vertexPosition;
		vertexPosition += Vector4(vRotation.x, vRotation.y, vRotation.z, 0);
		vertexPosition += Vector4(svPosition.x, svPosition.y, svPosition.z, 0);

		float randGreen = ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 100.0f + 40.0f) / 255.0f);
		vertices.push_back(VertexFormat(vertexPosition.x, vertexPosition.y, vertexPosition.z, 0, randGreen, 0, 1, randMovement));


		// 5
		vertexPosition = Vector4(fOffsetX2 + fHDist, fHeight2, 0, 0);
		vRotation = trans * vertexPosition;
		vertexPosition += Vector4(vRotation.x, vRotation.y, vRotation.z, 0);
		vertexPosition += Vector4(svPosition.x, svPosition.y, svPosition.z, 0);

		randGreen = ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 100.0f + 40.0f) / 255.0f);
		vertices.push_back(VertexFormat(vertexPosition.x, vertexPosition.y, vertexPosition.z, 0, randGreen, 0, 1, randMovement));

		// 6
		vertexPosition = Vector4(fOffsetX3, fHeight3, 0, 0);
		vRotation = trans * vertexPosition;
		vertexPosition += Vector4(vRotation.x, vRotation.y, vRotation.z, 0);
		vertexPosition += Vector4(svPosition.x, svPosition.y, svPosition.z, 0);

		randGreen = ((static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 100.0f + 40.0f) / 255.0f);
		vertices.push_back(VertexFormat(vertexPosition.x, vertexPosition.y, vertexPosition.z, 0, randGreen, 0, 1, randMovement));
	}
}

void Grass::CreateGrassBlade()
{
	std::vector<std::vector<VertexFormat> > vertices;
	std::vector<std::vector<unsigned int> > indices;

	for (int i = 0; i < _lodLevels; i++) {
		vertices.push_back(std::vector<VertexFormat>());
		indices.push_back(std::vector<unsigned int>());
	}
	//int zoneDimension = _hm->GetHeightMapSize().x;
	int zoneDimension = 256;
	int terrainX = zoneDimension * _terrainScaleFactor;
	int terrainZ = zoneDimension * _terrainScaleFactor;

	for (int i = 0; i < _lodLevels; i++) {
		for (int j = 0; j < _grassCount; j++)
		{
			float fOffsetX = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 256;
			float fOffsetZ = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 256;

			while (_coordX + fOffsetX > terrainX - _terrainScaleFactor) {
				fOffsetX = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 256;
			}

			while (_coordZ + fOffsetZ > terrainZ - _terrainScaleFactor) {
				fOffsetZ = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 256;
			}

			float fCoordX = _coordX + fOffsetX;
			float fCoordZ = _coordZ + fOffsetZ;

			float fCoordY = _hm->GetCoordHeight(fCoordX / _terrainScaleFactor, fCoordZ / _terrainScaleFactor);

			if (j % 3 == 0) {
				CreateFullBlade(Vector3(fCoordX, fCoordY, fCoordZ), vertices[i], i);
				ComputeFullBladeIndices(indices[i], i);
			}
			else {
				CreatePartialBlade(Vector3(fCoordX, fCoordY, fCoordZ), vertices[i], i);
				ComputePartialBladeIndices(indices[i], i);
			}
		}

		unsigned int gl_vertex_array_object, gl_vertex_buffer_object, gl_index_buffer_object;

		glGenVertexArrays(1, &gl_vertex_array_object);
		glBindVertexArray(gl_vertex_array_object);

		glGenBuffers(1, &gl_vertex_buffer_object);
		glBindBuffer(GL_ARRAY_BUFFER, gl_vertex_buffer_object);
		glBufferData(GL_ARRAY_BUFFER, vertices[i].size() * sizeof(VertexFormat), &vertices[i][0], GL_STATIC_DRAW);

		glGenBuffers(1, &gl_index_buffer_object);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_index_buffer_object);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices[i].size() * sizeof(unsigned int), &indices[i][0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(float) * 3));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(float) * 6));

		_grass_vao[i] = gl_vertex_array_object;
		_grass_vbo[i] = gl_vertex_buffer_object;
		_grass_ibo[i] = gl_index_buffer_object;
		_grass_indices[i] = indices[i].size();
	}
}

void Grass::CreateHighDetailGrass()
{
	std::vector<VertexFormat> vertices;

	Matrix4 matRotateY45, matRotateY45I, matTranslate;

	std::vector<unsigned int> indices;

	Matrix4 trans;
	int h = 0;
	//int fTerrainWidth = _hm->GetHeightMapSize().x;
	//int fTerrainHeight = _hm->GetHeightMapSize().y;
	int fTerrainWidth = 255;
	int fTerrainHeight = 255;

	float noGrassPerTile = _grassCount / (fTerrainWidth * fTerrainHeight);
	float crtGrassPieces = 0;

	for (size_t i = 0; i < _grassCount * 4; i += 4)
	{
		int coordX = rand() % fTerrainWidth;
		int coordZ = rand() % fTerrainHeight;
		int coordY = _hm->GetCoordHeight(coordX, coordZ);

		coordX *= 20 * 3;
		coordZ *= 20 * 3;

		float fCoordX = static_cast <float> (coordX) + static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 20.0f * 3.0f;
		float fCoordZ = static_cast <float> (coordZ) + static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 20.0f * 3.0f;

		h++;

		float rotateStep = rand() % 360;
		trans = Matrix4::Rotation(rotateStep, Vector3(0, 1, 0));


		/**************************************************************************************************
		************************************* FIRST BLADES OF GRASS **************************************/

		// first grass object
		Vector4 vRotation = trans * Vector4(0, 0, 0, 0);
		VertexFormat vertex1 = VertexFormat(fCoordX + vRotation.x, coordY + vRotation.y, fCoordZ + vRotation.z, 0, 1, 0, 1, 1);

		vRotation = trans * Vector4(_nGrassSize, 0, 0, 0);
		VertexFormat vertex2 = VertexFormat(fCoordX + vRotation.x + _nGrassSize, coordY + vRotation.y, fCoordZ + vRotation.z, 0, 1, 0, 0, 1);

		vRotation = trans * Vector4(_nGrassSize, _nGrassSize, 0, 0);
		VertexFormat vertex3 = VertexFormat(fCoordX + vRotation.x + _nGrassSize, coordY + vRotation.y + _nGrassSize, fCoordZ + vRotation.z, 0, 1, 0, 0, 0);

		vRotation = trans * Vector4(0, _nGrassSize, 0, 0);
		VertexFormat vertex4 = VertexFormat(fCoordX + vRotation.x, coordY + _nGrassSize + vRotation.y, fCoordZ + vRotation.z, 0, 1, 0, 1, 0);

		/********************************************************************************************************
		****************************************** BUILD MESH **************************************************/
		vertices.push_back(vertex1);  vertices.push_back(vertex2); vertices.push_back(vertex3); vertices.push_back(vertex4);

		indices.push_back(i + 0); indices.push_back(i + 1); indices.push_back(i + 2);
		indices.push_back(i + 2); indices.push_back(i + 3);  indices.push_back(i + 0);
	}

	unsigned int gl_vertex_array_object, gl_vertex_buffer_object, gl_index_buffer_object;

	//vertex array object
	glGenVertexArrays(1, &gl_vertex_array_object);
	glBindVertexArray(gl_vertex_array_object);

	//vertex buffer object
	glGenBuffers(1, &gl_vertex_buffer_object);
	glBindBuffer(GL_ARRAY_BUFFER, gl_vertex_buffer_object);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexFormat), &vertices[0], GL_STATIC_DRAW);

	//index buffer object
	glGenBuffers(1, &gl_index_buffer_object);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_index_buffer_object);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	//legatura intre atributele vertecsilor si pipeline, datele noastre sunt INTERLEAVED.
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)0);					
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(float) * 3));	
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(float) * 6));	

	/*
	_vao[ = gl_vertex_array_object;
	_vbo = gl_vertex_buffer_object;
	_ibo = gl_index_buffer_object;
	_indicesCount = indices.size();
	*/
}

void Grass::CreateGrass()
{
	std::vector<VertexFormat> vertices;
	Grass tool = Grass();

	float fCenterX;
	float fCenterY;
	float fCenterZ;
	float fRotateAngle = M_PI / 4;

	Matrix4 matRotateY45, matRotateY45I, matTranslate;

	std::vector<unsigned int> indices;

	std::vector<std::vector<float> > matGrassObject;

	float t[16] = {
		std::cos(fRotateAngle), 0.0f, -std::sin(fRotateAngle), 0.0f,
		0.0f, 1, 0.0f, 0.0f,
		-std::sin(fRotateAngle), 0, std::cos(fRotateAngle), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	matRotateY45 = Matrix4(t);
	float tt[16] = {
		std::cos(-fRotateAngle), 0.0f, -std::sin(-fRotateAngle), 0.0f,
			0.0f, 1, 0.0f, 0.0f,
			-std::sin(-fRotateAngle), 0, std::cos(-fRotateAngle), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
	};
	matRotateY45I = Matrix4(tt);

	int h = 0;
	for (size_t i = 0; i < 3 * _grassCount * 4; i += 12)
	{
		float fX = _vecPositions[h].x;
		float fY = _vecPositions[h].y;
		float fZ = _vecPositions[h].z;
		h++;
		fCenterX = fCenterY = fCenterZ = _nGrassSize / 2;


		/**************************************************************************************************
		 ************************************* FIRST BLADES OF GRASS **************************************/

		 // first grass object
		VertexFormat vertex11 = VertexFormat(fX, fY, fZ, 0, 1, 0, 1, 1);
		VertexFormat vertex12 = VertexFormat(fX + _nGrassSize, fY, fZ, 0, 1, 0, 0, 1);
		VertexFormat vertex13 = VertexFormat(fX + _nGrassSize, fY + _nGrassSize, fZ, 0, 1, 0, 0, 0);
		VertexFormat vertex14 = VertexFormat(fX, fY + _nGrassSize, fZ, 0, 1, 0, 1, 0);

		/**************************************************************************************************
		************************************* SECOND BLADES OF GRASS **************************************/

		// second grass object initial position
		Vector4 vecVertex145 = Vector4(-fCenterX, -fCenterY, 0, 1);
		Vector4 vecVertex245 = Vector4(_nGrassSize - fCenterX, -fCenterY, 0, 1);
		Vector4 vecVertex345 = Vector4(_nGrassSize - fCenterX, _nGrassSize - fCenterY, 0, 1);
		Vector4 vecVertex445 = Vector4(-fCenterX, _nGrassSize - fCenterY, 0, 1);

		// rotate second grass object 45 degrees
		vecVertex145 = tool.V4xM4(vecVertex145, matRotateY45);
		vecVertex245 = tool.V4xM4(vecVertex245, matRotateY45);
		vecVertex345 = tool.V4xM4(vecVertex345, matRotateY45);
		vecVertex445 = tool.V4xM4(vecVertex445, matRotateY45);

		// second grass object
		VertexFormat vertex21 = VertexFormat(fX + vecVertex145.x + fCenterX, fY + vecVertex145.y + fCenterY, fZ + vecVertex145.z, 0, 1, 0, 1, 1);
		VertexFormat vertex22 = VertexFormat(fX + vecVertex245.x + fCenterX, fY + vecVertex245.y + fCenterY, fZ + vecVertex245.z, 0, 1, 0, 0, 1);
		VertexFormat vertex23 = VertexFormat(fX + vecVertex345.x + fCenterX, fY + vecVertex345.y + fCenterY, fZ + vecVertex345.z, 0, 1, 0, 0, 0);
		VertexFormat vertex24 = VertexFormat(fX + vecVertex445.x + fCenterX, fY + vecVertex445.y + fCenterY, fZ + vecVertex445.z, 0, 1, 0, 1, 0);


		/**************************************************************************************************
		************************************* THIRD BLADES OF GRASS ***************************************/

		// third grass object initial position
		Vector4 vecVertex145I = Vector4(-fCenterX, -fCenterY, 0, 1);
		Vector4 vecVertex245I = Vector4(_nGrassSize - fCenterX, -fCenterY, 0, 1);
		Vector4 vecVertex345I = Vector4(_nGrassSize - fCenterX, _nGrassSize - fCenterY, 0, 1);
		Vector4 vecVertex445I = Vector4(-fCenterX, _nGrassSize - fCenterY, 0, 1);

		// rotate third grass object -45 degrees
		vecVertex145I = tool.V4xM4(vecVertex145I, matRotateY45I);
		vecVertex245I = tool.V4xM4(vecVertex245I, matRotateY45I);
		vecVertex345I = tool.V4xM4(vecVertex345I, matRotateY45I);
		vecVertex445I = tool.V4xM4(vecVertex445I, matRotateY45I); 

		// third grass object
		VertexFormat vertex31 = VertexFormat(fX + vecVertex145I.x + fCenterX, fY + vecVertex145I.y + fCenterY, fZ + vecVertex145I.z, 0, 1, 0, 1, 1);
		VertexFormat vertex32 = VertexFormat(fX + vecVertex245I.x + fCenterX, fY + vecVertex245I.y + fCenterY, fZ + vecVertex245I.z, 0, 1, 0, 0, 1);
		VertexFormat vertex33 = VertexFormat(fX + vecVertex345I.x + fCenterX, fY + vecVertex345I.y + fCenterY, fZ + vecVertex345I.z, 0, 1, 0, 0, 0);
		VertexFormat vertex34 = VertexFormat(fX + vecVertex445I.x + fCenterX, fY + vecVertex445I.y + fCenterY, fZ + vecVertex445I.z, 0, 1, 0, 1, 0);

		/********************************************************************************************************
		 ****************************************** BUILD MESH **************************************************/
		vertices.push_back(vertex11);  vertices.push_back(vertex12); vertices.push_back(vertex13); vertices.push_back(vertex14);

		indices.push_back(i + 0); indices.push_back(i + 1); indices.push_back(i + 2);
		indices.push_back(i + 2); indices.push_back(i + 3);  indices.push_back(i + 0);

		vertices.push_back(vertex21);  vertices.push_back(vertex22); vertices.push_back(vertex23); vertices.push_back(vertex24);

		indices.push_back(i + 4); indices.push_back(i + 5); indices.push_back(i + 6);
		indices.push_back(i + 6); indices.push_back(i + 7);  indices.push_back(i + 4);

		vertices.push_back(vertex31);  vertices.push_back(vertex32); vertices.push_back(vertex33); vertices.push_back(vertex34);

		indices.push_back(i + 8); indices.push_back(i + 9); indices.push_back(i + 10);
		indices.push_back(i + 10); indices.push_back(i + 11);  indices.push_back(i + 8);

	}

	unsigned int gl_vertex_array_object, gl_vertex_buffer_object, gl_index_buffer_object;

	//vertex array object
	glGenVertexArrays(1, &gl_vertex_array_object);
	glBindVertexArray(gl_vertex_array_object);

	//vertex buffer object 
	glGenBuffers(1, &gl_vertex_buffer_object);
	glBindBuffer(GL_ARRAY_BUFFER, gl_vertex_buffer_object);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexFormat), &vertices[0], GL_STATIC_DRAW);

	//index buffer object 
	glGenBuffers(1, &gl_index_buffer_object);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl_index_buffer_object);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)0);					
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(float) * 3));	
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(float) * 6));	

	/*
	_vao = gl_vertex_array_object;
	_vbo = gl_vertex_buffer_object;
	_ibo = gl_index_buffer_object;
	_indicesCount = indices.size();
	*/
}

void Grass::DrawGrass(int lodLevel)
{
	if (lodLevel != INVISIBLE) {
		glBindVertexArray(_grass_vao[lodLevel]);
		glDrawElements(GL_TRIANGLES, _grass_indices[lodLevel], GL_UNSIGNED_INT, 0);
	}

}

Vector3 Grass::GetStartPosition()
{
	return _startPosition;
}

int Grass::SelectGrassLod(Vector3 eyePosition, Vector3 treePosition, float grassRadius) {
	float distance = Grass().Distance(eyePosition, treePosition);
	//std::cout << distance << std::endl;
	if (distance < grassRadius) {
		return FULLLOD;
	}
	else if (distance < grassRadius * 5) {
		return PARTIALLOD;
	}
	else if (distance < grassRadius * 6) {
		return LOWLOD;
	}
	else {
		return INVISIBLE;
	}
}
