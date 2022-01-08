#pragma once
#include <string>
#include "Mesh.h"

class HeightMap :public Mesh {
public:
	HeightMap(const std::string& name);
	~HeightMap(void) {
		delete _data;
	};

	Vector3 GetHeightMapSize() const { return heightMapSize; }
	float GetCoordHeight(float x, float y);

	unsigned char*  _data;
	int _PicWidth;
	int _PicHeight;

protected:
	Vector3 heightMapSize;
};
