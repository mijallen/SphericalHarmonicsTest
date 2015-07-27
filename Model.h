#ifndef _MODEL_H_
#define _MODEL_H_

#include <string>

class Model {
    float* vertexPointer;
    float* normalPointer;
    unsigned short* indexPointer;

  // based on lib3ds, so maximum number of vertices and triangles is 2^16

    unsigned short triangleCount;
    unsigned short vertexCount;

public:
    Model();
    Model(const std::string& filePath);

    ~Model();

    void loadFromFile(const std::string& filePath);

    const float* getVertexPointer() const;
    const float* getNormalPointer() const;
    const unsigned short* getIndexPointer() const;

    unsigned short getTriangleCount() const;
    unsigned short getVertexCount() const;
};

#endif
