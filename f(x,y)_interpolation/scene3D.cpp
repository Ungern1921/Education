#include <QtGui> // подключаем модуль QtGui
#include "scene3D.h" // подключаем заголовочный файл scene3D.h
#include <math.h> // подключаем математическую библиотеку

#define DEFAULT_A -10
#define DEFAULT_B 10
#define DEFAULT_N 10
#define N 2000
#define MAX_N 500

static double f0(double x, double y) {
    (void)x;
    (void)y;
    return 1;
}

static double f1(double x, double y) {
    (void)y;
    return x;
}

static double f2(double x, double y) {
    (void)x;
    return y;
}

static double f3(double x, double y) {
    return x + y;
}

static double f4(double x, double y) {
    return sqrt(x * x + y * y);
}

static double f5(double x, double y) {
    return x * x + y * y;
}

static double f6(double x, double y) {
    return exp(x * x - y * y);
}

static double f7(double x, double y) {
    return 1 / (25 * (x * x + y * y) + 1);
}

const static float pi = 3.141593; // глобальная переменная
Scene3D::Scene3D(QWidget *parent /*= 0*/)
    : QGLWidget(parent), xRot(-90), yRot(0), zRot(0), zTra(0),
      nSca(0.8) // конструктор класса Scene3D
{
    ax = ay = DEFAULT_A;
    bx = by = DEFAULT_B;
    nx = ny = DEFAULT_N;

    ax0 = ax;
    bx0 = bx;
    nx0 = nx;

    ay0 = ay;
    by0 = by;
    ny0 = ny;

    func_id = 7;
    scale = 0;
    method = 0;
    ctx = NULL;

    change_func();
}

int Scene3D::parse_command_line(int argc, char *argv[])
{
    if (argc == 1)
        return 0;

    if (argc != 8)
        return -1;

    if (sscanf(argv[1], "%lf", &ax) != 1 || sscanf(argv[2], "%lf", &bx) != 1 ||
        sscanf(argv[3], "%lf", &ay) != 1 || sscanf(argv[4], "%lf", &by) != 1 ||
        sscanf(argv[5], "%d", &nx) != 1 || sscanf(argv[6], "%d", &ny) != 1 ||
        sscanf(argv[7], "%d", &func_id) != 1 || bx - ax < 1.e-6 || by - ay < 1.e-6||
        nx <= 3|| ny <= 3)
        return -2;

    nx = std::min(nx, MAX_N);
    ny = std::min(ny, MAX_N);

    ax0 = ax;
    bx0 = bx;
    nx0 = nx;

    ay0 = ay;
    by0 = by;
    ny0 = ny;

    func_id--;
    change_func();

    return 0;
}

void Scene3D::change_func()
{
    func_id = (func_id + 1) % 8;

    switch (func_id) {
    case 0:
        f_name = "f (x, y) = 1";
        f = f0;
        break;
    case 1:
        f_name = "f (x, y) = x";
        f = f1;
        break;
    case 2:
        f_name = "f (x, y) = y";
        f = f2;
        break;
    case 3:
        f_name = "f (x, y) = x + y";
        f = f3;
        break;
    case 4:
        f_name = "f (x, y) = sqrt(x * x + y * y)";
        f = f4;
        break;
    case 5:
        f_name = "f (x, y) = x * x + y * y";
        f = f5;
        break;
    case 6:
        f_name = "f (x, y) = exp(x * x - y * y)";
        f = f6;
        break;
    case 7:
        f_name = "f (x, y) = 1 / (25 * (x * x + y * y) + 1)";
        f = f7;
        break;
    }

    if(ctx)
        interpolation2_destroy(ctx);

    if(method > 0)
        ctx = interpolation2_create((method + 1) % 2 + 1, nx, ny, func_id,
                                                    ax, bx, ay, by);
    new_scale();
    update();
}

/*virtual*/ void Scene3D::initializeGL() // инициализация
{
    qglClearColor(Qt::white); // цвет для очистки буфера изображения - здесь
                              // просто фон окна
    glEnable(GL_DEPTH_TEST); // устанавливает режим проверки глубины пикселей
    glShadeModel(GL_FLAT); // отключает режим сглаживания цветов
    glEnable(GL_CULL_FACE); // устанавливается режим, когда строятся только
                            // внешние поверхности

}

