#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H


/**
 * @brief The Polynomial class assumes an univariate polynomial
 * it currently supports up to a cubic polynomial but
 * can be modified later
 */
class Polynomial
{
    // P(t) = at³ + bt² * ct + d
    float a, b, c, d;
public:
    Polynomial();
    Polynomial(float a, float b, float c, float d);

    float getValAt(float t);
    void setParameters(float a, float b, float c, float d);
    float getDerivativeAt(float t);

    inline float getA() {return a;}
    inline float getB() {return b;}
    inline float getC() {return c;}
    inline float getD() {return d;}
    inline void setA(float a) {this->a = a;}
    inline void setB(float b) {this->b = b;}
    inline void setC(float c) {this->c = c;}
    inline void setD(float d) {this->d = d;}
};

#endif // POLYNOMIAL_H
