/*  -_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

 > MatrixWidget class definition for the drawing a 3d cube QGLWidget object to diplay in the
 > main window with settings. The cube object can be manupilated using
 > the settings using the connects function from the QObject class. 

 > Copyright (C) 2014 by Daniel Intskirveli, Gurpreet Singh, Christopher Zhang.

 > matrixwidget.cpp - draws a 3d cube in the left of the settings.
 
 > Written by: Daniel Intskirveli, Gurpreet Singh, Christopher Zhang, 2014.

  -_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_ */

#include "matrixwidget.h"
#include <QtOpenGL>
#include <cmath>
#include <QTimer>
#include <iostream>
#include <sys/timeb.h>

// constructor for the widget
MatrixWidget::MatrixWidget(QWidget *parent) : QGLWidget(parent) {
    settings = new QSettings("groupname", "LEDcube");
    mode = settings->value("drawMode", MODE_POINTS).toInt();
    ledSize =1;
    spacing = settings->value("spacing", 0.5f).toFloat();
    transparency = 0.05f;
    rawZoom = 0;
    setZoom(rawZoom);
    DRAW_OFF_LEDS_AS_TRANSLUSCENT = false;
    
    xCubes = settings->value("xSize", 20).toInt();
    yCubes = settings->value("ySize", 20).toInt();
    zCubes = settings->value("zSize", 20).toInt();
    
    calcCubeSize();
    
    setXRotation(45);
    setYRotation(45);
    setZRotation(0);

    faceAnimation = false;
    waveAnimation = false;
    noAnimation = true;

    Vertices = new std::vector<Vector3>;

    // set up timer to call updateGL() fps times a second
    // if I recall correctly updateGL() should be a no-op
    // if the current call to paintGL is still running, so 
    // in theory this should be okay even for systems that 
    // cant handle the fps. So, fps is an upper bound
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    double fps = 30.0; //aim for 30 frames per second
    timer->start(1000/fps);
}

//os-portable way to get time with millisecond precision
//taken from http://www.cplusplus.com/forum/general/43203/
//Note: time.h's clock() function isn't good for animation
//because it is based on CPU ticks, the speed of which varies
static int getMilliCount() {
    timeb tb;
    ftime(&tb);
    int nCount = tb.millitm + (tb.time & 0xfffff) * 1000;
    return nCount;
}

QSize MatrixWidget::sizeHint() const {
    return QSize(400, 400);
}
 
void MatrixWidget::initializeGL() {
    glClearColor(0,0,0,0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    //anti-aliasing
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);

    glMatrixMode(GL_MODELVIEW);
}

// untility function to find the maximum of three numbers
static float maximum(float x, float y, float z) {
    int max = x; 
    if (y > max) {
        max = y;
    } else if (z > max) {
        max = z;
    }
    return max;
}

float MatrixWidget::delta() {
    // if cubes are being drawn then delta is
    // spacing + cube size else, if points then 
    // delta is spacing because points don't have area
    return spacing + (mode == MODE_POINTS ? 0 : ledSize);
}

void MatrixWidget::calcCubeSize() {
    // cube size in each direction is calculated using the delta()
    // function. the number of cubes in each direction is multiplied by
    // delta and subtracted by spacing because the last cube doesn't
    // need to include spacing in each direction.
    // take the maximum of the cubes in each direction
    // because we don't want the widget to be clipping in the viewport.
    xCubeSize = xCubes*delta() - spacing;
    yCubeSize = yCubes*delta() - spacing;
    zCubeSize = zCubes*delta() - spacing;
    maxCube = maximum(xCubeSize, yCubeSize, zCubeSize);
}

bool MatrixWidget::isOn(int x, int y, int z, int t) {
    if (false) return true;

    if (waveAnimation) {
        if (xCubes == 1 && yCubes == 1 && zCubes == 1) {
            return true;
        }
        if(y == round(sin(z/2 + t/100)*2)+ round(sin(x/2)*2) + yCubes/2) {
            return true;
        }
    } else if (faceAnimation) {
        // iterate the vector
        for (std::vector<Vector3>::iterator it = Vertices->begin() ; it != Vertices->end(); ++it) {
            Vector3 a = *it;
            if (a.x == x && a.y == y && a.z == z)   // verify if the coordinate is in the vector
                return true;
        }
    }

    return false;
}

