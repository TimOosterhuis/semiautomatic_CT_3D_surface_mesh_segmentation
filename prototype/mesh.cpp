#include "mesh.h"
#include "math.h"

Mesh::Mesh() {
  qDebug() << "✓✓ Mesh constructor (Empty)";
}

Mesh::Mesh(Scan scan, int th) {

  qDebug() << "✓✓ Mesh constructor (scan)";
  unsigned int maxInt = (unsigned int)-1;
  qDebug() << " scan depth width height " << scan.getDepth() << scan.getWidth() << scan.getHeight();
  int num_edges = scan.getDepth()*scan.getHeight()*scan.getWidth()*3 ;
  qDebug() << num_edges;
  int *vertIndices;
  vertIndices = (int*) calloc(num_edges, sizeof(int));
  for (int t = 0; t < num_edges; t++)
  {
      vertIndices[t] = -1;
  }

  nVertices = 0;
  unsigned int idxVerts = 0;
  unsigned int idxFaces = 0;
  unsigned int w = scan.getWidth() - 1, h = scan.getHeight() - 1, d = scan.getDepth() - 1;

  int cube_nVertices = 0;
  int cube_nTriangles = 0;
  int cnt = 0;
  qDebug() << "this right? " << Vertices.size();
  for (unsigned int i = 0; i < d; i++)
  {
    for (unsigned int j = 0; j < h; j++)
    {
      for (unsigned int k = 0; k < w; k++)
      {
        cnt++;
          if (cnt%1000000==0)
          {
              qDebug() << cnt/1000000 << "times million cube";
          }
        //add poligons and vertices from marching cube(value at i, j, k,...,i + 1, j + 1, k + 1)//raadpleeg tabeletjes
        int pos[3] = {k, j, i};
        int vals[8];
        scan.getCube(vals, k, j, i);
        cubeInfo cube = marchingCube(pos, th, vals);
        cube_nVertices += cube.nVertices;
        cube_nTriangles += cube.nTriangles;
        //qDebug() << " cube found v" << cube.nVertices << "t" << cube.nTriangles;
        int meshVertIdx[12] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
        int meshVertPlc[12] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
        int doubleVerts = 0;
        for (int vertI = 0; vertI < cube.nVertices; vertI++)
        {
          //qDebug() << "vertex at edge: " << cube.relVertIdx[vertI];
          VertexDisp newVert(cube.vertices.at(vertI).normalizedCentered(w, h, d), idxVerts + vertI - doubleVerts);
          unsigned int vIdx = getVertIdx(scan.getWidth(), scan.getHeight(), k, j, i, cube.relVertIdx[vertI]);

          //qDebug() << " - " << vIdx << " with coords " << newVert.coords.x() <<  newVert.coords.y() <<  newVert.coords.z();
          if (vertIndices[vIdx] == -1)
          {
              //qDebug() << " new vertix at " << vIdx << " with coords " << newVert.coords.x() <<  newVert.coords.y() <<  newVert.coords.z();
              newVert.scanCoords = getScanCoords(k, j, i, cube.relVertIdx[vertI]);
              Vertices.push_back(newVert);
              meshVertIdx[cube.relVertIdx[vertI]] = idxVerts + vertI - doubleVerts;
              vertIndices[vIdx] = idxVerts + vertI - doubleVerts;
              nVertices++;
          } else {
              doubleVerts++;
              meshVertIdx[cube.relVertIdx[vertI]] = vertIndices[vIdx];
          }
          meshVertPlc[cube.relVertIdx[vertI]] = (int)vIdx;

        }

        for (int triaI = 0; triaI < cube.nTriangles; triaI++)
        {
          QVector<VertexDisp> triaVerts(0);

          VertexDisp firstVert = Vertices.at(meshVertIdx[cube.relTriaIdx[triaI*3]]);
          VertexDisp secondVert = Vertices.at(meshVertIdx[cube.relTriaIdx[triaI*3 + 1]]);
          VertexDisp thirdVert = Vertices.at(meshVertIdx[cube.relTriaIdx[triaI*3 + 2]]);
          if (firstVert.coords == secondVert.coords ||
              firstVert.coords == thirdVert.coords ||
              secondVert.coords == thirdVert.coords)
          {
            continue;
          }
          triaVerts.push_back(firstVert);
          triaVerts.push_back(secondVert);
          triaVerts.push_back(thirdVert);

          Faces.push_back(Face(idxFaces, triaVerts));

          firstVert.adjFaces.push_back(idxFaces);
          firstVert.valence++;
          Vertices[meshVertIdx[cube.relTriaIdx[triaI*3]]] = firstVert;
          secondVert.adjFaces.push_back(idxFaces);
          secondVert.valence++;
          Vertices[meshVertIdx[cube.relTriaIdx[triaI*3 + 1]]] = secondVert;
          thirdVert.adjFaces.push_back(idxFaces);
          thirdVert.valence++;
          Vertices[meshVertIdx[cube.relTriaIdx[triaI*3 + 2]]] = thirdVert;

          idxFaces++;
        }
        idxVerts += (cube.nVertices - doubleVerts);
      }
    }
  }
  qDebug() << "total found v" << cube_nVertices << "t" << cube_nTriangles;
  free(vertIndices);
}

