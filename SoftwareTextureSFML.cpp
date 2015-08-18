#include "SoftwareTextureSFML.h"

SoftwareTextureSFML::SoftwareTextureSFML() {
}

// might want to add loading function for post-constructor calling

SoftwareTextureSFML::SoftwareTextureSFML(const std::string& filePath) {
    this->image.loadFromFile(filePath);
    this->texture.loadFromImage(image);
}

const sf::Texture* SoftwareTextureSFML::getTexturePointer() const {
    return &this->texture;
}

// need to add texture wrapping and alternative interpolation methods

sf::Vector3f SoftwareTextureSFML::getColorFromTexCoords(const sf::Vector2f& texCoords) const {
  // currently no texture wrapping, so texCoords beyond [0,1]x[0,1] -> bad
    sf::Vector2f normalizedTexCoords;
    normalizedTexCoords.x = texCoords.x;
    normalizedTexCoords.y = texCoords.y;

    sf::Vector2u dimensions = this->image.getSize();
    float imageX = (float)(dimensions.x - 1) * normalizedTexCoords.x;
    float imageY = (float)(dimensions.y - 1) * normalizedTexCoords.y;

  // currently using nearest neighbor for pixel color, want bilinear and bicubic
    unsigned int sampleX = (unsigned int)imageX;
    unsigned int sampleY = (unsigned int)imageY;

    sf::Color sample = this->image.getPixel(sampleX, sampleY);

    sf::Vector3f finalColor;
    finalColor.x = (float)sample.r / 255.f;
    finalColor.y = (float)sample.g / 255.f;
    finalColor.z = (float)sample.b / 255.f;

    return finalColor;
}
