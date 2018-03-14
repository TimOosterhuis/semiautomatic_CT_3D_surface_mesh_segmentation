#include "im_processing.h"
int* intensityHistogram(Scan scan)
{
	int min = scan.getMin();
	int range = (scan.getMax() - min) + 1;
	int *hist = (int*) calloc(range, sizeof(int));
	for (int i = 0; i < scan.getDepth(); i++)
	{
		for (int j = 0; j < scan.getHeight(); j++)
		{
			for (int k = 0; k < scan.getWidth(); k++)
			{
				hist[scan.valueAt(k, j, i) - min]++;
			}
		}
	}
	//printf("%d %d %d %d %d %d %d %d %d\n", hist[0], hist[1], hist[2], hist[3], hist[4], hist[5], hist[6], hist[7], hist[8]);
	return hist;
}

Scan absThreshold(Scan scan, int val)
{
	Scan thresholded(scan.getWidth(), scan.getHeight(), scan.getDepth());
	for (int i = 0; i < scan.getDepth(); i++)
	{
		for (int j = 0; j < scan.getHeight(); j++)
		{
			for (int k = 0; k < scan.getWidth(); k++)
			{
				if (scan.valueAt(k, j, i) >= val)
				{
					thresholded.setValue(k, j, i, 1);
				} 
				else
				{
					thresholded.setValue(k, j, i, 0);
				}
			}
		}
	}
	return thresholded;
}

Scan relThreshold(Scan scan, float pct)
{
	int thres = round(scan.getMin() + (float)(scan.getMax() - scan.getMin()) * pct);
	return absThreshold(scan, thres);
}

Scan otsuThreshold(Scan scan)
{
	int *hist = intensityHistogram(scan);
	
	int max = scan.getMax();
	int min = scan.getMin();	
	double globalMean = calcMeanIntensity(hist, min, max, min);
	//double globalVar = calcVarIntensity(hist, min, max, mean, min); <--used to calculate opt. threshold quality
	int globalCount = calcCountIntensity(hist, min, max, min);
	
	int optThres = min;
	int optVar = 0;
	for(int thres = min; thres < max; thres++)
	{
		double pClassOne = (double) calcCountIntensity(hist, min, thres, min) / (double) globalCount;
		double meanClassOne = calcMeanIntensity(hist, min, thres, min);
	
		double pClassTwo = (double) calcCountIntensity(hist, thres + 1, max, min) / (double) globalCount;
		double meanClassTwo = calcMeanIntensity(hist, thres + 1, max, min);
		
		double betwClassVar = pClassOne * pClassTwo * (meanClassOne - meanClassTwo) * (meanClassOne - meanClassTwo);
		if (betwClassVar > optVar)
		{
			optVar = betwClassVar;
			optThres = thres;
		}
	}
	
	return absThreshold(scan, optThres);
}

double calcMeanIntensity(int* hist, int minVal, int maxVal, int offset)
{
	int sum = 0;
	int count = 0;
	for (int val = minVal; val <= maxVal; val++)
	{
		count += hist[val - offset];
		sum += val * hist[val - offset];
	}
	return (double) sum / (double) count;
}

double calcVarIntensity(int* hist, int minVal, int maxVal, double mean, int offset)
{
	double var = 0;
	for (int val = minVal; val <= maxVal; val++)
	{
		var += (mean - val) * (mean - val) * hist[val - offset];
	}
	return var;
}

int calcCountIntensity(int* hist, int minVal, int maxVal, int offset)
{
	int count = 0;
	for (int val = minVal; val <= maxVal; val++)
	{
		count += hist[val - offset];
	}
	return count;
}

