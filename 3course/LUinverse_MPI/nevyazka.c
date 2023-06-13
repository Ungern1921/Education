#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<mpi.h>

double scalar_product4(double *a, double *b, double *buff, int n, int raw1, int col2, int rank, int size)
{

	
	double local_sum = 0.0, global_sum = 0.0;
	if (rank == raw1 % size)
		for (int i = 0; i < n; ++i)
			buff[i] = b[raw1 / size * n + i];

	MPI_Bcast(buff, 2*n, MPI_DOUBLE, raw1 % size, MPI_COMM_WORLD);

	for (int i = 0; i < n; ++i)
		if (rank == i % size)
			local_sum += buff[i] * a[i / size * n + col2];

	MPI_Allreduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

	return global_sum;
}

/*double nevyazka(double *a, double *e, int n)
{
	double s = 0, c = 0;

	for(int i = 0; i < n; i++)
	{
		for(int j = 0; j < n; j++)
		{
			c = 0;

			for(int k = 0; k < n; k++)
			{
				c = c + a[i*n + k]*e[k*n + j];
			}

			if(i == j) 
				c = c - 1;

			s = s + c*c;	
		}
	}

	return sqrt(s);
}*/

double nevyazka(double *a, double *b, double *buff, int n, int rank, int size)
{
	double s = 0, c = 0;
	

	for(int i = 0; i < n; i++)
	{
		
		for(int j = 0; j < n; j++)
		{
			c = 0;

			c = scalar_product4(a, b, buff, n, i, j, rank, size);

			if(i == j) 
				c = c - 1;

			s = s + c*c;
			
				
		}
	}

	return sqrt(s);
}
