/******************************************************************************
Class:Mesh
Implements:
Author:Rich Davison	 <richard-gordon.davison@newcastle.ac.uk>
Description:Wrapper around OpenGL primitives, geometry and related 
OGL functions.

There's a couple of extra functions in here that you didn't get in the tutorial
series, to draw debug normals and tangents. 


-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "OGLRenderer.h"
#include <vector>
#include <string>

//A handy enumerator, to determine which member of the bufferObject array
//holds which data
enum MeshBuffer {
	VERTEX_BUFFER	,
	COLOUR_BUFFER	, 
	TEXTURE_BUFFER	,
	NORMAL_BUFFER	, 
	TANGENT_BUFFER	,

	WEIGHTVALUE_BUFFER,		//new this year, weight values of vertices
	WEIGHTINDEX_BUFFER,	//new this year, indices of weights

	INDEX_BUFFER	,

	MAX_BUFFER
};

class Mesh	{
public:	
	struct SubMesh {
		int start;
		int count;
	};

	Mesh(void);
	~Mesh(void);

	void Draw();
	void DrawSubMesh(int i);

	static Mesh* LoadFromMeshFile(const std::string& name);

	unsigned int GetTriCount() const {
		int primCount = indices ? numIndices : numVertices;
		return primCount / 3;
	}

	unsigned int GetJointCount() const {
		return (unsigned int)jointNames.size();
	}

	int GetIndexForJoint(const std::string& name) const;
	int GetParentForJoint(const std::string& name) const;
	int GetParentForJoint(int i) const;

	const Matrix4* GetBindPose() const {
		return bindPose;
	}

	const Matrix4* GetInverseBindPose() const {
		return inverseBindPose;
	}

	int		GetSubMeshCount() const {
		return (int)meshLayers.size(); 
	}

	bool GetSubMesh(int i, const SubMesh* s) const;
	bool GetSubMesh(const std::string& name, const SubMesh* s) const;

	static Mesh* GenerateTriangle();
	static Mesh* GenerateQuad();
	static Mesh* GenerateObject(std::vector<Vector3>& vertices, std::vector<Vector2>& uvs, Vector3& objSize);
	Vector3 ModelSize;

	void GenerateNormals();
	bool GetVertexIndicesForTri(unsigned int i, unsigned int& a, unsigned int& b, unsigned int& c) const;

	void GenerateTangents();
	Vector4 GenerateTangent(int a, int b, int c);

	void	BufferData();

	void InitVertices(int nums) {
		numVertices = nums;
		vertices = new Vector3[nums];
	}

	void InitTexcoords(int nums) {
		textureCoords = new Vector2[nums];
	}

	void InitIndices(int nums) {
		numIndices = nums;
		indices = new GLuint[nums];
	}

	void SetDrawType(GLuint t) {
		type = t;
	}

	void SetVertices(int index, Vector3 val) { vertices[index] = val; }
	void SetTexcoords(int index, Vector2 val) { textureCoords[index] = val; }
	void SetIndices(int index, GLuint val) { indices[index] = val;}

protected:

	GLuint	arrayObject;

	GLuint	bufferObject[MAX_BUFFER];

	GLuint	numVertices;
	GLuint	numIndices;
	
	GLuint	type;

	Vector3*		vertices;
	Vector4*		colours;
	Vector2*		textureCoords;
	Vector3*		normals;
	Vector4*		tangents;

	Vector4*		weights;
	int*			weightIndices;

	unsigned int*	indices;

	Matrix4* bindPose;
	Matrix4* inverseBindPose;

	std::vector<std::string>	jointNames;
	std::vector<int>			jointParents;
	std::vector< SubMesh>		meshLayers;
	std::vector<std::string>	layerNames;
};

