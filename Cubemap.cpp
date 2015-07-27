#include "Cubemap.h"

#include <cmath>

const float Cubemap::texCoordPointer[48] = {
  // negative X face
    0.f, 0.f,
    0.f, 1.f,
    1.f, 1.f,
    1.f, 0.f,
  // positive X face
    0.f, 0.f,
    0.f, 1.f,
    1.f, 1.f,
    1.f, 0.f,
  // negative Y face
    0.f, 0.f,
    0.f, 1.f,
    1.f, 1.f,
    1.f, 0.f,
  // positive Y face
    0.f, 0.f,
    0.f, 1.f,
    1.f, 1.f,
    1.f, 0.f,
  // negative Z face
    0.f, 0.f,
    0.f, 1.f,
    1.f, 1.f,
    1.f, 0.f,
  // positive Z face
    0.f, 0.f,
    0.f, 1.f,
    1.f, 1.f,
    1.f, 0.f
};

const float Cubemap::vertexPointer[72] = {
  // negative X face
    -1.f, +1.f, +1.f,
    -1.f, -1.f, +1.f,
    -1.f, -1.f, -1.f,
    -1.f, +1.f, -1.f,
  // positive X face
    +1.f, +1.f, -1.f,
    +1.f, -1.f, -1.f,
    +1.f, -1.f, +1.f,
    +1.f, +1.f, +1.f,
  // negative Y face
    -1.f, -1.f, -1.f,
    -1.f, -1.f, +1.f,
    +1.f, -1.f, +1.f,
    +1.f, -1.f, -1.f,
  // positive Y face
    -1.f, +1.f, +1.f,
    -1.f, +1.f, -1.f,
    +1.f, +1.f, -1.f,
    +1.f, +1.f, +1.f,
  // negative Z face
    -1.f, +1.f, -1.f,
    -1.f, -1.f, -1.f,
    +1.f, -1.f, -1.f,
    +1.f, +1.f, -1.f,
  // positive Z face
    +1.f, +1.f, +1.f,
    +1.f, -1.f, +1.f,
    -1.f, -1.f, +1.f,
    -1.f, +1.f, +1.f
};

// indices based on use of GL_QUADS
// for GL_TRIANGLES: array size -> 6, sequence 0 1 2 3 -> 0 1 2 0 2 3

const unsigned int Cubemap::negativeXIndexPointer[4] = {
    0, 1, 2, 3
};

const unsigned int Cubemap::positiveXIndexPointer[4] = {
    4, 5, 6, 7
};

const unsigned int Cubemap::negativeYIndexPointer[4] = {
    8, 9, 10, 11
};

const unsigned int Cubemap::positiveYIndexPointer[4] = {
    12, 13, 14, 15
};

const unsigned int Cubemap::negativeZIndexPointer[4] = {
    16, 17, 18, 19
};

const unsigned int Cubemap::positiveZIndexPointer[4] = {
    20, 21, 22, 23
};

Cubemap::Cubemap() {
}

// might want to add loading functions for individual faces

Cubemap::Cubemap(const std::string& directory):
    negativeX(directory + "/negativeX.png"),
    positiveX(directory + "/positiveX.png"),
    negativeY(directory + "/negativeY.png"),
    positiveY(directory + "/positiveY.png"),
    negativeZ(directory + "/negativeZ.png"),
    positiveZ(directory + "/positiveZ.png")
{
}

const sf::Texture* Cubemap::getNegativeXTexturePointer() const {
    return this->negativeX.getTexturePointer();
}

const sf::Texture* Cubemap::getPositiveXTexturePointer() const {
    return this->positiveX.getTexturePointer();
}

const sf::Texture* Cubemap::getNegativeYTexturePointer() const {
    return this->negativeY.getTexturePointer();
}

const sf::Texture* Cubemap::getPositiveYTexturePointer() const {
    return this->positiveY.getTexturePointer();
}

const sf::Texture* Cubemap::getNegativeZTexturePointer() const {
    return this->negativeZ.getTexturePointer();
}

const sf::Texture* Cubemap::getPositiveZTexturePointer() const {
    return this->positiveZ.getTexturePointer();
}

const float* Cubemap::getTexCoordPointer() {
    return Cubemap::texCoordPointer;
}

const float* Cubemap::getVertexPointer() {
    return Cubemap::vertexPointer;
}

const unsigned int* Cubemap::getNegativeXIndexPointer() {
    return Cubemap::negativeXIndexPointer;
}

const unsigned int* Cubemap::getPositiveXIndexPointer() {
    return Cubemap::positiveXIndexPointer;
}

const unsigned int* Cubemap::getNegativeYIndexPointer() {
    return Cubemap::negativeYIndexPointer;
}

const unsigned int* Cubemap::getPositiveYIndexPointer() {
    return Cubemap::positiveYIndexPointer;
}

const unsigned int* Cubemap::getNegativeZIndexPointer() {
    return Cubemap::negativeZIndexPointer;
}

const unsigned int* Cubemap::getPositiveZIndexPointer() {
    return Cubemap::positiveZIndexPointer;
}

sf::Vector3f Cubemap::getColorFromTexCoords(const sf::Vector3f& texCoords) {
    sf::Vector3f finalColor;

    float maximum = fabs(texCoords.x);
    if (fabs(texCoords.y) > maximum) maximum = fabs(texCoords.y);
    if (fabs(texCoords.z) > maximum) maximum = fabs(texCoords.z);

    if (maximum == fabs(texCoords.x)) {
        sf::Vector2f faceTexCoords(texCoords.z, -texCoords.y);
        faceTexCoords /= (float)fabs(texCoords.x);

        if (texCoords.x < 0.f) faceTexCoords.x = -faceTexCoords.x;

        faceTexCoords = 0.5f * faceTexCoords + sf::Vector2f(0.5f, 0.5f);

        if (texCoords.x < 0.f)
            finalColor = this->negativeX.getColorFromTexCoords(faceTexCoords);
        else
            finalColor = this->positiveX.getColorFromTexCoords(faceTexCoords);
    }
    else if (maximum == fabs(texCoords.y)) {
        sf::Vector2f faceTexCoords(texCoords.x, -texCoords.z);
        faceTexCoords /= (float)fabs(texCoords.y);

        if (texCoords.y < 0.f) faceTexCoords.y = -faceTexCoords.y;

        faceTexCoords = 0.5f * faceTexCoords + sf::Vector2f(0.5f, 0.5f);

        if (texCoords.y < 0.f)
            finalColor = this->negativeY.getColorFromTexCoords(faceTexCoords);
        else
            finalColor = this->positiveY.getColorFromTexCoords(faceTexCoords);
    }
    else if (maximum == fabs(texCoords.z)) {
        sf::Vector2f faceTexCoords(texCoords.x, -texCoords.y);
        faceTexCoords /= (float)fabs(texCoords.z);

        if (texCoords.z > 0.f) faceTexCoords.x = -faceTexCoords.x;

        faceTexCoords = 0.5f * faceTexCoords + sf::Vector2f(0.5f, 0.5f);

        if (texCoords.z < 0.f)
            finalColor = this->negativeZ.getColorFromTexCoords(faceTexCoords);
        else
            finalColor = this->positiveZ.getColorFromTexCoords(faceTexCoords);
    }

    return finalColor;
}
