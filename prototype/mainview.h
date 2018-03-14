#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QOpenGLFunctions_4_1_Core>
#include <QOpenGLWidget>
#include <QOpenGLDebugLogger>
#include <QOpenGLShaderProgram>

#include <QVector3D>
#include <QMouseEvent>
#include "mesh.h"

class MainView : public QOpenGLWidget, protected QOpenGLFunctions_4_1_Core {

  Q_OBJECT

public:
  MainView(QWidget *parent = 0);
  ~MainView();

  bool toggle;

  void updateScanBuffers(Mesh* currentMesh);
  void updateExampleBuffers(Mesh* currentMesh);
  void clearArrays();

  //wrapper
  void setExample();

  float FoV;
  float dispRatio;
  float rotYAngle;
  float rotXAngle;
  float rotZAngle;

  void updateMatrices();
  void updateMatricesScan();
  void updateMatricesExample();

protected:
  void initializeGL();
  void resizeGL(int newWidth, int newHeight);
  void paintGL();

  void renderMesh();
  void renderSecondMesh();

  void mousePressEvent(QMouseEvent* event);
  void wheelEvent(QWheelEvent* event);
  void keyPressEvent(QKeyEvent* event);


private:
  QOpenGLDebugLogger* debugLogger;

  QOpenGLShaderProgram* mainShaderProg;
  QOpenGLShaderProgram* rShaderProg;
  GLuint meshVAO, meshCoordsBO, meshNormalsBO, meshColorsBO, meshIndexBO;
  GLuint explMeshVAO, explMeshCoordsBO, explMeshNormalsBO, explMeshColorsBO, explMeshIndexBO;
  unsigned int meshIBOSize, explMeshIBOSize;

  void createShaderPrograms();
  void createBuffers();

  void updateUniforms();
  void updateUniformsScan();
  void updateUniformsExpl();

  unsigned int maxInt;
  bool updateUniformsRequired;

  QVector<QVector3D> vertexCoords;
  QVector<QVector3D> vertexNormals;
  QVector<QVector3D> vertexColors;
  QVector<unsigned int> polyIndices;

  QVector<QVector3D> explVertexCoords;
  QVector<QVector3D> explVertexNormals;
  QVector<QVector3D> explVertexColors;
  QVector<unsigned int> explPolyIndices;

  QMatrix4x4 modelViewMatrixScan, projectionMatrixScan;
  QMatrix3x3 normalMatrixScan;

  QMatrix4x4 modelViewMatrixExpl, projectionMatrixExpl;
  QMatrix3x3 normalMatrixExpl;

  GLint uniModelViewMatrixL, uniProjectionMatrixL, uniNormalMatrixL;
  GLint uniModelViewMatrixR, uniProjectionMatrixR, uniNormalMatrixR;

  QVector<QVector3D> cols = {QVector3D(1.0, 0.0, 0.0), QVector3D(0.0, 1.0, 0.0), QVector3D(0.0, 0.0, 1.0), QVector3D(1.0, 1.0, 0.0),
                            QVector3D(1.0, 0.0, 1.0), QVector3D(0.0, 1.0, 1.0), QVector3D(1.0, 1.0, 1.0), QVector3D(1.0, 0.5, 0.0),
                            QVector3D(1.0, 0.0, 0.5), QVector3D(0.0, 1.0, 0.5), QVector3D(0.5, 1.0, 0.0), QVector3D(0.0, 0.5, 1.0),
                            QVector3D(0.5, 0.0, 1.0), QVector3D(0.5, 0.5, 0.5), QVector3D(0.75, 0.75, 0.75)};

private slots:
  void onMessageLogged( QOpenGLDebugMessage Message );

};

#endif // MAINVIEW_H
