#ifndef __ALGORITHM2_H
#define __ALGORITHM2_H

struct interpolation2_ctx_inner;
typedef struct interpolation2_ctx_inner* interpolation2_ctx;

void count_Arr_1(interpolation2_ctx res_ptr);
double P(double x, double y, interpolation2_ctx res_ptr);

#endif
