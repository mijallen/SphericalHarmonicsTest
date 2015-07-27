#include "Model.h"
#include "Cubemap.h"

#include <iostream>
#include <string>

#include <SFML/Graphics.hpp>
#include <GL/gl.h>
#include <GL/glu.h>

void setup() {
    glClearColor(0.f, 0.f, 0.5f, 1.f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

  // wide perspective to see more of cubemap
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(120, 1.333, 1, 1000);

    glMatrixMode(GL_MODELVIEW);
}

// global variable to hold vertex colors of model
float* modelColors;

void drawModel(const Model& model) {
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, model.getVertexPointer());

    glEnableClientState(GL_NORMAL_ARRAY);
    glNormalPointer(GL_FLOAT, 0, model.getNormalPointer());

    glEnableClientState(GL_COLOR_ARRAY);
    glColorPointer(3, GL_FLOAT, 0, modelColors);

    glDrawElements(GL_TRIANGLES, 3 * model.getTriangleCount(), GL_UNSIGNED_SHORT, model.getIndexPointer());

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

void drawCubemap(const Cubemap& cubemap) {
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, Cubemap::getVertexPointer());

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glTexCoordPointer(2, GL_FLOAT, 0, Cubemap::getTexCoordPointer());

    sf::Texture::bind(cubemap.getNegativeXTexturePointer());
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, Cubemap::getNegativeXIndexPointer());

    sf::Texture::bind(cubemap.getPositiveXTexturePointer());
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, Cubemap::getPositiveXIndexPointer());

    sf::Texture::bind(cubemap.getNegativeYTexturePointer());
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, Cubemap::getNegativeYIndexPointer());

    sf::Texture::bind(cubemap.getPositiveYTexturePointer());
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, Cubemap::getPositiveYIndexPointer());

  // negative Z and positive Z sometimes get flipped (OpenGL vs. DirectX)
  // the current solution is to just rename positiveZ.png to negativeZ.png and vice versa

    sf::Texture::bind(cubemap.getNegativeZTexturePointer());
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, Cubemap::getNegativeZIndexPointer());

    sf::Texture::bind(cubemap.getPositiveZTexturePointer());
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, Cubemap::getPositiveZIndexPointer());

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

float angle = 0.f;

int main() {
  // still need to create 9 coefficients per vertex-normal
    Model testModel("Vase.3ds");

    sf::RenderWindow window(sf::VideoMode(800, 600), "Spherical Harmonics Test");
    window.setFramerateLimit(60);

    modelColors = new float[3 * testModel.getVertexCount()];

  // provide a directory containing images named negativeX.png, positiveY.png, etc.
    Cubemap testCubemap("gradientCube");

  // currently just samples cubemap based on normal and puts colors on model
  // for spherical harmonics, end result will be 9 coefficients representing cubemap
    for (int iter = 0; iter < testModel.getVertexCount(); iter++) {
        sf::Vector3f texCoords;
        texCoords.x = testModel.getNormalPointer()[3 * iter + 0];
        texCoords.y = testModel.getNormalPointer()[3 * iter + 1];
        texCoords.z = testModel.getNormalPointer()[3 * iter + 2];

        sf::Vector3f sampleColor = testCubemap.getColorFromTexCoords(texCoords);
        modelColors[3 * iter + 0] = sampleColor.x;
        modelColors[3 * iter + 1] = sampleColor.y;
        modelColors[3 * iter + 2] = sampleColor.z;
    }
/*
    // software texture test
    SoftwareTextureSFML texture("pngtest.png");
    sf::Vector3f sample = texture.getColorFromTexCoords(sf::Vector2f(0.5f, 0.5f));
    std::cout << "red = " << sample.x << ", green = " << sample.y << ", blue = " << sample.z << std::endl;
*/
    setup();

    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, 0);

        glLoadIdentity();
        glTranslatef(0.f, 0.f, -2.f);
        glRotatef(30.f, 1.f, 0.f, 0.f);
        glRotatef(angle, 0.f, 1.f, 0.f);
        glScalef(0.1f, 0.1f, 0.1f);

        drawModel(testModel);

  // cubemap is "infinitely far away", no translation in modelview matrix
  // also, its faces just looks weird at an angle

        glLoadIdentity();
        glRotatef(30.f, 1.f, 0.f, 0.f);
        glRotatef(angle, 0.f, 1.f, 0.f); angle += 1.f;
        glScalef(5.f, 5.f, 5.f);

        drawCubemap(testCubemap);

        window.display();
    }

    return 0;
}
