/*  -_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

 > Window Class for the Cube Simulation.
 > Copyright (C) 2014 by Daniel Intskirveli, Gurpreet Singh, Christopher Zhang.

 > window.h - Displays Widgets that will manipulate Cubes using QT.
 
 > Written by: Daniel Intskirveli, Gurpreet Singh, Christopher Zhang, 2014.

  -_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_ */

#ifndef WINDOW_H
#define WINDOW_H

#include <QtGui>
#include <QMainWindow>
#include "matrixwidget.h"

QT_BEGIN_NAMESPACE
class QSlider;
class QSpinBox;
class QCheckBox;
class QLabel;
class QComboBox;
QT_END_NAMESPACE

class MatrixWidget;

//! The main window of the application
/*!
    Is the container for the LEDMatrix widget and the various sliders, spinboxes, etc...
    
*/
class Window : public QMainWindow
{
    Q_OBJECT

public:
    Window();

public slots:
	void setSpacingSliderEnabled(bool enabled);
	void setCubicDimensions(bool cubic);
	void maybeSetAllDimensions(int value);

protected:
    void keyPressEvent(QKeyEvent *event);
    void makeConnections();

private:
    QSlider *createSlider(int min = 0, int max = 360, int singleStep = 1, int pageStep = 20, int tickInterval = 20);
    QSpinBox* createSpinBox();
    MatrixWidget *matrixWidget;

    QSlider* zoomSlider;
    QSlider* spacingSlider;
    QSlider* transparencySlider;

    QCheckBox* drawOff;
    QCheckBox* isCube;
    QComboBox *comboBox;
    int drawMode;
    
    QLabel *xSliderLabel;
    QLabel *ySliderLabel;
    QLabel *zSliderLabel;
    QLabel *zoomLabel;
    QLabel *xSizeLabel;
    QLabel *ySizeLabel;
    QLabel *zSizeLabel;
    QLabel *spaceLabel;

    QGroupBox* Transforms;
    QGroupBox* Resolution;

    QSpinBox*   xSpinbox;
    QSpinBox*   ySpinbox;
    QSpinBox*   zSpinbox;

    QLabel* xLabel;
    QLabel* yLabel;
    QLabel* zLabel;
    QLabel* Status;
    QCheckBox* onStautus;
    QSpacerItem *Spacer;

    QVBoxLayout* resolutionLayout;
    QVBoxLayout*  LEDStatus;
    QHBoxLayout* sizeLayout;
};

#endif