Scan erode(Scan scan, int range, bool eightConn)
{
	int max = scan.getMax();
	
	Scan eroded(scan.getWidth(), scan.getHeight(), scan.getDepth());
	for (int i = 0; i < scan.getDepth(); i++)
	{
		for (int j = 0; j < scan.getHeight(); j++)
		{
			for (int k = 0; k < scan.getWidth(); k++)
			{
				int minVal = max; //Initialize current minimum
				if (eightConn)
				{
					for (int x=(std::max)(i-range, 0); x<(std::min)(i+range+1, (int) scan.getDepth()); x++)
					{
						for (int y= (std::max)(j-range, 0); y<(std::min)(j+range+1, (int) scan.getHeight()); y++)
						{
							for (int z= (std::max)(k-range, 0); z<(std::min)(k+range+1, (int) scan.getWidth()); z++)
							{
								minVal = (std::min)(minVal, scan.valueAt(z, y, x));
							}	
						}	
					}
				}
				else //fourConn
				{
					for (int x=(std::max)(i-range, 0); x<(std::min)(i+range+1, (int) scan.getDepth()); x++)
					{
						minVal = (std::min)(minVal, scan.valueAt(k, j, x));
					}
					for (int y= (std::max)(j-range, 0); y<(std::min)(j+range+1, (int) scan.getHeight()); y++)
					{
						minVal = (std::min)(minVal, scan.valueAt(k, y, i));
					}
					for (int z= (std::max)(k-range, 0); z<(std::min)(k+range+1, (int) scan.getWidth()); z++)
					{
						minVal = (std::min)(minVal, scan.valueAt(z, j, i));
					}
				}
				eroded.setValue(k, j, i, minVal);
			}
		}
	}
	return eroded;
}

Scan dilate(Scan scan, int range, bool eightConn)
{
	int min = scan.getMin();
	
	Scan dilated(scan.getWidth(), scan.getHeight(), scan.getDepth());
	for (int i = 0; i < scan.getDepth(); i++)
	{
		for (int j = 0; j < scan.getHeight(); j++)
		{
			for (int k = 0; k < scan.getWidth(); k++)
			{
				int maxVal = min; //Initialize current minimum
				if (eightConn)
				{
					for (int x=(std::max)(i-range, 0); x<(std::min)(i+range+1, (int) scan.getDepth()); x++)
					{
						for (int y=(std::max)(j-range, 0); y<(std::min)(j+range+1, (int) scan.getHeight()); y++)
						{
							for (int z=(std::max)(k-range, 0); z<(std::min)(k+range+1, (int) scan.getWidth()); z++)
							{
								maxVal = (std::max)(maxVal, scan.valueAt(z, y, x));
							}	
						}	
					}
				}
				else //fourConn
				{
					for (int x=(std::max)(i-range, 0); x<(std::min)(i+range+1, (int) scan.getDepth()); x++)
					{
						maxVal = (std::max)(maxVal, scan.valueAt(k, j, x));
					}
					for (int y=(std::max)(j-range, 0); y<(std::min)(j+range+1, (int) scan.getHeight()); y++)
					{
						maxVal = (std::max)(maxVal, scan.valueAt(k, y, i));
					}
					for (int z=(std::max)(k-range, 0); z<(std::min)(k+range+1, (int) scan.getWidth()); z++)
					{
						maxVal = (std::max)(maxVal, scan.valueAt(z, j, i));
					}
				}
				dilated.setValue(k, j, i, maxVal);
			}
		}
	}
	return dilated;
}

Scan open(Scan scan, int range, bool eightConn)
{
	Scan eroded = erode(scan, range, eightConn);
	return dilate(eroded, range, eightConn);
}

Scan close(Scan scan, int range, bool eightConn)
{
	Scan dilated = dilate(scan, range, eightConn);
	return erode(dilated, range, eightConn);
}

Scan reconstruct(Scan marker, Scan mask, int range, bool eightConn)
{
	int min = mask.getMin();
	//Warning: assumes mask and marker aren't the same at the start
	bool finished = false;
	while (!finished)
	{
		Scan old = marker.copy();
		
		for (int i = 0; i < marker.getDepth(); i++)
		{
			for (int j = 0; j < marker.getHeight(); j++)
			{
				for (int k = 0; k < marker.getWidth(); k++)
				{
					int maxVal = min; //Initialize current minimum
					if (eightConn)
					{
						for (int x= (std::max)(i-range, 0); x<(std::min)(i+range+1, (int) marker.getDepth()); x++)
						{
							for (int y= (std::max)(j-range, 0); y<(std::min)(j+range+1, (int) marker.getHeight()); y++)
							{
								for (int z= (std::max)(k-range, 0); z<(std::min)(k+range+1, (int) marker.getWidth()); z++)
								{
									maxVal = (std::max)(maxVal, marker.valueAt(z, y, x));
								}	
							}	
						}
					}
					else //fourConn
					{
						for (int x= (std::max)(i-range, 0); x<(std::min)(i+range+1, (int) marker.getDepth()); x++)
						{
							maxVal = (std::max)(maxVal, marker.valueAt(k, j, x));
						}
						for (int y= (std::max)(j-range, 0); y<(std::min)(j+range+1, (int) marker.getHeight()); y++)
						{
							maxVal = (std::max)(maxVal, marker.valueAt(k, y, i));
						}
						for (int z= (std::max)(k-range, 0); z<(std::min)(k+range+1, (int) marker.getWidth()); z++)
						{
							maxVal = (std::max)(maxVal, marker.valueAt(z, j, i));
						}
					}
					marker.setValue(k, j, i, (std::max)(maxVal, mask.valueAt(k, j, i)));
				}
			}
		}
		finished = marker.equals(old);
	}
	return marker;
} 

