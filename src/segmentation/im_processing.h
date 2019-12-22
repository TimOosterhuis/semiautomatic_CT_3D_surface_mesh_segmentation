#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stack>
#include <vector>
#include "coords.h"
#include "scan.h"

int* intensityHistogram(Scan);
int calcCountIntensity(int*, int, int, int);
double calcMeanIntensity(int*, int, int, int);
double calcVarIntensity(int*, int, int, double, int);

Scan absThreshold(Scan, int);
Scan relThreshold(Scan, float);
Scan otsuThreshold(Scan);

Scan erode(Scan, int, bool);
Scan dilate(Scan, int, bool);
Scan open(Scan, int, bool);
Scan close(Scan, int, bool);
Scan reconstruct(Scan, Scan, int, bool);

bool findPath(Scan, Coords, Coords);
std::vector<Coords> regionSeeds(Scan);
bool fillRegion(Scan*, Scan*, unsigned int, Coords);
Scan getRegions(Scan);

