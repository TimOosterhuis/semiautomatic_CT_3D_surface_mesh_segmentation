#ifndef VERTEXDISP_H
#define VERTEXDISP_H

#include <QVector3D>
#include <QVector>
#include <QDebug>

#include "C:\Users\Tim Oosterhuis\Documents\Uni\MSc_project_CS\segmentation\vertex.h"

class Face;

class VertexDisp {
public:
  QVector3D coords;
  QVector<int> adjFaces;
  unsigned short valence;
  unsigned int index;
  QVector<unsigned int> scanCoords;
  unsigned short region;

  // Inline constructors
  VertexDisp() {
    // qDebug() << "Default Vertex Constructor";
    coords = QVector3D();
    adjFaces = QVector<int>();
    valence = 0;
    index = 0;
  }

  VertexDisp(Vertex vert, unsigned int vindex)
  {
    coords = QVector3D(vert.coords.x, vert.coords.y, vert.coords.z);
    index = vindex;
    adjFaces = QVector<int>();
    valence = 0;
  }

  VertexDisp(QVector3D vcoords, QVector<int> vfaces, unsigned short vvalence, unsigned int vindex) {
    //qDebug() << "QVector3D Vertex Constructor";
    coords = vcoords;
    adjFaces = vfaces;
    valence = vvalence;
    index = vindex;
  }
};

#endif // VERTEX_H