Mesh::Mesh(std::string fileDir)
{
  DIR *dir;
  struct dirent *ent;
  qDebug() << "" << fileDir.c_str();
  if ((dir = opendir (fileDir.c_str())) != NULL)
  {
    float  minx = 999999, maxx = -999999, miny = 999999, maxy = -999999, minz = 999999, maxz = -999999;
    nVertices = 0;
    unsigned int nSegments = 0;
    unsigned int nFaces = 0;
    while ((ent = readdir (dir)) != NULL) {
      if(hasEnding(ent->d_name, ".stl"))
      {
        qDebug() << ent->d_name;
        std::ifstream input;
        std::string fileName = ent->d_name;
        std::string filePath = fileDir + fileName;

        QString pathAsQSring(filePath.c_str());
        QFile stlFile(pathAsQSring);
        if(!stlFile.open(QIODevice::ReadOnly))
                return;

        QVector3D vn;
        QVector3D point1;
        QVector3D point2;
        QVector3D point3;

        Face *tempFace;

        stlFile.seek(0);
        QDataStream in(&stlFile);
        in.setByteOrder(QDataStream::LittleEndian);
        in.setFloatingPointPrecision(QDataStream::SinglePrecision);
        //unsigned int n_trias = 0;

        quint32 n_trias;
        quint16 control_bytes;
        quint32 count = 0;
        stlFile.seek(80);
        in >> n_trias;
        //input.read(reinterpret_cast<char*>(&n_trias), 4);
        qDebug() << n_trias << " triangles in this .stl";
        int n_verts = 0;
        float nx, ny, nz, x1, y1, z1, x2, y2, z2, x3, y3, z3;
        while (count < n_trias)
        {
          stlFile.seek(84+count*50+0+0);
          in >> nx;
          stlFile.seek(84+count*50+0+4);
          in >> ny;
          stlFile.seek(84+count*50+0+8);
          in >> nz;
          stlFile.seek(84+count*50+12+0);
          in >> x1;
          stlFile.seek(84+count*50+12+4);
          in >> y1;
          stlFile.seek(84+count*50+12+8);
          in >> z1;
          stlFile.seek(84+count*50+24+0);
          in >> x2;
          stlFile.seek(84+count*50+24+4);
          in >> y2;
          stlFile.seek(84+count*50+24+8);
          in >> z2;
          stlFile.seek(84+count*50+36+0);
          in >> x3;
          stlFile.seek(84+count*50+36+4);
          in >> y3;
          stlFile.seek(84+count*50+36+8);
          in >> z3;
          stlFile.seek(84+count*50+48);
          in >> control_bytes;
          if (control_bytes == 0 && in.status() == QDataStream::Ok)
          {
            vn = QVector3D(nx, ny, nz);
            Vector firstCoords = Vector(x1, y1, z1);
            Vector secondCoords = Vector(x2, y2, z2);
            Vector thirdCoords = Vector(x3, y3, z3);

            minx = (std::min)(minx, x1);
            maxx = (std::max)(maxx, x1);
            miny = (std::min)(miny, y1);
            maxy = (std::max)(maxy, y1);
            minz = (std::min)(minz, z1);
            maxz = (std::max)(maxz, z1);

            QVector<Vector> verts = {firstCoords, secondCoords, thirdCoords};

            QVector<VertexDisp> triaVerts(0);
            for (int v = 0; v < 3; v++)
            {
              VertexDisp newVert(Vertex(verts.at(v), Vector()), nVertices);
              //qDebug() << newVert.coords.x << newVert.coords.y << newVert.coords.z;
              int idx = -1;// find(newVert);
              if (idx > 0)
              {
                newVert = Vertices.at(idx);
                newVert.adjFaces.push_back(nFaces);
                newVert.valence++;
                Vertices[idx] = newVert;
              } else
              {
                idx = nVertices;
                nVertices++;
                n_verts++;
                newVert.adjFaces.push_back(nFaces);
                newVert.valence++;
                newVert.region = nSegments;
                Vertices.push_back(newVert);
              }
              triaVerts.push_back(newVert);
            }
            Face face = Face(nFaces, triaVerts);
            face.normal = vn;
            Faces.push_back(face);
            nFaces++;
            count++;
          }

        }
        nSegments++;
        qDebug() << "total verts in file" << nSegments << ":" << n_verts;

        //nVertices += n_verts;
        //input.close();
      }
    }
    qDebug() << nVertices << "xrange" << minx << maxx << "yrange" << miny << maxy << "zrange" << minz << maxz;
    for (int i = 0; i < nVertices; i++)
    {
      VertexDisp vert = Vertices.at(i);
      vert.coords.setX((vert.coords.x() - minx)/(maxx-minx) - 0.5);
      vert.coords.setY((vert.coords.y() - miny)/(maxx-minx) - 0.5);
      vert.coords.setZ((vert.coords.z() - minz)/(maxx-minx) - 0.5);
      Vertices[i] = vert;
    }
    closedir (dir);
  } else {
    /* could not open directory */
    perror ("could not open directory");
  }
}

