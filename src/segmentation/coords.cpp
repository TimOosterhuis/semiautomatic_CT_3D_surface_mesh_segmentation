#include "coords.h"

Coords::Coords (int a, int b, int c)
{
	x = a;
	y = b;
	z = c;
}

bool Coords::equals(Coords other)
{
	return (x == other.x && y == other.y && z == other.z);
}
