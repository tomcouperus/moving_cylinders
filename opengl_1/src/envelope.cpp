#include "envelope.h"

/**
 * @brief Envelope::Envelope Default constructor, defines a simple path for the cylinder
 * Uses default cylinder constructor.
 */
Envelope::Envelope()
    : cylinder(Cylinder()), path(CubicPath()), axisDirection(QVector3D(0.0,0.0,1.0)), sectors(20)
{
    t0 = -1;
    t1 = 1;
}

/**
 * @brief Envelope::Envelope Constructor for the envelope of a cylinder moving along
 * a path P(t) = at³+bt²+ct+d
 * @param cylinder Cylinder that defines the envelope
 * @param a 1st coefficient of the path equation
 * @param b 2nd coefficient of the path equation
 * @param c 3rd coefficient of the path equation
 * @param d 4th coefficient of the path equation
 * @param direction Vector describing the (initial) orientation of the cylinder
 * @param sectors number of sectors in the surface grid
 */
Envelope::Envelope(Cylinder cylinder, CubicPath path, QVector3D direction, int sectors)
    : cylinder(cylinder), path(path), axisDirection(direction), sectors(sectors)
{
    t0 = -1;
    t1 = 1;
}

void Envelope::setCylinder(Cylinder cylinder){
    this->cylinder = cylinder;
    // recompute envelope
}

void Envelope::setSectors(int sectors){
    this->sectors = sectors;
    // recompute envelope
}

void Envelope::setPath(CubicPath path){
    this->path = path;
    // recompute envelope
}
void Envelope::setAxisDirection(QVector3D direction){
    axisDirection = direction.normalized();
    // recompute envelope
}

void Envelope::initEnvelope(){
    computeEnvelope();
}

void Envelope::computeEnvelope(){
    vertexArr.clear();
    Vertex v1, v2, v3, v4;
    float tempT0, tempT1 = t0;
    float a0, a1 = 0;
    float tDelta = (t1-t0)/sectors;
    float aDelta = cylinder.getHeight()/cylinder.getSectors();
    for(size_t i=0;i<sectors;i++){
        tempT0 = tempT1;
        tempT1 += tDelta;

        for(size_t j=0;j<cylinder.getSectors();j++){
            a0 = a1;
            a1 += aDelta;
            v1 = calcSurfaceVertex(tempT0, a0);
            v2 = calcSurfaceVertex(tempT0, a1);
            v3 = calcSurfaceVertex(tempT1, a0);
            v4 = calcSurfaceVertex(tempT1, a1);

            // Add vertices to array
            vertexArr.append(v1);
            vertexArr.append(v4);
            vertexArr.append(v2);
            vertexArr.append(v1);
            vertexArr.append(v3);
            vertexArr.append(v4);
        }
    }
}

Vertex Envelope::calcSurfaceVertex(float t, float a){
    //QVector3D pt = path.getPathAt(t); // make a Path surface class?
    //QVector3D s = pt + a *axisDirection; // a0 = 0?
    // TODO: make a get r(a) function in cylinder
    QVector3D n;
    QVector3D xt; // = s;
    // xt += cylinder.getR()*n;
    return Vertex(xt,n);
}