QVector<unsigned int> Mesh::getScanCoords(unsigned int w, unsigned int h, unsigned int d, unsigned int edge)
{
    unsigned int dw = 0, dh = 0, dd = 0;
    if (edge == 1 || edge == 5 || edge == 9 || edge == 10)
    {
        w++;
    }
    if (edge > 3 && edge <= 7)
    {
        d++;
    }
    if (edge == 2 || edge == 6 || edge == 10 || edge == 11)
    {
        h++;
    }
    if (edge > 7)
    {
        dd = 1;
    }
    else if (edge%2 == 1)
    {
        dh = 1;
    }
    else
    {
        dw = 1;
    }
    return QVector<unsigned int>{w, h, d, w+dw, h+dh, d+dd};
}

bool Mesh::hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

Mesh::~Mesh() {
  qDebug() << "✗✗ Mesh destructor";

  qDebug() << "   # Vertices:" << Vertices.size();
  qDebug() << "   # Faces:" << Faces.size();

  Vertices.clear();
  Vertices.squeeze();
  Faces.clear();
  Faces.squeeze();
}

int Mesh::find(VertexDisp vertex)
{
    qDebug() << "finding vert among:" << Vertices.size();
    for (int i = 0; i<Vertices.size(); i++)
    {
        VertexDisp v = Vertices.at(i);
        if (v.coords.x() == vertex.coords.x() && v.coords.y() == vertex.coords.y() && v.coords.z() == vertex.coords.z())
        {
            return v.index;
        }
    }
    return -1;
}

