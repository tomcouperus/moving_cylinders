#include "envelope.h"

/**
 * @brief Envelope::Envelope Creates a new envelope with default values.
 */
Envelope::Envelope() :
    adjEnv(nullptr),
    toolMovement(nullptr),
    tool()
{
    sectorsA = 200;
    sectorsT = 200;
    qDebug() << "Envelope()";
}

/**
 * @brief Envelope::Envelope Creates a new envelope with the given values.
 * @param toolMovement Tool movement.
 * @param tool Tool pointer.
 */
Envelope::Envelope(CylinderMovement *toolMovement, Tool *tool) :
    adjEnv(nullptr),
    toolMovement(toolMovement),
    tool(tool)
{
    sectorsA = tool->getSectors();
    sectorsT = toolMovement->getPath().getSectors();
}

/**
 * @brief Envelope::Envelope Creates a new envelope with the given values.
 * @param toolMovement Tool movement.
 * @param tool Tool pointer.
 * @param adjEnvelope Adjacent envelope.
 */
Envelope::Envelope(CylinderMovement *toolMovement, Tool *tool, Envelope *adjEnvelope) :
    adjEnv(adjEnvelope),
    toolMovement(toolMovement),
    tool(tool)
{
    sectorsA = tool->getSectors();
    sectorsT = toolMovement->getPath().getSectors();
}

/**
 * @brief Envelope::initEnvelope Initialises the envelope.
 */
void Envelope::initEnvelope()
{
    computeEnvelope();
    computeToolCenters();
    computeGrazingCurves();
    computeNormals();
}

void Envelope::setToolMovement(CylinderMovement *toolMovement)
{
    this->toolMovement = toolMovement;
    sectorsT = toolMovement->getPath().getSectors();

    computeEnvelope();
    computeToolCenters();
    computeGrazingCurves();
    computeNormals();
}

void Envelope::setTool(Tool *tool)
{
    this->tool = tool;
    sectorsA = tool->getSectors();

    computeEnvelope();
    computeToolCenters();
    computeGrazingCurves();
    computeNormals();
}

void Envelope::setAdjacentEnvelope(Envelope *env){
    this->adjEnv = env;

    computeEnvelope();
    computeToolCenters();
    computeGrazingCurves();
    computeNormals();
}

/**
 * @brief Envelope::computeToolCenters Computes the vertex array of tool centers.
 */
void Envelope::computeToolCenters()
{
    vertexArrCenters.clear();
    endCurveArr.clear();
    QVector<Vertex> pathArr = toolMovement->getPath().getVertexArr();
    if(adjEnv != nullptr && contToAdj)
        pathArr.clear();

    QVector3D color = QVector3D(0,0,1);

    QVector3D v1, v2;
    SimplePath path = toolMovement->getPath();
    float aDelta = (tool->getA1()-tool->getA0())/sectorsA;
    float a1 = tool->getA1()-aDelta;
    float tDelta = (path.getT1()-path.getT0())/sectorsT;
    float t1 = path.getT1();
    for (float t = path.getT0(); t <= t1; t+=tDelta)
    {
        for (float a = tool->getA0(); a < a1; a+=aDelta)
        {
            v1 = calcToolCenterAt(t, a);
            v2 = calcToolCenterAt(t, a+aDelta);

            // Add vertices to array
            vertexArrCenters.append(Vertex(v1, color));
            vertexArrCenters.append(Vertex(v2, color));

            if(adjEnv != nullptr && contToAdj && a == tool->getA0()){
                pathArr.append(Vertex(v1, color));
            }
            if(a >= a1-aDelta){
                endCurveArr.append(v2);
                // qDebug() << "appended endpoint at (" << a+aDelta << ", " << t << ")";
            }
        }
    }
    toolMovement->setDiscPath(pathArr);
}

/**
 * @brief Envelope::computeGrazingCurves Computes the vertex array of the grazing curves.
 */
