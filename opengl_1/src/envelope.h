#ifndef ENVELOPE_H
#define ENVELOPE_H

#include "cylinder.h"
#include "simplepath.h"

class Envelope
{
    Cylinder cylinder;
    // TODO: finish abstract class and modify Envelope class accordingly
    std::unique_ptr<Path> path;
    // SimplePath path;
    // TODO: A w.r.t. time t
    QVector3D axisDirection;
    float t0, t1;
    QVector<Vertex> vertexArr;
public:
    Envelope();
    Envelope(Cylinder cylinder, Path* path, QVector3D direction, int sectors);

    void initEnvelope();
    void setCylinder(Cylinder cylinder);
    void setSectors(int sectors);
    void setPath(Path* path);
    void setAxisDirection(QVector3D direction);

    inline QVector<Vertex> getVertexArr(){ return vertexArr; }
private:
    int sectors;
    void computeEnvelope();
    Vertex calcSurfaceVertex(float t, float a);
};

#endif // ENVELOPE_H
