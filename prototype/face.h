#ifndef FACE
#define FACE

#include <QVector3D>

class VertexDisp;

class Face {

public:
  unsigned short valence;
  unsigned int index;
  QVector3D normal;
  QVector<VertexDisp> adjVertices;

  // Inline constructors

  Face() {
    valence = 0;
    index = 0;
    normal = QVector3D();
    adjVertices = QVector<VertexDisp>();
  }

  Face(unsigned int findex, QVector<VertexDisp> fvertices) {
    adjVertices = fvertices;
    valence = fvertices.size();
    index = findex;
    normal = QVector3D();
  }
};

#endif // FACE
