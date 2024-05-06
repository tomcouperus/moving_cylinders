#include "polynomial.h"

Polynomial::Polynomial()
    : a(1.0), b(0.0), c(0.0), d(0.0)
{}

Polynomial::Polynomial(float a, float b, float c, float d)
    : a(a), b(b), c(c), d(d)
{}

float Polynomial::getValAt(float t)
{
    float pt = a*t*t*t + b*t*t + c*t + d;
    return pt;
}

void Polynomial::setParameters(float a, float b, float c, float d)
{
    this->a = a;
    this->b = b;
    this->c = c;
    this->d = d;
}
