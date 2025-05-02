#include "envelope.h"

/**
 * @brief Envelope::Envelope Creates a new envelope with default values.
 * @param settings Readonly pointer to settings
 */
Envelope::Envelope(const Settings *settings) :
    settings(settings),
    adjEnv(nullptr),
    toolMovement(nullptr),
    tool()
{
    sectorsA = 20;
    sectorsT = 50;
}

/**
 * @brief Envelope::Envelope Creates a new envelope with the given values.
 * @param settings Readonly pointer to settings
 * @param toolMovement Tool movement.
 * @param tool Tool pointer.
 */
Envelope::Envelope(const Settings *settings, CylinderMovement *toolMovement, Tool *tool) :
    settings(settings),
    adjEnv(nullptr),
    toolMovement(toolMovement),
    tool(tool)
{
    sectorsA = tool->getSectors();
    sectorsT = toolMovement->getPath().getSectors();
}

/**
 * @brief Envelope::Envelope Creates a new envelope with the given values.
 * @param settigns Readonly pointer to settings
 * @param toolMovement Tool movement.
 * @param tool Tool pointer.
 * @param adjEnvelope Adjacent envelope.
 */
Envelope::Envelope(const Settings *settings, CylinderMovement *toolMovement, Tool *tool, Envelope *adjEnvelope) :
    settings(settings),
    toolMovement(toolMovement),
    tool(tool)
{
    setAdjacentEnvelope(adjEnvelope);
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
}

void Envelope::deregisterDependent(Envelope *dependent) {
    qsizetype num = dependentEnvelopes.removeAll(dependent);
    qDebug() << "Removed" << num << "dependents";
}

bool Envelope::checkDependencies() {
    // TODO check for circular dependencies
    return true;
}

void Envelope::setToolMovement(CylinderMovement *toolMovement)
{
    this->toolMovement = toolMovement;
    sectorsT = toolMovement->getPath().getSectors();
}

void Envelope::setTool(Tool *tool)
{
    this->tool = tool;
    sectorsA = tool->getSectors();
}

void Envelope::setAdjacentEnvelope(Envelope *env){
    if (adjEnv != nullptr) this->adjEnv->deregisterDependent(this);
    adjEnv = env;
    if (env != nullptr) env->registerDependent(this);
}

/**
 * @brief Envelope::computeToolCenters Computes the vertex array of tool centers.
 */
