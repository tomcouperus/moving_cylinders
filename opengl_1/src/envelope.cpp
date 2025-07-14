#include "envelope.h"
#include "mathutility.h"

/**
 * @brief Envelope::Envelope Creates a new envelope with default values.
 * @param index
 */
Envelope::Envelope(int index) :
    index(index),
    adjEnvA0(nullptr),
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
    adjEnvA0(nullptr),
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
    setAdjacentA0Envelope(adjEnvelope);
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

void Envelope::setAdjacentA0Envelope(Envelope *env){
    if (adjEnvA0 != nullptr) this->adjEnvA0->deregisterDependent(this);
    adjEnvA0 = env;
    if (env != nullptr) env->registerDependent(this);
}

void Envelope::setAdjacentA1Envelope(Envelope *env){
    if (adjEnvA1 != nullptr) this->adjEnvA1->deregisterDependent(this);
    adjEnvA1 = env;
    if (env != nullptr) env->registerDependent(this);
}

/**
 * @brief Envelope::computeEnvelope Computes the vertex array of the envelope.
 */
void Envelope::computeEnvelope()
{
    vertexArr.clear();

    QVector3D env[4];
    QVector3D norm[4];
    QVector3D col[4];
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

            env[0] = getEnvelopeAt(t, a);
            env[1] = getEnvelopeAt(t, a+aDelta);
            env[2] = getEnvelopeAt(t+tDelta, a);
            env[3] = getEnvelopeAt(t+tDelta, a+aDelta);

            norm[0] = getNormalAt(t, a);
            norm[1] = getNormalAt(t, a+aDelta);
            norm[2] = getNormalAt(t+tDelta, a);
            norm[3] = getNormalAt(t+tDelta, a+aDelta);

            for (int i = 0; i < 4; i++) {
                if (reflectionLines){
                    float alpha;
                    alpha = acos(QVector3D::dotProduct(norm[i],QVector3D(1,0,0)));
                    float aux = alpha * reflFreq;
                    if (aux -(int)aux <= percentBlack)
                        col[i] = QVector3D(0,0,0);
                    else
                        col[i] = QVector3D(1,1,1);
                } else {
                    col[i] = norm[i];
                }
            }

            Vertex v1{env[0], col[0]};
            Vertex v2{env[1], col[1]};
            Vertex v3{env[2], col[2]};
            Vertex v4{env[3], col[3]};

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
    return getPathAt(t) + tool->getSphereCenterHeightAt(a) * getAxisAt(t) + tool->getSphereRadiusAt(a) * getNormalAt(t, a);
}

QVector3D Envelope::getEnvelopeDtAt(float t, float a)
{
    return getPathDtAt(t) + tool->getSphereCenterHeightAt(a) * getAxisDtAt(t) + tool->getSphereRadiusAt(a) * getNormalDtAt(t, a);
}

QVector3D Envelope::getEnvelopeDt2At(float t, float a)
{
    return getPathDt2At(t) + tool->getSphereCenterHeightAt(a) * getAxisDt2At(t) + tool->getSphereRadiusAt(a) * getNormalDt2At(t, a);
}

