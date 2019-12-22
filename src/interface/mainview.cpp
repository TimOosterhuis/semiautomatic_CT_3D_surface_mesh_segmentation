#include "mainview.h"
#include "math.h"


MainView::MainView(QWidget *parent) : QOpenGLWidget(parent) {
  qDebug() << "✓✓ MainView constructor";

  rotYAngle = 0.0;
  rotXAngle = 0.0;
  rotZAngle = 0.0;
  FoV = 60.0;
  toggle = true;
}

MainView::~MainView() {

  clearArrays();

  glDeleteBuffers(1, &meshCoordsBO);
  glDeleteBuffers(1, &meshNormalsBO);
  glDeleteBuffers(1, &meshColorsBO);
  glDeleteBuffers(1, &meshIndexBO);
  glDeleteVertexArrays(1, &meshVAO);

  glDeleteBuffers(1, &explMeshCoordsBO);
  glDeleteBuffers(1, &explMeshNormalsBO);
  glDeleteBuffers(1, &explMeshColorsBO);
  glDeleteBuffers(1, &explMeshIndexBO);
  glDeleteVertexArrays(1, &meshVAO);
  delete mainShaderProg;
  delete rShaderProg;

  debugLogger->stopLogging();

  qDebug() << "✗✗ MainView destructor";
}

// ---

void MainView::createShaderPrograms() {

  // Qt approach
  mainShaderProg = new QOpenGLShaderProgram();
  mainShaderProg->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertshader.glsl");
  mainShaderProg->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragshader.glsl");

  mainShaderProg->link();

  rShaderProg = new QOpenGLShaderProgram();
  rShaderProg->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertshader.glsl");
  rShaderProg->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragshader.glsl");

  rShaderProg->link();

  // Qt wrappers
  // uniModelViewMatrix = mainShaderProg->uniformLocation("modelviewmatrix");
  // uniProjectionMatrix = mainShaderProg->uniformLocation("projectionmatrix");

  // Pure OpenGL
  uniModelViewMatrixL = glGetUniformLocation(mainShaderProg->programId(), "modelviewmatrix");
  uniProjectionMatrixL = glGetUniformLocation(mainShaderProg->programId(), "projectionmatrix");
  uniNormalMatrixL = glGetUniformLocation(mainShaderProg->programId(), "normalmatrix");
  uniModelViewMatrixR = glGetUniformLocation(rShaderProg->programId(), "modelviewmatrix");
  uniProjectionMatrixR = glGetUniformLocation(rShaderProg->programId(), "projectionmatrix");
  uniNormalMatrixR = glGetUniformLocation(rShaderProg->programId(), "normalmatrix");
}

void MainView::createBuffers() {

  qDebug() << ".. createBuffers";

  // Pure OpenGL
  glGenVertexArrays(1, &meshVAO);
  glBindVertexArray(meshVAO);

  glGenBuffers(1, &meshCoordsBO);
  glBindBuffer(GL_ARRAY_BUFFER, meshCoordsBO);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

  glGenBuffers(1, &meshNormalsBO);
  glBindBuffer(GL_ARRAY_BUFFER, meshNormalsBO);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

  glGenBuffers(1, &meshColorsBO);
  glBindBuffer(GL_ARRAY_BUFFER, meshColorsBO);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

  glGenBuffers(1, &meshIndexBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshIndexBO);

  glBindVertexArray(0);

  // Pure OpenGL
  glGenVertexArrays(1, &explMeshVAO);
  glBindVertexArray(explMeshVAO);

  glGenBuffers(1, &explMeshCoordsBO);
  glBindBuffer(GL_ARRAY_BUFFER, explMeshCoordsBO);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

  glGenBuffers(1, &explMeshNormalsBO);
  glBindBuffer(GL_ARRAY_BUFFER, explMeshNormalsBO);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

  glGenBuffers(1, &explMeshColorsBO);
  glBindBuffer(GL_ARRAY_BUFFER, explMeshColorsBO);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

  glGenBuffers(1, &explMeshIndexBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, explMeshIndexBO);

  glBindVertexArray(0);

}

