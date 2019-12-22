#include "vertex.h"

Vector::Vector(double a, double b, double c)
{
  x = a;
  y = b;
  z = c;
}

Vector::Vector(){}

bool Vector::eq(Vector v)
{
    return (x == v.x && y == v.y && z == v.z);
}

Vertex::Vertex(Vector c, Vector n)
{
  coords = c;
  normal = n;
}

Vertex Vertex::normalizedCentered(int w, int h, int d)
{
  Vertex copy(coords, normal);
  copy.coords.x = (coords.x / (float)w) - 0.5;
  copy.coords.y = (coords.y / (float)h) - 0.5;
  copy.coords.z = (coords.z / (float)d) - 0.5;
  return copy;
}

Vertex::Vertex(){}