/*virtual*/ void Scene3D::resizeGL(int nWidth, int nHeight) // окно виджета
{
    glMatrixMode(GL_PROJECTION); // устанавливает текущей проекционную матрицу
    glLoadIdentity(); // присваивает проекционной матрице единичную матрицу
    GLfloat ratio =
        (GLfloat)nHeight /
        (GLfloat)nWidth; // отношение высоты окна виджета к его ширине

    // мировое окно
    if (nWidth >= nHeight)
        glOrtho(-1.0 / ratio, 1.0 / ratio, -1.0, 1.0, -10.0,
                1.0); // параметры видимости ортогональной проекции
    else
        glOrtho(-1.0, 1.0, -1.0 * ratio, 1.0 * ratio, -10.0,
                1.0); // параметры видимости ортогональной проекции
    // плоскости отсечения (левая, правая, верхняя, нижняя, передняя, задняя)

    // поле просмотра
    glViewport(0, 0, (GLint)nWidth, (GLint)nHeight);
}

/*virtual*/ void Scene3D::paintGL() // рисование
{
    // glClear(GL_COLOR_BUFFER_BIT); // окно виджета очищается текущим цветом
    // очистки
    glClear(GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT); // очистка буфера изображения и глубины

    glMatrixMode(GL_MODELVIEW); // устанавливает положение и ориентацию матрице
                                // моделирования
    glLoadIdentity(); // загружает единичную матрицу моделирования

    // последовательные преобразования
    glScalef(nSca, nSca, nSca); // масштабирование
    glTranslatef(0.0f, zTra, 0.0f); // трансляция
    glRotatef(xRot, 1.0f, 0.0f, 0.0f); // поворот вокруг оси X
    glRotatef(yRot, 0.0f, 1.0f, 0.0f); // поворот вокруг оси Y
    glRotatef(zRot, 0.0f, 0.0f, 1.0f); // поворот вокруг оси Z

    drawAxis(); // рисование осей координат
    drawFigure(); // нарисовать фигуру
}

/*virtual*/ void
Scene3D::mousePressEvent(QMouseEvent *pe) // нажатие клавиши мыши
{
    // при нажатии пользователем кнопки мыши переменной ptrMousePosition будет
    // присвоена координата указателя мыши
    ptrMousePosition = pe->pos();

    // ptrMousePosition = (*pe).pos(); // можно и так написать
}

/*virtual*/ void
Scene3D::mouseReleaseEvent(QMouseEvent *pe) // отжатие клавиши мыши
{
    (void) pe;
    // некоторые функции, которые должны выполняться при отжатии клавиши мыши
}

/*virtual*/ void
Scene3D::mouseMoveEvent(QMouseEvent *pe) // изменение положения стрелки мыши
{
    xRot += 180 / nSca * (GLfloat)(pe->y() - ptrMousePosition.y()) /
            height(); // вычисление углов поворота
    zRot += 180 / nSca * (GLfloat)(pe->x() - ptrMousePosition.x()) / width();

    ptrMousePosition = pe->pos();

    updateGL(); // обновление изображения
}

/*virtual*/ void Scene3D::wheelEvent(QWheelEvent *pe) // вращение колёсика мыши
{
    if ((pe->angleDelta().y()) > 0)
        scale_plus();
    else if ((pe->angleDelta().y()) < 0)
        scale_minus();

    updateGL(); // обновление изображения
}


/*virtual*/ void
Scene3D::keyPressEvent(QKeyEvent *pe) // нажатие определенной клавиши
{
    switch (pe->key()) {
    case Qt::Key_Plus:
        scale_plus(); // приблизить сцену
        break;

    case Qt::Key_Equal:
        scale_plus(); // приблизить сцену
        break;

    case Qt::Key_Minus:
        scale_minus(); // удалиться от сцены
        break;

    case Qt::Key_Up:
        rotate_up(); // повернуть сцену вверх
        break;

    case Qt::Key_Down:
        rotate_down(); // повернуть сцену вниз
        break;

    case Qt::Key_Left:
        rotate_left(); // повернуть сцену влево
        break;

    case Qt::Key_Right:
        rotate_right(); // повернуть сцену вправо
        break;

    case Qt::Key_Z:
        translate_down(); // транслировать сцену вниз
        break;

    case Qt::Key_X:
        translate_up(); // транслировать сцену вверх
        break;

    case Qt::Key_Space: // клавиша пробела
        defaultScene(); // возвращение значений по умолчанию
        break;

    case Qt::Key_Escape: // клавиша "эскейп"
        this->close(); // завершает приложение
        break;

    case Qt::Key_0:
        this->change_func();
        break;

    case Qt::Key_1:
        this->change_method();
        break;

    case Qt::Key_2:
        this->zoom_in();
        break;

    case Qt::Key_3:
        this->zoom_out();
        break;

    case Qt::Key_4:
        this->increase_n();
        break;

    case Qt::Key_5:
        this->decrease_n();
        break;

    case Qt::Key_8:
        this->rotate_z(1);
        break;

    case Qt::Key_9:
        this->rotate_z(-1);
        break;
    }

    updateGL(); // обновление изображения
}

