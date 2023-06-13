#include <stdio.h>
#include<math.h>
#include "interpolation.h"



void gen1(int n, double* a, double* d, double* c, double * x, double *F, double *K, double *G)
{
	
	int m;
	m = n + 2;
	
	for(int i = 1; i < m - 1; i++)
	{
		d[i] = G[i];
	}
    
    d[0] = G[0];
    d[m-1] = G[m-1];
    
    
		
	for(int i = 0; i < m - 1; i++)
	{
		a[i] = F[i];
		a[(m - 1) + i] = d[i];
		a[2*(m - 1) + i] = ( K[i] - d[i] ) / (x[i+1] - x[i]);
		a[3*(m - 1) + i] = ( d[i] + d[i + 1] - 2*K[i] ) / pow((x[i+1] - x[i]), 2);
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
