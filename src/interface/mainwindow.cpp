#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  qDebug() << "✓✓ MainWindow constructor";
}

MainWindow::~MainWindow() {
  delete ui;
  qDebug() << "✗✗ MainWindow destructor";
}

void MainWindow::on_yRotDial_valueChanged(int value) {
  ui->mainView->rotYAngle = value;
  ui->mainView->updateMatrices();
}

void MainWindow::on_xRotDial_valueChanged(int value) {
  ui->mainView->rotXAngle = value;
  ui->mainView->updateMatrices();
}

void MainWindow::on_zRotDial_valueChanged(int value) {
  ui->mainView->rotZAngle = value;
  ui->mainView->updateMatrices();
}
