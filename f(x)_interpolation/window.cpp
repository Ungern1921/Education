#include <QPainter>
#include <QString>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "window.h"

extern "C"
{
#include "interpolation.h"
}

#define DEFAULT_A -10
#define DEFAULT_B 10
#define DEFAULT_N 10
#define N 200
#define MAX_N 10000000


static double f0(double x) {
    (void) x;
    return 1;
    }
static double f1(double x) { return x; }
static double f2(double x) { return x*x; }
static double f3(double x) { return x*x*x; }
static double f4(double x) { return x*x*x*x; }
static double f5(double x) { return exp(x); }
static double f6(double x) { return 1/(25*x*x+1); }

Window::Window(QWidget *parent) : QWidget(parent)
{
    a = DEFAULT_A;
    b = DEFAULT_B;
    n = DEFAULT_N;

    a0 = a;
    b0 = b;
    n0 = n;

    func_id = 0;
    s = 0;
    method = 1;

    change_func();
}

QSize Window::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize Window::sizeHint() const
{
    return QSize(1000, 1000);
}

int Window::parse_command_line(int argc, char *argv[])
{
    if (argc == 1)
        return 0;

    if (argc == 2 || argc > 4)
        return -1;

    if (sscanf(argv[1], "%lf", &a) != 1 || sscanf(argv[2], "%lf", &b) != 1 ||
        b - a < 1.e-6 || (argc > 3 && sscanf(argv[3], "%d", &n) != 1) || n <= 2)
        return -2;

    n = std::min(n, MAX_N);

    a0 = a;
    b0 = b;
    n0 = n;

    return 0;
}

/// change current function for drawing
void Window::change_func()
{
    func_id = (func_id + 1) % 7;

    switch (func_id) {
    case 0:
        f_name = "f (x) = 1";
        f = f0;
        break;
    case 1:
        f_name = "f (x) = x";
        f = f1;
        break;
    case 2:
        f_name = "f (x) = x * x";
        f = f2;
        break;
    case 3:
        f_name = "f (x) = x * x * x";
        f = f3;
        break;
    case 4:
        f_name = "f (x) = x * x * x * x";
        f = f4;
        break;
    case 5:
        f_name = "f (x) = exp(x)";
        f = f5;
        break;
    case 6:
        f_name = "f (x) = 1 / (25 * x * x + 1)";
        f = f6;
        break;
    }

    update();
}

void Window::change_method()
{
    method = (method + 1) % 4;

    update();
}

void Window::zoom_in()
{
    s++;
    double c = (a+b)/2 - (b-a)/4;
    double d = (a+b)/2 + (b-a)/4;
    a = c;
    b = d;
    update();
}

void Window::zoom_out()
{
    s--;
    double c = (a+b)/2 - (b-a);
    double d = (a+b)/2 + (b-a);
    a = c;
    b = d;
    update();
}

void Window::impr_acc()
{
    n *= 2;
    n = std::min(n, MAX_N);
    update();
}

void Window::degr_acc()
{
    n /= 2;
    n = std::max(n, 3);
    update();
}