void Scene3D::change_method(){
    method = (method + 1) % 5;

    if(ctx)
        interpolation2_destroy(ctx);

    if(method > 0)
        ctx = interpolation2_create((method + 1) % 2 + 1, nx, ny, func_id,
                                                    ax, bx, ay, by);

    new_scale();
    update();
}

void Scene3D::zoom_in()
{
    nSca *= 2;
    update();
}

void Scene3D::zoom_out()
{
    nSca /= 2;
    update();
}

void Scene3D::increase_n()
{
    nx *= 2;
    nx = std::min(nx, MAX_N);

    ny *= 2;
    ny = std::min(ny, MAX_N);

    if(ctx)
        interpolation2_destroy(ctx);

    if(method > 0)
        ctx = interpolation2_create((method + 1) % 2 + 1, nx, ny, func_id,
                                                    ax, bx, ay, by);

    new_scale();
    update();
}

void Scene3D::decrease_n()
{
    nx /= 2;
    nx = std::max(nx, 3);

    ny /= 2;
    ny = std::max(ny, 3);

    if(ctx)
       interpolation2_destroy(ctx);
    
    if(method > 0)
       ctx = interpolation2_create((method + 1) % 2 + 1, nx, ny, func_id,
                                                    ax, bx, ay, by);
    new_scale();
    
    update();
}

void Scene3D::rotate_z(int factor) {
    zRot += factor * 15;
}

void Scene3D::scale_plus() // приблизить сцену
{
    nSca = nSca * 1.1;
}

void Scene3D::scale_minus() // удалиться от сцены
{
    nSca = nSca / 1.1;
}

void Scene3D::rotate_up() // повернуть сцену вверх
{
    xRot += 1.0;
}

void Scene3D::rotate_down() // повернуть сцену вниз
{
    xRot -= 1.0;
}

void Scene3D::rotate_left() // повернуть сцену влево
{
    zRot += 1.0;
}

void Scene3D::rotate_right() // повернуть сцену вправо
{
    zRot -= 1.0;
}

void Scene3D::translate_down() // транслировать сцену вниз
{
    zTra -= 0.05;
}

void Scene3D::translate_up() // транслировать сцену вверх
{
    zTra += 0.05;
}

void Scene3D::defaultScene() // наблюдение сцены по умолчанию
{
    xRot = -90;
    yRot = 0;
    zRot = 0;
    zTra = 0;
    nSca = 0.8;
}

void Scene3D::drawAxis() // построить оси координат
{
    glLineWidth(2.0f); // устанавливаю ширину линии приближённо в пикселях
    // до вызова команды ширина равна 1 пикселю по умолчанию

    glColor4f(1.00f, 0.00f, 0.00f,
              1.0f); // устанавливается цвет последующих примитивов
    // ось x
    glBegin(GL_LINES); // построение линии
    glVertex3f(ax, 0.0f, 0.0f); // первая точка
    glVertex3f(bx, 0.0f, 0.0f); // вторая точка
    // ось y
    glVertex3f(0.0f, ay, 0.0f);
    glVertex3f(0.0f, by, 0.0f);
    // ось z
    glVertex3f(0.0f, 0.0f, 10.0f);
    glVertex3f(0.0f, 0.0f, -10.0f);
    glEnd();
}

