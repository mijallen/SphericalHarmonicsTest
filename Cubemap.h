#ifndef _CUBEMAP_H_
#define _CUBEMAP_H_

#include "SoftwareTextureSFML.h"

class Cubemap {
    SoftwareTextureSFML negativeX;
    SoftwareTextureSFML positiveX;
    SoftwareTextureSFML negativeY;
    SoftwareTextureSFML positiveY;
    SoftwareTextureSFML negativeZ;
    SoftwareTextureSFML positiveZ;

    static const float texCoordPointer[48];
    static const float vertexPointer[72];

    static const unsigned int negativeXIndexPointer[4];
    static const unsigned int positiveXIndexPointer[4];
    static const unsigned int negativeYIndexPointer[4];
    static const unsigned int positiveYIndexPointer[4];
    static const unsigned int negativeZIndexPointer[4];
    static const unsigned int positiveZIndexPointer[4];

public:
    Cubemap();
    Cubemap(const std::string& directory);

    const sf::Texture* getNegativeXTexturePointer() const;
    const sf::Texture* getPositiveXTexturePointer() const;
    const sf::Texture* getNegativeYTexturePointer() const;
    const sf::Texture* getPositiveYTexturePointer() const;
    const sf::Texture* getNegativeZTexturePointer() const;
    const sf::Texture* getPositiveZTexturePointer() const;

    static const float* getTexCoordPointer();
    static const float* getVertexPointer();

    static const unsigned int* getNegativeXIndexPointer();
    static const unsigned int* getPositiveXIndexPointer();
    static const unsigned int* getNegativeYIndexPointer();
    static const unsigned int* getPositiveYIndexPointer();
    static const unsigned int* getNegativeZIndexPointer();
    static const unsigned int* getPositiveZIndexPointer();

  // should this be a const function?
    sf::Vector3f getColorFromTexCoords(const sf::Vector3f& texCoords);
};

#endif
