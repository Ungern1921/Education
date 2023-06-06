#ifndef SCENE3D_H
#define SCENE3D_H

#include <QGLWidget> // подключаем класс QGLWidget
#include <QMouseEvent>
#include <QAction>
#include <QString>
#include <QWidget>
#include <QGLWidget>

extern "C"
{
    #include "interpolation2.h"
}

class Scene3D
    : public QGLWidget // класс Scene3D наследует встроенный класс QGLWidget
{
  private:
    GLfloat xRot; // переменная хранит угол поворота вокруг оси X
    GLfloat yRot; // переменная хранит угол поворота вокруг оси Y
    GLfloat zRot; // переменная хранит угол поворота вокруг оси Z
    GLfloat zTra; // переменная хранит величину трансляции оси Z
    GLfloat nSca; // переменная отвечает за масштабирование обьекта

    QPoint ptrMousePosition; // переменная хранит координату указателя мыши в
                             // момент нажатия
    int k = 0, func_id = 0;
    int scale = 0;
    int  nx, ny, nx0, ny0;
    double ax, bx, ay, by;
    double ax0, bx0, ay0, by0;
    interpolation2_ctx ctx;
    double abs_max;
    double abs_max_xy;

    int method;
    QString f_name;
    double (*f)(double, double);

    void scale_plus(); // приблизить сцену
    void scale_minus(); // удалиться от сцены
    void rotate_up(); // повернуть сцену вверх
    void rotate_down(); // повернуть сцену вниз
    void rotate_left(); // повернуть сцену влево
    void rotate_right(); // повернуть сцену вправо
    void translate_down(); // транслировать сцену вниз
    void translate_up(); // транслировать сцену вверх
    void defaultScene(); // наблюдение сцены по умолчанию

    void drawAxis(); // построить оси координат

    void drawFigure(); // построить фигуру

    void change_method();
    void change_func();
    void zoom_in();
    void zoom_out();
    void increase_n();
    void decrease_n();
    void rotate_z(int factor);
    void new_scale();


  protected:
    /*virtual*/ void
    initializeGL(); // метод для проведения инициализаций, связанных с OpenGL
    /*virtual*/ void resizeGL(
        int nWidth,
        int nHeight); // метод вызывается при изменении размеров окна виджета
    /*virtual*/ void
    paintGL(); // метод, чтобы заново перерисовать содержимое виджета

    /*virtual*/ void mousePressEvent(
        QMouseEvent
            *pe); // методы обработки события мыши при нажатии клавиши мыши
    /*virtual*/ void mouseMoveEvent(
        QMouseEvent *pe); // методы обработки события мыши при перемещении мыши
    /*virtual*/ void mouseReleaseEvent(
        QMouseEvent
            *pe); // методы обработки событий мыши при отжатии клавиши мыши
    /*virtual*/ void
    wheelEvent(QWheelEvent *pe); // метод обработки событий колесика мыши
    /*virtual*/ void keyPressEvent(
        QKeyEvent
            *pe); // методы обработки события при нажатии определенной клавиши

  public:
    int parse_command_line(int argc, char *argv[]);
    Scene3D(QWidget *parent = 0); // конструктор класса
    ~Scene3D() {
        interpolation2_destroy(ctx);
    } // конструктор класса
};
#endif
