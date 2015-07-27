#include "Model.h"

#include <lib3ds.h>
#include <cstdlib>

Model::Model():
    vertexPointer(NULL),
    normalPointer(NULL),
    indexPointer(NULL),
    triangleCount(0),
    vertexCount(0)
{
}

Model::Model(const std::string& filePath) {
    this->loadFromFile(filePath);
}

Model::~Model() {
    if (this->vertexCount > 0) {
        if (this->vertexPointer != NULL) delete[] this->vertexPointer;
        if (this->normalPointer != NULL) delete[] this->normalPointer;
    }

    if (this->triangleCount > 0) {
        if (this->indexPointer != NULL) delete[] this->indexPointer;
    }
}

void Model::loadFromFile(const std::string& filePath) {
    Lib3dsFile* file = lib3ds_file_open(filePath.c_str());
    Lib3dsMesh* mesh = file->meshes[0];

    this->vertexCount = mesh->nvertices;
    this->vertexPointer = new float[3 * this->vertexCount];

    for (int iter = 0; iter < this->vertexCount; ++iter) {
        this->vertexPointer[3 * iter + 0] = mesh->vertices[iter][0];
        this->vertexPointer[3 * iter + 1] = mesh->vertices[iter][1];
        this->vertexPointer[3 * iter + 2] = mesh->vertices[iter][2];
    }

    this->triangleCount = mesh->nfaces;
    this->indexPointer = new unsigned short[3 * this->triangleCount];

    this->normalPointer = new float[3 * this->vertexCount];
    float (*faceNormals)[3] = (float(*)[3])malloc(3 * 3 * triangleCount * sizeof(float)); // yikes!

    lib3ds_mesh_calculate_vertex_normals(mesh, faceNormals);

    for (int iter = 0; iter < this->triangleCount; ++iter) {
        this->indexPointer[3 * iter + 0] = mesh->faces[iter].index[0];
        this->indexPointer[3 * iter + 1] = mesh->faces[iter].index[1];
        this->indexPointer[3 * iter + 2] = mesh->faces[iter].index[2];
    }

  // convert per-triangle normals to per-vertex normals
  // can lose some normals if model is not entirely smooth

    for (int iter = 0; iter < this->triangleCount; ++iter) {
        int indexA = this->indexPointer[3 * iter + 0];
        int indexB = this->indexPointer[3 * iter + 1];
        int indexC = this->indexPointer[3 * iter + 2];

        for (int dim = 0; dim < 3; ++dim) {
            this->normalPointer[3 * indexA + dim] = faceNormals[3 * iter + 0][dim];
            this->normalPointer[3 * indexB + dim] = faceNormals[3 * iter + 1][dim];
            this->normalPointer[3 * indexC + dim] = faceNormals[3 * iter + 2][dim];
        }
    }

    free(faceNormals);

    lib3ds_file_free(file);
}

const float* Model::getVertexPointer() const {
    return this->vertexPointer;
}

const float* Model::getNormalPointer() const {
    return this->normalPointer;
}

const unsigned short* Model::getIndexPointer() const {
    return this->indexPointer;
}

unsigned short Model::getTriangleCount() const {
    return this->triangleCount;
}

unsigned short Model::getVertexCount() const {
    return this->vertexCount;
}
