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
    float t0, t1;
public:
    SimplePath();
    SimplePath(Polynomial x, Polynomial y, Polynomial z);

    QVector3D getPathAt(float t) override;
    int getIdxAtTime(float t);
    void initVertexArr() override;
    void updateVertexArr() override;
    QVector3D getTangentAt(float t);

    inline void setX(Polynomial x) {this->x = x; updateVertexArr();}
    inline void setY(Polynomial y) {this->y = y; updateVertexArr();}
    inline void setZ(Polynomial z) {this->z = z; updateVertexArr();}
    inline void setRange(float t0, float t1){this->t0 = t0; this->t1 = t1; updateVertexArr();}
    inline float getT0() {return t0;}
    inline float getT1() {return t1;}
    inline float getRange() {return t1-t0;}

    inline Polynomial getX() {return x;}
    inline Polynomial getY() {return y;}
    inline Polynomial getZ() {return z;}
};

#endif // SIMPLEPATH_H
