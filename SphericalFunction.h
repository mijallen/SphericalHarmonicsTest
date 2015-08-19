#ifndef _SPHERICALFUNCTION_H_
#define _SPHERICALFUNCTION_H_

#include <SFML/System/Vector3.hpp>
#include <cmath>

// everything must be defined in this header, no cpp file
// refer to http://stackoverflow.com/questions/1353973/c-template-linking-error for why

template <typename ReturnType> class SphericalFunctionSubroutine;

// abstract base class for spherical functions
template <typename ReturnType>
class SphericalFunction {
public:
    virtual ReturnType getValue(const sf::Vector3<float>& v) = 0;
    ReturnType operator()(const sf::Vector3f&);
    ReturnType integrate(unsigned int thetaResolution, unsigned int phiResolution);
};

// spherical function based on user-provided function call
template <typename ReturnType>
class SphericalFunctionSubroutine : public SphericalFunction<ReturnType> {
    ReturnType (*subroutine)(const sf::Vector3<float>&);
public:
    SphericalFunctionSubroutine(ReturnType (*subroutine)(const sf::Vector3<float>&));
    ReturnType getValue(const sf::Vector3<float>& v);
    SphericalFunctionSubroutine<ReturnType>& operator=(const SphericalFunctionSubroutine<ReturnType>&);
};

// spherical function based on product of two spherical functions
template <typename LeftType, typename RightType, typename ReturnType>
class SphericalFunctionProduct : public SphericalFunction<ReturnType> {
    SphericalFunction<LeftType>& leftFunction;
    SphericalFunction<RightType>& rightFunction;
public:
    SphericalFunctionProduct(SphericalFunction<LeftType>& leftFunction,
        SphericalFunction<RightType>& rightFunction);
    ReturnType getValue(const sf::Vector3<float>& v);
    SphericalFunctionProduct<LeftType, RightType, ReturnType>& operator=(
        const SphericalFunctionProduct<LeftType, RightType, ReturnType>&);
};

// implementation

// spherical function methods

template <typename ReturnType>
ReturnType SphericalFunction<ReturnType>::operator()(const sf::Vector3f& v) {
    return this->getValue(v);
}

/*
   Performance Testing:
   Processor:
    -Intel Core i5-2450M, 2.5GHz
    -running on single thread

   for Teapot.3ds normal visibilities, 25x50 per normal
    -SphericalFunction integrate naive: 120s
    -inline optimized: 3s (with some uncertainty)
    -inline naive: 15s
    -SphericalFunction integrate optimized: 12s (prediction: 24s)

   Note: optimization replaces sin/cos with precomputed look-ups
*/

template <typename ReturnType>
ReturnType SphericalFunction<ReturnType>::integrate(unsigned int thetaResolution,
    unsigned int phiResolution)
{
    float thetaDifferential = M_PI / (float)thetaResolution;
    float phiDifferential = 2.f * M_PI / (float)phiResolution;

    ReturnType integral;
    integral *= 0.f;

  /*
     NOTE: using a look-up table for sin(theta), cos(theta), sin(phi), cos(phi)
           provides a significant performance improvement; using the cartesian forms
           of spherical harmonic functions will forgo need to use sin/cos (x, x*y, etc.).
  */

    float* sinTheta = new float[thetaResolution];
    float* cosTheta = new float[thetaResolution];
    float* sinPhi = new float[phiResolution];
    float* cosPhi = new float[phiResolution];

  // precomputed look-up tables for cos(theta) and sin(theta)
    for (int thetaIter = 0; thetaIter < thetaResolution; thetaIter++) {
        float theta = thetaIter * thetaDifferential + 0.5f * thetaDifferential;
        theta = 2.f * acos(sqrt(1.f - (theta / M_PI))); // bias theta away from poles
        sinTheta[thetaIter] = sin(theta);
        cosTheta[thetaIter] = cos(theta);
    }

  // precomputed look-up tables for cos(phi) and sin(phi)
    for (int phiIter = 0; phiIter < phiResolution; phiIter++) {
        float phi = phiIter * phiDifferential + 0.5f * phiDifferential;
        sinPhi[phiIter] = sin(phi);
        cosPhi[phiIter] = cos(phi);
    }

  // need to compute integral of f(theta,phi) * sin(theta) over domain [0,pi]x[0,2pi]
  // to get average radius, divide by 4pi, since it is the integral of sin(theta) on same domain

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

            //integral += this->getValue(sampleVector) * sinTheta[thetaIter]; // for unbiased theta
            integral += this->getValue(sampleVector); // for biased theta
        }
    }

  // compute average of spherical function
    integral *= 1.f / (float)(thetaResolution * phiResolution);

  // compute integral of spherical function
    //float domain = 2.f * M_PI * M_PI; // for unbiased theta
    float domain = 4.f * M_PI; // for biased theta
    integral *= domain;

    delete[] sinTheta;
    delete[] cosTheta;
    delete[] sinPhi;
    delete[] cosPhi;

    return integral;
}

// spherical function subroutine methods

template <typename ReturnType>
SphericalFunctionSubroutine<ReturnType>::SphericalFunctionSubroutine(
    ReturnType (*subroutine)(const sf::Vector3<float>&))
:
    subroutine(subroutine)
{
}

template <typename ReturnType>
ReturnType SphericalFunctionSubroutine<ReturnType>::getValue(const sf::Vector3<float>& v) {
    return this->subroutine(v);
}

template <typename ReturnType>
SphericalFunctionSubroutine<ReturnType>&
SphericalFunctionSubroutine<ReturnType>::operator=(const SphericalFunctionSubroutine<ReturnType>& copy) {
    this->subroutine = copy.subroutine;
}

// spherical function product methods

template <typename LeftType, typename RightType, typename ReturnType>
SphericalFunctionProduct<LeftType, RightType, ReturnType>::SphericalFunctionProduct(
    SphericalFunction<LeftType>& leftFunction, SphericalFunction<RightType>& rightFunction)
:
    leftFunction(leftFunction), rightFunction(rightFunction)
{
}

template <typename LeftType, typename RightType, typename ReturnType>
ReturnType SphericalFunctionProduct<LeftType, RightType, ReturnType>::getValue(
    const sf::Vector3<float>& v)
{
    LeftType left = leftFunction.getValue(v);
    RightType right = rightFunction.getValue(v);
    return (left * right); // must evaluate to ReturnType!
}

template <typename LeftType, typename RightType, typename ReturnType>
SphericalFunctionProduct<LeftType, RightType, ReturnType>&
SphericalFunctionProduct<LeftType, RightType, ReturnType>::operator=(
    const SphericalFunctionProduct<LeftType, RightType, ReturnType>& copy)
{
    this->leftFunction = copy.leftFunction;
    this->rightFunction = copy.rightFunction;
}

#endif
