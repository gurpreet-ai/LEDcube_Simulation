 /*  -_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

 > MatrixWidget class header for the drawing a 3d cube QGLWidget object to diplay in the
 > main window with settings. The cube object can be manupilated using
 > the settings using the connects function from the QObject class. 

 > Copyright (C) 2014 by Daniel Intskirveli, Gurpreet Singh, Christopher Zhang.

 > matrixwidget.h - draws a 3d cube in the left of the settings.
 
 > Written by: Daniel Intskirveli, Gurpreet Singh, Christopher Zhang, 2014.

  -_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_ */

#ifndef MATRIXWIDGET_H
#define MATRIXWIDGET_H

#include <QGLWidget>
#include <QSettings>
#include <ctime>
#include <QWheelEvent>

//! LEDMatrix Widget
/*!
    Extension of QGLWidget on which the actual led matrix is displayed.
    
*/

struct Vector3 {
    float x, y, z;
};

class MatrixWidget : public QGLWidget
{
    Q_OBJECT

public:
    MatrixWidget(QWidget *parent = 0);
    enum { MODE_CUBES, MODE_POINTS };                       // able to change the mode from cubes to points or vice versa
    bool DRAW_OFF_LEDS_AS_TRANSLUSCENT;                     // decides whether or not to draw the leds that are off

public slots:
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);
    void setZoom(int zoom);
    void setMode(int cur);
    void setSpacing(int spacing);
    void setTransparency(int percent);
    void setXSize(int size);
    void setYSize(int size);
    void setZSize(int size);
    void toggleDrawOff(bool draw);

    void setNoAnimation     (bool);
    void setWaveAnimation   (bool);
    void setFaceAnimation   (bool);
    
signals:
    void xRotationChanged(int angle);
    void yRotationChanged(int angle);
    void zRotationChanged(int angle);
    void setSpacingSliderEnabled(bool enabled);
    void zoomChanged(int rawZoom);

protected:
    void drawCube();
    void drawPoint(); 
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent* event);
    QSize sizeHint() const;
    void calcCubeSize();
    float delta();
    bool isOn(int x, int y, int z, int t);
    void perspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);

private:
    int rawZoom;
    int mode;
    int xRot;
    int yRot;
    int zRot;
    QPoint lastPos;
    float spacing;
    float transparency;
    int xCubes;
    int yCubes;
    int zCubes;
    float ledSize;
    float xCubeSize;
    float yCubeSize;
    float zCubeSize;
    float maxCube;
    float zoom;
    QSettings * settings;
    std::vector<Vector3>* Vertices;
    bool faceAnimation;
    bool waveAnimation;
    bool noAnimation;
};

#endif
