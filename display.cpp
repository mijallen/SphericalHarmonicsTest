#include "Model.h"
#include "Cubemap.h"
#include "SphericalFunction.h"

#include <iostream>
#include <string>

#include <SFML/Graphics.hpp>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cmath>

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

class SphericalFunctionNormalVisibility : public SphericalFunction<float> {
    const sf::Vector3f& normal;
public:
    SphericalFunctionNormalVisibility(const sf::Vector3f& normal) :
        normal(normal)
    {
    }
    float getValue(const sf::Vector3f& v) const {
        return (normal.x * v.x + normal.y * v.y + normal.z * v.z);
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

float dotProduct(const sf::Vector3f& vectorA, const sf::Vector3f& vectorB) {
    return (vectorA.x * vectorB.x + vectorA.y * vectorB.y + vectorA.z * vectorB.z);
}

std::ostream& operator<<(std::ostream& out, const sf::Vector3f& vector) {
    out << "<" << vector.x << "," << vector.y << "," << vector.z << ">";
    return out;
}

// put pointers to these basis functions in an array
// NOTE: basis functions assume provided vector is normalized!

#define SH_COEFF0 0.28209479177387814347403972578039 // sqrt(1/pi)/2
#define SH_COEFF1 0.48860251190291992158638462283835 // sqrt(3/pi)/2
#define SH_COEFF2 1.0925484305920790705433857058027 // sqrt(15/pi)/2
#define SH_COEFF3 0.31539156525252000603089369029571 // sqrt(5/pi)/4

float shBasis0(const sf::Vector3<float>& vector) {
    return SH_COEFF0;
}

float shBasis1(const sf::Vector3f& vector) {
    return SH_COEFF1 * vector.y;
}

float shBasis2(const sf::Vector3f& vector) {
    return SH_COEFF1 * vector.z;
}

float shBasis3(const sf::Vector3f& vector) {
    return SH_COEFF1 * vector.x;
}

float shBasis4(const sf::Vector3f& vector) {
    return SH_COEFF2 * vector.x * vector.y;
}

float shBasis5(const sf::Vector3f& vector) {
    return SH_COEFF2 * vector.y * vector.z;
}

float shBasis6(const sf::Vector3f& vector) {
    return SH_COEFF3 * -(vector.x * vector.x + vector.y * vector.y - 2.f * vector.z * vector.z);
}

float shBasis7(const sf::Vector3f& vector) {
    return SH_COEFF2 * vector.z * vector.x;
}

float shBasis8(const sf::Vector3f& vector) {
    return SH_COEFF2 * 0.5f * (vector.x * vector.x - vector.y * vector.y);
}

int main() {
    Model testModel("Teapot.3ds");

    modelColors = new float[3 * testModel.getVertexCount()];
    float* normalSHCoeff = new float[9 * testModel.getVertexCount()];

    SphericalFunctionSubroutine<float> basis0(shBasis0);
    SphericalFunctionSubroutine<float> basis1(shBasis1);
    SphericalFunctionSubroutine<float> basis2(shBasis2);
    SphericalFunctionSubroutine<float> basis3(shBasis3);
    SphericalFunctionSubroutine<float> basis4(shBasis4);
    SphericalFunctionSubroutine<float> basis5(shBasis5);
    SphericalFunctionSubroutine<float> basis6(shBasis6);
    SphericalFunctionSubroutine<float> basis7(shBasis7);
    SphericalFunctionSubroutine<float> basis8(shBasis8);

  // calculate integrals of dot product function multiplied by basis functions
    std::cout << "calculating SH coefficients of normals..." << std::endl;

    for (int iter = 0; iter < testModel.getVertexCount(); iter++) {
        sf::Vector3f normal;
        normal.x = testModel.getNormalPointer()[3 * iter + 0];
        normal.y = testModel.getNormalPointer()[3 * iter + 1];
        normal.z = testModel.getNormalPointer()[3 * iter + 2];

        normalSHCoeff[9 * iter + 0] = 0.f;
        normalSHCoeff[9 * iter + 1] = 0.f;
        normalSHCoeff[9 * iter + 2] = 0.f;
        normalSHCoeff[9 * iter + 3] = 0.f;
        normalSHCoeff[9 * iter + 4] = 0.f;
        normalSHCoeff[9 * iter + 5] = 0.f;
        normalSHCoeff[9 * iter + 6] = 0.f;
        normalSHCoeff[9 * iter + 7] = 0.f;
        normalSHCoeff[9 * iter + 8] = 0.f;

        int thetaResolution = 25;
        int phiResolution = 50;

        float thetaDifferential = M_PI / (float)thetaResolution;
        float phiDifferential = 2.f * M_PI / (float)phiResolution;

        float* sinTheta = new float[thetaResolution];
        float* cosTheta = new float[thetaResolution];
        float* sinPhi = new float[phiResolution];
        float* cosPhi = new float[phiResolution];

  /*
     NOTE: using a look-up table for sin(theta), cos(theta), sin(phi), cos(phi)
           provides a significant performance improvement; using the cartesian forms
           of spherical harmonic functions will forgo need to use sin/cos (x, x*y, etc.).
  */

        for (int thetaIter = 0; thetaIter < thetaResolution; thetaIter++) {
            float theta = thetaIter * thetaDifferential + 0.5f * thetaDifferential;
            theta = 2.f * acos(sqrt(1.f - (theta / M_PI)));
            sinTheta[thetaIter] = sin(theta);
            cosTheta[thetaIter] = cos(theta);
        }

        for (int phiIter = 0; phiIter < phiResolution; phiIter++) {
            float phi = phiIter * phiDifferential + 0.5f * phiDifferential;
            sinPhi[phiIter] = sin(phi);
            cosPhi[phiIter] = cos(phi);
        }

        for (int thetaIter = 0; thetaIter < thetaResolution; thetaIter++) {
            //float theta = thetaIter * thetaDifferential + 0.5f * thetaDifferential;
            //theta = 2.f * acos(sqrt(1.f - (theta / M_PI)));

            for (int phiIter = 0; phiIter < phiResolution; phiIter++) {
                //float phi = phiIter * phiDifferential + 0.5f * phiDifferential;

                sf::Vector3f sampleVector;
                //sampleVector.x = sin(theta) * sin(phi);
                //sampleVector.y = cos(theta);
                //sampleVector.z = sin(theta) * cos(phi);
                sampleVector.x = sinTheta[thetaIter] * sinPhi[phiIter];
                sampleVector.y = cosTheta[thetaIter];
                sampleVector.z = sinTheta[thetaIter] * cosPhi[phiIter];

                //float sampleWeight = dotProduct(normal, sampleVector) * sinTheta[thetaIter];
                float sampleWeight = dotProduct(normal, sampleVector);
                if (sampleWeight < 0.f) sampleWeight = 0.f;

                normalSHCoeff[9 * iter + 0] += sampleWeight * shBasis0(sampleVector);
                normalSHCoeff[9 * iter + 1] += sampleWeight * shBasis1(sampleVector);
                normalSHCoeff[9 * iter + 2] += sampleWeight * shBasis2(sampleVector);
                normalSHCoeff[9 * iter + 3] += sampleWeight * shBasis3(sampleVector);
                normalSHCoeff[9 * iter + 4] += sampleWeight * shBasis4(sampleVector);
                normalSHCoeff[9 * iter + 5] += sampleWeight * shBasis5(sampleVector);
                normalSHCoeff[9 * iter + 6] += sampleWeight * shBasis6(sampleVector);
                normalSHCoeff[9 * iter + 7] += sampleWeight * shBasis7(sampleVector);
                normalSHCoeff[9 * iter + 8] += sampleWeight * shBasis8(sampleVector);
            }
        }

  // take calculated sum, divide by number of samples for average, multiply by domain for integral
  // essentially the same as factoring out (thetaDifferential * phiDifferential) from each sample

        //float correctionFactor = 2.f * M_PI * M_PI / (float)(thetaResolution * phiResolution);
        float correctionFactor = 4.f * M_PI / (float)(thetaResolution * phiResolution);
        normalSHCoeff[9 * iter + 0] *= correctionFactor;
        normalSHCoeff[9 * iter + 1] *= correctionFactor;
        normalSHCoeff[9 * iter + 2] *= correctionFactor;
        normalSHCoeff[9 * iter + 3] *= correctionFactor;
        normalSHCoeff[9 * iter + 4] *= correctionFactor;
        normalSHCoeff[9 * iter + 5] *= correctionFactor;
        normalSHCoeff[9 * iter + 6] *= correctionFactor;
        normalSHCoeff[9 * iter + 7] *= correctionFactor;
        normalSHCoeff[9 * iter + 8] *= correctionFactor;

        delete[] sinTheta;
        delete[] cosTheta;
        delete[] sinPhi;
        delete[] cosPhi;
    }

  /*
     Proposed improvements:
      -the SoftwareTextureSFML class can have textures that activate after the OpenGL window is made
        this would allow preprocessing to occur without an unresponsive window (for cubemap calculations)
      -a progress bar could display the progress of preprocessing calculations
        the user would know what's going on
      -multithreading (C++11 style)
        option A: speed up preprocess results, join threads, then display
        option B: possible streaming of results from another thread?
      -caching results in a file format to prevent redundant preprocessing
        need a way to determine whether cached results are valid or not
  */

    sf::RenderWindow window(sf::VideoMode(800, 600), "Spherical Harmonics Test");
    window.setFramerateLimit(60);

  // provide a directory containing images named negativeX.png, positiveY.png, etc.
    Cubemap testCubemap("gradientCube");

    SphericalFunctionCubemap sphericalCubemap(testCubemap);
    SphericalFunctionProduct<sf::Vector3f, float, sf::Vector3f> cubeBasis0(sphericalCubemap, basis0);
    std::cout << "test: " <<  cubeBasis0.getValue(sf::Vector3f(1.f, 0.f, 0.f)).x << std::endl;

    sf::Vector3f cubemapSHCoeff[9];

  // calculate integrals of cubemap "function" multiplied by basis functions
    std::cout << "calculating SH coefficients of cubemap..." << std::endl;

  // repeated code, will need a function to integrate based on a normal or cubemap

    if (true) {
        cubemapSHCoeff[0] = sf::Vector3f(0.f, 0.f, 0.f);
        cubemapSHCoeff[1] = sf::Vector3f(0.f, 0.f, 0.f);
        cubemapSHCoeff[2] = sf::Vector3f(0.f, 0.f, 0.f);
        cubemapSHCoeff[3] = sf::Vector3f(0.f, 0.f, 0.f);
        cubemapSHCoeff[4] = sf::Vector3f(0.f, 0.f, 0.f);
        cubemapSHCoeff[5] = sf::Vector3f(0.f, 0.f, 0.f);
        cubemapSHCoeff[6] = sf::Vector3f(0.f, 0.f, 0.f);
        cubemapSHCoeff[7] = sf::Vector3f(0.f, 0.f, 0.f);
        cubemapSHCoeff[8] = sf::Vector3f(0.f, 0.f, 0.f);
        
        int thetaResolution = 25;
        int phiResolution = 50;

        float thetaDifferential = M_PI / (float)thetaResolution;
        float phiDifferential = 2.f * M_PI / (float)phiResolution;

        float* sinTheta = new float[thetaResolution];
        float* cosTheta = new float[thetaResolution];
        float* sinPhi = new float[phiResolution];
        float* cosPhi = new float[phiResolution];

        for (int thetaIter = 0; thetaIter < thetaResolution; thetaIter++) {
            float theta = thetaIter * thetaDifferential + 0.5f * thetaDifferential;
            theta = 2.f * acos(sqrt(1.f - (theta / M_PI)));
            sinTheta[thetaIter] = sin(theta);
            cosTheta[thetaIter] = cos(theta);
        }

        for (int phiIter = 0; phiIter < phiResolution; phiIter++) {
            float phi = phiIter * phiDifferential + 0.5f * phiDifferential;
            sinPhi[phiIter] = sin(phi);
            cosPhi[phiIter] = cos(phi);
        }

        for (int thetaIter = 0; thetaIter < thetaResolution; thetaIter++) {
            for (int phiIter = 0; phiIter < phiResolution; phiIter++) {
                sf::Vector3f sampleVector;
                sampleVector.x = sinTheta[thetaIter] * sinPhi[phiIter];
                sampleVector.y = cosTheta[thetaIter];
                sampleVector.z = sinTheta[thetaIter] * cosPhi[phiIter];

                //sf::Vector3f sampleColor = testCubemap.getColorFromTexCoords(sampleVector) * sinTheta[thetaIter];
                sf::Vector3f sampleColor = testCubemap.getColorFromTexCoords(sampleVector);

                cubemapSHCoeff[0] += sampleColor * shBasis0(sampleVector);
                cubemapSHCoeff[1] += sampleColor * shBasis1(sampleVector);
                cubemapSHCoeff[2] += sampleColor * shBasis2(sampleVector);
                cubemapSHCoeff[3] += sampleColor * shBasis3(sampleVector);
                cubemapSHCoeff[4] += sampleColor * shBasis4(sampleVector);
                cubemapSHCoeff[5] += sampleColor * shBasis5(sampleVector);
                cubemapSHCoeff[6] += sampleColor * shBasis6(sampleVector);
                cubemapSHCoeff[7] += sampleColor * shBasis7(sampleVector);
                cubemapSHCoeff[8] += sampleColor * shBasis8(sampleVector);
            }
        }

        //float correctionFactor = 2.f * M_PI * M_PI / (float)(thetaResolution * phiResolution);
        float correctionFactor = 4.f * M_PI / (float)(thetaResolution * phiResolution);
        cubemapSHCoeff[0] *= correctionFactor;
        cubemapSHCoeff[1] *= correctionFactor;
        cubemapSHCoeff[2] *= correctionFactor;
        cubemapSHCoeff[3] *= correctionFactor;
        cubemapSHCoeff[4] *= correctionFactor;
        cubemapSHCoeff[5] *= correctionFactor;
        cubemapSHCoeff[6] *= correctionFactor;
        cubemapSHCoeff[7] *= correctionFactor;
        cubemapSHCoeff[8] *= correctionFactor;

        delete[] sinTheta;
        delete[] cosTheta;
        delete[] sinPhi;
        delete[] cosPhi;
    }

  // perform dot product of coefficients to calculate per-vertex colors
    std::cout << "using coefficients to calculate colors..." << std::endl;

    for (int iter = 0; iter < testModel.getVertexCount(); iter++) {
        sf::Vector3f finalColor(0.f, 0.f, 0.f);

        finalColor += cubemapSHCoeff[0] * normalSHCoeff[9 * iter + 0];
        finalColor += cubemapSHCoeff[1] * normalSHCoeff[9 * iter + 1];
        finalColor += cubemapSHCoeff[2] * normalSHCoeff[9 * iter + 2];
        finalColor += cubemapSHCoeff[3] * normalSHCoeff[9 * iter + 3];
        finalColor += cubemapSHCoeff[4] * normalSHCoeff[9 * iter + 4];
        finalColor += cubemapSHCoeff[5] * normalSHCoeff[9 * iter + 5];
        finalColor += cubemapSHCoeff[6] * normalSHCoeff[9 * iter + 6];
        finalColor += cubemapSHCoeff[7] * normalSHCoeff[9 * iter + 7];
        finalColor += cubemapSHCoeff[8] * normalSHCoeff[9 * iter + 8];

  /*
     dot product of coefficients is approximation of dot product times cubemap functions
     to get average color for vertex, divide by domain of integral (2pi x pi)
     however, average of dot product times cubemap will be too dark (most samples are zero)
     integral of dot product times sine is pi, so multiplying by 2pi fixes this
     since integral will be "brought up" to 2pi x pi and average "brought up" to 1 (at max)
  */

        finalColor /= (float)(4.f * M_PI);
        finalColor *= 4.f; // correction for dot product
        //finalColor /= (float)(2.f * M_PI * M_PI);
        //finalColor *= (float)(2.f * M_PI); // correction for dot times sine
        //finalColor *= 2.5f; // no explanation, but needed when sin(theta) not used

        modelColors[3 * iter + 0] = finalColor.x;
        modelColors[3 * iter + 1] = finalColor.y;
        modelColors[3 * iter + 2] = finalColor.z;
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
        glScalef(0.01f, 0.01f, 0.01f);

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
