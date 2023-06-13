#include "scene3D.h"
#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QPainter>
#include <QAction>
#include "scene3D.h"

int main(int argc, char **argv)
{
    QApplication app(argc,
                     argv); // создаём приложение, инициализация оконной системы
    //QMainWindow *window = new QMainWindow;
    //QMenuBar *tool_bar = new QMenuBar(window);
    //QWidget *widget = new QWidget(window);

    Scene3D scene1; // создаём виджет класса Scene3D
    if (scene1.parse_command_line(argc, argv)) {
        qWarning("Wrong input arguments!");
        return -1;
    }

    scene1.setWindowTitle("Graphics"); // название окна
    scene1.resize(800, 600); // размеры (nWidth, nHeight) окна
    scene1.show(); // изобразить виджет

    return app.exec();
}
