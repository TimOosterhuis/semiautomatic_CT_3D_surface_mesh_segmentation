#ifndef VERTEX_H
#define VERTEX_H

//#include "vector.h"

class Vector
{
  public:
  Vector (double, double, double);
  Vector();
  bool eq(Vector);
  double x;
  double y;
  double z;
};

class Vertex
{
  public:
  Vertex (Vector, Vector);
  Vertex ();
  Vector coords;
  Vector normal;
  int* neighbors;
  Vertex normalizedCentered(int, int, int);
};

#endif // VERTEX_H