void MatrixWidget::paintGL() {
    // Clear the buffer, clear the matrix 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // 'a' is the diagonal of the maximum cube. 
    // this is a good value to base frustum calculations on
    // ...because the cube will always fit,
    // and clipping will therefore not occur.
    // also calculating a similar value in resizeGL()
    float a = (((float) maxCube) * sqrt((float) 3)); 

    // multiplies the current matrix by a translation matrix.
    glTranslatef(0, 0, -4*a);

    // allow rotation on the X, Y and Z axis
    glRotatef(xRot,1.0f,0.0f,0.0f);
    glRotatef(yRot,0.0f,1.0f,0.0f);
    glRotatef(zRot,0.0f,0.0f,1.0f);

    bool on;
    int t = getMilliCount();

    // check if points are selected
    if (mode == MODE_POINTS) {
        glPointSize(spacing*10);
    }

    /* The cubes are drawn using the loops below. The loops run until all 
    the cubes specified by the user, xCubes, yCubes, and zCubes are not 
    drawn. Inside the third loop, the glPushMatrix() set where to start 
    the current object transformations. Then glTranslatef multiplies 
    the currect matrix by the translation matrix, basically where we want 
    the next cube to be drawn. Then we check what type of drawing the 
    user selected, then we check is the cube is to be drawn. If yes, 
    then draw a cube or a point and then glPopMatrix() end the current 
    object transformation.
    */

    for (float i = 0; i < xCubes; i++) {
        for (float j = 0; j < yCubes; j++) {
            for (float k = 0; k < zCubes; k++) {
                glPushMatrix();
                glTranslatef(
                    i*delta() - xCubeSize/2,
                    j*delta() - yCubeSize/2,
                    k*delta() - zCubeSize/2);
                if (noAnimation) {
                    on = true;
                } else if (waveAnimation || faceAnimation) { 
                    on = isOn(i,j,k,t);
                }

                if (on || DRAW_OFF_LEDS_AS_TRANSLUSCENT) {
                    glColor4f(1.0f, 1.0f, 1.0f, on ? 1.0 : transparency);
                    if (mode == MODE_POINTS && (on || transparency)) {
                        drawPoint();  
                    } else if (mode == MODE_CUBES && (on || transparency)) {
                        drawCube();  
                    }
                }
                glPopMatrix();
                
            }
        }
    }
}

void MatrixWidget::resizeGL(int w, int h) {
    // calculate the aspect ratio for frustum, then set the
    // matrix mode to GL_PROJECTION, and then calculate the 
    // 'a' is the diagonal of the maximum cube. 
    // this is a good value to base frustum calculations on
    // ...because the cube will always fit,
    // and clipping will therefore not occur.
    // the viewport width and height is specified
    
    float aspect =(float)w/ ( h ? h : 1 );
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float a = ((float) maxCube) * sqrt((float)3); 
    glViewport(0, 0, w, h);
    float z = zoom;

    glFrustum(-(a/2)*aspect*z,(a/2)*aspect*z,-(a/2)*z,(a/2)*z,3*a,6*a);
    
    glMatrixMode(GL_MODELVIEW);
}

// draw cube
void MatrixWidget::drawCube() {
    // draw The Cube Using quads
    glBegin(GL_QUADS);
    
    glVertex3f(ledSize, ledSize, 0);
    glVertex3f(0, ledSize, 0);
    glVertex3f(0, ledSize, ledSize);
    glVertex3f(ledSize, ledSize, ledSize);
    
    glVertex3f(ledSize, 0, ledSize);
    glVertex3f(0, 0, ledSize);
    glVertex3f(0, 0, 0);
    glVertex3f(ledSize, 0, 0);
    
    glVertex3f(ledSize, ledSize, ledSize);
    glVertex3f(0, ledSize, ledSize);
    glVertex3f(0, 0, ledSize);
    glVertex3f(ledSize, 0, ledSize);
    
    glVertex3f(ledSize, 0, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, ledSize, 0);
    glVertex3f(ledSize, ledSize, 0);
    
    glVertex3f(0, ledSize, ledSize);
    glVertex3f(0, ledSize, 0);
    glVertex3f(0, 0, 0);
    glVertex3f(0, 0, ledSize);
    
    glVertex3f(ledSize, ledSize, 0);
    glVertex3f(ledSize, ledSize, ledSize);
    glVertex3f(ledSize, 0, ledSize);
    glVertex3f(ledSize, 0, 0);

    glEnd();   
}

// draw point
void MatrixWidget::drawPoint() {
    glBegin(GL_POINTS);
    glVertex3f(0, 0, 0);   
    glEnd();
}

static void qNormalizeAngle(int &angle) {
    while (angle < 0) angle += 360;
    while (angle > 360) angle -= 360;
}

void MatrixWidget::setXRotation(int angle) {
    qNormalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;
        emit xRotationChanged(angle);
    }
}

void MatrixWidget::setYRotation(int angle) {
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
    }
}

void MatrixWidget::setZRotation(int angle) {
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
    }
}

void MatrixWidget::setZoom(int newZoom) {
    rawZoom = newZoom;
    // map zoom to -100 - 100 to 0-2
    zoom = ((float)newZoom/-100)+1;
    resizeGL(width(), height());
}

void MatrixWidget::setTransparency(int percent) {
    transparency = (float) percent / 100;
    resizeGL(width(), height());
}

void MatrixWidget::setSpacing(int intspaceing) {
    spacing = (float)intspaceing / (float)10;
    settings->setValue("spacing", spacing);
    calcCubeSize();
    resizeGL(width(), height());
}

void MatrixWidget::setMode(int cur) {
    if(cur == 0) {  
        mode = MODE_CUBES;
    } else if (cur == 1){
        mode = MODE_POINTS;
    }
    settings->setValue("drawMode", mode);
    calcCubeSize();
    resizeGL(width(), height());
}

