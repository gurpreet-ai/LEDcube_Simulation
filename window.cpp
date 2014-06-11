/*  -_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

 > WINDOW Class for the Cube Simulation.
 > Copyright (C) 2014 by Daniel Intskirveli, Gurpreet Singh, Christopher Zhang.

 > window.cpp - Displays Widgets that will manipulate Cubes using QT.
 
 > Written by: Daniel Intskirveli, Gurpreet Singh, Christopher Zhang, 2014.

  -_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_*/

#include "window.h"

Window::Window()
{
    QVBoxLayout *settingsLayout = new QVBoxLayout;

    matrixWidget = new MatrixWidget;
    matrixWidget->setMinimumWidth(500); 
    matrixWidget->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));
    QSettings *settings = new QSettings("groupname", "LEDcube");

    drawMode = settings->value("drawMode", MatrixWidget::MODE_POINTS).toInt();

    xLabel = new QLabel(tr("X Size"));                              // label for X size
    yLabel = new QLabel(tr("Y Size"));                              // label for Y size
    zLabel = new QLabel(tr("Z Size"));                              // label for Z size

    xSpinbox = createSpinBox();                                     // spinbox for x size with range from 0 - 100
    ySpinbox = createSpinBox();                                     // spinbox for y size with range from 0 - 100
    zSpinbox = createSpinBox();                                     // spinbox for z size with range from 0 - 100
    
    xSpinbox->setValue(settings->value("xSize", 20).toInt());       // set initial value of the spin boxes to 20
    ySpinbox->setValue(settings->value("ySize", 20).toInt());
    zSpinbox->setValue(settings->value("zSize", 20).toInt());

    sizeLayout = new QHBoxLayout;
    
    sizeLayout->addWidget(xLabel, Qt::AlignLeft);                   // add the x Size label to the size layout and align to Left
    sizeLayout->addWidget(xSpinbox, Qt::AlignLeft);                 // add the x spinbox to the size layout and align to Left
    xLabel->setBuddy(xSpinbox);                                     // set focus on the spinbox when label is clicked or vise-versa

    sizeLayout->addWidget(yLabel, Qt::AlignLeft);                   // add the y Size label to the size Layout and Align to Left
    sizeLayout->addWidget(ySpinbox, Qt::AlignLeft);                 // add the y spinbox to the size layout and align to left
    yLabel->setBuddy(ySpinbox);                                     // set focus on the spinbox when label is clicked or vise-versa,

    sizeLayout->addWidget(zLabel, Qt::AlignLeft);                   // add the z Size label to the size Layout and Align to left
    zLabel->setBuddy(zSpinbox);                                     // Add the z spinbox to the size Layout and align to left
    sizeLayout->addWidget(zSpinbox, Qt::AlignLeft);                 // set focus on the spinbox when label is clicked or vise-versa

    resolutionLayout = new QVBoxLayout;                             // resolution VLayout   
    resolutionLayout->addLayout(sizeLayout);                        // add the sizelayout to the resolution layout

    comboBox = new QComboBox;                                       // initialize a combobox
    comboBox->addItem(tr("Cubes"));                                 // add cubes item to the combo box
    comboBox->addItem(tr("Points"));                                // add the points item to the combo box
    if (drawMode == MatrixWidget::MODE_CUBES) {                     // draw mode is set to cubes
        comboBox->setCurrentIndex (0);
    } else if (drawMode == MatrixWidget::MODE_POINTS) {             // else points
        comboBox->setCurrentIndex (1);
    }

    drawOff = new QCheckBox("draw \"Off\" LEDs?");                  // create a checkbox for draw off LED's
    isCube = new QCheckBox("Keep dimensions cubic");                // create a checkbox for keeping cubic dimensions

    // connect the checkboxs to slots of the widget
    connect(drawOff, SIGNAL(toggled(bool)), matrixWidget, SLOT(toggleDrawOff(bool)));
    connect(isCube, SIGNAL(toggled(bool)), this, SLOT(setCubicDimensions(bool)));        

    LEDStatus     = new QVBoxLayout;                                // vertical layout for the LED status
    Status        = new QLabel(tr("LED Status"));                   // lable for the led status
    onStautus     = new QCheckBox(tr("On"));                        // lable for on checkbox 

    onStautus->setChecked(false);                                   // the the checkbox status to off
    LEDStatus->addWidget(Status);                                   // add the widgets to the layout
    LEDStatus->addWidget(comboBox);
    LEDStatus->addWidget(drawOff);

    resolutionLayout->addLayout(LEDStatus);                         // add the LED status layout to the resolution layout

    QVBoxLayout* labelLayout = new QVBoxLayout;
    QLabel* transparencyLabel = new QLabel(tr("Transparency"));
    QLabel* cubesizeLabel = new QLabel(tr("Cube Space"));
    
    labelLayout->addWidget(transparencyLabel);
    labelLayout->addWidget(cubesizeLabel); 

    QVBoxLayout* sliderLayout = new QVBoxLayout;
    
    transparencySlider = new QSlider(Qt::Horizontal);
    transparencySlider->setRange(0, 100);
    transparencySlider->setSingleStep(1);
    transparencySlider->setPageStep(1); 
    transparencySlider->setTickInterval(1);
    
    spacingSlider = new QSlider(Qt::Horizontal);
    spacingSlider->setRange(1, 40);
    spacingSlider->setSingleStep(1);
    spacingSlider->setPageStep(1); 
    spacingSlider->setTickInterval(1);

    sliderLayout->addWidget(transparencySlider);
    sliderLayout->addWidget(spacingSlider); 

    QHBoxLayout* finalLayout = new QHBoxLayout;
    finalLayout->addLayout(labelLayout);
    finalLayout->addLayout(sliderLayout);

    resolutionLayout->addLayout(finalLayout);

    Resolution = new QGroupBox(tr("Resolution"));                   // resolution GroupBox
    Resolution->setLayout(resolutionLayout);                        // set the resolution Groupbox layout to the resolutionLayout
    
    settingsLayout->addWidget(Resolution);                          // add the groupbox to the rightLayout

    connect(xSpinbox, SIGNAL(valueChanged(int)), this, SLOT(maybeSetAllDimensions(int)));

    // matrixWidget is the LED cube Widget. xSpinbox, ySpinbox, and zSpinbox are
    // the source and matrixWidget is the destination. Whenever the value of any of
    // the spinboxes is changed, a signal is emitted and the function in the SLOT is called.
    connect(xSpinbox, SIGNAL(valueChanged(int)), matrixWidget, SLOT(setXSize(int)));
    connect(ySpinbox, SIGNAL(valueChanged(int)), matrixWidget, SLOT(setYSize(int)));
    connect(zSpinbox, SIGNAL(valueChanged(int)), matrixWidget, SLOT(setZSize(int)));

    QLabel* xrotateLabel = new QLabel(tr("X Rotation"));            // x rotation label
    QLabel* yrotateLabel = new QLabel(tr("Y Rotation"));            // y rotation label
    QLabel* zrotateLabel = new QLabel(tr("Z Rotation"));            // z rotation label
    QLabel* zoomLabel    = new QLabel(tr("Zoom"));                  // zoom label

    QSlider* xrotateSlider  = createSlider();                       // x rotation slider
    QSlider* yrotateSlider  = createSlider();                       // y rotation slider
    QSlider* zrotateSlider  = createSlider();                       // z rotation slider
    QSlider* zoomSlider     = createSlider(-100, 100);              // zoom slider
    zoomSlider->setValue(0);

    QVBoxLayout* transformsLabels   = new QVBoxLayout;              // vertical labels layout
    QVBoxLayout* transformsSliders  = new QVBoxLayout;              // vertical sliders layout
    QVBoxLayout* transformsSpinBox  = new QVBoxLayout;              // vertical spinboxes layout
        
    transformsLabels->addWidget(xrotateLabel);                      // x rotation label added to VLayout
    transformsLabels->addWidget(yrotateLabel);                      // y rotation label added to VLayout
    transformsLabels->addWidget(zrotateLabel);                      // z rotation label added to VLayout
    transformsLabels->addWidget(zoomLabel);                         // zoom label added to VLayout
    
    transformsSliders->addWidget(xrotateSlider);                    // x rotation slider added to VLayout
    transformsSliders->addWidget(yrotateSlider);                    // y rotation slider added to VLayout
    transformsSliders->addWidget(zrotateSlider);                    // z rotation slider added to VLayout 
    transformsSliders->addWidget(zoomSlider);                       // zoom slider added to VLayout

    QHBoxLayout* transformsLayout = new QHBoxLayout;                // HLayout for cube transformation
    transformsLayout->addLayout(transformsLabels);                  // labels added to transforms VLayout
    transformsLayout->addLayout(transformsSliders);                 // slider added to transforms VLayout

    connect(xrotateSlider, SIGNAL(valueChanged(int)), matrixWidget, SLOT(setXRotation(int)));
    connect(matrixWidget, SIGNAL(xRotationChanged(int)), xrotateSlider, SLOT(setValue(int)));
    connect(yrotateSlider, SIGNAL(valueChanged(int)), matrixWidget, SLOT(setYRotation(int)));
    connect(matrixWidget, SIGNAL(yRotationChanged(int)), yrotateSlider, SLOT(setValue(int)));
    connect(zrotateSlider, SIGNAL(valueChanged(int)), matrixWidget, SLOT(setZRotation(int)));
    connect(matrixWidget, SIGNAL(zRotationChanged(int)), zrotateSlider, SLOT(setValue(int)));
    connect(zoomSlider, SIGNAL(valueChanged(int)), matrixWidget, SLOT(setZoom(int)));
    connect(matrixWidget, SIGNAL(zoomChanged(int)), zoomSlider, SLOT(setValue(int)));

    Transforms = new QGroupBox(tr("Transformations"));              // transformations GroupBox
    Transforms->setLayout(transformsLayout);                        // transformations layout added to the Transforms GBox 
    settingsLayout->addWidget(Transforms);
    
    QVBoxLayout* modelLayout = new QVBoxLayout;           

    QRadioButton* noAnimation       = new QRadioButton(tr("No Animation")); 
    QRadioButton* waveAnimation     = new QRadioButton(tr("Wave Animation"));      
    QRadioButton* faceAnimation     = new QRadioButton(tr("Draw Face"));      

    modelLayout->addWidget(noAnimation);                              
    modelLayout->addWidget(waveAnimation);                              
    modelLayout->addWidget(faceAnimation);                            
    noAnimation->setChecked(true);

    connect(noAnimation,  SIGNAL(clicked(bool)), matrixWidget, SLOT(setNoAnimation(bool)));
    connect(waveAnimation,    SIGNAL(clicked(bool)), matrixWidget, SLOT(setWaveAnimation(bool)));
    connect(faceAnimation,   SIGNAL(clicked(bool)), matrixWidget, SLOT(setFaceAnimation(bool)));
    
    QGroupBox *Animations = new QGroupBox(tr("3D Animations"));         
    Animations->setLayout(modelLayout);                    
    settingsLayout->addWidget(Animations);
    settingsLayout->addStretch(15);

        
    QHBoxLayout *mainLayout = new QHBoxLayout;
    QWidget *settingsWidget = new QWidget();
    settingsWidget->setLayout(settingsLayout);
    settingsWidget->setMaximumWidth(370);

    mainLayout->addWidget(matrixWidget);
    mainLayout->addWidget(settingsWidget);

    QWidget *central = new QWidget();                               // create a central QWidget
    central->setLayout(mainLayout);                                 // add the main layout to the central widget
    setCentralWidget(central);                                      // set the central widget to central

    zoomSlider->setValue(0);
    spacingSlider->setValue(settings->value("spacing", .5f).toFloat()*10);
    transparencySlider->setValue(5);

    setWindowTitle(tr("LEDcube"));                                  // window title
    makeConnections();                                              // make some connections
}