/// render graph
void Window::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    double x1, x2, y1, y2;
    double max_y, min_y;
    double y11, y12;
    double delta_y, delta_x = (b - a) / N;

    interpolation_ctx ctx1 = interpolation_create(INTERPOLATION_METHOD1,
                n, func_id, a0 * (fabs(a0) >= fabs(a)) + a * (fabs(a0) < fabs(a)),
                b0* (fabs(b0) >= fabs(b)) + b * (fabs(b0) < fabs(b)));
    interpolation_ctx ctx2 = interpolation_create(INTERPOLATION_METHOD2,
                n, func_id, a0 * (fabs(a0) >= fabs(a)) + a * (fabs(a0) < fabs(a)),
                b0* (fabs(b0) >= fabs(b)) + b * (fabs(b0) < fabs(b)));

    double (*f_appr)(interpolation_ctx, double);
    f_appr = interpolation_calculate;


    // calculate min and max for current function
    // y11 - max, y12 - min
    max_y = min_y = 0;
    for (x1 = a; x1 - b < 1e-6; x1 += delta_x) {
        y11 = (method == 1) * std::max(f(x1), f_appr(ctx1, x1)) +
            (method == 2) * std::max(f(x1), f_appr(ctx2, x1)) +
            (method == 3) * std::max(std::max(f(x1), f_appr(ctx1, x1)), f_appr(ctx2, x1)) +
            (method == 0) * std::max(fabs(f(x1)-f_appr(ctx1, x1)),
                                fabs(f(x1)-f_appr(ctx2, x1)));
        y12 = (method == 1) *std::min(f(x1), f_appr(ctx1, x1)) +
            (method == 2) * std::min(f(x1), f_appr(ctx2, x1)) +
            (method == 3) * std::min(std::min(f(x1), f_appr(ctx1, x1)), f_appr(ctx2, x1)) +
            (method == 0) * std::min(fabs(f(x1)-f_appr(ctx1, x1)),
                                fabs(f(x1)-f_appr(ctx2, x1)));
        if (y12 < min_y)
            min_y = y12;
        if (y11 > max_y)
            max_y = y11;
    }

    delta_y = std::max(0.01 * (max_y - min_y), 0.0001);
    double abs_max = std::max(max_y, (-1)*min_y);
    printf("max = %lf\n", abs_max);
    min_y -= delta_y;
    max_y += delta_y;

    // save current Coordinate System
    painter.save();

    // make Coordinate Transformations
    painter.translate(0.5 * width(), 0.5 * height());
    painter.scale(width() / (b - a), -height() / (max_y - min_y));
    painter.translate(-0.5 * (a + b), -0.5 * (min_y + max_y));
    // fprintf(stderr, "%.2f %f %f %f\n", a, b, min_y, max_y);

    QPen pen("black");
    pen.setWidth(0);
    pen.setCapStyle(Qt::RoundCap);
    painter.setPen(pen);

    // draw axis
    painter.drawLine(QPointF(a , 0), QPointF(b, 0));
    painter.drawLine(QPointF(0, min_y), QPointF(0, max_y));


    // draw approximated line for graph

    // сама функция
    if (method != 0) {
        pen.setWidth(0);
        pen.setColor("red");
        painter.setPen(pen);
        x1 = a;
        y1 = f(x1);
        for (x2 = x1 + delta_x; x2 - b < 1e-6; x2 += delta_x) {
            y2 = f(x2);
            painter.drawLine(QPointF(x1, y1), QPointF(x2, y2));

            x1 = x2, y1 = y2;
        }
        x2 = b;
        y2 = f(x2);
        painter.drawLine(QPointF(x1, y1), QPointF(x2, y2));
    }
    // 1 метод
    if (method == 1 || method == 3) {
        pen.setWidth(0);
        pen.setColor("blue");
        painter.setPen(pen);
        x1 = a;
        y1 = f_appr(ctx1, x1);
        for (x2 = x1 + delta_x; x2 - b < 1e-6; x2 += delta_x) {
            y2 = f_appr(ctx1, x2);
            painter.drawLine(QPointF(x1, y1), QPointF(x2, y2));

            x1 = x2, y1 = y2;
        }
        x2 = b;
        y2 = f_appr(ctx1, x2);
        painter.drawLine(QPointF(x1, y1), QPointF(x2, y2));
    }
    // 2 метод
    if (method == 2 || method == 3) {
        pen.setWidth(0);
        pen.setColor("green");
        painter.setPen(pen);
        x1 = a;
        y1 = f_appr(ctx2, x1);
        for (x2 = x1 + delta_x; x2 - b < 1e-6; x2 += delta_x) {
            y2 = f_appr(ctx2, x2);
            painter.drawLine(QPointF(x1, y1), QPointF(x2, y2));

            x1 = x2, y1 = y2;
        }
        x2 = b;
        y2 = f_appr(ctx2, x2);
        painter.drawLine(QPointF(x1, y1), QPointF(x2, y2));
    }
    // погрешности
    if (method == 0) {

        // yi1 - погрешность 1, yi2 - погрешность 2
        double y21, y22;

        x1 = a;
        y11 = fabs(f(x1) - f_appr(ctx1, x1));
        y12 = fabs(f(x1) - f_appr(ctx2, x1));
        for (x2 = x1 + delta_x; x2 - b < 1e-6; x2 += delta_x) {
            y21 = fabs(f(x2) - f_appr(ctx1, x2));
            y22 = fabs(f(x2) - f_appr(ctx2, x2));

            pen.setWidth(0);
            pen.setColor("blue");
            painter.setPen(pen);
            painter.drawLine(QPointF(x1, y11), QPointF(x2, y21));

            pen.setWidth(0);
            pen.setColor("green");
            painter.setPen(pen);
            painter.drawLine(QPointF(x1, y12), QPointF(x2, y22));

            x1 = x2, y11 = y21, y12 = y22;
        }
        x2 = b;
        y21 = fabs(f(x2) - f_appr(ctx1, x2));
        y22 = fabs(f(x2) - f_appr(ctx2, x2));

        pen.setWidth(0);
        pen.setColor("blue");
        painter.setPen(pen);
        painter.drawLine(QPointF(x1, y11), QPointF(x2, y21));

        pen.setWidth(0);
        pen.setColor("green");
        painter.setPen(pen);
        painter.drawLine(QPointF(x1, y12), QPointF(x2, y22));


    }
    interpolation_destroy(ctx1);

    interpolation_destroy(ctx2);
    // restore previously saved Coordinate System
    painter.restore();

    // render function name
    painter.setPen("black");
    painter.drawText(0, 20, QString("k = %1").arg(func_id));
    painter.drawText(0, 40, f_name);
    painter.drawText(0, 60, QString("s = %1").arg(s));
    painter.drawText(0, 80, QString("abs_y_max = %1, a = %2, b = %3").arg(abs_max).arg(a).arg(b));
    painter.drawText(0, 100, QString("n = %1").arg(n));
    painter.drawText(0, 120, QString("mode = %1").arg(method));
    painter.drawText(0, 150, QString("Initial data: n = %1, a = %2, b = %3").arg(n0).arg(a0).arg(b0));
    
}
