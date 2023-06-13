#include <stdlib.h>
#include <stdio.h>
#include "interpolation.h"
#include <math.h>

static double g0(double x) 
{
	return x*0;
}
static double g1(double x)
{
	return x*0 + 1;
}
static double g2(double x)
{
	return 2 * x;
}
static double g3(double x)
{
	return 3 * x * x;
}
static double g4(double x)
{
	return 4 * x * x * x;
}
static double g5(double x)
{
	return exp(x);
}
static double g6(double x)
{
	return ((-1)*50*x) / ((25.0 * x * x + 1)*(25.0 * x * x + 1));
}
	
/*double g2(double x, double(f(double))) // g2(x) = f''(x)
{
    double EPS = 0.000001;
    return (g(x + EPS, f) - g(x, f)) / EPS;
}*/
	


struct interpolation_ctx_inner {
	int	method; // the method by which we interpolate
	int	n; // many of interpolation points
	double* a;
	double* d;
	double* c;
	double* x;
	double* u;
	double* F;
	double* K;
	double* G;
//	double	*f_a;

	double	(*f)(double);
	double	(*g)(double);
};

static double f0(double x) 
{
	return x*0 + 1.0;
}
static double f1(double x)
{
	return x;
}
static double f2(double x)
{
	return x * x;
}
static double f3(double x)
{
	return x * x * x;
}
static double f4(double x)
{
	return x * x * x * x;
}
static double f5(double x)
{
	return exp(x);
}
static double f6(double x)
{
	return 1.0 / (25.0 * x * x + 1);
}

interpolation_ctx interpolation_create(int method, int n, int k,
				       double a, double b)
{
	interpolation_ctx res_ptr;
    int m;

	if (method < 1 || method > 2)
	{
		fprintf(stderr,"Incorrect method\n");
		return NULL;
	}
	if (n < 1 || k < 0 || k > 6 || n < 5)
	{
		fprintf(stderr, "Incorrect args\n");
		return NULL;
	}
	if (b <= a)
	{
		fprintf(stderr, "Incorrect interval\n");
		return NULL;
	}
	/* must check arguments here */

	res_ptr = malloc(sizeof(*res_ptr));

	res_ptr->method = method;
	res_ptr->n = n;
	//int m;
	m = n + 2;
	
	res_ptr->a = (double *)malloc(4*(m-1)*sizeof(double)); 
	res_ptr->x = (double *)malloc(m*sizeof(double));  // split points
	res_ptr->d = (double *)malloc(m*sizeof(double)); 
	//res_ptr->u = (double *)malloc(m*(m+1)*sizeof(double));
	res_ptr->u = (double *)malloc(4*m*sizeof(double));
	res_ptr->c = (double *)malloc(4*(m-1)*sizeof(double)); 
	res_ptr->F = (double *)malloc(m*sizeof(double));
	res_ptr->K = (double *)malloc((m-1)*sizeof(double));
	res_ptr->G = (double *)malloc(m*sizeof(double));
	//res_ptr->k = (double *)malloc((m-1)*sizeof(double)); // тут разделённые разности будут лежать
	// a, d, c, u - coefficients from the formula from the method
	
	if (res_ptr->a == NULL || res_ptr->x == NULL || res_ptr->d == NULL || res_ptr->u == NULL || res_ptr->c == NULL || res_ptr->F == NULL || res_ptr->K == NULL ||res_ptr->G == NULL)
	{
		fprintf(stderr, "Not enough memory\n");
		return NULL;
	}
	
	res_ptr->x[0] = a;
	for(int i = 1; i < m ; i++)
	{
		res_ptr->x[i] = res_ptr->x[i-1] + (b - a) / (n + 1);
	} // building split points

	/*for(int i = 0; i < m-1; i++)
	{
		k[i] = (f(x[i + 1]) - f(x[i])) / (x[i+1] - x[i]);
	}*/
	
	switch (k) {
	case 0:
		res_ptr->f = f0;
		res_ptr->g = g0;
		break;
	case 1:
		res_ptr->f = f1;
		res_ptr->g = g1;
		break;
	case 2:
		res_ptr->f = f2;
		res_ptr->g = g2;
		break;
	case 3:
		res_ptr->f = f3;
		res_ptr->g = g3;
		break;
	case 4:
		res_ptr->f = f4;
		res_ptr->g = g4;
		break;
	case 5:
		res_ptr->f = f5;
		res_ptr->g = g5;
		break;
	case 6:
		res_ptr->f = f6;
		res_ptr->g = g6;
	} // select the function
	
	for(int i = 0; i < m; i++)
	{
		res_ptr->F[i] = res_ptr->f(res_ptr->x[i]);
	}
	
	for(int i = 0; i < m; i++)
	{
		res_ptr->G[i] = res_ptr->g(res_ptr->x[i]);
	}
	
	for(int i = 0; i < m - 1; i++)
	{
		res_ptr->K[i] = (res_ptr->F[i + 1] - res_ptr->F[i]) / (res_ptr->x[i + 1] - res_ptr->x[i]);
	}
	
	if (method == 1)
		gen1(res_ptr->n, res_ptr->a, res_ptr->d, res_ptr->c, res_ptr->x, res_ptr->F, res_ptr->K, res_ptr->G);
	else
	{
		gen2(res_ptr->n, res_ptr->a, res_ptr->d, res_ptr->c, res_ptr->u, res_ptr->x, res_ptr->F, res_ptr->K, res_ptr->G);
	}
	return res_ptr;
}
double interpolation_calculate(interpolation_ctx ctx, double e)
{
	//return 0;
	
	double res, h;
	int n = ctx->n;
	
	int m, i;
	m = n + 2;
    
    // printf("f = %lf\n\n", ctx->f(e));
    
    	
	
	/*for(int i = 0; i < m - 1; i++)
		{
			if(e >= ctx->x[i] && e < ctx->x[i + 1])
			{
			
				printf("%d\n", i);
				res = ctx->c[i] + ctx->c[(m-1) + i]*(e - ctx->x[i]) + ctx->c[2*(m-1) + i]*pow((e - ctx->x[i]),2) + ctx->c[3*(m-1) + i]*pow((e - ctx->x[i]),3);

			}
		}*/
	
	h = ( ctx->x[m-1] - ctx->x[0] ) / ( m - 1 );
	
	i = (e - ctx->x[0]) / h;
	
	res = ctx->c[i] + ctx->c[(m-1) + i]*(e - ctx->x[i]) + ctx->c[2*(m-1) + i]*pow((e - ctx->x[i]),2) + ctx->c[3*(m-1) + i]*pow((e - ctx->x[i]),3);

	return res;
}

double norma(interpolation_ctx ctx, int N, double a, double b)
{
    double norma = 0;
    double h = 0;
    double raz = 0;
    double z;
    z = a;
    
    h = (b - a) / N;
    
    while(z < b)
    {
        raz = ctx->f(z) - interpolation_calculate(ctx, z);
        if(raz > norma)
            norma = raz;
           
        z = z + h;
    }
    
    return norma;
} // interpolation error

void interpolation_destroy(interpolation_ctx ctx)
{
	free(ctx->a);
	free(ctx->x);
	free(ctx->d);
	free(ctx->c);
	free(ctx->u);
	free(ctx->F);
	free(ctx->K);
	free(ctx->G);
	//free(ctx->k);
	free(ctx);
} // clear of memory