void MainView::updateScanBuffers(Mesh* currentMesh) {

  qDebug() << ".. updateBuffers";
  qDebug() << "current mesh v:" << currentMesh->nVertices << "f:" << currentMesh->Faces.size();

  clearArrays();

  vertexCoords.clear();
  vertexCoords.reserve(currentMesh->nVertices);
//  int goober = 0;
  for (unsigned int k=0; k<currentMesh->Vertices.size(); k++) {
    if (currentMesh->Vertices[k].index != maxInt)
    {
//        goober++;
        vertexCoords.append(currentMesh->Vertices[k].coords);
//        float gfx = (currentMesh->Vertices[k].coords.x());
//        float gfy = (currentMesh->Vertices[k].coords.y());
//        float gfz = (currentMesh->Vertices[k].coords.z());
    }
  }
//  qDebug() << "verts stored " << goober;
  vertexNormals.clear();
  vertexNormals.reserve(currentMesh->nVertices);
  for (unsigned int k=0; k<currentMesh->Faces.size(); k++) {
    currentMesh->setFaceNormal(&currentMesh->Faces[k]);
//    float ggx = currentMesh->Faces[k].normal.x();
//    float ggy = currentMesh->Faces[k].normal.y();
//    float ggz = currentMesh->Faces[k].normal.z();
  }
//  int gabber = 0;
  for (unsigned int k=0; k<currentMesh->Vertices.size(); k++) {
    if (currentMesh->Vertices[k].index != maxInt)
    {
      vertexNormals.append( currentMesh->computeVertexNormal(&currentMesh->Vertices[k]) );
//      float ggx = vertexNormals.at(k).x();
//      float ggy = vertexNormals.at(k).y();
//      float ggz = vertexNormals.at(k).z();
//      if ((ggx  != ggx  || ggy != ggy || ggz != ggz))
//      {
//         if (gabber == 0)
//         {
//             qDebug() << "example of abnormal normal" << ggx << ggy << ggz;
//             qDebug() << "faces sz" << currentMesh->Vertices[k].adjFaces.size();
//             qDebug() << "valence" << currentMesh->Vertices[k].valence;
//         }
//         gabber++;
//      }
    }
  }
//  qDebug() << "abnormal normals" << gabber;

  vertexColors.clear();
  vertexColors.reserve(currentMesh->nVertices);
  for (int i = 0; i < currentMesh->Vertices.size(); i++)
  {
     //currentMesh->Vertices[i].region = 0;
     vertexColors.append(0.8*cols.at(currentMesh->Vertices[i].region%cols.length()));
  }
  //polygons are triangles in practice for simplicity's sake
  polyIndices.clear();
  polyIndices.reserve(4 * currentMesh->Faces.size());
  for (unsigned int k=0; k<currentMesh->Faces.size(); k++)
  {
    //qDebug() << "Face" << k;
    for (unsigned int i = 0; i<currentMesh->Faces.at(k).valence; i++)
    {
      polyIndices.append(currentMesh->Faces.at(k).adjVertices.at(i).index);
      //qDebug() << "Vert index" << currentMesh->Faces.at(k).adjVertices.at(i).index;
    }
    polyIndices.append(maxInt);
  }
  qDebug() << "sz" << polyIndices.size();
  glBindBuffer(GL_ARRAY_BUFFER, meshCoordsBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D)*vertexCoords.size(), vertexCoords.data(), GL_DYNAMIC_DRAW);

  qDebug() << " → Updated meshCoordsBO";

  glBindBuffer(GL_ARRAY_BUFFER, meshNormalsBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D)*vertexNormals.size(), vertexNormals.data(), GL_DYNAMIC_DRAW);

  qDebug() << " → Updated meshNormalsBO";

  glBindBuffer(GL_ARRAY_BUFFER, meshColorsBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D)*vertexColors.size(), vertexColors.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshIndexBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*polyIndices.size(), polyIndices.data(), GL_DYNAMIC_DRAW);
  qDebug() << " → Updated meshIndexBO";


  meshIBOSize = polyIndices.size();
  qDebug() << meshIBOSize;
  update();
}

