#ifndef _SPHERICALFUNCTION_H_
#define _SPHERICALFUNCTION_H_

#include <SFML/System/Vector3.hpp>

// everything must be defined in this header, no cpp file
// refer to http://stackoverflow.com/questions/1353973/c-template-linking-error for why

// abstract base class for spherical functions
template <typename ReturnType>
class SphericalFunction {
public:
    virtual ReturnType getValue(const sf::Vector3<float>& v) = 0;
    ReturnType operator()(const sf::Vector3f&);
    ReturnType integrate();
};

// spherical function based on user-provided function call
template <typename ReturnType>
class SphericalFunctionSubroutine : public SphericalFunction<ReturnType> {
    ReturnType (*subroutine)(const sf::Vector3<float>&);
public:
    SphericalFunctionSubroutine(ReturnType (*subroutine)(const sf::Vector3<float>&));
    ReturnType getValue(const sf::Vector3<float>& v);
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
};

// implementation

// spherical function methods

template <typename ReturnType>
ReturnType SphericalFunction<ReturnType>::operator()(const sf::Vector3f& v) {
    return this->getValue(v);
}

template <typename ReturnType>
ReturnType SphericalFunction<ReturnType>::integrate() { // unimplemented
    ReturnType output;
    output *= 0.f;
    return output;
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

#endif
