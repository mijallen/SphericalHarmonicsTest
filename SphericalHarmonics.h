#include "SphericalFunction.h"

#define BASIS_FUNCTION_COUNT 9

// sqrt(1/pi)/2
#define HARMONIC_COEFFICIENT0 0.28209479177387814347403972578039

// sqrt(3/pi)/2
#define HARMONIC_COEFFICIENT1 0.48860251190291992158638462283835

// sqrt(15/pi)/2
#define HARMONIC_COEFFICIENT2 1.0925484305920790705433857058027

// sqrt(5/pi)/4
#define HARMONIC_COEFFICIENT3 0.31539156525252000603089369029571

// harmonic band 0

float HarmonicBasis0(const sf::Vector3<float>& vector) {
    return HARMONIC_COEFFICIENT0;
}

// harmonic band 1

float HarmonicBasis1(const sf::Vector3f& vector) {
    return HARMONIC_COEFFICIENT1 * vector.y;
}

float HarmonicBasis2(const sf::Vector3f& vector) {
    return HARMONIC_COEFFICIENT1 * vector.z;
}

float HarmonicBasis3(const sf::Vector3f& vector) {
    return HARMONIC_COEFFICIENT1 * vector.x;
}

// harmonic band 2

float HarmonicBasis4(const sf::Vector3f& vector) {
    return HARMONIC_COEFFICIENT2 * vector.x * vector.y;
}

float HarmonicBasis5(const sf::Vector3f& vector) {
    return HARMONIC_COEFFICIENT2 * vector.y * vector.z;
}

float HarmonicBasis6(const sf::Vector3f& vector) {
    return HARMONIC_COEFFICIENT3 * -(vector.x * vector.x + vector.y * vector.y - 2.f * vector.z * vector.z);
}

float HarmonicBasis7(const sf::Vector3f& vector) {
    return HARMONIC_COEFFICIENT2 * vector.z * vector.x;
}

float HarmonicBasis8(const sf::Vector3f& vector) {
    return HARMONIC_COEFFICIENT2 * 0.5f * (vector.x * vector.x - vector.y * vector.y);
}

// global spherical functions based on spherical harmonics

SphericalFunctionSubroutine<float> SphericalHarmonics[BASIS_FUNCTION_COUNT] =
{
    SphericalFunctionSubroutine<float>(HarmonicBasis0),
    SphericalFunctionSubroutine<float>(HarmonicBasis1),
    SphericalFunctionSubroutine<float>(HarmonicBasis2),
    SphericalFunctionSubroutine<float>(HarmonicBasis3),
    SphericalFunctionSubroutine<float>(HarmonicBasis4),
    SphericalFunctionSubroutine<float>(HarmonicBasis5),
    SphericalFunctionSubroutine<float>(HarmonicBasis6),
    SphericalFunctionSubroutine<float>(HarmonicBasis7),
    SphericalFunctionSubroutine<float>(HarmonicBasis8)
};
