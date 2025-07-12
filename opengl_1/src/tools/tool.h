#ifndef TOOL_H
#define TOOL_H
#include <QOpenGLDebugLogger>

#include "tooltype.h"
#include "../vertex.h"

/**
 * @brief The Tool class is an abstract class that represents a generic tool.
 */
class Tool
{
protected:
    ToolType toolType;
    float height = 2;

    float PI = acos(-1.0f);

    QVector3D posit;

    int sectors = 20;
    QVector3D axisVector = QVector3D(0.0,0.0,1.0);
    QVector3D perpVector = QVector3D(1.0,0.0,0.0); // a vector perpendicular to the axis
    QVector<Vertex> vertexArr;
public:
    Tool(ToolType toolType) : toolType(toolType), vertexArr(), sectors(50), height(2), posit(QVector3D(0,0,0)) {}
    Tool(ToolType toolType, int sectors, float height, QVector3D position)
        : toolType(toolType), vertexArr(), sectors(sectors), height(height), posit(position) {}

    inline void setHeight(float height) {this->height=height;}
    inline float getHeight(){ return height; }

    inline void setSectors(int sectors) {this->sectors=sectors;}
    inline int getSectors(){ return sectors; }

    inline void setPosit(QVector3D position) {posit=position;}
    inline QVector3D getPosition() {return posit; }


    virtual float getRadiusAt(float a) = 0;
    virtual float getRadiusDaAt(float a) = 0;
    virtual float getSphereCenterHeightAt(float a)=0;
    virtual float getSphereCenterHeightDaAt(float a)=0;
    virtual float getSphereRadiusAt(float a)=0;
    virtual float getSphereRadiusDaAt(float a)=0;

    virtual Vertex getToolSurfaceAt(float a, float tRad)=0;


    inline QVector3D getAxisVector() const {return axisVector.normalized(); }
    inline QVector3D getVectorPerpToAxis() const {return perpVector; }
    inline QVector<Vertex>& getVertexArr(){ return vertexArr; }
    inline ToolType getType() { return toolType; }

    inline void initTool() {computeTool();}
    inline void update() {computeTool();}
    void computeTool();
};
#endif // TOOL_H