void Scene3D::new_scale() {
    double x1, y1;
    double max_z, min_z;
    double z11;
    double step_x = (bx - ax) / N;
    double step_y = (by - ay) / N;
    
    double (*Pf)(interpolation2_ctx, double, double);
    Pf = interpolation2_calculate;

    double val_true = 0;
    max_z = min_z = 0;
    for (x1 = ax; x1 - bx < 1e-6; x1 += step_x) {
        for (y1 = ay; y1 - by < 1e-6; y1 += step_y) {
            val_true = f(x1, y1);

            if(method == 0)
                z11 = val_true;
            else
            {
                z11 = Pf(ctx, x1, y1);
                if(method == 3 || method == 4)
                    z11 = fabs(val_true - z11);
            }

            if (z11 < min_z)
                min_z = z11;
            if (z11 > max_z)
                max_z = z11;
        }
    }

    abs_max = std::max(std::max(fabs(max_z), fabs(min_z)), 1e-6);
    abs_max_xy = std::max(std::max(fabs(ax), fabs(ay)), std::max(fabs(bx), fabs(by)));

    if (func_id != 6) {
        glScalef(nSca / abs_max_xy, nSca / abs_max_xy, nSca / abs_max);
    }
    else {
        glScalef(nSca / abs_max_xy, nSca / abs_max_xy, nSca / (abs_max_xy*100));
    }
}

void Scene3D::drawFigure() // построить фигуру
{
    double x1, y1;
    double z1, z2, z3, z4;
    double step_x = (bx - ax) / N;
    double step_y = (by - ay) / N;
    double (*Pf)(interpolation2_ctx, double, double);
    Pf = interpolation2_calculate;

    if (func_id != 6) {
        glScalef(nSca / abs_max_xy, nSca / abs_max_xy, nSca / abs_max);
    }
    else {
        glScalef(nSca / abs_max_xy, nSca / abs_max_xy, nSca / (abs_max_xy*100));
    }
    
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.2f, 3.0f, 0.3f, 0.7);
    glBegin(GL_TRIANGLES);
    for(x1 = ax; x1 - bx < 1e-6; x1 += step_x) {
        for(y1 = ay; y1 - by < 1e-6; y1 += step_y) {
            glColor4f((ax + x1 )/ (bx - ax) / 2 + 0.6, (ay + y1) / (by - ay) / 2 + 0.8, 0.4f, 0.9);

            if(method == 1 || method == 2)
            {
                z1 = Pf(ctx, x1 - step_x, y1);
                z2 = Pf(ctx, x1, y1);
                z3 = Pf(ctx, x1 - step_x, y1 - step_y);
                z4 = Pf(ctx, x1, y1 - step_y);
            }
            else
            {
                z1 = f(x1 - step_x, y1);
                if(method == 3 || method == 4)
                    z1 = fabs(z1 - Pf(ctx, x1 - step_x, y1));
                z2 = f(x1, y1);
                if(method == 3 || method == 4)
                    z2 = fabs(z2 - Pf(ctx, x1, y1));
                z3 = f(x1 - step_x, y1 - step_y);
                if(method == 3 || method == 4)
                    z3 = fabs(z3 - Pf(ctx, x1 - step_x, y1 - step_y));
                z4 = f(x1, y1 - step_y);
                if(method == 3 || method == 4)
                    z4 = fabs(z4 - Pf(ctx, x1, y1 - step_y));
            }

            glVertex3f(x1 - step_x, y1, z1);
            glVertex3f(x1, y1, z2);
            glVertex3f(x1 - step_x, y1 - step_y, z3);

            glVertex3f(x1, y1 - step_y, z4);
            glVertex3f(x1, y1, z2);
            glVertex3f(x1 - step_x, y1 - step_y, z3);

        }
    }
    glEnd();

    qglColor("Black");
    renderText(0, 20, f_name + ", k = " + QString::number(func_id));
    renderText(0, 40, "method = " + QString::number(method));
    renderText(0, 60, "n_x = " + QString::number(nx));
    renderText(0, 80, "n_y = " + QString::number(ny));
    renderText(0, 100, "scale = " + QString::number(nSca/0.8));
    renderText(0, 120, "angle = " + QString::number(zRot - 360*floor(zRot/360)) + " degree");
    qglColor("Magenta");
    renderText(0, 140, "max = " + QString::number(abs_max));
    renderText(0, 160, "x: (" + QString::number(ax) + ", " + QString::number(bx) + ")");
    renderText(0, 180, "y: (" + QString::number(ay) + ", " + QString::number(by) + ")");

}
