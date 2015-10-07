#include "Model.h"
#include "Cubemap.h"
#include "SphericalFunction.h"
#include "SphericalHarmonics.h"

#include <iostream>
#include <string>
#include <vector>

#include <SFML/Graphics.hpp>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>

#include <pthread.h>

  /*
     Proposed improvements:
      -the SoftwareTextureSFML class can have textures that activate after the OpenGL window is made
        this would allow preprocessing to occur without an unresponsive window (for cubemap calculations)
      -a progress bar could display the progress of preprocessing calculations
        the user would know what's going on
      -possible streaming of results by having additional thread for just drawing
      -caching results in a file format to prevent redundant preprocessing
        need a way to determine whether cached results are valid or not
  */

// additional Vector3f functions

float dotProduct(const sf::Vector3f& vectorA, const sf::Vector3f& vectorB) {
    return (vectorA.x * vectorB.x + vectorA.y * vectorB.y + vectorA.z * vectorB.z);
}

std::ostream& operator<<(std::ostream& out, const sf::Vector3f& vector) {
    out << "<" << vector.x << "," << vector.y << "," << vector.z << ">";
    return out;
}

// implementation of SphericalFunction which uses cubemap texture look-up
class SphericalFunctionCubemap : public SphericalFunction<sf::Vector3f> {
    Cubemap& cubemap;
public:
    SphericalFunctionCubemap(Cubemap& cubemap) :
        cubemap(cubemap)
    {
    }
    sf::Vector3f getValue(const sf::Vector3f& v) {
        return cubemap.getColorFromTexCoords(v);
    }
};

// implementation of SphericalFunction which uses clamped dot product of a normal vector
class SphericalFunctionNormalVisibility : public SphericalFunction<float> {
    const sf::Vector3f& normal;
public:
    SphericalFunctionNormalVisibility(const sf::Vector3f& normal) :
        normal(normal)
    {
    }
    float getValue(const sf::Vector3f& v) {
        float visibility = dotProduct(normal, v);
        if (visibility < 0.f) visibility = 0.f;
        return visibility;
    }
};

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

/*
   current function for getting per-vertex colors based on spherical harmonics lighting
   takes normal coefficients and cubemap coefficients and performs dot product
   which happens after appropriate cubemap coefficient rotation operations
*/
void calculateModelColors(unsigned int vertexCount, float* modelColors,
    const float* normalSHCoeff, const sf::Vector3f* cubemapSHCoeff)
{
    for (int iter = 0; iter < vertexCount; iter++) {
        sf::Vector3f vertexColor(0.f, 0.f, 0.f);

        sf::Vector3f rotatedCubemapSHCoeff[BASIS_FUNCTION_COUNT];

      // needed because cos/sin evaluate to double and fail Vector3 template matching
      // but also creates slight gain in efficiency, so that's cool!
        float cosAngle = cos(angle);
        float sinAngle = sin(angle);
        float cosAngle2 = cos(2.f * angle);
        float sinAngle2 = sin(2.f * angle);

      /*
         rotation currently assumes there are exactly 9 basis functions (most likely true)
         also, it only allows rotation around Z axis right now
         will need the +/-90 degree rotations about X axis for ZYZ rotations (Z,X-90,Z,X+90,Z)
         also, will want better organization to perform rotations easier
      */
        rotatedCubemapSHCoeff[0] = cubemapSHCoeff[0];

        rotatedCubemapSHCoeff[1] = cosAngle * cubemapSHCoeff[1] + sinAngle * cubemapSHCoeff[3];
        rotatedCubemapSHCoeff[2] = cubemapSHCoeff[2];
        rotatedCubemapSHCoeff[3] = -sinAngle * cubemapSHCoeff[1] + cosAngle * cubemapSHCoeff[3];

        rotatedCubemapSHCoeff[4] = cosAngle2 * cubemapSHCoeff[4] + sinAngle2 * cubemapSHCoeff[8];
        rotatedCubemapSHCoeff[5] = cosAngle * cubemapSHCoeff[5] + sinAngle * cubemapSHCoeff[7];
        rotatedCubemapSHCoeff[6] = cubemapSHCoeff[6];
        rotatedCubemapSHCoeff[7] = -sinAngle * cubemapSHCoeff[5] + cosAngle * cubemapSHCoeff[7];
        rotatedCubemapSHCoeff[8] = -sinAngle2 * cubemapSHCoeff[4] + cosAngle2 * cubemapSHCoeff[8];

      // the per-color-channel dot product of cubemap coefficients and visibility coefficients
        for (int basis = 0; basis < BASIS_FUNCTION_COUNT; basis++) {
            vertexColor += rotatedCubemapSHCoeff[basis] *
                normalSHCoeff[BASIS_FUNCTION_COUNT * iter + basis];
        }
  /*
     dot product of coefficients is approximation of dot product times cubemap functions
     to get average color for vertex, divide by domain of integral (2pi x pi)
     however, average of dot product times cubemap will be too dark (most samples are zero)
     integral of dot product times sine is pi, so multiplying by 2pi fixes this
     since integral will be "brought up" to 2pi x pi and average "brought up" to 1 (at max)
  */

        vertexColor /= (float)(4.f * M_PI);
        vertexColor *= 4.f; // correction for dot product

        modelColors[3 * iter + 0] = vertexColor.x;
        modelColors[3 * iter + 1] = vertexColor.y;
        modelColors[3 * iter + 2] = vertexColor.z;
    }
}

