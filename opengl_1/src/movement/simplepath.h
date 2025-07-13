#ifndef SIMPLEPATH_H
#define SIMPLEPATH_H

#include "path.h"
#include "polynomial.h"
#include <QOpenGLDebugLogger>

/**
 * @brief The SimplePath class is a path parameterized by a
 * vector of three polynomials.
 */
class SimplePath : public Path
{
    // P(t) = (x(t), y(t), z(t))
    Polynomial x, y, z;
public:
    SimplePath();
    SimplePath(Polynomial x, Polynomial y, Polynomial z);

    QVector3D getPathAt(float t) override;
    void updateVertexArr() override;
    QVector3D getDerivativeAt(float t);
    QVector3D getDerivative2At(float t);
    QVector3D getDerivative3At(float t);
    QVector3D getDerivative4PlusAt(float t);

    inline void setX(Polynomial x) {this->x = x; updateVertexArr();}
    inline void setY(Polynomial y) {this->y = y; updateVertexArr();}
    inline void setZ(Polynomial z) {this->z = z; updateVertexArr();}

    inline Polynomial& getX() {return x;}
    inline Polynomial& getY() {return y;}
    inline Polynomial& getZ() {return z;}
};

#endif // SIMPLEPATH_H
