#include <QApplication>
#include <QDesktopWidget>
#include "matrixwidget.h"

#include "window.h"

//! Entry point for the app
/*!
    Creates the window and shows it.
*/
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Window window;
    window.resize(window.sizeHint());
    window.show();
    return app.exec();
}
