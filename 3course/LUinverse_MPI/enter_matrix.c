#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<mpi.h>

int max(int a, int b)
{
	if(a > b) return a;
	else return b;
}

int mod(int a)
{
	if(a > 0) return a;
	else return -a;
}

double f(int k, int n, int i, int j)
{

	if(k == 1)
		return n - max(i+1, j+1) + 1;
	else if(k == 2) 
		return max(i+1, j+1);
	else if(k == 3)
		return mod(i - j);
	else if(k == 4)
		return 1 / ((double)(i+1) + (double)(j+1) - 1);
	else return 0;

}

void print_matrix(double *a, int l, int n, int m, int rank, int size, double *buff)
{

	int i, j;
	MPI_Status status;

	for (i = 0; i < fmin(m, n); i++)
	{
		if (rank == 0)
		{
			if ( rank == i%size)
			{
				for (j = 0; j < fmin(m, l); j++)
					printf("%10.3e ", a[i/size * n + j]);
				printf("\n");
			}
			else
			{
				MPI_Recv(buff, fmin(m, l), MPI_DOUBLE, i%size, 0, MPI_COMM_WORLD, &status);
				for (j = 0; j < fmin(m, l); j++)
					printf("%10.3e ", buff[j]);
				printf("\n");
			}
		}
		else if (rank == i%size)
		{
			for (j = 0; j < fmin(m, l); j++)
				buff[j] = a[i/size * n + j];
			MPI_Send(buff, fmin(m, l), MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
		}
	}
	
}

int enter_matrix(int n, int k, FILE *in, double *a, int rank, int size)
{
	
	int i, j;
	int rows;
	double p;
	
	if (rank > n%size - 1) rows = n/size;  
	else rows = n/size + 1;
	
	if(k == 0)
	{
		for(i = 0; i < n; i++)
		{
			for(j = 0; j < n; j++)
			{
				if(rank != i%size)   
				{
					if( fscanf(in, "%lf", &p) != 1)
						{
							printf("Error reading file\n");
							return -1;
						}
				}
				else
				{
					if( fscanf(in, "%lf", &a[i/size * n + j]) != 1)
						{
							printf("Error reading file\n");
							return -1;
						}
				}
			}
		}
		
		fclose(in);
		return 0;
	
	}

	for (i = 0; i < rows; i++)
		for (j = 0; j < n; j++)
			a[i * n + j] = f(k, n, rank + size*i, j);
	 

	return 0;
}
