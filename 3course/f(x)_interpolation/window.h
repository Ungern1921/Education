#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>


class Window : public QWidget
{
    Q_OBJECT

  private:
    int func_id;
    const char *f_name;
    double a;
    double b;
    int n;

    double a0;
    double b0;
    int n0;
    
    double (*f)(double);
    int s;
    int method; //1 если 1, 2 если 2, 3 если оба, 0 если погрешности  

  public:
    Window(QWidget *parent);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    int parse_command_line(int argc, char *argv[]);

  public slots:
    void change_func();
    void change_method();
    void zoom_in();
    void zoom_out();
    void impr_acc();
    void degr_acc();


  protected:
    void paintEvent(QPaintEvent *event);
};

#endif
