#include <stdio.h>
#include "interpolation.h"
#include <math.h>


void gen2(double(f(double)), int n, double* a, double* d, double* c, double* u, double * x)
{
	
	int m;
    double l, z;
	m = n + 2;
	
	for(int i = 0; i < m; i++)
	{
		for(int j = 0; j < m + 1; j++)
		{
			u[i*(m + 1) + j] = 0;
		}
	}

	u[0] = 1;
	u[m] = g(x[0], f);
	u[(m-1)*(m+1) + m - 1 ] = 1;
	u[(m-1)*(m+1) + m] = g(x[m - 1], f);

	for(int i = 1; i < m - 1; i++)
	{
		u[i*(m+1) + i] = 2*(x[i+1] - x[i-1]);
		u[i*(m+1) + i + 1] = x[i] - x[i-1];
		u[i*(m+1) + i - 1] = x[i+1] - x[i];
		u[i*(m+1) + m] = 3*k(x[i-1], x[i], f)*(x[i+1] - x[i]) + 3*k(x[i], x[i+1], f)*(x[i] - x[i-1]);
	}



	//double l, z;

	for(int i = 0; i < m - 1; i++)
	{
		l = u[i*(m + 1) + i];
		z = u[(i + 1)*(m+1) + i];
		for(int j = 0; j < m + 1; j++)
		{
			u[(i+1)*(m+1) + j] = u[(i+1)*(m+1) + j] - u[i*(m+1) + j] * z / l ;


		}
	}


	for(int i = m - 1; i > 0; i--)
	{
		l = u[i*(m+1) + i];
		z = u[(i-1)*(m+1) + i];

		for(int j = 0; j < m + 1; j++)
		{
			u[(i-1)*(m+1) + j] = u[(i-1)*(m+1) + j] - u[i*(m+1) + j]*z / l; 
		}
	}



	for(int i = 0; i < m; i++)
	{
		d[i] = u[i*(m+1) + m] / u[i*(m+1) + i];
	}

		printf("\n\n"); 


			

	for(int i = 0; i < m - 1; i++)
	{
		a[i] = f(x[i]);
		a[(m - 1) + i] = d[i];
		a[2*(m - 1) + i] = ( k(x[i], x[i + 1], f) - d[i] ) / (x[i+1] - x[i]);
		a[3*(m - 1) + i] = (d[i] + d[i + 1] - 2*k(x[i], x[i + 1], f)) / pow((x[i+1] - x[i]), 2);
	}

	for(int i = 0; i < m - 1; i++)
	{
		c[i] = a[i];
		c[(m-1) + i] = a[(m-1) + i];
			
		c[2*(m-1) + i] = a[2*(m-1) + i] - a[3*(m-1) + i] * (x[i+1] - x[i]);
		c[3*(m-1) + i] = a[3*(m-1) + i];
	}
	
}
