#include "polynomial.h"

/**
 * @brief Polynomial::Polynomial Default constructor.
 
*/
Polynomial::Polynomial()
    : a(1.0), b(0.0), c(0.0), d(0.0)
{}

/**
 * @brief Polynomial::Polynomial Constructor for Polynomial.
 * @param a Coefficient for t³.
 * @param b Coefficient for t².
 * @param c Coefficient for t.
 * @param d Constant term.
*/
Polynomial::Polynomial(float a, float b, float c, float d)
    : a(a), b(b), c(c), d(d)
{}

/**
 * @brief Polynomial::getValAt Returns the value of the polynomial at a given time.
 * @param t The time.
 * @return The value of the polynomial at time t.
*/
float Polynomial::getValAt(float t)
{
    float pt = a*t*t*t + b*t*t + c*t + d;
    return pt;
}

/**
 * @brief Polynomial::setParameters Sets the parameters of the polynomial.
 * @param a Coefficient for t³.
 * @param b Coefficient for t².
 * @param c Coefficient for t.
 * @param d Constant term.
*/
void Polynomial::setParameters(float a, float b, float c, float d)
{
    this->a = a;
    this->b = b;
    this->c = c;
    this->d = d;
}

/**
 * @brief Polynomial::getDerivativeAt Returns the value of the derivative of the polynomial at a given time.
 * @param t The time.
 * @return The value of the derivative of the polynomial at time t.
*/
float Polynomial::getDerivativeAt(float t)
{
    float pt = 3*a*t*t + 2*b*t + c;
    return pt;
}