QVector3D Envelope::getEnvelopeDt3At(float t, float a)
{
    return getPathDt3At(t) + tool->getSphereCenterHeightAt(a) * getAxisDt3At(t) + tool->getSphereRadiusAt(a) * getNormalDt3At(t, a);
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
        v2 = getPathAt(t) + tool->getHeight() * getAxisAt(t);

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
    QVector<Vertex> normals;
    for (int tIdx = 0; tIdx <= sectorsT; tIdx++)
    {
        normals.clear();
        for (int aIdx = 0; aIdx <= sectorsA; aIdx++)
        {
            float t = (float) tIdx / sectorsT;
            float a = (float) aIdx / sectorsA;
            p1 = getPathAt(t) + tool->getSphereCenterHeightAt(a)*getAxisAt(t);
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
    QVector3D sa = tool->getSphereCenterHeightDaAt(a) * getAxisAt(t);
    QVector3D st = getPathDtAt(t) + tool->getSphereCenterHeightAt(a) * getAxisDtAt(t);
    QVector3D sNormal = QVector3D::crossProduct(sa, st).normalized();

    float ra = tool->getSphereRadiusDaAt(a);

    float E = QVector3D::dotProduct(sa, sa);
    float F = QVector3D::dotProduct(sa, st);
    float G = QVector3D::dotProduct(st, st);
    float EG_FF = E * G - F * F;

    float m11 = G / EG_FF;
    float m21 = -F / EG_FF;

    float alpha = -m11 * ra;
    float beta = -m21 * ra;

    float sqrt_term = 1 - ra * ra * m11;
    float gamma = (EG_FF > 0 ? 1 : -1) * sqrt(sqrt_term);

    QVector3D n = alpha * sa + beta * st + gamma * sNormal;
    return n.normalized();
}

QVector3D Envelope::getNormalDtAt(float t, float a)
{
    QVector3D sa = tool->getSphereCenterHeightDaAt(a) * getAxisAt(t);
    QVector3D sat = tool->getSphereCenterHeightDaAt(a) * getAxisDtAt(t);
    QVector3D st = getPathDtAt(t) + tool->getSphereCenterHeightAt(a) * getAxisDtAt(t);
    QVector3D stt = getPathDt2At(t) + tool->getSphereCenterHeightAt(a) * getAxisDt2At(t);
    QVector3D sNormal = QVector3D::crossProduct(sa, st).normalized();
    QVector3D sNormal_t = MathUtility::normalVectorDerivative(QVector3D::crossProduct(sa, st), QVector3D::crossProduct(sat, st) + QVector3D::crossProduct(sa, stt));

    float ra = tool->getSphereRadiusDaAt(a);

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

    float sqrt_term = 1 - ra * ra * m11;
    float gamma = (EG_FF > 0 ? 1 : -1) * sqrt(sqrt_term);
    float gamma_t = (EG_FF > 0 ? 1 : -1) * -ra * ra * m11_t / (2 * sqrt(sqrt_term));

    QVector3D n = alpha * sa + beta * st + gamma * sNormal;
    QVector3D nt = alpha * sat + alpha_t * sa + beta * stt + beta_t * st + gamma * sNormal_t + gamma_t * sNormal;
    return MathUtility::normalVectorDerivative(n, nt);
}

QVector3D Envelope::getNormalDt2At(float t, float a)
{
    QVector3D sa = tool->getSphereCenterHeightDaAt(a) * getAxisAt(t);
    QVector3D sat = tool->getSphereCenterHeightDaAt(a) * getAxisDtAt(t);
    QVector3D satt = tool->getSphereCenterHeightDaAt(a) * getAxisDt2At(t);
    QVector3D st = getPathDtAt(t) + tool->getSphereCenterHeightAt(a) * getAxisDtAt(t);
    QVector3D stt = getPathDt2At(t) + tool->getSphereCenterHeightAt(a) * getAxisDt2At(t);
    QVector3D sttt = getPathDt3At(t) + tool->getSphereCenterHeightAt(a) * getAxisDt3At(t);
    QVector3D sNormal = QVector3D::crossProduct(sa, st);
    QVector3D sNormal_t = QVector3D::crossProduct(sat, st) + QVector3D::crossProduct(sa, stt);
    QVector3D sNormal_tt = QVector3D::crossProduct(satt, st) + QVector3D::crossProduct(sat, stt) + QVector3D::crossProduct(sat, stt) + QVector3D::crossProduct(sa, sttt);
    sNormal_tt = MathUtility::normalVectorDerivative2(sNormal, sNormal_t, sNormal_tt);
    sNormal_t = MathUtility::normalVectorDerivative(sNormal, sNormal_t);
    sNormal.normalize();

    float ra = tool->getSphereRadiusDaAt(a);

    float E = QVector3D::dotProduct(sa, sa);
    float Et = 2 * QVector3D::dotProduct(sa, sat);
    float Ett = 2 * QVector3D::dotProduct(sat, sat) + 2 * QVector3D::dotProduct(sa, satt);
    float F = QVector3D::dotProduct(sa, st);
    float Ft = QVector3D::dotProduct(sat, st) + QVector3D::dotProduct(sa, stt);
    float Ftt = QVector3D::dotProduct(satt, st) + 2 * QVector3D::dotProduct(sat, stt) + QVector3D::dotProduct(sa, sttt);
    float G = QVector3D::dotProduct(st, st);
    float Gt = 2 * QVector3D::dotProduct(st, stt);
    float Gtt = 2 * QVector3D::dotProduct(stt, stt) + 2 * QVector3D::dotProduct(st, sttt);
    float EG_FF = E * G - F * F;
    float EG_FF_2 = EG_FF * EG_FF;
    float EG_FF_3 = EG_FF * EG_FF * EG_FF;
    float EG_FF_t = Et * G + E * Gt - 2 * F * Ft;
    float EG_FF_tt = Ett * G + 2 * Et * Gt + E * Gtt - 2 * (Ft * Ft + F * Ftt);

    float m11 = G / EG_FF;
    float m11_t = (EG_FF * Gt - G * EG_FF_t) / EG_FF_2;
    float m11_tt = (Gtt * EG_FF_2 -
                    2 * Gt * EG_FF * EG_FF_t -
                    G * EG_FF * EG_FF_tt +
                    2 * G * EG_FF_t * EG_FF_t) / EG_FF_3;

    float m21 = -F / EG_FF;
    float m21_t = -(EG_FF * Ft - F * EG_FF_t) / EG_FF_2;
    float m21_tt = -(Ftt * EG_FF_2 -
                     2 * Ft * EG_FF * EG_FF_t -
                     F * EG_FF * EG_FF_tt +
                     2 * F * EG_FF_t * EG_FF_t) / EG_FF_3;

    float alpha = -m11 * ra;
    float alpha_t = -m11_t * ra;
    float alpha_tt = -m11_tt * ra;

    float beta = -m21 * ra;
    float beta_t = -m21_t * ra;
    float beta_tt = -m21_tt * ra;

    float sqrt_term = 1 - ra * ra * m11;
    float gamma = (EG_FF > 0 ? 1 : -1) * sqrt(sqrt_term);
    float gamma_t = (EG_FF > 0 ? 1 : -1) * -ra * ra * m11_t / (2 * sqrt(sqrt_term));
    float gamma_tt = (EG_FF > 0 ? 1 : -1) *
                     -ra * ra / 2 *
                     (m11_tt / sqrt(sqrt_term) +
                      (ra * ra * m11_t * m11_t) / (2 * pow(sqrt_term, 1.5f)));

    QVector3D n = alpha * sa +
                beta * st +
                gamma * sNormal;
    QVector3D nt = alpha * sat + alpha_t * sa +
                 beta * stt + beta_t * st +
                 gamma * sNormal_t + gamma_t * sNormal;
    QVector3D ntt = alpha * satt +
                  2 * alpha_t * sat +
                  alpha_tt * sa +
                  beta * sttt +
                  2 * beta_t * stt +
                  beta_tt * st +
                  gamma * sNormal_tt +
                  2 * +gamma_t * sNormal_t +
                  gamma_tt * sNormal;
    return MathUtility::normalVectorDerivative2(n, nt, ntt);
}

QVector3D Envelope::getNormalDt3At(float t, float a)
{
    QVector3D sa = tool->getSphereCenterHeightDaAt(a) * getAxisAt(t);
    QVector3D sat = tool->getSphereCenterHeightDaAt(a) * getAxisDtAt(t);
    QVector3D satt = tool->getSphereCenterHeightDaAt(a) * getAxisDt2At(t);
    QVector3D sattt = tool->getSphereCenterHeightDaAt(a) * getAxisDt3At(t);
    QVector3D st = getPathDtAt(t) + tool->getSphereCenterHeightAt(a) * getAxisDtAt(t);
    QVector3D stt = getPathDt2At(t) + tool->getSphereCenterHeightAt(a) * getAxisDt2At(t);
    QVector3D sttt = getPathDt3At(t) + tool->getSphereCenterHeightAt(a) * getAxisDt3At(t);
    QVector3D stttt = getPathDt4At(t) + tool->getSphereCenterHeightAt(a) * getAxisDt4At(t);
    QVector3D sNormal = QVector3D::crossProduct(sa, st);
    QVector3D sNormal_t = QVector3D::crossProduct(sat, st) + QVector3D::crossProduct(sa, stt);
    QVector3D sNormal_tt = QVector3D::crossProduct(satt, st) + 2 * QVector3D::crossProduct(sat, stt) + QVector3D::crossProduct(sa, sttt);
    QVector3D sNormal_ttt = QVector3D::crossProduct(sattt, st) + 3 * QVector3D::crossProduct(satt, stt) + 3 * QVector3D::crossProduct(sat, sttt) + QVector3D::crossProduct(sa, stttt);
    sNormal_ttt = MathUtility::normalVectorDerivative3(sNormal, sNormal_t, sNormal_tt, sNormal_ttt);
    sNormal_tt = MathUtility::normalVectorDerivative2(sNormal, sNormal_t, sNormal_tt);
    sNormal_t = MathUtility::normalVectorDerivative(sNormal, sNormal_t);
    sNormal.normalize();

    float ra = tool->getSphereRadiusDaAt(a);

    float E = QVector3D::dotProduct(sa, sa);
    float Et = 2 * QVector3D::dotProduct(sa, sat);
    float Ett = 2 * QVector3D::dotProduct(sat, sat) + 2 * QVector3D::dotProduct(sa, satt);
    float Ettt = 6 * QVector3D::dotProduct(sat, satt) + 2 * QVector3D::dotProduct(sa, sattt);
    float F = QVector3D::dotProduct(sa, st);
    float Ft = QVector3D::dotProduct(sat, st) + QVector3D::dotProduct(sa, stt);
    float Ftt = QVector3D::dotProduct(satt, st) + 2 * QVector3D::dotProduct(sat, stt) + QVector3D::dotProduct(sa, sttt);
    float Fttt = QVector3D::dotProduct(sattt, st) + 3 * QVector3D::dotProduct(satt, stt) + 3 * QVector3D::dotProduct(sat, sttt) + QVector3D::dotProduct(sa, stttt);
    float G = QVector3D::dotProduct(st, st);
    float Gt = 2 * QVector3D::dotProduct(st, stt);
    float Gtt = 2 * QVector3D::dotProduct(stt, stt) + 2 * QVector3D::dotProduct(st, sttt);
    float Gttt = 6 * QVector3D::dotProduct(stt, sttt) + 2 * QVector3D::dotProduct(st, stttt);
    float EG_FF = E * G - F * F;
    float EG_FF_2 = EG_FF * EG_FF;
    float EG_FF_3 = EG_FF * EG_FF * EG_FF;
    float EG_FF_4 = EG_FF_2 * EG_FF_2;
    float EG_FF_t = Et * G + E * Gt - 2 * F * Ft;
    float EG_FF_tt = Ett * G + 2 * Et * Gt + E * Gtt - 2 * (Ft * Ft + F * Ftt);
    float EG_FF_ttt = Ettt * G + 3 * Ett * Gt + 3 * Et * Gtt + E * Gttt - 2 * (3 * Ft * Ftt + F * Fttt);

    float m11 = G / EG_FF;
    float m11_t = (EG_FF * Gt - G * EG_FF_t) / EG_FF_2;
    float m11_tt = (Gtt * EG_FF_2 -
                    2 * Gt * EG_FF * EG_FF_t -
                    G * EG_FF * EG_FF_tt +
                    2 * G * EG_FF_t * EG_FF_t) / EG_FF_3;
    float m11_ttt = (Gttt * EG_FF_3 -
                     3 * EG_FF_2 * Gtt * EG_FF_t -
                     3 * EG_FF_2 * Gt * EG_FF_tt +
                     6 * EG_FF * Gt * EG_FF_t * EG_FF_t -
                     G * EG_FF_ttt * EG_FF_2 -
                     6 * G * EG_FF_t * EG_FF_t * EG_FF_t +
                     6 * G * EG_FF * EG_FF_t * EG_FF_tt) / EG_FF_4;

    float m21 = -F / EG_FF;
    float m21_t = -(EG_FF * Ft - F * EG_FF_t) / EG_FF_2;
    float m21_tt = -(Ftt * EG_FF_2 -
                     2 * Ft * EG_FF * EG_FF_t -
                     F * EG_FF * EG_FF_tt +
                     2 * F * EG_FF_t * EG_FF_t) / EG_FF_3;
    float m21_ttt = -(Fttt * EG_FF_3 -
                      3 * EG_FF_2 * Ftt * EG_FF_t -
                      3 * EG_FF_2 * Ft * EG_FF_tt +
                      6 * EG_FF * Ft * EG_FF_t * EG_FF_t -
                      F * EG_FF_ttt * EG_FF_2 -
                      6 * F * EG_FF_t * EG_FF_t * EG_FF_t +
                      6 * F * EG_FF * EG_FF_t * EG_FF_tt) / EG_FF_4;

    float alpha = -m11 * ra;
    float alpha_t = -m11_t * ra;
    float alpha_tt = -m11_tt * ra;
    float alpha_ttt = -m11_ttt * ra;

    float beta = -m21 * ra;
    float beta_t = -m21_t * ra;
    float beta_tt = -m21_tt * ra;
    float beta_ttt = -m21_ttt * ra;

    float sqrt_term = 1 - ra * ra * m11;
    float gamma = (EG_FF > 0 ? 1 : -1) * sqrt(sqrt_term);
    float gamma_t = (EG_FF > 0 ? 1 : -1) * -ra * ra * m11_t / (2 * sqrt(sqrt_term));
    float gamma_tt = (EG_FF > 0 ? 1 : -1) *
                     -ra * ra / 2 *
                     (m11_tt / sqrt(sqrt_term) +
                      (ra * ra * m11_t * m11_t) / (2 * pow(sqrt_term, 1.5f)));
    float gamma_ttt = (EG_FF > 0 ? 1 : -1) *
                      -ra * ra / 2 *
                      ((m11_ttt * sqrt(sqrt_term) + (ra * ra * m11_t * m11_tt) / (2 * sqrt(sqrt_term))) / (sqrt_term) +
                       (ra * ra * m11_t * m11_tt * sqrt_term + 1.5f * pow(ra, 4) * pow(m11_t, 3)) / (pow(sqrt_term, 2.5f)));

    QVector3D n = alpha * sa +
                beta * st +
                gamma * sNormal;
    QVector3D nt = alpha * sat + alpha_t * sa +
                 beta * stt + beta_t * st +
                 gamma * sNormal_t + gamma_t * sNormal;
    QVector3D ntt = alpha * satt +
                  2 * alpha_t * sat +
                  alpha_tt * sa +
                  beta * sttt +
                  2 * beta_t * stt +
                  beta_tt * st +
                  gamma * sNormal_tt +
                  2 * gamma_t * sNormal_t +
                  gamma_tt * sNormal;
    QVector3D nttt = alpha * sattt +
                   3 * alpha_t * satt +
                   3 * alpha_tt * sat +
                   alpha_ttt * sa +
                   beta * stttt +
                   3 * beta_t * sttt +
                   3 * beta_tt * stt +
                   beta_ttt * st +
                   gamma * sNormal_ttt +
                   3 * gamma_t * sNormal_tt +
                   3 * gamma_tt * sNormal_t +
                   gamma_ttt * sNormal;
    return MathUtility::normalVectorDerivative3(n, nt, ntt, nttt);
}



QVector3D Envelope::getPathAt(float t)
{
    if (isTanContinuous())
    {
        return adjEnvA0->getEnvelopeAt(t, 1) - tool->getSphereRadiusAt(0) * adjEnvA0->getNormalAt(t, 1) - tool->getSphereCenterHeightAt(0) * getAxisAt(t);
    }
    else if (isPositContinuous())
    {
        // The normal is orthogonal to X_t of the adjacent envelope, and at an angle to the axis of its own envelope.
        // Thus start with the cross product of these two, and rotate it a certain amount around X_t of the adjacent.
        QVector3D adjEnv = adjEnvA0->getEnvelopeAt(t, 1);
        QVector3D adjEnv_t = adjEnvA0->getEnvelopeDtAt(t, 1);
        QVector3D axis = getAxisAt(t);
        float dotValue = -tool->getSphereRadiusDaAt(0) / tool->getSphereCenterHeightDaAt(0);

        // MATH
        QVector3D v = adjEnv_t.normalized();
        QVector3D p = QVector3D(0,1,0);
        if (p == v) p = QVector3D(1,0,0);
        QVector3D w1 = (p - QVector3D::dotProduct(p, v) * v).normalized();
        QVector3D w2 = QVector3D::crossProduct(v, w1);
        float a_dot_w1 = QVector3D::dotProduct(axis, w1);
        float a_dot_w2 = QVector3D::dotProduct(axis, w2);

        float phi = atan2(a_dot_w2, a_dot_w1);
        float theta = phi - acos(dotValue / sqrt(a_dot_w1 * a_dot_w1 + a_dot_w2 * a_dot_w2));

        QVector3D normal = w1 * cos(theta) + w2 * sin(theta);

        return adjEnv - tool->getSphereRadiusAt(0) * normal - tool->getSphereCenterHeightAt(0) * axis;
    }
    else
    {
        return toolMovement.getPath().getPathAt(t);
    }
}

QVector3D Envelope::getPathDtAt(float t)
{
    if (isTanContinuous())
    {
        return adjEnvA0->getEnvelopeDtAt(t, 1) - tool->getSphereRadiusAt(0) * adjEnvA0->getNormalDtAt(t, 1) - tool->getSphereCenterHeightAt(0) * getAxisDtAt(t);
    }
    else if (isPositContinuous())
    {
        QVector3D axis = getAxisAt(t); // unit vector
        QVector3D axis_t = getAxisDtAt(t); // derivative of unit vector
        QVector3D adjEnv_t = adjEnvA0->getEnvelopeDtAt(t, 1); // not yet unit vector
        QVector3D adjEnv_tt = adjEnvA0->getEnvelopeDt2At(t, 1); // not yet derivative of unit vector
        float dotValue = -tool->getSphereRadiusDaAt(0) / tool->getSphereCenterHeightDaAt(0);

        // MATH
        QVector3D v = adjEnv_t.normalized();
        QVector3D v_t = MathUtility::normalVectorDerivative(adjEnv_t, adjEnv_tt);
        QVector3D p = QVector3D(0,1,0);
        if (p == v) p = QVector3D(1,0,0);
        QVector3D w1 = p - QVector3D::dotProduct(p, v) * v;
        QVector3D w1_t = -(QVector3D::dotProduct(p, v) * v_t + QVector3D::dotProduct(p, v_t) * v);
        w1_t = MathUtility::normalVectorDerivative(w1, w1_t);
        w1.normalize();
        QVector3D w2 = QVector3D::crossProduct(v, w1);
        QVector3D w2_t = QVector3D::crossProduct(v_t, w1) + QVector3D::crossProduct(v, w1_t);
        float a_dot_w1 = QVector3D::dotProduct(axis, w1);
        float a_dot_w1_t = QVector3D::dotProduct(axis_t, w1) + QVector3D::dotProduct(axis, w1_t);
        float a_dot_w2 = QVector3D::dotProduct(axis, w2);
        float a_dot_w2_t = QVector3D::dotProduct(axis_t, w2) + QVector3D::dotProduct(axis, w2_t);

        float phi = atan2(a_dot_w2, a_dot_w1);
        float theta = phi - acos(dotValue / sqrt(a_dot_w1 * a_dot_w1 + a_dot_w2 * a_dot_w2));
        float c_theta = cos(theta);
        float s_theta = sin(theta);
        float theta_t = -(c_theta * a_dot_w1_t + s_theta * a_dot_w2_t) /
                        (-s_theta * a_dot_w1 + c_theta * a_dot_w2);

        QVector3D normal_t = theta_t * (-s_theta * w1 + c_theta * w2) + c_theta * w1_t + s_theta * w2_t;

        return adjEnv_t - tool->getSphereRadiusAt(0) * normal_t - tool->getSphereCenterHeightAt(0) * axis_t;
    }
    else
    {
        return toolMovement.getPath().getDerivativeAt(t);
    }
}

QVector3D Envelope::getPathDt2At(float t)
{
    if (isTanContinuous())
    {
        return adjEnvA0->getEnvelopeDt2At(t, 1) - tool->getSphereRadiusAt(0) * adjEnvA0->getNormalDt2At(t, 1) - tool->getSphereCenterHeightAt(0) * getAxisDt2At(t);
    }
    else if (isPositContinuous())
    {
        QVector3D axis = getAxisAt(t); // unit vector
        QVector3D axis_t = getAxisDtAt(t); // derivative of unit vector
        QVector3D axis_tt = getAxisDt2At(t); // 2nd derivative of unit vector
        QVector3D adjEnv_t = adjEnvA0->getEnvelopeDtAt(t, 1); // not yet unit vector
        QVector3D adjEnv_tt = adjEnvA0->getEnvelopeDt2At(t, 1); // not yet derivative of unit vector
        QVector3D adjEnv_ttt = adjEnvA0->getEnvelopeDt3At(t, 1); // not yet derivative of unit vector
        float dotValue = -tool->getSphereRadiusDaAt(0) / tool->getSphereCenterHeightDaAt(0);

        // MATH
        QVector3D v = adjEnv_t.normalized();
        QVector3D v_t = MathUtility::normalVectorDerivative(adjEnv_t, adjEnv_tt);
        QVector3D v_tt = MathUtility::normalVectorDerivative2(adjEnv_t, adjEnv_tt, adjEnv_ttt);
        QVector3D p = QVector3D(0,1,0);
        if (p == v) p = QVector3D(1,0,0);
        QVector3D w1 = p - QVector3D::dotProduct(p, v) * v;
        QVector3D w1_t = -(QVector3D::dotProduct(p, v_t) * v + QVector3D::dotProduct(p, v) * v_t);
        QVector3D w1_tt = -(QVector3D::dotProduct(p, v_tt) * v + 2 * QVector3D::dotProduct(p, v_t) * v_t + QVector3D::dotProduct(p, v) * v_tt);
        w1_tt = MathUtility::normalVectorDerivative2(w1, w1_t, w1_tt);
        w1_t = MathUtility::normalVectorDerivative(w1, w1_t);
        w1.normalize();
        QVector3D w2 = QVector3D::crossProduct(v, w1);
        QVector3D w2_t = QVector3D::crossProduct(v_t, w1) + QVector3D::crossProduct(v, w1_t);
        QVector3D w2_tt = QVector3D::crossProduct(v_tt, w1) + 2 * QVector3D::crossProduct(v_t, w1_t) + QVector3D::crossProduct(v, w1_tt);
        float a_dot_w1 = QVector3D::dotProduct(axis, w1);
        float a_dot_w1_t = QVector3D::dotProduct(axis_t, w1) + QVector3D::dotProduct(axis, w1_t);
        float a_dot_w1_tt = QVector3D::dotProduct(axis_tt, w1) + 2 * QVector3D::dotProduct(axis_t, w1_t) + QVector3D::dotProduct(axis, w1_tt);
        float a_dot_w2 = QVector3D::dotProduct(axis, w2);
        float a_dot_w2_t = QVector3D::dotProduct(axis_t, w2) + QVector3D::dotProduct(axis, w2_t);
        float a_dot_w2_tt = QVector3D::dotProduct(axis_tt, w2) + 2 * QVector3D::dotProduct(axis_t, w2_t) + QVector3D::dotProduct(axis, w2_tt);

        float phi = atan2(a_dot_w2, a_dot_w1);
        float theta = phi - acos(dotValue / sqrt(a_dot_w1 * a_dot_w1 + a_dot_w2 * a_dot_w2));
        float c_theta = cos(theta);
        float s_theta = sin(theta);

        float k = c_theta * a_dot_w1_t + s_theta * a_dot_w2_t;
        float l = -s_theta * a_dot_w1 + c_theta * a_dot_w2;
        float theta_t = -k / l;

        float dk = theta_t * -s_theta * a_dot_w1_t + c_theta * a_dot_w1_tt +
                   theta_t * c_theta * a_dot_w2_t + s_theta * a_dot_w2_tt;
        float dl = -(theta_t * c_theta * a_dot_w1 + s_theta * a_dot_w1_t) +
                   theta_t * -s_theta * a_dot_w2 + c_theta + a_dot_w2_t;
        float theta_tt = -(l * dk - k * dl) / (l * l);


        QVector3D i = -s_theta * w1 + c_theta * w2;
        QVector3D di = -(theta_t * c_theta * w1 + s_theta * w1_t) +
                     theta_t * -s_theta * w2 + c_theta * w2_t;
        QVector3D normal_t = theta_t * i + c_theta * w1_t + s_theta * w2_t;
        QVector3D normal_tt = theta_tt * i + theta_t * di +
                            theta_t * -s_theta * w1_t + c_theta * w1_tt +
                            theta_t * c_theta * w2_t + s_theta * w2_tt;

        return adjEnv_tt - tool->getSphereRadiusAt(0) * normal_tt - tool->getSphereCenterHeightAt(0) * axis_tt;
    }
    else
    {
        return toolMovement.getPath().getDerivative2At(t);
    }
}

QVector3D Envelope::getPathDt3At(float t)
{
    // Need to check if these are required for chaining position continuous envelopes
    return toolMovement.getPath().getDerivative3At(t);
}

QVector3D Envelope::getPathDt4At(float t)
{
    // Need to check if these are required for chaining position continuous envelopes
    return toolMovement.getPath().getDerivative4PlusAt(t);
}


QQuaternion Envelope::calcAxisRotationAt(float t)
{
    if (!isTanContinuous()) return QQuaternion();
    // First rotate the axis of the previous envelope to its normal.
    // This is to establish a frame of reference for all its derivatives.
    QVector3D adjNormal = adjEnvA0->getNormalAt(t, 1);
    QVector3D adjAxis = adjEnvA0->getAxisAt(t);
    QQuaternion rotationFrame = QQuaternion::rotationTo(adjAxis, adjNormal);

    // Then rotate w.r.t. tangent continuity. Which rotates around the cross product of the adjacent normal and axis
    float degrees = qRadiansToDegrees(acos(-tool->getSphereRadiusDaAt(0) / tool->getSphereCenterHeightDaAt(0)));
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
    if (isAxisConstrained())
    {
        QVector3D x0 = adjEnvA0->getEnvelopeAt(t, 1);
        QVector3D x1 = adjEnvA1->getEnvelopeAt(t, 0);
        QVector3D deltaX = x1 - x0;
        QVector3D deltaX_hat = deltaX.normalized();

        QVector3D x0_t = adjEnvA0->getEnvelopeDtAt(t, 1);
        QVector3D x1_t = adjEnvA1->getEnvelopeDtAt(t, 0);
        QVector3D x0_t_hat = x0_t.normalized();
        QVector3D x1_t_hat = x1_t.normalized();

        // The following method only works when Delta X can be made with a cylinder of hight and radius of 1, where x1 lies on the bottom ring of the cylinder and x2 on the top ring.

        // Separately find the parts of the axis parallel and perpendicular to Delta X
        // By utilizing Delta X dot A = 1 we can calculate the angle between Delta X and A
        float c_theta = std::clamp(1.0f / deltaX.length(), -1.0f, 1.0f);
        float s_theta = sqrt(1 - c_theta * c_theta);
        QVector3D axis_par_deltaX = c_theta * deltaX_hat;

        // For the perpendicular part we make an orthonormal basis on the plane perpendicular to Delta X, with radius Sin(theta)
        QVector3D v = QVector3D(0,1,0);
        if (v == deltaX_hat) v = QVector3D(1,0,0);
        QVector3D v1 = v - QVector3D::dotProduct(v, deltaX_hat) * deltaX_hat;
        v1 = s_theta * v1.normalized();
        QVector3D v2 = QVector3D::crossProduct(deltaX_hat, v1);

        // The normals at x1 and x2 are perpendicular to the respective time derivates, as well as the axis.
        // This means each normal is the cross product of the time derivative and the axis (which is split in the parallel and perpendicular part).
        // This eventually leads to the form A*cos(phi) + B*sin(phi)=C, where A, B, and C are all coplanar vectors (by construction), which is the only reason this works.
        QVector3D D = v1 - QVector3D::crossProduct(x1_t_hat, v1) + QVector3D::crossProduct(x0_t_hat, v1);
        QVector3D E = v2 - QVector3D::crossProduct(x1_t_hat, v2) + QVector3D::crossProduct(x0_t_hat, v2);
        QVector3D F = deltaX + QVector3D::crossProduct(x1_t_hat, axis_par_deltaX) - QVector3D::crossProduct(x0_t_hat, axis_par_deltaX) - axis_par_deltaX;

        // By using dot product we can find phi
        float h = QVector3D::dotProduct(D, D);
        float i = QVector3D::dotProduct(D, E);
        float j = QVector3D::dotProduct(E, E);
        float k = QVector3D::dotProduct(D, F);
        float l = QVector3D::dotProduct(E, F);
        float phi = atan2(l * h - k * i, k * j - l * i);
        float c_phi = cos(phi);
        float s_phi = sin(phi);

        QVector3D axis_perp_deltaX = v1 * c_phi + v2 * s_phi;
        axis = axis_par_deltaX + axis_perp_deltaX;
    }
    else if (isTanContinuous())
    {
        // Rotate the normal of the adjacent envelope around the cross product with the axis of the adjacent envelope
        axis = calcAxisRotationAt(t) * adjEnvA0->getAxisAt(t);
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
    if (isAxisConstrained())
    {
        QVector3D x0 = adjEnvA0->getEnvelopeAt(t, 1);
        QVector3D x1 = adjEnvA1->getEnvelopeAt(t, 0);
        QVector3D deltaX = x1 - x0;
        QVector3D deltaX_hat = deltaX.normalized();

        QVector3D x0_t = adjEnvA0->getEnvelopeDtAt(t, 1);
        QVector3D x1_t = adjEnvA1->getEnvelopeDtAt(t, 0);
        QVector3D deltaX_t = x1_t - x0_t;
        QVector3D deltaX_hat_t = MathUtility::normalVectorDerivative(deltaX, deltaX_t);
        QVector3D x0_t_hat = x0_t.normalized();
        QVector3D x1_t_hat = x1_t.normalized();

        QVector3D x0_tt = adjEnvA0->getEnvelopeDt2At(t, 1);
        QVector3D x1_tt = adjEnvA1->getEnvelopeDt2At(t, 0);
        QVector3D x0_t_hat_t = MathUtility::normalVectorDerivative(x0_t, x0_tt);
        QVector3D x1_t_hat_t = MathUtility::normalVectorDerivative(x1_t, x1_tt);

        // The following method only works when Delta X can be made with a cylinder of hight and radius of 1, where x1 lies on the bottom ring of the cylinder and x2 on the top ring.

        // Separately find the parts of the axis parallel and perpendicular to Delta X
        // By utilizing Delta X dot A = 1 we can calculate the angle between Delta X and A
        float c_theta = std::clamp(1.0f / deltaX.length(), -1.0f, 1.0f);
        float c_theta_t = -QVector3D::dotProduct(deltaX, deltaX_t) / pow(deltaX.length(), 3);
        float s_theta = sqrt(1 - c_theta * c_theta);
        float s_theta_t = -c_theta * c_theta_t / s_theta;
        if (s_theta == 0)
        {
            s_theta_t = 0;
        }
        QVector3D axis_par_deltaX = c_theta * deltaX_hat;
        QVector3D axis_par_deltaX_t = c_theta_t * deltaX_hat + c_theta * deltaX_hat_t;

        // For the perpendicular part we make an orthonormal basis on the plane perpendicular to Delta X, with radius Sin(theta)
        QVector3D v = QVector3D(0,1,0);
        if (v == deltaX_hat) v = QVector3D(1,0,0);
        QVector3D v1 = v - QVector3D::dotProduct(v, deltaX_hat) * deltaX_hat;
        QVector3D v1_t = -(QVector3D::dotProduct(v, deltaX_hat_t) * deltaX_hat + QVector3D::dotProduct(v, deltaX_hat) * deltaX_hat_t);
        v1_t = s_theta_t * v1.normalized() + s_theta * MathUtility::normalVectorDerivative(v1, v1_t);
        v1 = s_theta * v1.normalized();
        QVector3D v2 = QVector3D::crossProduct(deltaX_hat, v1);
        QVector3D v2_t = QVector3D::crossProduct(deltaX_hat_t, v1) + QVector3D::crossProduct(deltaX_hat, v1_t);

        // The normals at x1 and x2 are perpendicular to the respective time derivates, as well as the axis.
        // This means each normal is the cross product of the time derivative and the axis (which is split in the parallel and perpendicular part).
        // This eventually leads to the form A*cos(phi) + B*sin(phi)=C, where A, B, and C are all coplanar vectors (by construction), which is the only reason this works.
        QVector3D D = v1 - QVector3D::crossProduct(x1_t_hat, v1) + QVector3D::crossProduct(x0_t_hat, v1);
        QVector3D D_t = v1_t -
                      (QVector3D::crossProduct(x1_t_hat_t, v1) + QVector3D::crossProduct(x1_t_hat, v1_t)) +
                      (QVector3D::crossProduct(x0_t_hat_t, v1) + QVector3D::crossProduct(x0_t_hat, v1_t));
        QVector3D E = v2 - QVector3D::crossProduct(x1_t_hat, v2) + QVector3D::crossProduct(x0_t_hat, v2);
        QVector3D E_t = v2_t -
                      (QVector3D::crossProduct(x1_t_hat_t, v2) + QVector3D::crossProduct(x1_t_hat, v2_t)) +
                      (QVector3D::crossProduct(x0_t_hat_t, v2) + QVector3D::crossProduct(x0_t_hat, v2_t));
        QVector3D F = deltaX + QVector3D::crossProduct(x1_t_hat, axis_par_deltaX) - QVector3D::crossProduct(x0_t_hat, axis_par_deltaX) - axis_par_deltaX;
        QVector3D F_t = deltaX_t +
                      (QVector3D::crossProduct(x1_t_hat_t, axis_par_deltaX) + QVector3D::crossProduct(x1_t_hat, axis_par_deltaX_t)) -
                      (QVector3D::crossProduct(x0_t_hat_t, axis_par_deltaX) + QVector3D::crossProduct(x0_t_hat, axis_par_deltaX_t)) -
                      axis_par_deltaX_t;

        // By using dot product we can find phi
        float h = QVector3D::dotProduct(D, D);
        float i = QVector3D::dotProduct(D, E);
        float j = QVector3D::dotProduct(E, E);
        float k = QVector3D::dotProduct(D, F);
        float l = QVector3D::dotProduct(E, F);
        float phi = atan2(l * h - k * i, k * j - l * i);
        float c_phi = cos(phi);
        float s_phi = sin(phi);

        QVector3D helper = -D * s_phi + E * c_phi;
        float phi_t = QVector3D::dotProduct(helper, F_t - D_t * c_phi - E_t * s_phi) / helper.lengthSquared();
        if (helper.lengthSquared() == 0)
        {
            phi_t = 0;
        }

        QVector3D axis_perp_deltaX = v1 * c_phi + v2 * s_phi;
        QVector3D axis_perp_deltaX_t = v1_t * c_phi + v1 * phi_t * -s_phi +
                                     v2_t * s_phi + v2 * phi_t * c_phi;
        axis = axis_par_deltaX + axis_perp_deltaX;

        axis_t = axis_par_deltaX_t + axis_perp_deltaX_t;
    }
    else if (isTanContinuous())
    {
        axis_t = calcAxisRotationAt(t) * adjEnvA0->getAxisDtAt(t);
    }
    else
    {
        axis_t = toolMovement.getAxisDtAt(t);
    }
    return axis_t;
}

QVector3D Envelope::getAxisDt2At(float t)
{
    // TODO: for now the axis constrained case only works once. If the tool isn't big enough, chaining constrained envelopes together won't work as it needs higher the higher derivatives of the envelope and normal.
    QVector3D axis, axis_t, axis_tt;
    if (isTanContinuous())
    {
        axis_tt = calcAxisRotationAt(t) * adjEnvA0->getAxisDt2At(t);
    }
    else
    {
        axis_tt = toolMovement.getAxisDt2At(t);
    }
    return axis_tt;
}

QVector3D Envelope::getAxisDt3At(float t)
{
    // TODO: for now the axis constrained case only works once. If the tool isn't big enough, chaining constrained envelopes together won't work as it needs higher the higher derivatives of the envelope and normal.
    QVector3D axis, axis_t, axis_tt, axis_ttt;
    if (isTanContinuous())
    {
        axis_ttt = calcAxisRotationAt(t) * adjEnvA0->getAxisDt3At(t);
    }
    else
    {
        axis_ttt = toolMovement.getAxisDt3At(t);
    }
    return axis_ttt;
}

QVector3D Envelope::getAxisDt4At(float t)
{
    // TODO: for now the axis constrained case only works once. If the tool isn't big enough, chaining constrained envelopes together won't work as it needs higher the higher derivatives of the envelope and normal.
    QVector3D axis, axis_t, axis_tt, axis_ttt, axis_tttt;
    if (isTanContinuous())
    {
        axis_tttt = calcAxisRotationAt(t) * adjEnvA0->getAxisDt4At(t);
    }
    else
    {
        axis_tttt = toolMovement.getAxisDt4At(t);
    }
    return axis_tttt;
}


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