void Mesh::setFaceNormal(Face* currentFace) {

  QVector3D faceNormal;
  int k;

  faceNormal = QVector3D(0.0, 0.0, 0.0);

  for (k=0; k<currentFace->valence; k++) {
    faceNormal += QVector3D::crossProduct(
          currentFace->adjVertices.at((k+1)%currentFace->valence).coords - currentFace->adjVertices.at(k).coords,
          currentFace->adjVertices.at((k+currentFace->valence-1)%currentFace->valence).coords - currentFace->adjVertices.at(k).coords );
  }
  currentFace->normal = faceNormal / faceNormal.length();
}

//static const int edgeConnection[12][2] =
//{
//  {0,1}, {1,2}, {2,3}, {3,0},
//  {4,5}, {5,6}, {6,7}, {7,4},
//  {0,4}, {1,5}, {2,6}, {3,7}
//};

unsigned int Mesh::getVertIdx(unsigned int w, unsigned int h, unsigned int k, unsigned int j, unsigned int i, unsigned int edge)
{
  //qDebug() << "w h" << w << h << "k" << k << "j" << j << "i" << i << "edge" << edge;
  if (edge == 1 || edge == 5 || edge == 9 || edge == 10)
  {
      k++;
  }
  if (edge > 3 && edge <= 7)
  {
      i++;
  }
  if (edge == 2 || edge == 6 || edge == 10 || edge == 11)
  {
      j++;
  }
  int dir = 0;
  if (edge > 7)
  {
      dir += 2;
  }
  else if (edge%2 == 1)
  {
      dir++;
  }
  //qDebug() << "vIdx" << (w*h*i + w*j + k)*3 + dir;
  return (w*h*i + w*j + k)*3 + dir;
}

QVector3D Mesh::computeVertexNormal(VertexDisp* currentVertex) {

  QVector3D vertexNormal;
  int k;
  float faceAngle;

  vertexNormal = QVector3D(0.0, 0.0, 0.0);
  for (k=0; k<currentVertex->valence; k++) {

//    unsigned short faceVal = currentVertex->adjFaces.at(k).valence;
//    for (int i=0; i<3; i++)
//    {
//        faceAngle += acos( fmax(-1.0, QVector3D::dotProduct(
//                    (currentVertex->adjFaces.at(k).adjVertices.at((i+faceVal-1)%faceVal).coords - currentVertex->coords).normalized(),
//                    (currentVertex->adjFaces.at(k).adjVertices.at((i+1)%faceVal).coords - currentVertex->coords).normalized() )));
//        qDebug() << "face angle:" << faceAngle;
//    }


    vertexNormal += Faces.at(currentVertex->adjFaces.at(k)).normal;


  }

  return (-1)*vertexNormal / vertexNormal.length();

}

void Mesh::getRegions(Scan regions)
{
    for (int i; i < Vertices.size(); i++)
    {
        QVector<unsigned int> coords = Vertices.at(i).scanCoords;
        Vertices[i].region = (std::max)(regions.valueAt(coords[0], coords[1], coords[2]), regions.valueAt(coords[3], coords[4], coords[5]));
    }
}


void Mesh::dispVertInfo(unsigned short vertIndex) {
  VertexDisp* dispVert = &Vertices[vertIndex];
  qDebug() << "Vertex at Index =" << dispVert->index << "Coords =" << dispVert->coords << "Val =" << dispVert->valence;
}

void Mesh::dispFaceInfo(unsigned short faceIndex){
  Face* dispFace = &Faces[faceIndex];
  qDebug() << "Face at Index =" << dispFace->index << "Val =" << dispFace->valence;
}
