#include "cylinder.h"

/**
 * @brief Cylinder::Cylinder Default constructor builds cylinder to preset size
 */
Cylinder::Cylinder()
    : Tool(ToolType::Tool_Cylinder), r(0.5), angle(0.0) {}

/**
 * @brief Cylinder::Cylinder Cylinder constructor
 * @param baseRadius radius of the smaller end
 * @param angle opening angle
 * @param height cylinder height
 * @param sectors number of horizontal and vertical sectors for the cylinder
 * @param position position of the center of the base of the cylinder
 */
Cylinder::Cylinder(float baseRadius, float angle, float height, int sectors, QVector3D position)
    : Tool(ToolType::Tool_Cylinder, sectors, height, position), r(baseRadius), angle(angle)
{}

Vertex Cylinder::getToolSurfaceAt(float a, float tRad) {
    float toolRad = getRadiusAt(a);
    QVector3D p(
        toolRad*cos(tRad),
        toolRad*sin(tRad),
        a*height
        );
    QVector3D c(1,0,0);
    return Vertex(p,c);
}