void MainView::updateExampleBuffers(Mesh *currentMesh)
{
  qDebug() << "updating example mesh";
  clearArrays();

  float  minx = 999999, maxx = -999999, miny = 999999, maxy = -999999, minz = 999999, maxz = -999999;
  vertexCoords.clear();
  vertexCoords.reserve(currentMesh->nVertices);
  for (int i = 0; i < currentMesh->Vertices.size(); i++)
  {
      vertexCoords.append(currentMesh->Vertices[i].coords);
      minx = (std::min)(minx, currentMesh->Vertices[i].coords.x());
      maxx = (std::max)(maxx, currentMesh->Vertices[i].coords.x());
      miny = (std::min)(miny, currentMesh->Vertices[i].coords.y());
      maxy = (std::max)(maxy, currentMesh->Vertices[i].coords.y());
      minz = (std::min)(minz, currentMesh->Vertices[i].coords.z());
      maxz = (std::max)(maxz, currentMesh->Vertices[i].coords.z());
  }
  qDebug() << "NIEUWNIEUWNIEUW" << "xrange" << minx << maxx << "yrange" << miny << maxy << "zrange" << minz << maxz;
  vertexNormals.clear();
  vertexNormals.reserve(currentMesh->nVertices);
  for (unsigned int k=0; k<currentMesh->Faces.size(); k++)
  {
    currentMesh->setFaceNormal(&currentMesh->Faces[k]);
  }

  for (int i = 0; i < currentMesh->Vertices.size(); i++)
  {
    vertexNormals.append( currentMesh->computeVertexNormal(&currentMesh->Vertices[i])*(-1) );
  }

  vertexColors.clear();
  vertexColors.reserve(currentMesh->nVertices);
  for (int i = 0; i < currentMesh->Vertices.size(); i++)
  {
    QVector3D col = 0.8*cols.at(currentMesh->Vertices[i].region%cols.length());
    //qDebug() << "Kleur kleur" << col.x() << col.y() << col.z();
    //qDebug() << "Kleur kleur" << 0.8*cols.at(1).x() << 0.8*cols.at(1).y() << 0.8*cols.at(1).z();
     vertexColors.append(col);
  }

  polyIndices.clear();
  polyIndices.reserve(4 * currentMesh->Faces.size());
  for (unsigned int k=0; k<currentMesh->Faces.size(); k++)
  {
    for (unsigned int i = 0; i<currentMesh->Faces.at(k).valence; i++)
    {
      polyIndices.append(currentMesh->Faces.at(k).adjVertices.at(i).index);
    }
    polyIndices.append(maxInt);
  }

  glBindBuffer(GL_ARRAY_BUFFER, explMeshCoordsBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D)*vertexCoords.size(), vertexCoords.data(), GL_DYNAMIC_DRAW);

  qDebug() << " → Updated example meshCoordsBO";

  glBindBuffer(GL_ARRAY_BUFFER, explMeshNormalsBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D)*vertexNormals.size(), vertexNormals.data(), GL_DYNAMIC_DRAW);

  qDebug() << " → Updated example meshNormalsBO";

  glBindBuffer(GL_ARRAY_BUFFER, explMeshColorsBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(QVector3D)*vertexColors.size(), vertexColors.data(), GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, explMeshIndexBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*polyIndices.size(), polyIndices.data(), GL_DYNAMIC_DRAW);
  qDebug() << " → Updated example meshIndexBO";


  explMeshIBOSize = polyIndices.size();
  qDebug() << explMeshIBOSize;

  update();
}

void MainView::updateMatrices() {
  qDebug() << "update matrices";
  updateMatricesScan();
  updateMatricesExample();

  updateUniformsRequired = true;
  update();
}

void MainView::updateMatricesScan()
{
    modelViewMatrixScan.setToIdentity();
    modelViewMatrixScan.translate(QVector3D(0.0, 0.0, -3.0));
    modelViewMatrixScan.scale(QVector3D(1.0, 1.0, 1.0));
    modelViewMatrixScan.rotate(rotYAngle*3, QVector3D(0.0, 1.0, 0.0));
    modelViewMatrixScan.rotate(rotXAngle*3, QVector3D(1.0, 0.0, 0.0));
    modelViewMatrixScan.rotate(rotZAngle*3, QVector3D(0.0, 0.0, 1.0));

    projectionMatrixScan.setToIdentity();
    projectionMatrixScan.translate(QVector3D(-0.5, 0.0, 0.0));
    projectionMatrixScan.perspective(FoV, dispRatio, 0.2, 4.0);

    normalMatrixScan = modelViewMatrixScan.normalMatrix();
}

