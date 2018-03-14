#include <stdlib.h>
#include <stdio.h>

#pragma once

class Coords
{
	public:
	Coords (int, int, int);
	int x;
	int y;
	int z;
	bool equals(Coords);
};
