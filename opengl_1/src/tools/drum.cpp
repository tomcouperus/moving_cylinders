#include "drum.h"

/**
 * @brief Drum::Drum Creates a new drum with default values.
 */
Drum::Drum()
    : Tool(ToolType::Tool_Drum), r(0.5), rho(4)
{}

/**
 * @brief Drum::Drum Creates a new drum with the given values.
 * @param radius Radius at the centre of the drum.
 * @param curveRadius Radius of curvature of the drum.
 * @param height Height of the drum.
 * @param sectors Number of sectors of the drum.
 * @param position Position of the drum.
 */
Drum::Drum(float radius, float curveRadius, float height, int sectors, QVector3D position)
    : Tool(ToolType::Tool_Drum, sectors, height, position), r(radius), rho(curveRadius)
{}

float Drum::getRadiusAt(float a)
{
    float theta = getAngleAt(a);
    return getSphereRadiusAt(a) * cos(theta);
}

float Drum::getRadiusDaAt(float a)
{
    float theta = getAngleAt(a);
    float theta_a = getAngleDaAt(a);
    float c_theta = cos(theta);
    float s_theta = sin(theta);
    return getSphereRadiusDaAt(a) * c_theta +
           getSphereRadiusAt(a) * -s_theta * theta_a;
}

float Drum::getSphereCenterHeightAt(float a)
{
    float theta = getAngleAt(a);
    return D() * tan(theta) + height / 2;
}

float Drum::getSphereCenterHeightDaAt(float a)
{
    float theta = getAngleAt(a);
    float theta_a = getAngleDaAt(a);
    float c_theta = cos(theta);
    return D() / (c_theta * c_theta) * theta_a;
}

float Drum::getSphereRadiusAt(float a)
{
    float theta = getAngleAt(a);
    return rho - D() / cos(theta);
}

float Drum::getSphereRadiusDaAt(float a)
{
    float theta = getAngleAt(a);
    float theta_a = getAngleDaAt(a);
    return -D() * tan(theta) / cos(theta) * theta_a;
}

Vertex Drum::getToolSurfaceAt(float a, float tRad)
{
    float toolRad = getRadiusAt(a);
    QVector3D p(
        toolRad * cos(tRad),
        toolRad * sin(tRad),
        (a * 2 - 1) * height / 2 + height / 2
        );
    QVector3D c(0,1,0);
    return Vertex(p,c);
}

float Drum::getAngleAt(float a)
{
    a = a * 2 - 1;
    float term = (a * height) / (2 * rho);
    return asin(term);
}

float Drum::getAngleDaAt(float a)
{
    a = a * 2 - 1;
    float term = (a * height) / (2 * rho);
    float term_a = height / (2 * rho);
    return term_a / sqrt(1 - term * term);

}
