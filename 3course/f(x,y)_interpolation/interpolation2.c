#include <stdlib.h>
#include<stdio.h>
#include "interpolation2.h"
#include"algorithm.h"
#include <math.h>
#define EPS (1e-25)

struct interpolation2_ctx_inner {
	int	method;
	int	n_x, n_y;
	double	x_a, x_b, y_a, y_b, step_x, step_y;
	double* Arr;

	//for method 2
	double* F;
	double* Fx;
	double* Fy;
	double* Fxy;
	//


	double	(*f)(double, double);
	double	(*dxf)(double, double);
	double	(*dyf)(double, double);
	double	(*dxyf)(double, double);
};

static double f0(double x, double y) {
	return x*0 + y*0 + 1;
}

static double dxf0(double x, double y) {
	return x*0 + y*0 + 0;
}

static double dyf0(double x, double y) {
	return x*0 + y*0 + 0;
}

static double dxyf0(double x, double y) {
	return x*0 + y*0 + 0;
}


static double f1(double x, double y) {
	return x + y*0;
}

static double dxf1(double x, double y) {
	return x*0 + y*0 + 1;
}

static double dyf1(double x, double y) {
	return x*0 + y*0 + 0;
}

static double dxyf1(double x, double y) {
	return x*0 + y*0 + 0;
}


static double f2(double x, double y) {
	return y + x*0;
}


static double dxf2(double x, double y) {
	return 0 + x*0 + y*0;
}

static double dyf2(double x, double y) {
	return 1 + x*0 + y*0;
}

static double dxyf2(double x, double y) {
	return 0 + x*0 + y * 0;
}


static double f3(double x, double y) {
	return x + y;
}


static double dxf3(double x, double y) {
	return 1 + x*0 + y*0;
}

static double dyf3(double x, double y) {
	return 1 + x*0 + y*0;
}

static double dxyf3(double x, double y) {
	return 0 + x*0 + y*0;
}


static double f4(double x, double y) {
	return sqrt(x * x + y * y);
}


static double dxf4(double x, double y) {
	return x / sqrt(x * x + y * y);
}

static double dyf4(double x, double y) {
	return y / sqrt(x * x + y * y);
}

static double dxyf4(double x, double y) {
	return -x * y / (sqrt(x * x + y * y) * sqrt(x * x + y * y) * sqrt(x * x + y * y));
}



static double f5(double x, double y) {
	return x * x + y * y;
}

static double dxf5(double x, double y) {
	return 2 * x + y*0;
}

static double dyf5(double x, double y) {
	return 2 * y + x*0;
}

static double dxyf5(double x, double y) {
	return 0 + x*0 + y*0;
}


static double f6(double x, double y) {

	return exp(x * x - y * y);
}

static double dxf6(double x, double y) {
	return 2 * x * exp(x * x - y * y);
}

static double dyf6(double x, double y) {
	return -2 * y * exp(x * x - y * y);
}

static double dxyf6(double x, double y) {
	return -4 * y * x * exp(x * x - y * y);
}


static double f7(double x, double y) {

	return 1.0 / (25 * (x * x + y * y) + 1);
}

static double dxf7(double x, double y) {
	return -50.0 * x / ((1 + 25 * (x * x + y * y)) * (1 + 25 * (x * x + y * y)));
}

static double dyf7(double x, double y) {
	return -50.0 * y / ((1 + 25 * (x * x + y * y)) * (1 + 25 * (x * x + y * y)));
}

static double dxyf7(double x, double y) {
	return 200.0 * x * y / ((1 + 25 * (x * x + y * y)) * (1 + 25 * (x * x + y * y)) * (1 + 25 * (x * x + y * y)));
}


