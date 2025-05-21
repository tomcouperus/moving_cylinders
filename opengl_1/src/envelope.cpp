#include "envelope.h"
#include "mathutility.h"

/**
 * @brief Envelope::Envelope Creates a new envelope with default values.
 * @param index
 */
Envelope::Envelope(int index) :
    index(index),
    adjEnv(nullptr),
    tool()
{
    sectorsA = 20;
    sectorsT = 50;
}

/**
 * @brief Envelope::Envelope Creates a new envelope with the given values.
 * @param tool Tool pointer.
 */
Envelope::Envelope(int index, Tool *tool, const SimplePath &path) :
    index(index),
    adjEnv(nullptr),
    tool(tool),
    toolMovement(path, tool)
{
    // TODO: envelope controls everyone else's sectors. They shouldn't even HAVE sectors.
    sectorsA = tool->getSectors();
    sectorsT = toolMovement.getPath().getSectors();
}

/**
 * @brief Envelope::Envelope Creates a new envelope with the given values.
 * @param tool Tool pointer.
 * @param adjEnvelope Adjacent envelope.
 */
Envelope::Envelope(int index, Tool *tool, const SimplePath &path, Envelope *adjEnvelope) :
    index(index),
    tool(tool),
    toolMovement(path, tool)
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
    computeEnvelope();
    computeToolCenters();
    computeGrazingCurves();
    computeNormals();
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
    QVector<Envelope *> queue(dependentEnvelopes);
    while (queue.size() > 0) {
        Envelope *env = queue.takeFirst();
        for (int i = 0; i < env->dependentEnvelopes.size(); i++) {
            if (env->dependentEnvelopes[i] == this) {
                return false;
            }
            queue.append(env->dependentEnvelopes[i]);
        }
    }
    return true;
}

/**
 * @brief Envelope::getAllDependents Gathers all envelopes dependent on this one. Returns empty set on depency error.
 * @return
 */
QSet<int> Envelope::getAllDependents() {
    QSet<int> dependencySet;
    QVector<Envelope*> queue;
    if (!checkDependencies()) {
        return dependencySet;
    }
    queue.append(this);
    while (queue.size() > 0) {
        Envelope *env = queue.takeFirst();
        dependencySet += env->index;
        queue += env->dependentEnvelopes;
    }
    return dependencySet;
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

            float t = (float) tIdx / sectorsT;
            float a = (float) aIdx / sectorsA;

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
    return getPathAt(t) + a * tool->getHeight() * getAxisAt(t) + getToolRadiusAt(a) * getNormalAt(t, a);
}

QVector3D Envelope::getEnvelopeDtAt(float t, float a)
{
    return getPathDtAt(t) + a * tool->getHeight() * getAxisDtAt(t) + getToolRadiusAt(a) * getNormalDtAt(t, a);
}