bool findPath(Scan scan, Coords start, Coords dest)
{
	//printf("FindPath\n");
	assert(scan.getMax() == 1 && scan.getMin() == 0 && "Scan must be binarized for path finding");
	//printf("t ");
	Scan discovered(scan.getWidth(), scan.getHeight(), scan.getDepth());
	//printf("h ");
	std::stack<Coords> toVisit;
	//printf("i ");
	toVisit.push(start);
	//printf("s\n");
	discovered.setValue(start.x, start.y, start.z, 1);
	//printf("dist x %d y %d z %d\n", std::abs(dest.x - start.x), std::abs(dest.y - start.y), std::abs(dest.z - start.z));
	//int bs = 0;
	while (!toVisit.empty())
	{
		//bs++;
		//if (bs % 10000000 == 0)
		//{
		//	printf("current %d\n", bs / 10000000);
		//}
		//printf("* ");
		Coords current = toVisit.top();
		toVisit.pop();
		if (current.equals(dest))
		{
			//printf("T\n");
			discovered.freeValues();
			return true; // destination reached
		}
		std::stack<Coords> bestNeighbors;
		// evaluate the 6 neighbors of the currently visited point
		if (current.x + 1 < scan.getWidth() && scan.valueAt(current.x + 1, current.y, current.z) == 1 && 
			discovered.valueAt(current.x + 1, current.y, current.z) == 0)
		{
			discovered.setValue(current.x + 1, current.y, current.z, 1);
			if (std::abs(dest.x - (current.x + 1)) < std::abs(dest.x - current.x))
			{
				bestNeighbors.push(Coords(current.x + 1, current.y, current.z));
			} 
			else
			{
				toVisit.push(Coords(current.x + 1, current.y, current.z));
			}
		}
		if (current.x - 1 >= 0 && scan.valueAt(current.x - 1, current.y, current.z) == 1 && 
			discovered.valueAt(current.x - 1, current.y, current.z) == 0)
		{
			discovered.setValue(current.x - 1, current.y, current.z, 1);
			if (std::abs(dest.x - (current.x - 1)) < std::abs(dest.x - current.x))
			{
				bestNeighbors.push(Coords(current.x - 1, current.y, current.z));
			} 
			else
			{
				toVisit.push(Coords(current.x - 1, current.y, current.z));
			}
		}
		if (current.y + 1 < scan.getHeight() && scan.valueAt(current.x, current.y + 1, current.z) == 1 && 
			discovered.valueAt(current.x, current.y + 1, current.z) == 0)
		{
			discovered.setValue(current.x, current.y + 1, current.z, 1);
			if (std::abs(dest.y - (current.y + 1)) < std::abs(dest.y - current.y))
			{
				bestNeighbors.push(Coords(current.x, current.y + 1, current.z));
			} 
			else
			{
				toVisit.push(Coords(current.x, current.y + 1, current.z));
			}
		}
		if (current.y - 1 >= 0 && scan.valueAt(current.x, current.y - 1, current.z) == 1 && 
			discovered.valueAt(current.x, current.y - 1, current.z) == 0)
		{
			discovered.setValue(current.x, current.y - 1, current.z, 1);
			if (std::abs(dest.y - (current.y - 1)) < std::abs(dest.y - current.y))
			{
				bestNeighbors.push(Coords(current.x, current.y - 1, current.z));
			} 
			else
			{
				toVisit.push(Coords(current.x, current.y - 1, current.z));
			}
		}
		if (current.z + 1 < scan.getDepth() && scan.valueAt(current.x, current.y, current.z + 1) == 1 && 
			discovered.valueAt(current.x, current.y, current.z + 1) == 0)
		{
			discovered.setValue(current.x, current.y, current.z + 1, 1);
			if (std::abs(dest.z - (current.z + 1)) < std::abs(dest.z - current.z))
			{
				bestNeighbors.push(Coords(current.x, current.y, current.z + 1));
			} 
			else
			{
				toVisit.push(Coords(current.x, current.y, current.z + 1));
			}
		}
		if (current.z - 1 >= 0 && scan.valueAt(current.x, current.y, current.z - 1) == 1 && 
			discovered.valueAt(current.x, current.y, current.z - 1) == 0)
		{
			discovered.setValue(current.x, current.y, current.z - 1, 1);
			if (std::abs(dest.z - (current.z - 1)) < std::abs(dest.z - current.z))
			{
				bestNeighbors.push(Coords(current.x, current.y, current.z - 1));
			} 
			else
			{
				toVisit.push(Coords(current.x, current.y, current.z - 1));
			}
		}
		// visit the neighbors which are closest to the goal first
		while (!bestNeighbors.empty())
		{
			toVisit.push(bestNeighbors.top());
			bestNeighbors.pop();
		}	
	}
	discovered.freeValues();
	//printf("F\n");
	return false;
}

