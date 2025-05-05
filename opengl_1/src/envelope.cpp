#include "envelope.h"
#include "mathutility.h"

/**
 * @brief Envelope::Envelope Creates a new envelope with default values.
 * @param settings Readonly pointer to settings
 * @param index
 */
Envelope::Envelope(const Settings *settings, int index) :
    settings(settings),
    index(index),
    adjEnv(nullptr),
    tool()
{
    sectorsA = 20;
    sectorsT = 50;
}

/**
 * @brief Envelope::Envelope Creates a new envelope with the given values.
 * @param settings Readonly pointer to settings
 * @param tool Tool pointer.
 */
Envelope::Envelope(const Settings *settings, int index, Tool *tool) :
    settings(settings),
    index(index),
    adjEnv(nullptr),
    tool(tool)
{
    // TODO: envelope controls everyone else's sectors. They shouldn't even HAVE sectors.
    sectorsA = tool->getSectors();
    sectorsT = toolMovement.getPath().getSectors();
}

/**
 * @brief Envelope::Envelope Creates a new envelope with the given values.
 * @param settigns Readonly pointer to settings
 * @param tool Tool pointer.
 * @param adjEnvelope Adjacent envelope.
 */
Envelope::Envelope(const Settings *settings, int index, Tool *tool, Envelope *adjEnvelope) :
    settings(settings),
    index(index),
    tool(tool)
{
    setAdjacentEnvelope(adjEnvelope);
    sectorsA = tool->getSectors();
    sectorsT = toolMovement.getPath().getSectors();
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
    active = true;
}

void Envelope::update() {
    if (!checkDependencies()) {
        throw new std::runtime_error("Dependency issue. Likely a circular dependency. Should be handled on input of dependency.");
    }
    computeEnvelope();
    computeToolCenters();
    computeGrazingCurves();
    computeNormals();

    for (int i = 0; i < dependentEnvelopes.size(); i++) {
        dependentEnvelopes[i]->update();
    }
}

void Envelope::registerDependent(Envelope *dependent) {
    // TODO check for circular dependencies when adding
    if (dependentEnvelopes.contains(dependent)) return;
    dependentEnvelopes.append(dependent);
    qDebug() << "Added dependent";
}

void Envelope::deregisterDependent(Envelope *dependent) {
    qsizetype num = dependentEnvelopes.removeAll(dependent);
    qDebug() << "Removed" << num << "dependents";
}

/**
 * @brief Envelope::checkDependencies Checks for dependency issues in the envelope's dependents. Issues such as circular dependency.
 * @return True if no issues were found, False if there is an issue.
 */
bool Envelope::checkDependencies() {
    // TODO check for circular dependencies
    return true;
}

void Envelope::setTool(Tool *tool)
{
    this->tool = tool;
    // TODO envelope controls tool's sectors!
    sectorsA = tool->getSectors();
}

void Envelope::setAdjacentEnvelope(Envelope *env){
    if (adjEnv != nullptr) this->adjEnv->deregisterDependent(this);
    adjEnv = env;
    if (env != nullptr) env->registerDependent(this);
}

/**
 * @brief Envelope::computeEnvelope Computes the vertex array of the envelope.
 */
