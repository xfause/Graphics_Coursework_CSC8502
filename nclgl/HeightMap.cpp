#include "HeightMap.h"
#include <iostream>

HeightMap::HeightMap(const std::string& name) {
	int iWidth, iHeight, iChans;
	unsigned char* data = SOIL_load_image(name.c_str(), &iWidth, &iHeight, &iChans, 1);
	if (!data) {
		std::cout << "Heightmap cant load file\n";
		return;
	}
	_data = data;
	_PicWidth = iWidth;
	_PicHeight = iHeight;
	numVertices = iWidth * iHeight;
	numIndices = (iWidth - 1) * (iHeight - 1) * 6;
	vertices = new Vector3[numVertices];
	textureCoords = new Vector2[numVertices];
	indices = new GLuint[numIndices];

	Vector3 vertexScale = Vector3(16.0f, 1.0f, 16.0f);
	Vector2 textureScale = Vector2(1 / 16.0f, 1 / 16.0f);

	for (int z = 0; z < iHeight; z++) {
		for (int x = 0; x < iWidth; x++) {
			int offset = (z * iWidth) + x;
			vertices[offset] = Vector3(x, data[offset], z) * vertexScale;
			textureCoords[offset] = Vector2(x, z) * textureScale;
		}
	}

	//SOIL_free_image_data(data);

	int i = 0;
	for (int z = 0; z < iHeight - 1; z++) {
		for (int x = 0; x < iWidth - 1; x++) {
			int a = (z * iWidth) + x;
			int b = (z * iWidth) + x + 1;
			int c = ((z + 1) * iWidth) + x + 1;
			int d = ((z + 1) * iWidth) + x;

			indices[i++] = a;
			indices[i++] = d;
			indices[i++] = c;

			indices[i++] = a;
			indices[i++] = c;
			indices[i++] = b;
		}
	}
	GenerateNormals();
	GenerateTangents();

	BufferData();

	heightMapSize.x = vertexScale.x * (iWidth - 1);
	heightMapSize.y = vertexScale.y * 255.0f;
	heightMapSize.z = vertexScale.z * (iHeight - 1);
}

float HeightMap::GetCoordHeight(float x, float z) {
	int offset = ((int)z * (_PicWidth)) + (int)x;
	return (float)_data[offset] ;
}