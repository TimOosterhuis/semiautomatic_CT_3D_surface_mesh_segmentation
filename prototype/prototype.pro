#-------------------------------------------------
#
# Project created by QtCreator 2018-02-06T10:37:04
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BasicOpenGL
TEMPLATE = app

INCLUDEPATH += D:/Programs/ImageMagick-7.0.7-Q16/lib \
    D:/Programs/ImageMagick-7.0.7-Q16/include \
    D:/Programs/ImageMagick-7.0.7-Q16

LIBS += D:/Programs/ImageMagick-7.0.7-Q16/lib/CORE_RL_MagickCore_.lib \
    D:/Programs/ImageMagick-7.0.7-Q16/lib/CORE_RL_Magick++_.lib

SOURCES += main.cpp\
        mainwindow.cpp \
    mainview.cpp \
    mesh.cpp \
    ../segmentation/coords.cpp \
    ../segmentation/im_processing.cpp \
    ../segmentation/scan.cpp \
    ../segmentation/marchingcubes.cpp \
    ../segmentation/vertex.cpp

HEADERS  += mainwindow.h \
    mainview.h \
    mesh.h \
    face.h \
    ../segmentation/coords.h \
    ../segmentation/im_processing.h \
    ../segmentation/scan.h \
    ../segmentation/marchingcubes.h \
    vertexdisp.h \
    ../segmentation/vertex.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc

DISTFILES += \
    shaders/fragshader_deltanormmap.glsl \
    shaders/vertshader_deltanormmap.glsl