void Envelope::computeEnvelope()
{
    vertexArr.clear();

    QVector3D e1, e2, e3, e4;
    QVector3D c1, c2, c3, c4;
    // SimplePath path = toolMovement->getPath();
    // float aDelta = (tool->getA1()-tool->getA0())/sectorsA;
    // float a1 = tool->getA1()-aDelta;
    // float tDelta = (path.getT1()-path.getT0())/sectorsT;
    // float t1 = path.getT1()-tDelta;
    for (int tIdx = 0; tIdx < sectorsT; tIdx++)
    {
        for (int aIdx = 0; aIdx < sectorsA; aIdx++)
        {
            float tDelta = 1.0f/sectorsT;
            float aDelta = 1.0f/sectorsA;

            float t = tIdx / sectorsT;
            float a = aIdx / sectorsA;

            e1 = getEnvelopeAt(t, a);
            e2 = getEnvelopeAt(t, a+aDelta);
            e3 = getEnvelopeAt(t+tDelta, a);
            e4 = getEnvelopeAt(t+tDelta, a+aDelta);

            c1 = getNormalAt(t, a);
            c2 = getNormalAt(t, a+aDelta);
            c3 = getNormalAt(t+tDelta, a);
            c4 = getNormalAt(t+tDelta, a+aDelta);

            Vertex v1{e1, c1};
            Vertex v2{e2, c2};
            Vertex v3{e3, c3};
            Vertex v4{e4, c4};

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

QVector3D Envelope::getEnvelopeAt(float t, float a)
{
    return getPathAt(t) + a * getAxisAt(t) - getToolRadiusAt(a) * getNormalAt(t, a);
}

QVector3D Envelope::getEnvelopeDtAt(float t, float a)
{
    return getPathDtAt(t) + a * getAxisDtAt(t) - getToolRadiusAt(a) * getNormalDtAt(t, a);
}

/**
 * @brief Envelope::computeToolCenters Computes the vertex array of tool centers.
 */
void Envelope::computeToolCenters()
{
    vertexArrCenters.clear();
    QVector<Vertex>& pathArr = toolMovement.getPathVertexArr();
    pathArr.clear();

    QVector3D color = QVector3D(0,0,1);

    QVector3D v1, v2;
    // SimplePath path = toolMovement.getPath();
    // float aDelta = (tool->getA1()-tool->getA0())/sectorsA;
    // float a1 = tool->getA1()-aDelta;
    // float tDelta = (path.getT1()-path.getT0())/sectorsT;
    // float t1 = path.getT1();
    for (int tIdx = 0; tIdx <= sectorsT; tIdx++)
    {
        float tDelta = 1.0f/sectorsT;

        float t = tIdx / sectorsT;

        v1 = getPathAt(t);
        v2 = getPathAt(t) + getAxisAt(t);

        // Add vertices to array
        vertexArrCenters.append(Vertex(v1, color));
        vertexArrCenters.append(Vertex(v2, color));

        pathArr.append(Vertex(v1, color));
    }
}

/**
 * @brief Envelope::computeGrazingCurves Computes the vertex array of the grazing curves.
 */
void Envelope::computeGrazingCurves()
{
    vertexArrGrazingCurve.clear();

    QVector3D color = QVector3D(0,1,0);

    QVector3D v1, v2;
    // SimplePath path = toolMovement->getPath();
    // float aDelta = (tool->getA1()-tool->getA0())/sectorsA;
    // float a1 = tool->getA1()-aDelta;
    // float tDelta = (path.getT1()-path.getT0())/sectorsT;
    // float t1 = path.getT1();
    for (int tIdx = 0; tIdx <= sectorsT; tIdx++)
    {
        for (int aIdx = 0; aIdx < sectorsA; aIdx++)
        {
            float aDelta = 1.0f/sectorsA;

            float t = tIdx / sectorsT;
            float a = aIdx / sectorsA;

            v1 = getEnvelopeAt(t, a);
            v2 = getEnvelopeAt(t, a+aDelta);

            // Add vertices to array
            vertexArrGrazingCurve.append(Vertex(v1, color));
            vertexArrGrazingCurve.append(Vertex(v2, color));
        }
    }
}

/**
 * @brief Envelope::computeNormals Computes the vertex array of the normals.
 */
void Envelope::computeNormals(){
    vertexArrNormals.clear();

    QVector3D c = QVector3D(0,1,0);

    QVector3D v1, p1;
    // SimplePath path = toolMovement->getPath();
    // float aDelta = (tool->getA1()-tool->getA0())/sectorsA;
    // float a1 = tool->getA1()-aDelta;
    // float tDelta = (path.getT1()-path.getT0())/sectorsT;
    // float t1 = path.getT1();
    QVector<Vertex> normals;
    for (int tIdx = 0; tIdx <= sectorsT; tIdx++)
    {
        normals.clear();
        for (int aIdx = 0; aIdx <= sectorsA; aIdx++)
        {
            float t = tIdx / sectorsT;
            float a = aIdx / sectorsA;
            p1 = getPathAt(t) + a*getAxisAt(t);
            v1 = getEnvelopeAt(t, a);

            // Add vertices to array
            normals.append(Vertex(p1,c));
            normals.append(Vertex(v1,c));
        }
        vertexArrNormals.append(normals);
    }
}

QVector3D Envelope::getNormalAt(float t, float a)
{
    QVector3D sa = getAxisAt(t);
    QVector3D st = getPathDtAt(t) + a * getAxisDtAt(t);
    QVector3D sNormal = QVector3D::crossProduct(sa, st).normalized();

    float ra = getToolRadiusAt(a);

    float E = QVector3D::dotProduct(sa, sa);
    float F = QVector3D::dotProduct(sa, st);
    float G = QVector3D::dotProduct(st, st);
    float EG_FF = E * G - F * F;

    float m11 = G / EG_FF;
    float m21 = -F / EG_FF;

    float alpha = -m11 * ra;
    float beta = -m21 * ra;
    float gamma = (EG_FF > 0 ? 1 : -1) * std::sqrt(1 - ra * ra * m11);

    QVector3D n = alpha * sa + beta * st + gamma * sNormal;
    return n.normalized();
}

QVector3D Envelope::getNormalDtAt(float t, float a)
{
    QVector3D sa = getAxisAt(t);
    QVector3D sat = getAxisDtAt(t);
    QVector3D st = getPathDtAt(t) + a * getAxisDtAt(t);
    QVector3D stt = getPathDt2At(t) + a * getAxisDt2At(t);
    QVector3D sNormal = QVector3D::crossProduct(sa, st).normalized();
    QVector3D sNormal_t = MathUtility::normalVectorDerivative(QVector3D::crossProduct(sa, st), QVector3D::crossProduct(sat, st) + QVector3D::crossProduct(sa, stt));

    float ra = getToolRadiusAt(a);

    float E = QVector3D::dotProduct(sa, sa);
    float Et = 2 * QVector3D::dotProduct(sa, sat);
    float F = QVector3D::dotProduct(sa, st);
    float Ft = QVector3D::dotProduct(sat, st) + QVector3D::dotProduct(sa, stt);
    float G = QVector3D::dotProduct(st, st);
    float Gt = 2 * QVector3D::dotProduct(st, stt);
    float EG_FF = E * G - F * F;
    float EG_FF_t = Et * G + E * Gt - 2 * F * Ft;

    float m11 = G / EG_FF;
    float m11_t = (EG_FF * Gt - G * EG_FF_t) / (EG_FF * EG_FF);
    float m21 = -F / EG_FF;
    float m21_t = -(EG_FF * Ft - F * EG_FF_t) / (EG_FF * EG_FF);

    float alpha = -m11 * ra;
    float alpha_t = -m11_t * ra;
    float beta = -m21 * ra;
    float beta_t = -m21_t * ra;
    float gamma = (EG_FF > 0 ? 1 : -1) * std::sqrt(1 - ra * ra * m11);
    float gamma_t = (EG_FF > 0 ? 1 : -1) * -ra * ra * m11_t / (2 * std::sqrt(1 - ra * ra * m11));

    QVector3D n = alpha * sa + beta * st + gamma * sNormal;
    QVector3D nt = alpha * sat + alpha_t * sa + beta * stt + beta_t * st + gamma * sNormal_t + gamma_t * sNormal;
    return MathUtility::normalVectorDerivative(n, nt);
}

/**
 * @brief Envelope::getPathAt Returns the path of the tool movement at a given time.
 * @param t Time.
 * @return Path at time t.
 */
QVector3D Envelope::getPathAt(float t){
    if (isPositContinuous()){
        // use positional continuity formula
        return adjEnv->getEnvelopeAt(t, 1) +
               getToolRadiusAt(0) * getNormalAt(t, 0);
    } else {
        return toolMovement.getPath().getPathAt(t);
    }
}

/**
 * @brief Envelope::getPathDtAt Returns the tangent of path of the tool movement at a given time.
 * @param t Time.
 * @return Path tangent at time t.
 */
QVector3D Envelope::getPathDtAt(float t){
    if (isPositContinuous()){
        // use positional continuity formula
        return adjEnv->getEnvelopeDtAt(t, 1); /*+
               getToolRadiusAt(0) * getNormalDtAt(t, 0);*/
    } else {
        return toolMovement.getPath().getTangentAt(t);
    }
}

/**
 * @brief Envelope::getPathDt2At Returns the acceleration of path of the tool movement at a given time.
 * @param t Time.
 * @return Path acceleration at time t.
 */
QVector3D Envelope::getPathDt2At(float t){
    if (isPositContinuous())
    {
        return adjEnv->getPathDt2At(t); // TODO this is wrong, but will do for now
    }
    else
    {
        return toolMovement.getPath().getAccelerationAt(t);
    }
}

QQuaternion Envelope::calcAxisRotationAt(float t)
{
    if (!isTanContinuous()) return QQuaternion();
    // First rotate w.r.t. tangent continuity.
    float degrees = qRadiansToDegrees(std::acos(-getToolRadiusDaAt(0)));
    QVector3D adjNormal = adjEnv->getNormalAt(t, 1);
    QVector3D adjAxis = adjEnv->getAxisAt(t);
    QVector3D rotationAxis = QVector3D::crossProduct(adjNormal, adjAxis);
    QQuaternion rotation = QQuaternion::fromAxisAndAngle(rotationAxis, degrees);

    // Then rotate w.r.t. the last freedom: around the normal of the previous envelope.
    double angle = adjAxisAngle1 + (adjAxisAngle2-adjAxisAngle1)*t;
    QQuaternion rotationFree = QQuaternion::fromAxisAndAngle(adjNormal, angle);
    return rotationFree * rotation;
}

QVector3D Envelope::getAxisAt(float t)
{
    QVector3D axis;
    // if (!IsAxisConstrained)
    // {
        if (isTanContinuous())
        {
            // Rotate the normal of the adjacent envelope around the cross product with the axis of the adjacent envelope
            axis = calcAxisRotationAt(t) * adjEnv->getNormalAt(t, 1);
        }
        else
        {
            QVector3D axisT0 = toolMovement.getAxisDirectionAt(0);
            QVector3D axisT1 = toolMovement.getAxisDirectionAt(1);
            axis =  axisT0 + (axisT1 - axisT0)*t;
            axis.normalize();
        }
    // }
    // else
    // {
    //     axis = adjacentEnvelopeA1.GetEnvelopeAt(t, 0) - adjacentEnvelopeA0.GetEnvelopeAt(t, 1);
    //     axis.Normalize();
    // }
    return axis;
}

QVector3D Envelope::getAxisDtAt(float t)
{
    QVector3D axis, axis_t;
    // if (!IsAxisConstrained)
    // {
        if (isTanContinuous())
        {
            // Rotate the normal of the adjacent envelope around the cross product with the axis of the adjacent envelope
            axis_t = calcAxisRotationAt(t) * adjEnv->getNormalDtAt(t, 1);
        }
        else
        {
            QVector3D axisT0 = toolMovement.getAxisDirectionAt(0);
            QVector3D axisT1 = toolMovement.getAxisDirectionAt(1);
            axis =  axisT0 + (axisT1 - axisT0)*t;
            axis_t = axisT1 - axisT0;
            axis_t = MathUtility::normalVectorDerivative(axis, axis_t);
        }
    // }
    // else
    // {
    //     axis = adjacentEnvelopeA1.GetEnvelopeAt(t, 0) - adjacentEnvelopeA0.GetEnvelopeAt(t, 1);
    //     axis_t = adjacentEnvelopeA1.GetEnvelopeDtAt(t, 0) - adjacentEnvelopeA0.GetEnvelopeDtAt(t, 1);
    //     axis_t = MathUtility.NormalVectorDerivative(axis, axis_t);

    // }
    return axis_t;
}

QVector3D Envelope::getAxisDt2At(float t)
{
    // TODO should find a solution for the axis constrained and tangent continuous cases. For now works, but only in narrow cases.
    QVector3D axisT0 = toolMovement.getAxisDirectionAt(0);
    QVector3D axisT1 = toolMovement.getAxisDirectionAt(1);
    QVector3D axis =  axisT0 + (axisT1 - axisT0)*t;
    QVector3D axis_t = axisT1 - axisT0;
    QVector3D axis_tt(0,0,0);
    return MathUtility::normalVectorDerivative2(axis, axis_t, axis_tt);
}

/**
 * @brief Envelope::getToolToPathTransform Calculates the transformation to move the tool to the path. Should not really be needed to be called anywhere but Envelope::getToolTransform.
 * @return
 */
QMatrix4x4 Envelope::getToolToPathTransform() {
    QMatrix4x4 toolToPath;
    toolToPath.setToIdentity();
    toolToPath.translate(-tool->getA0() * tool->getAxisVector());
    return toolToPath;
}

/**
 * @brief Envelope::getToolAlongPathTransform Calculates the transformation to move the tool along the path. Should not really be needed to be called anywhere but Envelope::getToolTransform.
 * @return
 */
QMatrix4x4 Envelope::getToolAlongPathTransform() {
    QMatrix4x4 toolAlongPath;
    toolAlongPath.setToIdentity();
    toolAlongPath.translate(getPathAt(settings->time));
    return toolAlongPath;
}

/**
 * @brief Envelope::getToolRotationTransform Calculates the transformation to rotate the tool. Should not really be needed to be called anywhere but Envelope::getToolTransform.
 * @return
 */
QMatrix4x4 Envelope::getToolRotationTransform() {
    QMatrix4x4 toolRotation;
    if (isTanContinuous()) {
        // TODO check for circular dependencies
        if (!checkDependencies()) {
            throw new std::runtime_error("Dependency issue. Likely a circular dependency. Should be handled on input of dependency.");
        }
        QMatrix4x4 axisRotation;
        axisRotation.rotate(calcAxisRotationAt(settings->time));
        toolRotation = axisRotation * adjEnv->getToolRotationTransform();
    } else {
        toolRotation = toolMovement.getMovementRotation(settings->time);
    }
    return toolRotation;
}

/**
 * @brief Envelope::getToolTransform Calculates the transformation matrix of the Envelope's tool relative to the envelope itself. The modelTransform is separate from the toolTransform.
 * @return
 */
QMatrix4x4 Envelope::getToolTransform() {
    return getToolAlongPathTransform() * getToolRotationTransform() * getToolToPathTransform();
}