// utitlity function to create a slider
QSlider *Window::createSlider(int min, int max, int singleStep, int pageStep, int tickInterval)
{
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(min, max);
    slider->setSingleStep(singleStep);
    slider->setPageStep(pageStep); 
    slider->setTickInterval(tickInterval);
    slider->setTickPosition(QSlider::TicksRight);
    return slider;
}

// create a spinbox
QSpinBox *Window::createSpinBox()
{
    QSpinBox *spin = new QSpinBox();
    spin->setRange(0, 100);
    return spin;
}

// set the spacing slider snabled
void Window::setSpacingSliderEnabled(bool enabled) {
    spacingSlider->setEnabled(enabled);
}

// set the cubic dimensions for the widget
void Window::setCubicDimensions(bool cubic) {
    // if cubic, disable the y and z spinboxes
    // and make the value of both y, z the same as x
    if (cubic) {
        int val = xSpinbox->value();
        ySpinbox->setEnabled(false);
        zSpinbox->setEnabled(false);
        ySpinbox->setValue(val);
        zSpinbox->setValue(val);
    } else {
        // else enable both y and z
        ySpinbox->setEnabled(true);
        zSpinbox->setEnabled(true);
    }
}

// close the application using the escape button
void Window::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape)
        close();
    else
        QWidget::keyPressEvent(e);
}

void Window::maybeSetAllDimensions(int val) {
    if (isCube->isChecked()) {
        ySpinbox->setValue(val);
        zSpinbox->setValue(val);
    }
}

void Window::makeConnections() {
    connect(spacingSlider, SIGNAL(valueChanged(int)), matrixWidget, SLOT(setSpacing(int)));
    connect(transparencySlider, SIGNAL(valueChanged(int)), matrixWidget, SLOT(setTransparency(int)));
    connect(comboBox, SIGNAL(activated(int)), matrixWidget, SLOT(setMode(int)));
}
