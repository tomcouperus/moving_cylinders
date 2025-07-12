#include "tool.h"

void Tool::computeTool() {
    vertexArr.clear();
    Vertex v1,v2,v3,v4;
    for (int aIdx = 0; aIdx<sectors;aIdx++) {
        float a0 = (float)aIdx/sectors;
        float a1 = (float)(aIdx+1)/sectors;
        for (int tIdx = 0; tIdx < sectors;tIdx++) {
            float t0 = (float)tIdx/sectors;
            float t1 = (float)(tIdx+1)/sectors;

            v1 = getToolSurfaceAt(a0, t0*2*PI);
            v2 = getToolSurfaceAt(a1,t0*2*PI);
            v3 = getToolSurfaceAt(a0,t1*2*PI);
            v4 = getToolSurfaceAt(a1,t1*2*PI);

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