bool fillRegion(Scan* scan, Scan* regions, unsigned int nRegions, Coords start)
{
	assert(scan->getMax() == 1 && scan->getMin() == 0 && "Scan must be binarized for region filling");

	std::stack<Coords> toVisit;
	toVisit.push(start);

	if (regions->valueAt(start.x, start.y, start.z) > 0)
	{
        //printf("onderdeel van regio %d\n", regions->valueAt(start.x, start.y, start.z));
		return false;
	}
	else {
        //printf("nieuwe regio %d\n", nRegions);
		regions->setValue(start.x, start.y, start.z, nRegions);
	}

	while (!toVisit.empty())
	{
		Coords current = toVisit.top();
		toVisit.pop();

		// evaluate the 6 neighbors of the currently visited point
		if (current.x + 1 < scan->getWidth() && scan->valueAt(current.x + 1, current.y, current.z) == 1 &&
            regions->valueAt(current.x + 1, current.y, current.z) == 0)
		{
            regions->setValue(current.x + 1, current.y, current.z, nRegions);
			toVisit.push(Coords(current.x + 1, current.y, current.z));
		}

		if (current.x - 1 >= 0 && scan->valueAt(current.x - 1, current.y, current.z) == 1 &&
			regions->valueAt(current.x - 1, current.y, current.z) == 0)
		{
			regions->setValue(current.x - 1, current.y, current.z, nRegions);
			toVisit.push(Coords(current.x - 1, current.y, current.z));
		}

		if (current.y + 1 < scan->getHeight() && scan->valueAt(current.x, current.y + 1, current.z) == 1 &&
			regions->valueAt(current.x, current.y + 1, current.z) == 0)
		{
			regions->setValue(current.x, current.y + 1, current.z, nRegions);
			toVisit.push(Coords(current.x, current.y + 1, current.z));
		}

		if (current.y - 1 >= 0 && scan->valueAt(current.x, current.y - 1, current.z) == 1 &&
            regions->valueAt(current.x, current.y - 1, current.z) == 0)
		{
			regions->setValue(current.x, current.y - 1, current.z, nRegions);
			toVisit.push(Coords(current.x, current.y - 1, current.z));
		}

		if (current.z + 1 < scan->getDepth() && scan->valueAt(current.x, current.y, current.z + 1) == 1 &&
			regions->valueAt(current.x, current.y, current.z + 1) == 0)
		{
			regions->setValue(current.x, current.y, current.z + 1, nRegions);
			toVisit.push(Coords(current.x, current.y, current.z + 1));
		}

		if (current.z - 1 >= 0 && scan->valueAt(current.x, current.y, current.z - 1) == 1 &&
			regions->valueAt(current.x, current.y, current.z - 1) == 0)
		{
			regions->setValue(current.x, current.y, current.z - 1, nRegions);
		    toVisit.push(Coords(current.x, current.y, current.z - 1));
		}
	}
	//printf("F\n");
	return true;
}

