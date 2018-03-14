#ifndef MARCHINGCUBES_H
#define MARCHINGCUBES_H

#include <stdlib.h>
#include <stdio.h>
#include <vector>

#include "vertex.h"


struct cubeInfo {
    int nTriangles;
    int nVertices;
    std::vector<Vertex> vertices;
    int relVertIdx[12];
    int relTriaIdx[15];
};

cubeInfo marchingCube(int pos[3], int threshold, int values[8]);

#endif // MARCHINGCUBES_H
