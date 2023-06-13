#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<mpi.h>

int min(int i, int j)
{
	if(i < j) return i;
	else return j;
}

int LU(double *a, double *buff, int n, int rank, int size) 
{
    int i, j, k;
    int rows;
    
    if (rank + 1 > n%size) rows = n/size;
	else rows = n/size + 1; 
	
	for(j = 1; j < n; j++)
    	{
        	if (rank == 0)
        	{
            	if(a[0] == 0) return -1;
                else a[j] = a[j] / a[0];
        	}
    	} 
    	
    	for(k = 1; k < n; k++)
    	{
    		if(rank == (k - 1)%size)
    		{
    			MPI_Bcast(a + (k-1)/size * n, n, MPI_DOUBLE, (k-1)%size, MPI_COMM_WORLD); 
    			
    			for(i = 0; i < rows; i++)
    			{
    				if(rank + i*size >= k) 
    				{
    					for(j = k; j < n; j++) 
    					{
    						a[i * n + j] -= a[i * n + (k-1)]*a[(k-1)/size * n + j]; 
    						
    						if(rank + i*size == k && j != k)
    						{
    							if(a[i*n + k] != 0)
							{
								a[i*n + j] = a[i*n + j] / a[i*n + k]; 
							}
							else return -1;
    						}
    					}
    				}
    			}
    			
    			
    		}
    		else
    		{
    			MPI_Bcast(buff, n, MPI_DOUBLE, (k-1)%size, MPI_COMM_WORLD); 
    			
    			for(i = 0; i < rows; i++)
    			{
    				if(rank + i*size >= k)
    				{
    					for(j = k; j < n; j++)
    					{
    						a[i * n + j] -= a[i * n + (k-1)]*buff[j]; 
    						if(rank + i*size == k && j != k)
    						{
    							if(a[i*n + k] != 0)
							{
								a[i*n + j] = a[i*n + j] / a[i*n + k];
							}
							else return -1;
    						}
    					}
    				}
    			}
    			
    			
    		}
    		
 
    	}
    	
    	
    	
    	
	

    return 0;
}

int Gaus(int n, double *a, double *b, double *x, int my_rank, int p)
{
	int i, j, k;
	int rows;
	int first;
	double tmp;

	if (my_rank + 1 > n%p) rows = n/p;
	else rows = n/p + 1; 

	for (i = 0; i < rows; i++)
		for (j = 0; j < n; j++)
			b[i * n + j] = (my_rank + p * i == j); 

	for (i = n - 1; i >= 1; i--)
	{
		if (my_rank == i%p)
		{
			MPI_Bcast(b + i/p * n, n, MPI_DOUBLE, i%p, MPI_COMM_WORLD); 
			for (j = i/p - 1; j >= 0 ; j--)
				for (k = 0; k < n; k++)
					b[j * n + k] -= b[i/p * n + k] * a[j * n + i];
		}
		else
		{
			MPI_Bcast(x, n, MPI_DOUBLE, i%p, MPI_COMM_WORLD);

			if (my_rank < i%p) first = i/p;
			else if (i/p - 1 >= 0) first = i/p - 1;
			else continue;

			for (j = first; j >= 0; j--)
				for (k = 0; k < n; k++)
					b[j * n + k] -= x[k] * a[j * n + i];
		}
	}
	
	for(int i = 0; i < n; i++)
	{
		if(my_rank == i%p)
		{
			for(int j = i + 1; j < n; j++)
			{
				a[i/p * n + j] = 0;	
			}	
		}	
	}
	
	
	for (i = 0; i < n; i++)
	{
		if (my_rank == i%p)
		{
			if(a[i/p * n + i] == 0)
				return -1;
			tmp = 1.0/a[i/p * n + i];
			for (j = i; j < n; j++)
				a[i/p * n + j] *= tmp;
			for (j = 0; j < n; j++)
				if(j <= i)
					b[i/p * n + j] *= tmp;

			if (i == n - 1) continue;

			for (j = i; j < n; j++)
				x[j] = a[i/p * n + j];
			for (j = 0; j < n; j++)
				if(j <= i)
					x[j + n] = b[i/p * n + j];
				else
					x[j + n] = 0;

			MPI_Bcast(x, 2 * n, MPI_DOUBLE, i%p, MPI_COMM_WORLD);
			for (j = i/p + 1; j < rows; j++)
			{
				tmp = a[j * n + i];
				for (k = i; k < n; k++)
					a[j * n + k] -= tmp * a[i/p * n + k];
				for (k = 0; k < n; k++)
					if(k <= i)
						b[j * n + k] -= tmp * b[i/p * n + k];
			}
		}
		else
		{
			if (i == n - 1) continue;

			if (my_rank > i%p) first = i/p;
			else first = i/p + 1;

			MPI_Bcast(x, 2 * n, MPI_DOUBLE, i%p, MPI_COMM_WORLD);
			for (j = first; j < rows; j++)
			{
				tmp = a[j * n + i];
				for (k = i; k < n; k++)
					a[j * n + k] -= tmp * x[k];
				for (k = 0; k < n; k++)
					b[j * n + k] -= tmp * x[k + n];
			}
		}
	} 
	
	return 0;
	
}

int inv(int n, double *b, double *a, double *x, double *buff, int my_rank, int p)
{

	int rows, i, j, k;
	
	if (my_rank + 1 > n%p) rows = n/p;
	else rows = n/p + 1; 

	if(LU(a, buff, n, my_rank, p) == -1)
		return -1;
	
	if(Gaus(n, a, b, x, my_rank, p) == -1)
		return -1;

	for(int i = 0; i < n; i++)
	{
		if(my_rank == i%p)
			a[i/ p * n + i] = 0;
	}

	
	
	for(k = n-1; k >= 0; k--) 
    	{
    		if(my_rank == k%p)
    		{
    			MPI_Bcast(b + k/p * n, n, MPI_DOUBLE, k%p, MPI_COMM_WORLD); 
    				
    			for(i = 0; i < rows; i++)
    			{
    				if(my_rank + i*p <= k) 
    				{
    					for(j = 0; j <= k; j++) 
    					{
    						if(my_rank + i*p < k && j < k)
    						{
    							a[i * n + j] += b[i * n + k]*b[k/p * n + j]; 
    						}
    						
    						if(my_rank + i*p == k)
    						{
    							a[i * n + j] += b[i * n + j]; 
    						}
    						
    						if(j == k && my_rank + i*p < k)
    						{
    							a[i * n + j] += b[k/p * n + k]*b[i*n + k];
    						}
    						
    					}
    				}
    			}
    			
    			
    		}
    		else
    		{
    			MPI_Bcast(buff, n, MPI_DOUBLE, k%p, MPI_COMM_WORLD); 
    			
    			for(i = 0; i < rows; i++)
    			{
    				if(my_rank + i*p <= k) 
    				{
    					for(j = 0; j <= k; j++) 
    					{
    						if(my_rank + i*p < k && j < k)
    						{
    							a[i * n + j] += b[i * n + k]*buff[j]; 
    						}
    						
    						if(my_rank + i*p == k)
    						{
    							a[i * n + j] += buff[j]; 
    						}
    						
    						if(j == k && my_rank + i*p < k)
    						{
    							a[i * n + j] += buff[k]*b[i*n + k];
    						}
    						
    					}
    				}
    			}
    			
    			
    			
    			
    		}
    		
 
    	}
	
	
	
	return 0;

}