void Envelope::computeGrazingCurves()
{
    vertexArrGrazingCurve.clear();

    QVector3D color = QVector3D(0,1,0);

    QVector3D v1, v2;
    SimplePath path = toolMovement->getPath();
    float aDelta = (tool->getA1()-tool->getA0())/sectorsA;
    float a1 = tool->getA1()-aDelta;
    float tDelta = (path.getT1()-path.getT0())/sectorsT;
    float t1 = path.getT1();
    for (float t = path.getT0(); t < t1; t+=tDelta)
    {
        for (float a = tool->getA0(); a < a1; a+=aDelta)
        {
            v1 = calcGrazingCurveAt(t, a);
            v2 = calcGrazingCurveAt(t, a+aDelta);

            // Add vertices to array
            vertexArrGrazingCurve.append(Vertex(v1, color));
            vertexArrGrazingCurve.append(Vertex(v2, color));
        }
    }
}

/**
 * @brief Envelope::computeEnvelope Computes the vertex array of the envelope.
 */
void Envelope::computeEnvelope()
{
    vertexArr.clear();

    Vertex v1, v2, v3, v4;
    SimplePath path = toolMovement->getPath();
    float aDelta = (tool->getA1()-tool->getA0())/sectorsA;
    float a1 = tool->getA1()-aDelta;
    float tDelta = (path.getT1()-path.getT0())/sectorsT;
    float t1 = path.getT1()-tDelta;
    qDebug()<< "t1" << t1;
    for (float t = path.getT0(); t < t1; t+=tDelta)
    {
        for (float a = tool->getA0(); a < a1; a+=aDelta)
        {
            v1 = calcEnvelopeAt(t, a);
            v2 = calcEnvelopeAt(t, a+aDelta);
            v3 = calcEnvelopeAt(t+tDelta, a);
            v4 = calcEnvelopeAt(t+tDelta, a+aDelta);

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

/**
 * @brief Envelope::computeNormals Computes the vertex array of the normals.
 */
void Envelope::computeNormals(){
    vertexArrNormals.clear();

    QVector3D c = QVector3D(0,1,0);

    QVector3D v1, v2, p1, p2;
    SimplePath path = toolMovement->getPath();
    float aDelta = (tool->getA1()-tool->getA0())/sectorsA;
    float a1 = tool->getA1()-aDelta;
    float tDelta = (path.getT1()-path.getT0())/sectorsT;
    float t1 = path.getT1();
    QVector<Vertex> normals;
    for (float t = path.getT0(); t < t1; t+=tDelta)
    {
        normals.clear();
        for (float a = tool->getA0(); a < a1; a+=aDelta)
        {
            p1 = calcToolCenterAt(t,a);
            p2 = calcToolCenterAt(t, a+aDelta);
            v1 = p1 + tool->getRadiusAt(a)*computeNormal(t, a, contToAdj);
            v2 = p2 + tool->getRadiusAt(a)*computeNormal(t, a+aDelta, contToAdj);

            // Add vertices to array
            normals.append(Vertex(p1,c));
            normals.append(Vertex(v1,c));
            normals.append(Vertex(p2,c));
            normals.append(Vertex(v2,c));
        }
        vertexArrNormals.append(normals);
    }
}

/**
 * @brief Envelope::calcToolCenterAt Calculates the tool center at a given time and axis height a.
 * @param t Time.
 * @param a Axis height.
 * @return Tool center.
 */
QVector3D Envelope::calcToolCenterAt(float t, float a)
{
    QVector3D axis = calcToolAxisDirecAt(t).normalized();
    float a0 = tool->getA0();
    QVector3D center = getPathAt(t) + (a-a0)*(axis);

    return center;
}

/**
 * @brief Envelope::getPathAt Returns the path of the tool movement at a given time.
 * @param t Time.
 * @return Path at time t.
 */
QVector3D Envelope::getPathAt(float t){
    if (adjEnv != nullptr && contToAdj){
        QVector3D adjCenter = adjEnv->endCurveArr[adjEnv->toolMovement->getPath().getIdxAtTime(t)];
        float rAdjEnv = adjEnv->tool->getRadiusAt(adjEnv->tool->getA1());
        float rEnv = tool->getRadiusAt(tool->getA0());
        float a0 = tool->getA0();
        QVector3D axis = calcToolAxisDirecAt(t);
        return adjCenter + (rAdjEnv - rEnv)*computeNormal(t,adjEnv->tool->getA1(),adjEnv);
    } else {
        SimplePath path = toolMovement->getPath();
        return path.getPathAt(t);
    }
}

/**
 * @brief Envelope::getPathDirecAt Returns the path direction of the tool movement at a given time.
 * @param t Time.
 * @return Path direction at time t.
 */
QVector3D Envelope::getPathDirecAt(float t){
    int idx = toolMovement->getPath().getIdxAtTime(t);
    if(idx == adjEnv->endCurveArr.size()-1) {
        return (adjEnv->endCurveArr[idx] - adjEnv->endCurveArr[idx-1]);
    } else {
        return (adjEnv->endCurveArr[idx+1] - adjEnv->endCurveArr[idx]);
    }
}

/**
 * @brief Envelope::calcToolAxisDirecAt Calculates the tool axis direction at a given time.
 * @param t Time.
 * @return Tool axis direction.
 */
QVector3D Envelope::calcToolAxisDirecAt(float t)
{
    QVector3D axis = toolMovement->getAxisDirectionAt(t);
    if (axis != axis)
        qDebug() << "Problem with axis at time " << t;
    if (adjEnv != nullptr && contToAdj){
        QMatrix4x4 rotation = getAdjMovementRotation(t);
        QVector4D axisDirecNew = rotation * QVector4D(axis,0);
        axis = QVector3D(axisDirecNew.x(),axisDirecNew.y(),axisDirecNew.z());
    }
    return axis;
}

/**
 * @brief Envelope::calcAxisRateOfChange Calculates the rate of change of the axis of the tool at a given time.
 * @param t Time.
 * @return Rate of change of the axis.
 */
QVector3D Envelope::calcAxisRateOfChange(float t)
{
    QVector3D axis = toolMovement->getAxisRateOfChange(t);
    if (adjEnv != nullptr && contToAdj){
        QMatrix4x4 rotation = getAdjMovementRotation(t);
        QVector4D axisDirecNew = rotation * QVector4D(axis,0);
        axis = QVector3D(axisDirecNew.x(),axisDirecNew.y(),axisDirecNew.z());
    }
    return axis;
}

/**
 * @brief Envelope::getAdjMovementRotation Calculates the rotation matrix of the adjacent tool at a given time.
 * @param time Time.
 * @return Rotation matrix.
 */
QMatrix4x4 Envelope::getAdjMovementRotation(float time)
{
    QMatrix4x4 rotation;
    rotation.setToIdentity();
    // rotation for continuity
    QVector3D axis = toolMovement->getAxisDirectionAt(time);
    QVector3D normal = adjEnv->computeNormal(time,adjEnv->tool->getA1(),false);
    QVector3D rotationAxis = QVector3D::crossProduct(normal, adjEnv->toolMovement->getAxisDirectionAt(time)); // adjEnv->toolMovement->getPath().getTangentAt(time);
    double ra = qRadiansToDegrees((tool->getRadiusDerivativeWRTa(tool->getA0()) - adjEnv->tool->getRadiusDerivativeWRTa(adjEnv->tool->getA0())));
    if(ra != 0) {
        rotation.rotate(ra, rotationAxis);
    }

    // rotation w.r.t input
    int i = toolMovement->getPath().getIdxAtTime(time);
    double angleDelta = (qRadiansToDegrees(adjAxisAngle2) - qRadiansToDegrees(adjAxisAngle1))/sectorsT;
    double angle = qRadiansToDegrees(adjAxisAngle1) + angleDelta*i;
    if(angle != 0) {
        rotation.rotate(angle, normal);
    }
    return rotation;
}

/**
 * @brief Envelope::calcEnvelopeAt Calculates the envelope at a given time and axis height.
 * @param t Time.
 * @param a Axis height.
 * @return The vertex of the envelope.
 */
Vertex Envelope::calcEnvelopeAt(float t, float a)
{
    QVector3D color = QVector3D(1,0,0);

    QVector3D center = calcToolCenterAt(t, a);

    float r = tool->getRadiusAt(a) + 0.0001; // + 0.0001 to avoid floating point weirdness
    QVector3D normal = computeNormal(t, a, contToAdj);
    QVector3D posit = center + r*normal;

    return Vertex(posit, normal);
}

/**
 * @brief Envelope::calcGrazingCurveAt Calculates the grazing curve at a given time and axis height.
 * @param t Time.
 * @param a Axis height.
 * @return The point of the grazing curve.
 */
QVector3D Envelope::calcGrazingCurveAt(float t, float a)
{
    QVector3D center = calcToolCenterAt(t, a);
    float r = tool->getRadiusAt(a) + 0.0002; // + 0.0002 to avoid floating point weirdness
    QVector3D normal = computeNormal(t, a, contToAdj);
    QVector3D posit = center + r*normal;

    return posit;
}

/**
 * @brief Envelope::computeNormal Computes the normal at a given time and axis height.
 * @param t Time.
 * @param a Axis height.
 * @param cont True if the envelope is continuous to the adjacent envelope.
 * @return The normal.
 */
QVector3D Envelope::computeNormal(float t, float a, bool cont)
{
    SimplePath path = toolMovement->getPath();
    QVector3D sa = calcToolAxisDirecAt(t);
    float a0 = tool->getA0();
    // the paper doesn't compute st, if AxisRateOfChange is not normalized then the grazing curves are ruled
    QVector3D st = path.getTangentAt(t) + (a-a0)*(calcAxisRateOfChange(t).normalized());
    //    qDebug() << "st: " << st;
    QVector3D sNorm = QVector3D::crossProduct(sa, st).normalized();

    //if (st != st)
    //    qDebug() << "Problem with st at (" << a << ", " << t <<")" ;
    //if (sa != sa)
        //qDebug() << "Problem with sa at (" << a << ", " << t <<")" ;

    // Calculate alpha, beta, gamma
    float alpha, beta, gamma;
    float ra = tool->getRadiusDerivativeWRTa(a);
    //    qDebug() << "ra: " << ra;
    double matDet = (QVector3D::dotProduct(sa,sa) * QVector3D::dotProduct(st,st))
                    - (QVector3D::dotProduct(sa,st) * QVector3D::dotProduct(st,sa));
    // didn't find determinant problems for the first envelope
    if (matDet < 0.001)
        qDebug() << "small determinant at (" << a << ", " << t <<")" ;
    if (1/matDet != 1/matDet && adjEnv != adjEnv)
        qDebug() << "determinant problem at (" << a << ", " << t <<"): det = " << matDet <<", st = " << st <<", sa = " << sa;
    //    qDebug() << "mInv: " << mInv;
    double a11 = 1/matDet * QVector3D::dotProduct(st,st);
    alpha = a11 * (-ra);
    double a21 = -1/matDet * QVector3D::dotProduct(st,sa);
    beta = a21 * (-ra);
    //    qDebug() << "A11"<< mInv.column(0).normalized().x();
    gamma = sqrt(1-ra*ra*a11); // abs to solve issue with imaginaries?
    // adjEnv != adjEnv to focus on the first envelope only
    if (gamma != gamma && adjEnv != adjEnv)
        qDebug() << "alpha: " << alpha << " beta: " << beta << " gamma: " << gamma << " det: " << matDet;

    QVector3D normal = alpha*sa + beta*st + gamma*sNorm;
    if (normal != normal && adjEnv != adjEnv)
        qDebug() << "alpha: " << alpha << " beta: " << beta << " gamma: " << gamma << " det: " << matDet;
    // qDebug() << "norm: " << normal;
    normal.normalize();
    return normal;
}
