#include "Water.h"
#include "../nclgl/OGLRenderer.h"

std::vector<GLuint> BetterWater::init_indices(int size)
{
    auto indices = std::vector<GLuint>();

    for (int z = 0; z < size - 1; ++z)
    {
        for (int x = 0; x < size - 1; ++x)
        {
            int start = x + z * size;
            indices.push_back(start);
            indices.push_back(start + 1);
            indices.push_back(start + size);
            indices.push_back(start + 1);
            indices.push_back(start + 1 + size);
            indices.push_back(start + size);
        }
    }

    return indices;
}

std::vector<float> BetterWater::init_plane(int size, float width, float height)
{
    auto vertices = std::vector<float>();

    float start = -(size / 2) * width;

    for (int x = 0; x < size; x++)
    {
        float pos_x = start + x * width;
        for (int z = 0; z < size; z++)
        {
            float pos_z = -(start + z * width);
            vertices.push_back(pos_x);
            vertices.push_back(height);
            vertices.push_back(pos_z);

            // Texture coordinates
            vertices.push_back(pos_x);
            vertices.push_back(pos_z);
        }
    }
    return vertices;
}

Mesh* BetterWater::load_object(std::vector<float>& vertices, std::vector<GLuint>& indices)
{
    Mesh* m = new Mesh();
    int numVertices = vertices.size() / 5;
    int numIndices = indices.size();
    m->InitVertices(numVertices);
    m->InitTexcoords(numVertices);
    m->InitIndices(numIndices);
    m->SetDrawType(GL_TRIANGLES);

    for (int i = 0; i < numVertices; i++) {
        float x = vertices[i*5], y = vertices[i * 5 + 1], z = vertices[i * 5 + 2];
        m->SetVertices(i, Vector3(x, y, z));
        float tx = vertices[i * 5 + 3], ty = vertices[i * 5 + 4];
        m->SetTexcoords(i, Vector2(tx, ty));
    }
    for (int i = 0; i < numIndices; i ++) {
        m->SetIndices(i, indices[i]);
    }
    m->GenerateNormals();
    m->GenerateTangents();

    m->BufferData();
    return m;
}
