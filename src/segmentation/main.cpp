#include <iostream>
#include "scan.h"
#include "im_processing.h"
#include "marchingcubes.h"

/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char** argv) 
{ 
	printf("shadilay\n");
	Scan scan("C:\\Users\\Tim Oosterhuis\\Documents\\Uni\\MSc_project_CS\\test dataset TIM_SKULL\\CT\\");
	//scan.createExample();
	//scan.prettyPrint();
	Scan thresholded = otsuThreshold(scan);
	int trpoints = 0;
	for (int i = 0; i<scan.getDepth(); i++)
	{
		for (int j = 0; j<scan.getHeight(); j++)
		{
			for (int k = 0; k<scan.getWidth(); k++)
			{
				if (thresholded.valueAt(k, j, i) == 1)
				{
					trpoints++;
				}
			}
		}
	}
	printf("number of points %d\n", trpoints);
	//thresholded.prettyPrint();
	//int *hist = intensityHistogram(scan);
	//double mean = calcMeanIntensity(hist, scan.getMin(), scan.getMax(), scan.getMin());
	//double var = calcVarIntensity(hist, scan.getMin(), scan.getMax(), mean, scan.getMin());
	//printf("mean: %lf, var: %lf\n", mean, var);
	//Scan eroded = erode(thresholded, 1, false);
	//eroded.prettyPrint();
	//printf("%d %d %d %d\n", findPath(thresholded, Coords(1,1,1), Coords(0,0,1)), findPath(thresholded, Coords(0,0,1), Coords(1,1,1)),
	//        findPath(thresholded, Coords(1,0,1), Coords(1,0,1)), findPath(eroded, Coords(1,1,1), Coords(0,0,0)));
	//std::vector<Coords> seeds = regionSeeds(thresholded);
	//printf("number of seeds %d\n", seeds.size());
	Scan regions = getRegions(thresholded);
	printf("number of regions %d\n", regions.getMax());
	//for (int i = 0; i < seeds.size(); i++)
	//{
	//	printf("seed: %d %d %d\n", seeds.at(i).x, seeds.at(i).y, seeds.at(i).z);
	//}
	int posi[3] = {0,0,0};
	int valsvals[8] = {scan.valueAt(posi[0], posi[1], posi[2]), scan.valueAt(posi[0] + 1, posi[1], posi[2]), scan.valueAt(posi[0], posi[1]  + 1, posi[2]),
	                   scan.valueAt(posi[0] + 1, posi[1]  + 1, posi[2]), scan.valueAt(posi[0], posi[1], posi[2] + 1), scan.valueAt(posi[0] + 1, posi[1], posi[2] + 1),
	                   scan.valueAt(posi[0], posi[1] + 1, posi[2] + 1), scan.valueAt(posi[0] + 1, posi[1] + 1, posi[2] + 1)};
	cubeInfo march = marchingCube(posi, 3, valsvals);
	printf("march meta info v %d t %d\n", march.nVertices, march.nTriangles);
	printf("march vert idxs %d %d %d %d %d\n", march.relVertIdx[0], march.relVertIdx[1], march.relVertIdx[2], march.relVertIdx[3], march.relVertIdx[4]);
    printf("march tria idxs %d %d %d %d %d %d %d %d %d\n", march.relTriaIdx[0], march.relTriaIdx[1], march.relTriaIdx[2], march.relTriaIdx[3], march.relTriaIdx[4],
	      march.relTriaIdx[5], march.relTriaIdx[6], march.relTriaIdx[7], march.relTriaIdx[8]);
	for (int i = 0; i < march.nVertices; i++)
	{
		printf("vert %f %f %f\n", march.vertices.at(i).coords.x, march.vertices.at(i).coords.y, march.vertices.at(i).coords.z);
    }
	return 1;
}