void MainView::updateMatricesExample()
{
    modelViewMatrixExpl.setToIdentity();
    modelViewMatrixExpl.translate(QVector3D(0.0, 0.0, -3.0));
    modelViewMatrixExpl.scale(QVector3D(1.0, 1.0, 1.0));
    modelViewMatrixExpl.rotate(rotYAngle*3, QVector3D(0.0, 1.0, 0.0));
    modelViewMatrixExpl.rotate(rotXAngle*3, QVector3D(1.0, 0.0, 0.0));
    modelViewMatrixExpl.rotate(rotZAngle*3, QVector3D(0.0, 0.0, 1.0));

    projectionMatrixExpl.setToIdentity();
    projectionMatrixExpl.translate(QVector3D(0.5, 0.0, 0.0));
    projectionMatrixExpl.perspective(FoV, dispRatio, 0.2, 4.0);

    normalMatrixExpl = modelViewMatrixExpl.normalMatrix();
}

void MainView::updateUniforms()
{
    mainShaderProg->bind();
    updateUniformsScan();
    mainShaderProg->release();
    rShaderProg->bind();
    updateUniformsExpl();
    rShaderProg->release();
    qDebug() << "updated my uniforms";
    updateUniformsRequired = false;
}

void MainView::updateUniformsScan() {

  // Qt wrappers
  // mainShaderProg->setUniformValue(uniModelViewMatrix, modelViewMatrix);
  // mainShaderProg->setUniformValue(uniProjectionMatrix, projectionMatrix);

  // Pure OpenGL
  glUniformMatrix4fv(uniModelViewMatrixL, 1, false, modelViewMatrixScan.data());
  glUniformMatrix4fv(uniProjectionMatrixL, 1, false, projectionMatrixScan.data());
  glUniformMatrix3fv(uniNormalMatrixL, 1, false, normalMatrixScan.data());
  //qDebug() << "updated my uniforms";
  //updateUniformsRequired = false;
}

void MainView::updateUniformsExpl() {

  // Qt wrappers
  // mainShaderProg->setUniformValue(uniModelViewMatrix, modelViewMatrix);
  // mainShaderProg->setUniformValue(uniProjectionMatrix, projectionMatrix);

  // Pure OpenGL
  glUniformMatrix4fv(uniModelViewMatrixR, 1, false, modelViewMatrixExpl.data());
  glUniformMatrix4fv(uniProjectionMatrixR, 1, false, projectionMatrixExpl.data());
  glUniformMatrix3fv(uniNormalMatrixR, 1, false, normalMatrixExpl.data());
  //qDebug() << "updated my uniforms";
  //updateUniformsRequired = false;
}

void MainView::clearArrays() {
  vertexCoords.clear();
  vertexCoords.squeeze();
  vertexNormals.clear();
  vertexNormals.squeeze();
  polyIndices.clear();
  polyIndices.squeeze();
}

// ---

