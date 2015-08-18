#ifndef _SOFTWARETEXTURESFML_H_
#define _SOFTWARETEXTURESFML_H_

#include <string>

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

class SoftwareTextureSFML {
    sf::Image image;
    sf::Texture texture;

public:
    SoftwareTextureSFML();
    SoftwareTextureSFML(const std::string& filePath);

    const sf::Texture* getTexturePointer() const;

  // should this be a const function?
    sf::Vector3f getColorFromTexCoords(const sf::Vector2f& texCoords) const;
};

#endif