std::vector<Coords> regionSeeds(Scan scan)
{
	assert(scan.getMax() == 1 && scan.getMin() == 0 && "Scan must be binarized for seed finding");
	//Scan seeds(scan.getWidth(), scan.getHeight(), scan.getDepth());
	//Scan empty(scan.getWidth(), scan.getHeight(), scan.getDepth());
	Scan eroded = scan.copy();
	Scan previous = scan.copy();
	std::vector<Coords> seeds;
	printf("RegionSeeds\n");
	printf("w %d h %d d %d\n", scan.getWidth(), scan.getHeight(), scan.getDepth());
	int nr = 0;
	bool is_empty = false;
	while(!is_empty)//(!eroded.equals(empty))
	{
		is_empty = true; //in case empty array is passed to function
		nr++;
		printf("erosion cycle %d\n", nr);
		for (int i = 0; i < eroded.getDepth(); i++)
		{
			for (int j = 0; j < eroded.getHeight(); j++)
			{
				for (int k = 0; k < eroded.getWidth(); k++)
				{
//					if (i == 8 && j > 400)
//					{
//						printf("now at %d %d, trying to find source of crash -- ", j, k);
//						printf("er val: %d scn val %d\n", eroded.valueAt(k, j, i), scan.valueAt(k, j, i));
//					}
					if (eroded.valueAt(k, j, i) == 1)
					{
						int res = 1; //Initialize current minimum
						for (int x=(std::max)(i-1, 0); x<(std::min)(i+2, (int) scan.getDepth()); x++)
						{
							for (int y=(std::max)(j-1, 0); y<(std::min)(j+2, (int) scan.getHeight()); y++)
							{
								for (int z=(std::max)(k-1, 0); z<(std::min)(k+2, (int) scan.getWidth()); z++)
								{
									//printf("erode with %d %d %d ", x, y, z);
									//printf("old res: %d erode val: %d ", res, scan.valueAt(z, y, x));
									res = res && previous.valueAt(z, y, x);
									//printf("new res: %d\n", res);
								}	
							}	
						}
						//printf("\n");
						if (res == 0 && eroded.valueAt(k, j, i) == 1)
						{
							eroded.setValue(k, j, i, 0);
							bool neighborsLeft = 0;
							for (int x=(std::max)(i-1, 0); x<(std::min)(i+2, (int) scan.getDepth()); x++)
							{
								for (int y=(std::max)(j-1, 0); y<(std::min)(j+2, (int) scan.getHeight()); y++)
								{
									for (int z=(std::max)(k-1, 0); z<(std::min)(k+2, (int) scan.getWidth()); z++)
									{
										//printf("neigbors at %d %d %d ", x, y, z);
										neighborsLeft = neighborsLeft || eroded.valueAt(z, y, x);
									}	
								}	
							}
							//printf("\n");
							if (!neighborsLeft)
							{
								//printf("new last point\n");
								Coords seedPoint = Coords(k, j, i);
								bool newRegion = true;
								//for (int idx = 0; idx < seeds.size(); idx++)
								//{
								//	//printf("path %d\n", idx);
								//	if (findPath(scan, seeds.at(idx), seedPoint))
								//	{
								//		newRegion = false;
								//		break;
								//	}
								//}
								if (newRegion)
								{
									//printf("new region\n");
									seeds.push_back(seedPoint);
									//printf("Seed: %d %d %d\n", k, j, i);
								}
							}
						}
						else
						{
							is_empty = false;
						}
					}
				}
				//printf("height at: %d\n", j);
			}
			//printf("depth at: %d\n", i);
		}
		//printf("end of round %d\n", nr);
		previous = eroded.copy();
	}
	previous.freeValues();
	eroded.freeValues();
	return seeds;
}

Scan getRegions(Scan scan)
{
	Scan regions = Scan(scan.getWidth(), scan.getHeight(), scan.getDepth());
	std::vector<Coords> seeds = regionSeeds(scan);
	unsigned int nRegions = 1;
	printf("now filling %zd times\n", seeds.size());
	for (unsigned int i = 0; i < seeds.size(); i++)
	{
		Coords seed = seeds.at(i);
        //printf("from seed %d\n", i);

		if (regions.valueAt(seed.x, seed.y, seed.z) == 0)
		{
			if (fillRegion(&scan, &regions, nRegions, seeds.at(i)))
			{
				nRegions++;
			}
		}

	}
	return regions;
}













