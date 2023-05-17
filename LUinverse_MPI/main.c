#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<mpi.h>

#include"funkzii.h"

int main(int argc, char *argv[])
{
	if(argc < 4 || argc > 5) 
	{
		printf("argc = %d\n", argc);
		return -1;
	}

	char *file_name;
	int n, m, k, rank, size, max_rows;
	double *a, *b, *buff, *x;
	double starttime, endtime, nev;

	n = atoi(argv[1]);
	m = atoi(argv[2]);
	k = atoi(argv[3]);
	file_name = argv[4];
	
	FILE *in;
	if(k == 0)
	{
		if((in = fopen(file_name, "r")) == NULL)
		{
			printf("ERROR: fail ne otkrilsya\n");
			return 0;
		}
	}
	
	if(m > n)
	{
		printf("ne moshet bit m > n. sdelaem m = n\n");
		m = n;
	}
	if(n <= 0)
	{
		printf("ERROR: n = %d\n", n);
		return -1;
	}
	if(m < -1)
	{
		printf("ERROR: m = %d\n", m);
		return -1;
	}
	if(k < 0 || k > 4)
	{
		printf("ERROR: k = %d\n", k);
		return -1;
	}
	
	/* Начать работу с MPI */
	MPI_Init(NULL, NULL);
	
	/* Получить номер текущего процесса в группе всех процессов */
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	/* Получить общее количество запущенных процессов */
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	/*Вычисляем максимальное количество строк на процесс*/
	max_rows = n / size + n % size;
	
	/*Выделение памяти под массивы*/
	a = (double *)malloc(max_rows*n*sizeof(double));
	b = (double *)malloc(max_rows*n*sizeof(double));
	x = (double *)malloc(2*n*sizeof(double));
	buff = (double *)malloc(2*n*sizeof(double));
	
	if(a == NULL)
	{
		printf("Not enough memory\n");
		return -1;
	}
	
	if(b == NULL)
	{
		printf("Not enough memory\n");
		return -1;
	}
	
	if(x == NULL)
	{
		printf("Not enough memory\n");
		return -1;
	}
	
	if(buff == NULL)
	{
		printf("Not enough memory\n");
		return -1;
	}
	
	if(enter_matrix(n, k, in, a, rank, size) == -1) 
	{
		return -1;
	}

	
	
	
	starttime = MPI_Wtime();
	
	
	
	if(inv(n, b, a, x, buff, rank, size) == -1)
	{
		
			if(m == 0)
			{
				if(rank == 0)
					printf("\n\n ne obrashaetsa \n\n");
				MPI_Abort(MPI_COMM_WORLD, -1);
				return -1;
			}
			else if(m == -1)
			{
				if(rank == 0)
					printf("\n\n ne obrashaetsa \n\n");
				MPI_Abort(MPI_COMM_WORLD, -1);
				return -1;
			}
			else
			{
				if(rank == 0)
					printf("\n\n a = \n\n");
					
				if(k == 0)
				{	
					if((in = fopen(file_name, "r")) == NULL)
						{
							printf("ERROR: fail ne otkrilsya\n");
							return 0;
						}
				}
					
				if(enter_matrix(n, k, in, b, rank, size) == -1) 
				{
					return -1;
				}
				
				print_matrix(b, n, n, m, rank, size, buff);
				
				if(rank == 0)
					printf("\n\n ne obrashaetsa \n\n");
				MPI_Abort(MPI_COMM_WORLD, -1);
				return -1;
				
			}
		
	}
	
	endtime   = MPI_Wtime();
	
	if(k == 0)
	{	
		if((in = fopen(file_name, "r")) == NULL)
		{
			printf("ERROR: fail ne otkrilsya\n");
			return 0;
			}
	}
	
	if(enter_matrix(n, k, in, b, rank, size) == -1) 
	{
		return -1;
	}
	
	
	if(m == 0)
	{	
	
		nev = nevyazka(a, b, buff, n, rank, size);
		
		if(rank == 0)
		{
			printf("\n\nnevyazka = %10.3e\n\n", nev);
			printf("\n\n time = %.2f c \n\n", endtime-starttime);
		}
	}
	else if(m == -1)
	{
		if(rank == 0)
			printf("\n\n");
			
		print_matrix(a, n, n, n, rank, size, buff);
		
		if(rank == 0)
			printf("\n\n");
	}
	else
	{
	
		nev = nevyazka(a, b, buff, n, rank, size);
		
		if(rank == 0)
			printf("\n\na =\n\n");
			
		print_matrix(b, n, n, m, rank, size, buff);
		
		if(rank == 0)
			printf("\n\na^(-1) =\n\n");
			
		print_matrix(a, n, n, m, rank, size, buff);
		
		if(rank == 0)
		{
			printf("\n\nnevyazka = %10.3e\n\n", nev);
			
			printf("\n\n time = %.2f c \n\n", endtime-starttime);
		}
	}
	
	/*if(rank == 0)
	{
		printf("\n a^(-1): \n");
	}
	
	print_matrix(a, n, n, m, rank, size, buff);
	
	if(enter_matrix(n, k, in, b, rank, size) == -1) 
	{
		return -1;
	}
	
	double nev;
	nev = nevyazka(a, b, buff, n, rank, size);
	
	if(rank == 0)
	{
		printf("\n nevyazka = %10.2e \n", nev);
		printf("\nThat took %.2f seconds\n", endtime-starttime);
	} */
	
	
	free(a);
	free(b);
	free(x);
	free(buff);
	
	MPI_Finalize();
	
	return 0;
	
}