interpolation2_ctx interpolation2_create(int method, int n_x, int n_y, int k,
	double x_a, double x_b,
	double y_a, double y_b) {
	interpolation2_ctx res_ptr;

	/* must check arguments here */

	if (method != 1 && method != 2) {
		return NULL;
	}
	if (n_x < 3 || n_y < 3) {
		return NULL;
	}
	if (x_b < x_a + EPS || y_b < y_a + EPS) {
		return NULL;
	}
	if (k < 0 || k>7) {
		return NULL;
	}
	if (x_b <= x_a || y_b <= y_a) {
		return NULL;
	}

	res_ptr = (interpolation2_ctx)malloc(sizeof(*res_ptr));

	res_ptr->method = method;
	res_ptr->n_x = n_x;
	res_ptr->n_y = n_y;
	res_ptr->x_a = x_a;
	res_ptr->x_b = x_b;
	res_ptr->y_a = y_a;
	res_ptr->y_b = y_b;

	switch (k) {
	case INTERPOLATION2_F_1:
		res_ptr->f = f0;
		res_ptr->dxf = dxf0;
		res_ptr->dyf = dyf0;
		res_ptr->dxyf = dxyf0;
		break;
	case INTERPOLATION2_F_X:
		res_ptr->f = f1;
		res_ptr->dxf = dxf1;
		res_ptr->dyf = dyf1;
		res_ptr->dxyf = dxyf1;
		break;
	case INTERPOLATION2_F_Y:
		res_ptr->f = f2;
		res_ptr->dxf = dxf2;
		res_ptr->dyf = dyf2;
		res_ptr->dxyf = dxyf2;
		break;
	case INTERPOLATION2_F_XY:
		res_ptr->f = f3;
		res_ptr->dxf = dxf3;
		res_ptr->dyf = dyf3;
		res_ptr->dxyf = dxyf3;
		break;
	case INTERPOLATION2_F_SQRX2Y2:
		res_ptr->f = f4;
		res_ptr->dxf = dxf4;
		res_ptr->dyf = dyf4;
		res_ptr->dxyf = dxyf4;
		break;
	case INTERPOLATION2_F_X2Y2:
		res_ptr->f = f5;
		res_ptr->dxf = dxf5;
		res_ptr->dyf = dyf5;
		res_ptr->dxyf = dxyf5;
		break;
	case INTERPOLATION2_F_EX2Y2:
		res_ptr->f = f6;
		res_ptr->dxf = dxf6;
		res_ptr->dyf = dyf6;
		res_ptr->dxyf = dxyf6;
		break;
	default:
		res_ptr->f = f7;
		res_ptr->dxf = dxf7;
		res_ptr->dyf = dyf7;
		res_ptr->dxyf = dxyf7;
	}

	count_Arr_1(res_ptr);

	return res_ptr;
}

double interpolation2_calculate(interpolation2_ctx ctx, double x, double y)
{

	return P(x, y, ctx);
}

void interpolation2_destroy(interpolation2_ctx ctx)
{
	if (ctx->method == 2) {
		free(ctx->F);
		free(ctx->Fx);
		free(ctx->Fy);
		free(ctx->Fxy);
	}
	free(ctx->Arr);
	free(ctx);
}

// в норме мы проходим N^2 точек

/*double norma(interpolation2_ctx ctx, int N, double x_a, double x_b, double y_a, double y_b)
{
    double norma = 0;
    double h_x = 0;
    //double h_y = 0;
    //double raz = 0;
    //double z_x, z_y;
    //z_x = x_a;
    //z_y = y_a;
    
    //printf("\n\n y_a = %10.3e \n\n", y_a);
    //printf("\n\n z_y = %10.3e \n\n", z_y);
    
    h_x = (x_b - x_a)/ N;
    h_y = (y_b - y_a)/ N;
    
    //printf("\n\n h_x = %10.3e \n\n", h_x);
    //printf("\n\n h_y = %10.3e \n\n", h_y);
    
    
    
    while(z_y < y_b)
    {
    	while(z_x < x_b)
    	{
		raz = ctx->f(z_x, z_y) - interpolation2_calculate(ctx, z_x, z_y);
		//printf("%10.3e\n", raz);
		if(raz > norma)
		{
			norma = raz;
		}
		    
        	 z_x = z_x + h_x;
        }
        z_y = z_y + h_y;
       
    }
    
    zx = z_x; 
    zy = z_y;
    
    
    for(int i = N/3; i < N/2; i++)
    {
    	for(int j = N/3; j < N/2; j++)
    	{
    	
    		raz = 0;
    		raz = ctx->f(z_x + i*h_x, z_y + j*h_y) - interpolation2_calculate(ctx, z_x + i*h_x, z_y + j*h_y);
    		
    		//printf("\n\n z_x = %10.3e \n\n", z_x + i*h_x);
    		//printf("\n\n z_y = %10.3e \n\n", z_y + j*h_y);
    		//printf("\n\n raz = %10.3e \n\n", raz);
    		//printf("\n\n P(x) = %10.3e \n\n", interpolation2_calculate(ctx, z_x + i*h_x, z_y + j*h_y));
    		
		//printf("%10.3e\n", raz);
		if(raz > norma)
		{
			norma = raz;
			zx = z_x + i*h_x;
			zy = z_y + j*h_y;
		}
    	}
    } 
    
    //printf("\n\n zx = %10.3e \n\n", zx);
    //printf("\n\n zy = %10.3e \n\n", zy);
    
    return norma;
} */