void MatrixWidget::setXSize(int size) {
    xCubes = size;
    settings->setValue("xSize", xCubes);
    calcCubeSize();
    resizeGL(width(), height());
}

void MatrixWidget::setYSize(int size) {
    yCubes = size;
    settings->setValue("ySize", yCubes);
    calcCubeSize();
    resizeGL(width(), height());
}

void MatrixWidget::setZSize(int size) {
    zCubes = size;
    settings->setValue("zSize", zCubes);
    calcCubeSize();
    resizeGL(width(), height());
}

void MatrixWidget::toggleDrawOff(bool draw) {
    DRAW_OFF_LEDS_AS_TRANSLUSCENT = draw;
    //updateGL();
}

void MatrixWidget::mousePressEvent(QMouseEvent *event) {
    lastPos = event->pos();
}

void MatrixWidget::wheelEvent(QWheelEvent* event) {
    //divide by two to decrease zoom speed
    rawZoom+=event->delta()/2;

    if (rawZoom > 100) rawZoom = 100;
    if (rawZoom < -100) rawZoom = -100;

    setZoom(rawZoom);
    emit zoomChanged(rawZoom);
}

void MatrixWidget::mouseMoveEvent(QMouseEvent *event) {
    // subtracts the current event from when the mouse
    // was clicked.
    int dx = event->x() - lastPos.x();          
    int dy = event->y() - lastPos.y();
    
    // if the left button is pressed then rotate x direction
    // and y direction by delta amount
    if (event->buttons() & Qt::LeftButton) {
        setXRotation(xRot + dy);
        setYRotation(yRot + dx);
    }
    // save the current event in the lastPos
    lastPos = event->pos();
}

void MatrixWidget::setNoAnimation (bool set) {
    // noAnimation on the LED cube widget
    noAnimation = true;
    waveAnimation = false;
    faceAnimation = false;
}

void MatrixWidget::setWaveAnimation (bool set) {
    // WaveAnimation on the LED cube widget
    noAnimation = false;
    waveAnimation = true;
    faceAnimation = false;
}

void MatrixWidget::setFaceAnimation (bool set) {
    /* these boolean variables are flags for 
       drawing animations in the widget. */
    noAnimation = false;                                
    waveAnimation = false;                              
    faceAnimation = true;

    // create a vector of vertices
    // open the file window to input the file to QString
    Vertices = new std::vector<Vector3>;          
    QString file = QFileDialog::getOpenFileName(
        this,
        tr("Open XYZ File"),
        tr("XYZ file (.xyz)")
        );

    // if the file is not empty then read from the file
    // create an object of Qfile and then open the QFile
    // converts file to textstream, read until end of file,
    if(!file.isEmpty()) {
        QFile sfile(file);
        if(sfile.open(QFile::ReadOnly)) {
            QTextStream in (&sfile);
            while (!in.atEnd()) {
                // read the row and places the row in QString line
                // QByteArray provides an array of bytes
                // set str char to the data stored in the byte array
                QString line = in.readLine();
                QByteArray ba = line.toLocal8Bit();
                const char* str = ba.data();

                // create a object of type Vector3
                Vector3 v;                       

                // reads the first char in the line as x coordinate
                // reads the second char in the line as y coordinate
                // reads the third char in the line as z coordinate
                v.x = strtof(str, (char**)&str);
                v.y = strtof(str, (char**)&str);
                v.z = strtof(str, (char**)&str);    

                // adds the object to the vector of vertices
                Vertices->push_back(v);            
            }
            // close the file
            sfile.close();
        }
    }

    // maximum and minimum from the data set
    float xMax = 0.0;
    float yMax = 0.0;
    float zMax = 0.0;
    float xMin = 0.0;
    float yMin = 0.0;
    float zMin = 0.0;

    // iterate the vertices vector and find the max and min the data
    for (std::vector<Vector3>::iterator it = Vertices->begin(); 
                            it != Vertices->end(); ++it){

        xMax = std::max(xMax, it->x);     
        yMax = std::max(yMax, it->y);     
        zMax = std::max(zMax, it->z);
        xMin = std::min(xMin, it->x);     
        yMin = std::min(yMin, it->y);     
        zMin = std::min(zMin, it->z); 
    }

    // maximum and minimum from the normalized set
    float xnormmax = xCubes;
    float xnormmin = 1;
    float ynormmax = yCubes;
    float ynormmin = 1;
    float znormmax = zCubes;
    float znormmin = 1;

    // iterate the vertices vector and normalize the data
    for (std::vector<Vector3>::iterator it = Vertices->begin();
                            it != Vertices->end(); ++it) {
        it->x =  floor( xnormmin + ((it->x - xMin) 
                * (xnormmax - xnormmin))/(xMax - xMin) );
        it->y =  floor( ynormmin + ((it->y - yMin) 
                * (ynormmax - ynormmin))/(yMax - yMin) );
        it->z =  floor( znormmin + ((it->z - zMin) 
                * (znormmax - znormmin))/(zMax - zMin) );
    }
}