// convenience structure to hold the parameters for the threaded function call
struct CalculateVisibilityCoefficientsParameters {
    const float* normalPointer;
    float* coefficientPointer;
    int startIndex;
    int endIndex;
};

// function to calculate a segment of the visibility coefficients
void* calculateVisibilityCoefficientsThreaded(void* input) {
    CalculateVisibilityCoefficientsParameters* parameters =
        (CalculateVisibilityCoefficientsParameters*)input;

    for (int iter = parameters->startIndex; iter < parameters->endIndex; iter++) {
        sf::Vector3f normal;
        normal.x = parameters->normalPointer[3 * iter + 0];
        normal.y = parameters->normalPointer[3 * iter + 1];
        normal.z = parameters->normalPointer[3 * iter + 2];

        SphericalFunctionNormalVisibility visibleFunction(normal);

        unsigned int thetaResolution = 16, phiResolution = 32;
        for (int basis = 0; basis < BASIS_FUNCTION_COUNT; basis++) {
            SphericalFunctionProduct<float,float,float> product(visibleFunction,
                SphericalHarmonics[basis]);

            parameters->coefficientPointer[BASIS_FUNCTION_COUNT * iter + basis] =
                product.integrate(thetaResolution, phiResolution);
        }
    }
}

int main(int argc, char** argv) {
    std::string modelPath = "Teapot.3ds";
    if (argc > 1) modelPath = argv[1];

    std::string cubemapDir = "gradientCube";
    if (argc > 2) cubemapDir = argv[2];

    Model testModel(modelPath);

    modelColors = new float[3 * testModel.getVertexCount()];
    float* normalSHCoeff = new float[BASIS_FUNCTION_COUNT * testModel.getVertexCount()];

  // calculate integrals of dot product function multiplied by basis functions
    std::cout << "calculating SH coefficients of normals..." << std::endl;

  // divide work into threads
    const int threadCount = 4;
    pthread_t threadArray[threadCount];
    CalculateVisibilityCoefficientsParameters inputArray[threadCount];

    for (int threadIndex = 0; threadIndex < threadCount; threadIndex++) {
        pthread_t* thread = &threadArray[threadIndex];
        CalculateVisibilityCoefficientsParameters* input = &inputArray[threadIndex];

        input->normalPointer = testModel.getNormalPointer();
        input->coefficientPointer = normalSHCoeff;

        int totalIterations = testModel.getVertexCount();
        input->startIndex = (threadIndex + 0) * totalIterations / threadCount;
        input->endIndex = (threadIndex + 1) * totalIterations / threadCount;

        pthread_create(thread, NULL, calculateVisibilityCoefficientsThreaded, (void*)input);
    }

    for (int threadIndex = 0; threadIndex < threadCount; threadIndex++) {
        pthread_join(threadArray[threadIndex], NULL);
    }

    sf::RenderWindow window(sf::VideoMode(800, 600), "Spherical Harmonics Test");
    window.setFramerateLimit(60);

  // provide a directory containing images named negativeX.png, positiveY.png, etc.
    Cubemap testCubemap(cubemapDir);

    SphericalFunctionCubemap sphericalCubemap(testCubemap);

    sf::Vector3f cubemapSHCoeff[BASIS_FUNCTION_COUNT];

  // calculate integrals of cubemap "function" multiplied by basis functions
    std::cout << "calculating SH coefficients of cubemap..." << std::endl;

    unsigned int thetaResolution = 256, phiResolution = 512;
    for (int basis = 0; basis < BASIS_FUNCTION_COUNT; basis++) {
        SphericalFunctionProduct<sf::Vector3f, float, sf::Vector3f> product(sphericalCubemap,
            SphericalHarmonics[basis]);

        cubemapSHCoeff[basis] = product.integrate(thetaResolution, phiResolution);
    }

    setup();

    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
        }

        calculateModelColors(testModel.getVertexCount(), modelColors, normalSHCoeff, cubemapSHCoeff);
        angle += 0.01f;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, 0);

        glLoadIdentity();
        glTranslatef(0.f, 0.f, -2.f);
        //glRotatef(30.f, 1.f, 0.f, 0.f);
        //glRotatef(angle, 0.f, 1.f, 0.f);
        glScalef(0.01f, 0.01f, 0.01f);

        drawModel(testModel);

  // cubemap is "infinitely far away", no translation in modelview matrix
  // also, its faces just looks weird at an angle

        glLoadIdentity();
        //glRotatef(30.f, 1.f, 0.f, 0.f);
        //glRotatef(angle, 0.f, 1.f, 0.f); angle += 1.f;

      // rotation now only needed for cubemap, mirrors coefficient rotation
      // needs to be converted to degrees for OpenGL's rotate function
        glRotatef(angle * 180.f / M_PI, 0.f, 0.f, 1.f);
        glScalef(5.f, 5.f, 5.f);

        drawCubemap(testCubemap);

        window.display();
    }

    return 0;
}