QVector3D Envelope::getEnvelopeDt2At(float t, float a)
{
    return getPathDt2At(t) + a * tool->getHeight() * getAxisDt2At(t) + getToolRadiusAt(a) * getNormalDt2At(t, a);
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

        float t = (float) tIdx / sectorsT;

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

            float t = (float) tIdx / sectorsT;
            float a = (float) aIdx / sectorsA;

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
            float t = (float) tIdx / sectorsT;
            float a = (float) aIdx / sectorsA;
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
    QVector3D sa = tool->getHeight() * getAxisAt(t);
    QVector3D st = getPathDtAt(t) + a * tool->getHeight() * getAxisDtAt(t);
    QVector3D sNormal = QVector3D::crossProduct(sa, st).normalized();

    float ra = getToolRadiusDaAt(a);

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
    QVector3D sa = tool->getHeight() * getAxisAt(t);
    QVector3D sat = tool->getHeight() * getAxisDtAt(t);
    QVector3D st = getPathDtAt(t) + a * tool->getHeight() * getAxisDtAt(t);
    QVector3D stt = getPathDt2At(t) + a * tool->getHeight() * getAxisDt2At(t);
    QVector3D sNormal = QVector3D::crossProduct(sa, st).normalized();
    QVector3D sNormal_t = MathUtility::normalVectorDerivative(QVector3D::crossProduct(sa, st), QVector3D::crossProduct(sat, st) + QVector3D::crossProduct(sa, stt));

    float ra = getToolRadiusDaAt(a);

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

QVector3D Envelope::getNormalDt2At(float t, float a)
{
    QVector3D sa = tool->getHeight() * getAxisAt(t);
    QVector3D sat = tool->getHeight() * getAxisDtAt(t);
    QVector3D satt = tool->getHeight() * getAxisDt2At(t);
    QVector3D st = getPathDtAt(t) + a * tool->getHeight() * getAxisDtAt(t);
    QVector3D stt = getPathDt2At(t) + a * tool->getHeight() * getAxisDt2At(t);
    QVector3D sttt = getPathDt3At(t) + a * tool->getHeight() * getAxisDt3At(t);
    QVector3D sNormal = QVector3D::crossProduct(sa, st);
    QVector3D sNormal_t = QVector3D::crossProduct(sat, st) + QVector3D::crossProduct(sa, stt);
    QVector3D sNormal_tt = QVector3D::crossProduct(satt, st) + QVector3D::crossProduct(sat, stt) + QVector3D::crossProduct(sat, stt) + QVector3D::crossProduct(sa, sttt);
    sNormal_tt = MathUtility::normalVectorDerivative2(sNormal, sNormal_t, sNormal_tt);
    sNormal_t = MathUtility::normalVectorDerivative(sNormal, sNormal_t);
    sNormal.normalize();

    float ra = getToolRadiusDaAt(a);

    float E = QVector3D::dotProduct(sa, sa);
    float Et = 2 * QVector3D::dotProduct(sa, sat);
    float Ett = 2 * (QVector3D::dotProduct(sat, sat) + QVector3D::dotProduct(sa, satt));
    float F = QVector3D::dotProduct(sa, st);
    float Ft = QVector3D::dotProduct(sat, st) + QVector3D::dotProduct(sa, stt);
    float Ftt = QVector3D::dotProduct(satt, st) + QVector3D::dotProduct(sat, stt) + QVector3D::dotProduct(sat, stt) + QVector3D::dotProduct(sa, sttt);
    float G = QVector3D::dotProduct(st, st);
    float Gt = 2 * QVector3D::dotProduct(st, stt);
    float Gtt = 2 * (QVector3D::dotProduct(stt, stt) + QVector3D::dotProduct(st, sttt));
    float EG_FF = E * G - F * F;
    float EG_FF_2 = EG_FF * EG_FF;
    float EG_FF_t = Et * G + E * Gt - 2 * F * Ft;
    float EG_FF_2_t = 2 * EG_FF * EG_FF_t;
    float EG_FF_tt = Ett * G + Et * Gt + Et * Gt + E * Gtt - 2 * (Ft * Ft + F * Ftt);

    float m11 = G / EG_FF;
    float m11_t = (EG_FF * Gt - G * EG_FF_t) / EG_FF_2;
    float m11_tt = (EG_FF_2 * ((EG_FF_t * Gt + EG_FF * Gtt) - (Gt * EG_FF_t + G * EG_FF_tt)) - (EG_FF * Gt - G * EG_FF_t) * EG_FF_2_t) / (EG_FF_2 * EG_FF_2);
    float m21 = -F / EG_FF;
    float m21_t = -(EG_FF * Ft - F * EG_FF_t) / EG_FF_2;
    float m21_tt = -(EG_FF_2 * ((EG_FF_t * Ft + EG_FF * Ftt) - (Ft * EG_FF_t + F * EG_FF_tt)) - (EG_FF * Ft - F * EG_FF_t) * EG_FF_2_t) / (EG_FF_2 * EG_FF_2);

    float alpha = -m11 * ra;
    float alpha_t = -m11_t * ra;
    float alpha_tt = -m11_tt * ra;
    float beta = -m21 * ra;
    float beta_t = -m21_t * ra;
    float beta_tt = -m21_tt * ra;
    float gamma = (EG_FF > 0 ? 1 : -1) * sqrt(1 - ra * ra * m11);
    float gamma_t = (EG_FF > 0 ? 1 : -1) * -ra * ra * m11_t / (2 * sqrt(1 - ra * ra * m11));
    float gamma_tt = (EG_FF > 0 ? 1 : -1) *
                     (2 * sqrt(1 - ra * ra * m11) * -ra * ra * m11_tt - ra * ra * ra * ra * m11_t * m11_t / sqrt(1 - ra * ra * m11)) /
                     (4 - 4 * ra * ra * m11);

    QVector3D n = alpha * sa +
                beta * st +
                gamma * sNormal;
    QVector3D nt = alpha * sat + alpha_t * sa +
                 beta * stt + beta_t * st +
                 gamma * sNormal_t + gamma_t * sNormal;
    QVector3D ntt = alpha_t * sat + alpha * satt + alpha_tt * sa + alpha_t * sat +
                  beta_t * stt + beta * sttt + beta_tt * st + beta_t * st +
                  gamma_t * sNormal_t + gamma * sNormal_tt + gamma_tt * sNormal + gamma_t * sNormal_t;
    return MathUtility::normalVectorDerivative2(n, nt, ntt);
}

/**
 * @brief Envelope::getPathAt Returns the path of the tool movement at a given time.
 * @param t Time.
 * @return Path at time t.
 */
QVector3D Envelope::getPathAt(float t){
    if (isTanContinuous())
    {
        return adjEnv->getEnvelopeAt(t, 1) - getToolRadiusAt(0) * adjEnv->getNormalAt(t, 1);
    }
    else if (isPositContinuous())
    {
        // The normal is orthogonal to X_t of the adjacent envelope, and at an angle to the axis of its own envelope.
        // Thus, start with the normal of the adjacent envelope (also orthogonal to X_t of the adjacent envelope by definition),
        // and rotate it around X_t of the adjacent envelope until the angle with the axis is achieved.
        // TODO does not actually work yet. Only works for cylindrical case for now, but that is enough for my purposes. If time allows, will expand.
        QVector3D normal = QVector3D::crossProduct(getAxisAt(t), adjEnv->getEnvelopeDtAt(t, 1)).normalized();
        // QVector3D normal = getNormalRotation(t) * adjEnv->getNormalAt(t, 1);
        return adjEnv->getEnvelopeAt(t, 1) - getToolRadiusAt(0) * normal;
    }
    else
    {
        return toolMovement.getPath().getPathAt(t);
    }
}

/**
 * @brief Envelope::getPathDtAt Returns the tangent of path of the tool movement at a given time.
 * @param t Time.
 * @return Path tangent at time t.
 */
QVector3D Envelope::getPathDtAt(float t){
    if (isTanContinuous())
    {
        return adjEnv->getEnvelopeDtAt(t, 1) - getToolRadiusAt(0) * adjEnv->getNormalDtAt(t, 1);
    }
    else if (isPositContinuous())
    {
        // TODO only works for cylindrical tool, due to the angle between the normal and the axis. General should be possible though
        QVector3D axis = getAxisAt(t);
        QVector3D axis_t = getAxisDtAt(t);
        QVector3D adjEnv_t = adjEnv->getEnvelopeDtAt(t, 1);
        QVector3D adjEnv_tt = adjEnv->getEnvelopeDt2At(t, 1);
        QVector3D normal = QVector3D::crossProduct(axis, adjEnv_t);
        QVector3D normal_t = QVector3D::crossProduct(axis_t, adjEnv_t) + QVector3D::crossProduct(axis, adjEnv_tt);
        normal_t = MathUtility::normalVectorDerivative(normal, normal_t);
        // QVector3D adjEnv_t = adjEnv->getEnvelopeDtAt(t, 1);
        // QVector3D normal_t = getNormalRotation(t) * adjEnv->getNormalDtAt(t, 1);
        return adjEnv_t - getToolRadiusAt(0) * normal_t;
    }
    else
    {
        return toolMovement.getPath().getDerivativeAt(t);
    }
}

/**
 * @brief Envelope::getPathDt2At Returns the acceleration of path of the tool movement at a given time.
 * @param t Time.
 * @return Path acceleration at time t.
 */
QVector3D Envelope::getPathDt2At(float t){
    // Need to check if these are required for chaining position continuous envelopes
    if (isTanContinuous())
    {
        return adjEnv->getEnvelopeDt2At(t, 1) - getToolRadiusAt(0) * adjEnv->getNormalDt2At(t, 1);
    }
    else
    {
        return toolMovement.getPath().getDerivative2At(t);
    }
}

/**
 * @brief Envelope::getPathDt3At Returns the third derivative of path of the tool movement at a given time.
 * @param t Time.
 * @return Path third derivative at time t.
 */
QVector3D Envelope::getPathDt3At(float t){
    // Need to check if these are required for chaining position continuous envelopes
    return toolMovement.getPath().getDerivative3At(t);
}

QQuaternion Envelope::calcAxisRotationAt(float t)
{
    if (!isTanContinuous()) return QQuaternion();
    // First rotate the axis of the previous envelope to its normal.
    // This is to establish a frame of reference for all its derivatives.
    QVector3D adjNormal = adjEnv->getNormalAt(t, 1);
    QVector3D adjAxis = adjEnv->getAxisAt(t);
    QQuaternion rotationFrame = QQuaternion::rotationTo(adjAxis, adjNormal);

    // Then rotate w.r.t. tangent continuity. Which rotates around the cross product of the adjacent normal and axis
    float degrees = qRadiansToDegrees(acos(-getToolRadiusDaAt(0)));
    QVector3D rotationAxis = QVector3D::crossProduct(adjNormal, adjAxis);
    QQuaternion rotationTangent = QQuaternion::fromAxisAndAngle(rotationAxis, degrees);

    // Lastly rotate w.r.t. the last freedom: around the normal of the previous envelope.
    double angle = adjAxisAngle1 + (adjAxisAngle2-adjAxisAngle1)*t;
    QQuaternion rotationFree = QQuaternion::fromAxisAndAngle(adjNormal, angle);
    return rotationFree * rotationTangent * rotationFrame;
}

QVector3D Envelope::getAxisAt(float t)
{
    QVector3D axis;
    // if (IsAxisConstrained)
    // {
    //     axis = adjacentEnvelopeA1.getEnvelopeAt(t, 0) - adjEnv->getEnvelopeAt(t, 1);
    //     axis.normalize();
    // }
    // else
        if (isTanContinuous())
    {
        // Rotate the normal of the adjacent envelope around the cross product with the axis of the adjacent envelope
        axis = calcAxisRotationAt(t) * adjEnv->getAxisAt(t);
    }
    else
    {
        axis = toolMovement.getAxisAt(t);
    }
    return axis;
}

QVector3D Envelope::getAxisDtAt(float t)
{
    QVector3D axis, axis_t;
    // if (IsAxisConstrained)
    // {
    //     axis = adjacentEnvelopeA1.getEnvelopeAt(t, 0) - adjEnv->getEnvelopeAt(t, 1);
    //     axis_t = adjacentEnvelopeA1.getEnvelopeDtAt(t, 0) - adjEnv->getEnvelopeDtAt(t, 1);
    //     axis_t = MathUtility::normalVectorDerivative(axis, axis_t);
    // }
    // else
        if (isTanContinuous())
    {
        // TODO There is still a slight div by 0 error here.
        axis_t = calcAxisRotationAt(t) * adjEnv->getAxisDtAt(t);
    }
    else
    {
        axis_t = toolMovement.getAxisDtAt(t);
    }
    return axis_t;
}

QVector3D Envelope::getAxisDt2At(float t)
{
    QVector3D axis, axis_t, axis_tt;
    // TODO should find a solution for the axis constrained and tangent continuous cases. For now works, but only in narrow cases.
    if (isTanContinuous())
    {
        axis_tt = calcAxisRotationAt(t) * adjEnv->getAxisDt2At(t);
    }
    else
    {
        axis_tt = toolMovement.getAxisDt2At(t);
    }
    return axis_tt;
}

QVector3D Envelope::getAxisDt3At(float t)
{
    // TODO should find a solution for the axis constrained and tangent continuous cases. For now works, but only in narrow cases.
    QVector3D axis, axis_t, axis_tt, axis_ttt;
    if (isTanContinuous())
    {
        axis_ttt = calcAxisRotationAt(t) * adjEnv->getAxisDt3At(t);
    }
    else
    {
        axis_ttt = toolMovement.getAxisDt3At(t);
    }
    return axis_ttt;
}

float Envelope::getToolRadiusAt(float a) {
    return tool->getRadiusAt(a);
}

float Envelope::getToolRadiusDaAt(float a) {
    return tool->getRadiusDerivativeWRTa(a);
}

// /**
//  * @brief Envelope::getToolToPathTransform Calculates the transformation to move the tool to the path. Should not really be needed to be called anywhere but Envelope::getToolTransform.
//  * @return
//  */
// QMatrix4x4 Envelope::getToolToPathTransform() {
//     QMatrix4x4 toolToPath;
//     toolToPath.setToIdentity();
//     toolToPath.translate(-tool->getA0() * tool->getAxisVector());
//     return toolToPath;
// }

/**
 * @brief Envelope::getToolTransform Calculates the transformation matrix of the Envelope's tool relative to the envelope itself. The modelTransform is separate from the toolTransform.
 * @return
 */
QMatrix4x4 Envelope::getToolTransformAt(float t) {
    // First rotate such that the tool's axis aligns with the surface axis
    QVector3D toolAxis = tool->getAxisVector();
    QMatrix4x4 rotation;
    rotation.rotate(QQuaternion::rotationTo(toolAxis, getAxisAt(t)));

    // Then translate it to the path
    QMatrix4x4 translation;
    translation.translate(getPathAt(t));
    return translation * rotation;
}
