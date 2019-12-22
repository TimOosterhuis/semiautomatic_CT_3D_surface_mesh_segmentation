#ifndef MESH_H
#define MESH_H

#include <QVector>
#include <QDataStream>
#include <QTextStream>
#include <QFile>
#include <fstream>

#include "vertexdisp.h"
#include "face.h"
#include "C:\Users\Tim Oosterhuis\Documents\Uni\MSc_project_CS\segmentation\scan.h"
#include "C:\Users\Tim Oosterhuis\Documents\Uni\MSc_project_CS\segmentation\marchingcubes.h"
#include "C:\Users\Tim Oosterhuis\Documents\Uni\MSc_project_CS\segmentation\im_processing.h"

class Mesh {

public:
  Mesh();
  Mesh(Scan scan, int th);
  Mesh(std::string fileDir);
  ~Mesh();

  QVector<VertexDisp> Vertices;
  QVector<Face> Faces;
  int nVertices;

  void setFaceNormal(Face* currentFace);
  QVector3D computeVertexNormal(VertexDisp* currentVertex);
  int find(VertexDisp);
  QVector<unsigned int> getScanCoords(unsigned int, unsigned int, unsigned int, unsigned int);
  unsigned int getVertIdx(unsigned int, unsigned int, unsigned int, unsigned int, unsigned int, unsigned int);
  bool hasEnding(std::string const &, std::string const &);

  float parse_float(std::ifstream& s);
  Vector parse_point(std::ifstream& s);
  // For debugging
  void dispVertInfo(unsigned short vertIndex);
  void dispHalfEdgeInfo(unsigned short edgeIndex);
  void dispFaceInfo(unsigned short faceIndex);

  void getRegions(Scan);
};

#endif // MESH_H
