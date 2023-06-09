#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QVBoxLayout>

#include "window.h"

extern "C"
{
#include "interpolation.h"
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QMainWindow *window = new QMainWindow;
    QMenuBar *tool_bar = new QMenuBar(window);
    Window *graph_area = new Window(window);
    QAction *action;

    if (graph_area->parse_command_line(argc, argv)) {
        qWarning("Wrong input arguments!");
        //       QMessageBox::warning (0, "Wrong input arguments!",
        //                             "Wrong input arguments!");
        return -1;
    }

    action = tool_bar->addAction("Change function", graph_area,
                                 SLOT(change_func()));
    action->setShortcut(QString("0"));

    action = tool_bar->addAction("Change method", graph_area,
                                 SLOT(change_method()));
    action->setShortcut(QString("1"));

    action = tool_bar->addAction("Zoom in", graph_area,
                                 SLOT(zoom_in()));
    action->setShortcut(QString("2"));

    action = tool_bar->addAction("Zoom out", graph_area,
                                 SLOT(zoom_out()));
    action->setShortcut(QString("3"));

    action = tool_bar->addAction("Improve accuracy", graph_area,
                                 SLOT(impr_acc()));
    action->setShortcut(QString("4"));

    action = tool_bar->addAction("Degrade accuracy", graph_area,
                                 SLOT(degr_acc()));
    action->setShortcut(QString("5"));


    action = tool_bar->addAction("E&xit", window, SLOT(close()));
    action->setShortcut(QString("Ctrl+X"));

    tool_bar->setMaximumHeight(30);

    window->setMenuBar(tool_bar);
    window->setCentralWidget(graph_area);
    window->setWindowTitle("Graph");

    window->show();

    return app.exec();
}
