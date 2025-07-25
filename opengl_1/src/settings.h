#ifndef SETTINGS_H
#define SETTINGS_H

#include <QOpenGLDebugLogger>
#include <QVector3D>
#include <QRegularExpression>

/**
 * Struct that contains all the settings of the program. Initialised with a
 * number of default values.
 */
typedef struct Settings {
    int selectedIdx = -1;
    bool showEnvelope = true;
    bool showTool = true;
    bool showPath = false;
    bool showGrazingCurve = false;
    bool showToolAxis = false;
    bool showNormals = false;
    bool showSpheres = false;
    bool reflectionLines = false;
    float reflFreq = 20;
    float percentBlack = 0.5;
    int aIdx = 0;
    int timeIdx = 0;
    int aSectors = 20;
    int tSectors = 50;

    const size_t NUM_ENVELOPES = 4;

    inline float a() const { return (float) aIdx / aSectors; }
    inline float t() const { return (float) timeIdx / tSectors; }

    inline QVector3D stringToVector3D(const QString& vector)
    {
        QString xStr;
        QString yStr;
        QString zStr;
        int coord=0;
        for(int i=0; i<vector.size()-1; ++i){
            if (vector[i]== QChar(',') ){
                coord++;
                continue;
            }
            if (vector[i]==QChar('(') || vector[i]==QChar(')'))
                continue;
            switch (coord){
            case 0:
                xStr.append(vector[i]);
                break;
            case 1:
                yStr.append(vector[i]);
                break;
            case 2:
                zStr.append(vector[i]);
                break;
            }
        }

        // Convert the captured strings to floats
        float x = xStr.toFloat();
        float y = yStr.toFloat();
        float z = zStr.toFloat();

        // Create and return a QVector3D
        return QVector3D(x, y, z);
    }

} Settings;

#endif // SETTINGS_H
