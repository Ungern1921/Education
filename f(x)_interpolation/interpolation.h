#ifndef __INTERPOLATION_H
#define __INTERPOLATION_H

struct interpolation_ctx_inner;

typedef struct interpolation_ctx_inner *interpolation_ctx;

#define INTERPOLATION_METHOD1	1
#define INTERPOLATION_METHOD2	2

#define INTERPOLATION_F_1	0
#define INTERPOLATION_F_X	1
#define INTERPOLATION_F_X2	2
#define INTERPOLATION_F_X3	3
#define INTERPOLATION_F_X4	4
#define INTERPOLATION_F_EX	5
#define INTERPOLATION_F_1DIVX2	6

interpolation_ctx interpolation_create(int method, int n, int k,
				       double a, double b);

double interpolation_calculate(interpolation_ctx ctx, double e);
void gen1(double(f(double)), int n, double* a, double* d, double* c, double* x); // method1 
void gen2(double(f(double)), int n, double* a, double* d, double* c, double* u, double* x); // metod2
void interpolation_destroy(interpolation_ctx ctx);
double g(double x, double(f(double)));
double g2(double x, double(f(double)));
double k(double x, double y, double(f(double)));
double norma(interpolation_ctx ctx, int N, double a, double b);

#endif
