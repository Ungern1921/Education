#include <stdio.h>
#include<math.h>
#include "interpolation.h"



void gen1(double(f(double)), int n, double* a, double* d, double* c, double * x)
{
	
	int m;
	m = n + 2;
	
	for(int i = 1; i < m - 1; i++)
	{
		d[i] = g(x[i], f);
	}
    
    d[0] = g(x[0], f);
    d[m-1] = g(x[m-1], f);
    
    
		
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
		// printf("%f\n", c[(m-1)+i]);
		c[2*(m-1) + i] = a[2*(m-1) + i] - a[3*(m-1) + i] * (x[i+1] - x[i]);
		c[3*(m-1) + i] = a[3*(m-1) + i];
	}
}
