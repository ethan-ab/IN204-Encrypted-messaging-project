#include "mainwindow.h"

#include <iostream>
#include <boost/asio.hpp>
#include <msgpack.hpp>
#include <QApplication>
using namespace boost::asio;


#include <string>



int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_UseOpenGLES);
    QApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}