void Envelope::computeToolCenters()
{
    vertexArrCenters.clear();
    endCurveArr.clear();
    QVector<Vertex> pathArr = toolMovement->getPath().getVertexArr();
    if(isPositContinuous())
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
        for (float a = tool->getA0(); a <= a1; a+=aDelta)
        {
            v1 = calcToolCenterAt(t, a);
            if(a+aDelta >= a1) // fix for division error
               v2 = calcToolCenterAt(t, tool->getA1());
            else
               v2 = calcToolCenterAt(t, a+aDelta);


            // Add vertices to array
            vertexArrCenters.append(Vertex(v1, color));
            vertexArrCenters.append(Vertex(v2, color));

            // Compute the path of an envelope with respect to its adjacent one
            if(isPositContinuous() && a == tool->getA0()){
                pathArr.append(Vertex(v1, color));
            }
            if(a+aDelta >= a1){
                endCurveArr.append(v2);
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
    for (float t = path.getT0(); t <= t1; t+=tDelta)
    {
        for (float a = tool->getA0(); a <= a1; a+=aDelta)
        {
            v1 = calcGrazingCurveAt(t, a);
            if(a+aDelta >= a1) // fix for division error
                v2 = calcGrazingCurveAt(t, tool->getA1());
            else
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
    for (float t = path.getT0(); t <= t1; t+=tDelta)
    {
        for (float a = tool->getA0(); a <= a1; a+=aDelta)
        {
            v1 = calcEnvelopeAt(t, a);
            v3 = calcEnvelopeAt(t+tDelta, a);
            if(a+aDelta >= a1) { // fix for division error
                v2 = calcEnvelopeAt(t, tool->getA1());
                v4 = calcEnvelopeAt(t+tDelta, tool->getA1());
            } else {
                v2 = calcEnvelopeAt(t, a+aDelta);
                v4 = calcEnvelopeAt(t+tDelta, a+aDelta);
            }

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
    for (float t = path.getT0(); t <= t1; t+=tDelta)
    {
        normals.clear();
        for (float a = tool->getA0(); a <= a1; a+=aDelta)
        {
            p1 = calcToolCenterAt(t,a);
            v1 = p1 + tool->getRadiusAt(a)*computeNormal(t, a);
            if(a+aDelta >= a1) { // fix for division error
                p2 = calcToolCenterAt(t, tool->getA1());
                v2 = p2 + tool->getRadiusAt(a)*computeNormal(t, tool->getA1());
            } else {
                p2 = calcToolCenterAt(t, a+aDelta);
                v2 = p2 + tool->getRadiusAt(a)*computeNormal(t, a+aDelta);
            }

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
    if (isPositContinuous()){
        // use positional continuity formula
        QVector3D adjCenter = adjEnv->endCurveArr[adjEnv->toolMovement->getPath().getIdxAtTime(t)];
        float rAdjEnv = adjEnv->tool->getRadiusAt(adjEnv->tool->getA1());
        float rEnv = tool->getRadiusAt(tool->getA0());
        float a0 = tool->getA0();
        QVector3D axis = calcToolAxisDirecAt(t);
        return adjCenter + rAdjEnv*adjEnv->computeNormal(t,adjEnv->tool->getA1()) - rEnv*computeNormal(t,tool->getA0());
    } else {
        SimplePath path = toolMovement->getPath();
        return path.getPathAt(t);
    }
}

/**
 * @brief Envelope::getPathTangentAt Returns the vector tangent to the curve that defines the tool path at time t.
 * @param t Time
 * @return Tangent vector at t
 */
QVector3D Envelope::getPathTangentAt(float t){
    SimplePath path;
    Envelope *env;
    if (isPositContinuous()){
        // compute according to discrete path obtained from the adjacent path formula
        path = adjEnv->toolMovement->getPath();
        env = adjEnv;
    } else {
        path = toolMovement->getPath();
        env = this;
    }
    // this tangent can be computed exactly but is done as an approximation for congruence
    float tDelta = (path.getT1()-path.getT0())/sectorsT;
    if(t == path.getT1()) {
        return (env->getPathAt(t) - env->getPathAt(t-tDelta));
    } else {
        return (env->getPathAt(t+tDelta) - env->getPathAt(t));
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

    // AxisRateOfChange is a direction, if AxisRateOfChange is not normalized then the grazing curves are ruled
    return axis.normalized();
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
    QVector3D normal = adjEnv->computeNormal(time,adjEnv->tool->getA1());
    QVector3D rotationAxis = QVector3D::crossProduct(normal, axis);
    // I first compute the rotation such that the axis aligns with the normal
    double ra = qRadiansToDegrees(-acos(QVector3D::dotProduct(normal, toolMovement->getAxisDirectionAt(time))));
    if(ra != 0) {
        rotation.rotate(ra, rotationAxis);
    }
    // then we can rotate the axis to its place according to our formula A(t) dot n1(a1,t) = ra(a0)
    rotationAxis = QVector3D::crossProduct(normal, adjEnv->toolMovement->getAxisDirectionAt(time));
    ra = qRadiansToDegrees(acos(-(tool->getRadiusDerivativeWRTa(tool->getA0()))));
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
    QVector3D center = calcToolCenterAt(t, a);

    float r = tool->getRadiusAt(a) + 0.0001; // + 0.0001 to avoid floating point weirdness
    QVector3D normal = computeNormal(t, a);
    QVector3D posit = center + r*normal;

    QVector3D color;
    if (settings->reflectionLines){
        float alpha;
        alpha = acos(QVector3D::dotProduct(normal,QVector3D(1,0,0)));
        float aux = alpha * settings->reflFreq;
        if (aux -(int)aux <= settings->percentBlack)
            color = QVector3D(0,0,0);
        else
            color = QVector3D(1,1,1);
    } else {
        color = normal;
    }

    return Vertex(posit, color);
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
    QVector3D normal = computeNormal(t, a);
    QVector3D posit = center + r*normal;

    return posit;
}

/**
 * @brief Envelope::computeNormal Computes the normal of the envelope at a given time and axis height.
 * @param t Time.
 * @param a Axis height.
 * @return The normal.
 */
QVector3D Envelope::computeNormal(float t, float a)
{
    QVector3D sa = calcToolAxisDirecAt(t);
    float a0 = tool->getA0();
    QVector3D st = getPathTangentAt(t) + (a-a0)*calcAxisRateOfChange(t);
    QVector3D sNorm = QVector3D::crossProduct(sa, st).normalized();

    // Calculate alpha, beta, gamma
    float alpha, beta, gamma;
    float ra = tool->getRadiusDerivativeWRTa(a);
    double matDet = (QVector3D::dotProduct(sa,sa) * QVector3D::dotProduct(st,st))
                    - (QVector3D::dotProduct(sa,st) * QVector3D::dotProduct(st,sa));
    if (1/matDet != 1/matDet)
        qDebug() << "determinant problem at (" << a << ", " << t <<"): det = " << matDet <<", st = " << st <<", sa = " << sa;
    double a11 = 1/matDet * QVector3D::dotProduct(st,st);
    alpha = a11 * (-ra);
    double a21 = -1/matDet * QVector3D::dotProduct(st,sa);
    beta = a21 * (-ra);
    gamma = (matDet>0 ? 1 : -1)*sqrt(1-ra*ra*a11);
    // adjEnv != adjEnv to focus on the first envelope only
    if (gamma != gamma && adjEnv != adjEnv)
        qDebug() << "alpha: " << alpha << " beta: " << beta << " gamma: " << gamma << " det: " << matDet;

    QVector3D normal = alpha*sa + beta*st + gamma*sNorm;
    normal.normalize();
    return normal;
}
