#include <stdio.h>
#include "interpolation.h"
#include <math.h>


void gen2(int n, double* a, double* d, double* c, double* u, double * x, double* F, double *K, double* G)
{
	
	int m;
    	double l;
	m = n + 2;
	
	/*u[0] = 0;
	u[(m-1)*4 + 2] = 0;*/
	
	/*for(int i = 0; i < m; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			u[i*4 + j] = 0;
		}
	} */
	
	u[0] = 0;
	u[1] = 1;
	u[2] = 0;
	u[3] = G[0];
	u[(m-1)*4 + 0] = 0;
	u[(m-1)*4 + 1] = 1;
	u[(m-1)*4 + 2] = 0;
	u[(m-1)*4 + 3] = G[m - 1];

	for(int i = 1; i < m - 1; i++)
	{
		u[i*4 + 1] = 2*(x[i+1] - x[i-1]);
		u[i*4 + 2] = x[i] - x[i-1];
		u[i*4 + 0] = x[i+1] - x[i];
		u[i*4 + 3] = 3*K[i-1]*(x[i+1] - x[i]) + 3*K[i]*(x[i] - x[i-1]);
	}

	for(int i = 1; i < m - 1; i++)
	{
		l = u[i*4] / u[(i-1)*4 + 1];
	
		u[i*4] -= u[(i-1)*4 + 1] *  l;
		u[i*4 + 1] -= u[(i-1)*4 + 2] * l;
		u[i*4 + 3] -= u[(i-1)*4 + 3] * l;
	}


	for(int i = m-2; i > 0; i--)
	{
		l = u[i*4 + 2] / u[(i+1)*4 + 1];
		
		u[i*4 + 2] -= u[(i+1)*4 + 1] * l;
		u[i*4 + 3] -= u[(i+1)*4 + 3] * l;
		
	}



	for(int i = 0; i < m; i++)
	{
		d[i] = u[i*4 + 3] / u[i*4 + 1];
	}

		printf("\n\n"); 


			

	for(int i = 0; i < m - 1; i++)
	{
		a[i] = F[i];
		a[(m - 1) + i] = d[i];
		a[2*(m - 1) + i] = ( K[i] - d[i] ) / (x[i+1] - x[i]);
		a[3*(m - 1) + i] = (d[i] + d[i + 1] - 2*K[i]) / pow((x[i+1] - x[i]), 2);
	}

	for(int i = 0; i < m - 1; i++)
	{
		c[i] = a[i];
		c[(m-1) + i] = a[(m-1) + i];
			
		c[2*(m-1) + i] = a[2*(m-1) + i] - a[3*(m-1) + i] * (x[i+1] - x[i]);
		c[3*(m-1) + i] = a[3*(m-1) + i];
	}
	
}