void MainView::initializeGL() {

  qDebug() << ":: Initializing OpenGL";
  initializeOpenGLFunctions();

  debugLogger = new QOpenGLDebugLogger();
  connect( debugLogger, SIGNAL( messageLogged( QOpenGLDebugMessage ) ), this, SLOT( onMessageLogged( QOpenGLDebugMessage ) ), Qt::DirectConnection );

  if ( debugLogger->initialize() ) {
    qDebug() << ":: Logging initialized";
    debugLogger->startLogging( QOpenGLDebugLogger::SynchronousLogging );
    debugLogger->enableMessages();
  }

  QString glVersion;
  glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
  qDebug() << ":: Using OpenGL" << qPrintable(glVersion);

  // Enable depth buffer
  glEnable(GL_DEPTH_TEST);
  // Default is GL_LESS
  glDepthFunc(GL_LEQUAL);

  glPointSize(16);

  glEnable(GL_PRIMITIVE_RESTART);
  maxInt = ((unsigned int) -1);
  glPrimitiveRestartIndex(maxInt);

  // ---

  createShaderPrograms();
  createBuffers();

  Mesh someMesh("C:\\Users\\Tim Oosterhuis\\Documents\\Uni\\MSc_project_CS\\test dataset TIM_SKULL\\");

  Scan scan("C:\\Users\\Tim Oosterhuis\\Documents\\Uni\\MSc_project_CS\\test dataset TIM_SKULL\\CT\\");
  Scan thresholded = absThreshold(scan, scan.getMax()*0.5);
  Scan regions = getRegions(thresholded);
  //thresholded.freeValues();
  //Scan regions = (scan);
  //Scan scan(4, 4, 4);
  //scan.createExample();
  thresholded.freeValues();
  //qDebug() << "max is :" << scan.getMax();
  Mesh exampleMesh(scan, scan.getMax()*0.5);
  exampleMesh.getRegions(regions);
  updateScanBuffers(&exampleMesh);

  updateExampleBuffers(&someMesh);
  updateMatrices();
  scan.freeValues();

  regions.freeValues();
}

void MainView::setExample()
{
    Scan scan(4,4,4);
    scan.createExample();
    Mesh exampleMesh(scan, 1);

    updateScanBuffers(&exampleMesh);

    updateMatrices();
}

void MainView::resizeGL(int newWidth, int newHeight) {
  qDebug() << ".. resizeGL";

  dispRatio = (float)newWidth/newHeight;
  updateMatrices();
}

void MainView::paintGL() {
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



  qDebug() << "do i need to update?" << updateUniformsRequired;
  if (updateUniformsRequired) {
    updateUniforms();
  }
  //if (toggle)
  //{
  mainShaderProg->bind();
  //updateMatricesScan();

  //updateUniformsScan();
  renderMesh();
  mainShaderProg->release();
  //} else {
  rShaderProg->bind();
  //updateMatricesExample();

  //updateUniformsExpl();
  renderSecondMesh();
  rShaderProg->release();
  //}
}


void MainView::renderMesh() {
  qDebug() << "render" << meshVAO;
  //mainShaderProg->bind();
  glBindVertexArray(meshVAO);
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
  glEnable(GL_PRIMITIVE_RESTART);
  glPrimitiveRestartIndex(maxInt);
  //qDebug() << "primitive restart with" << maxInt;
  glDisable(GL_CULL_FACE);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshIndexBO);
  glDrawElements(GL_TRIANGLE_FAN, meshIBOSize, GL_UNSIGNED_INT, 0);
  if(glGetError() == GL_NO_ERROR)
  {
      qDebug() << "errors fine";
  } else {
      qDebug() << "not fine:" << glGetError();
  }
  //qDebug() << "shader status" << mainShaderProg->isLinked();
  glBindVertexArray(0);
}

void MainView::renderSecondMesh() {
  qDebug() << "RENDERRENDER" << explMeshVAO;
  glBindVertexArray(explMeshVAO);
  glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
  glEnable(GL_PRIMITIVE_RESTART);
  glPrimitiveRestartIndex(maxInt);
  glDisable(GL_CULL_FACE);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, explMeshIndexBO);
  glDrawElements(GL_TRIANGLE_FAN, explMeshIBOSize, GL_UNSIGNED_INT, 0);

  glBindVertexArray(0);
  //mainShaderProg->release();
}



// ---

void MainView::mousePressEvent(QMouseEvent* event) {
  setFocus();
}

void MainView::wheelEvent(QWheelEvent* event) {
  FoV -= event->delta() / 60.0;
  updateMatrices();
}

void MainView::keyPressEvent(QKeyEvent* event) {
  switch(event->key()) {
  case 'Z':
     FoV -= 1;
     updateMatrices();
     break;
  case 'X':
     FoV += 1;
     //qDebug() << "zzzzzzzzzzzzzzzzzzzzzz";
     updateMatrices();
     break;
  case 'T':
      toggle = !toggle;
      //qDebug() << "aaaaaaaaaaaaaaaaaaaaa" << toggle;
      updateMatrices();
      break;
  }
}

void MainView::onMessageLogged( QOpenGLDebugMessage Message ) {
  qDebug() << " → Log:" << Message;
}
