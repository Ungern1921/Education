#include <stdlib.h>
#include <stdio.h>
#include "algorithm.h"
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






static void Multiply_Ax(double* X, double* Y, double* Z) {
	//Ax * F у Aх верхние две строчки это строчки E
	int i, j, k;
	double q;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 2; j++) {
			Z[i + 4 * j] = Y[i + 4 * j];
		}
	}
	for (i = 0; i < 4; i++) {
		for (j = 2; j < 4; j++) {
			q=0;
			for (k = 0; k < 4; k++) {
				q = q + X[k + j * 4] * Y[i + k * 4];
			}
			Z[i + 4 * j] = q;
		}
	}
}

static void Multiply_transpose_Ay(double* X, double* Y, double* Z) {
	// F * Ay^T - 
	int i, j, k;
	double q;
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 4; j++) {
			Z[i + 4 * j] = X[i + 4 * j];
		}
	}
	for (i = 2; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			q=0;
			for (k = 0; k < 4; k++) {
				q = q + X[k + j * 4] * Y[k + i * 4];
			}
			Z[i + 4 * j] = q;
		}
	}
}



static void Fill_F(interpolation2_ctx res_ptr, double* F, int i, int j, double step_x, double step_y) {
	double x = res_ptr->x_a, y = res_ptr->y_a;


	F[0] = res_ptr->f(x + i * step_x, y + j * step_y);
	F[2] = res_ptr->f(x + i * step_x, y + (j + 1) * step_y);
	F[0 + 4 * 2] = res_ptr->f(x + (i + 1) * step_x, y + j * step_y);
	F[2 + 4 * 2] = res_ptr->f(x + (i + 1) * step_x, y + (j + 1) * step_y);

	F[1] = res_ptr->dyf(x + i * step_x, y + j * step_y);
	F[3] = res_ptr->dyf(x + i * step_x, y + (j + 1) * step_y);
	F[1 + 4 * 2] = res_ptr->dyf(x + (i + 1) * step_x, y + j * step_y);
	F[3 + 4 * 2] = res_ptr->dyf(x + (i + 1) * step_x, y + (j + 1) * step_y);

	F[0 + 4 * 1] = res_ptr->dxf(x + i * step_x, y + j * step_y);
	F[2 + 4 * 1] = res_ptr->dxf(x + i * step_x, y + (j + 1) * step_y);
	F[0 + 4 * 3] = res_ptr->dxf(x + (i + 1) * step_x, y + j * step_y);
	F[2 + 4 * 3] = res_ptr->dxf(x + (i + 1) * step_x, y + (j + 1) * step_y);

	F[1 + 4 * 1] = res_ptr->dxyf(x + i * step_x, y + j * step_y);
	F[3 + 4 * 1] = res_ptr->dxyf(x + i * step_x, y + (j + 1) * step_y);
	F[1 + 4 * 3] = res_ptr->dxyf(x + (i + 1) * step_x, y + j * step_y);
	F[3 + 4 * 3] = res_ptr->dxyf(x + (i + 1) * step_x, y + (j + 1) * step_y);

}







static void gauss_version_two_BOOST(double* Arr, double* X,double *B, int n,int m) {
	int i,j;
	//pryamoy hod
	for (i = 0; i < n - 1; i++) {
		Arr[0 + i * 4] = Arr[0 + i * 4] / Arr[1 + i * 4];
		Arr[2 + i * 4] = Arr[2 + i * 4] / Arr[1 + i * 4];
		//Arr[3 + i * 4] = Arr[3 + i * 4] / Arr[1 + i * 4];
		for (j=0;j<m;j++){
			B[i*m+j] = B[i*m+j]/ Arr[1 + i * 4];
		}
		Arr[1 + i * 4] = 1;


		Arr[1 + (i + 1) * 4] = Arr[1 + (i + 1) * 4] - Arr[2 + i * 4] * Arr[(i + 1) * 4];
		//Arr[3 + (i + 1) * 4] = Arr[3 + (i + 1) * 4] - Arr[3 + i * 4] * Arr[(i + 1) * 4];
		for (j=0;j<m;j++){
			B[(i+1)*m+j] = B[(i+1)*m+j] - B[i*m+j] * Arr[(i + 1) * 4];
		}
	}

	//obratniy hod
	//d[n - 1] = Arr[3 + (n - 1) * 4] / Arr[1 + (n - 1) * 4];
	for (j=0;j<m;j++){
			X[(n - 1)*m+j] = B[(n - 1)*m+j]/ Arr[1 + (n - 1) * 4];
		}
	for (i = n-2; i >= 0; i--) {
		//d[i] = Arr[3 + (i) * 4] - Arr[2 + (i) * 4] * d[i + 1];
		for (j=0;j<m;j++){
			X[(i)*m+j] = B[(i)*m+j]- Arr[2 + (i) * 4] * X[(i+1)*m+j];
		}
	}
}


static void Fill_F_2(interpolation2_ctx res_ptr, double* F, int i, int j) {
	int n_y = res_ptr->n_y,n_x = res_ptr->n_x;
	
	
	
	F[0] = res_ptr->F[i * (n_y + 1) + j];
	F[2] = res_ptr->F[i * (n_y + 1) + j + 1];
	F[0 + 4 * 2] = res_ptr->F[(i + 1) * (n_y + 1) + j];
	F[2 + 4 * 2] = res_ptr->F[(i + 1) * (n_y + 1) + j + 1];

	F[1] = res_ptr->Fy[j * (n_x + 1) + i];
	F[3] = res_ptr->Fy[j * (n_x + 1) + i + 1];
	F[1 + 4 * 2] = res_ptr->Fy[(j + 1) * (n_x + 1) + i];
	F[3 + 4 * 2] = res_ptr->Fy[(j + 1) * (n_x + 1) + i + 1];

	F[0 + 4 * 1] = res_ptr->Fx[i * (n_y + 1) + j];
	F[2 + 4 * 1] = res_ptr->Fx[i * (n_y + 1) + j + 1];
	F[0 + 4 * 3] = res_ptr->Fx[(i + 1) * (n_y + 1) + j];
	F[2 + 4 * 3] = res_ptr->Fx[(i + 1) * (n_y + 1) + j + 1];

	F[1 + 4 * 1] = res_ptr->Fxy[j * (n_x + 1) + i];
	F[3 + 4 * 1] = res_ptr->Fxy[j * (n_x + 1) + i + 1];
	F[1 + 4 * 3] = res_ptr->Fxy[(j + 1) * (n_x + 1) + i];
	F[3 + 4 * 3] = res_ptr->Fxy[(j + 1) * (n_x + 1) + i + 1];

}

static void Fill_Arr(double* Arr, int n) {
	int i;
	Arr[0] = 0.0;
	Arr[1] = 1.0;
	Arr[2] = 2.0;
	for (i = 1; i < n - 1; i++) {
		Arr[0 + i * 4] = 1.0;
		Arr[1 + i * 4] = 4.0;
		Arr[2 + i * 4] = 1.0;
	}
	Arr[0 + (n - 1) * 4] = 2.0;
	Arr[1 + (n - 1) * 4] = 1.0;
	Arr[2 + (n - 1) * 4] = 0.0;
}





static void BxA(double *A,double *Z,int n,int m, double step){
	//B- matrix n x n         A - matrix n x m
	int i, j;
	for (j = 0; j < m; j++) {
			Z[j] = (-2.5*A[j+(0)*m]+2.0*A[j+(1)*m]+0.5*A[j+(2)*m])/step;
	}
	for (i = 1; i < n-1; i++) {
		for (j = 0; j < m; j++) {
			Z[j + m * i] = (A[j+(i+1)*m]+A[j+(i-1)*m])*3/step;
		}
	}
	for (j = 0; j < m; j++) {
			Z[j + m * (n-1)] = -(-2.5*A[j+(n-1)*m]+2.0*A[j+(n-2)*m]+0.5*A[j+(n-3)*m])/step;
	}
	
}

static void BxA_Transpose(double *A,double *Z,int n,int m, double step){
	//B- matrix n x n         A - matrix m x n
	int i, j;
	for (j = 0; j < m; j++) {
			Z[j] = (-2.5*A[0+(j)*n]+2.0*A[1+(j)*n]+0.5*A[2+(j)*n])/step;
	}
	for (i = 1; i < n-1; i++) {
		for (j = 0; j < m; j++) {
			Z[j + m * i] = (A[j*n+(i+1)]+A[j*n+(i-1)])*3/step;
		}
	}
	for (j = 0; j < m; j++) {
			Z[j + m * (n-1)] = -(-2.5*A[j*n+(n-1)]+2.0*A[j*n+(n-2)]+0.5*A[j*n+(n-3)])/step;
	}
	
}


static void Fill_Fs(interpolation2_ctx res_ptr, double step_x, double step_y) {
	double x_a = res_ptr->x_a, y_a = res_ptr->y_a;
	int n_x = res_ptr->n_x, n_y = res_ptr->n_y, i, j,LL;

	double* F = (double*)malloc(sizeof(double) * (n_x + 1) * (n_y + 1));
	double* Fx = (double*)malloc(sizeof(double) * (n_x + 1) * (n_y + 1));
	double* Fy = (double*)malloc(sizeof(double) * (n_x + 1) * (n_y + 1));
	double* Fxy = (double*)malloc(sizeof(double) * (n_x + 1) * (n_y + 1));
	if (!(F = (double*)malloc(sizeof(double) * (n_x + 1) * (n_y + 1)))) {
		printf("error_F");
	}
	if (!(Fx = (double*)malloc(sizeof(double) * (n_x + 1) * (n_y + 1)))) {
		printf("error_Fx");
	}
	if (!(Fy = (double*)malloc(sizeof(double) * (n_x + 1) * (n_y + 1)))) {
		printf("error_Fy");
	}
	if (!(Fxy = (double*)malloc(sizeof(double) * (n_x + 1) * (n_y + 1)))) {
		printf("error_Fxy");
	}
	double* B;
	double* Arr = (double*)malloc(sizeof(double) * 4 * (n_y + n_x + 1));
	if(n_x>n_y){
		LL=n_x;
	} else{
		LL=n_y;
	}

	if (!(B = (double*)malloc(sizeof(double) * (LL + 1) * (LL + 1)))) {
		printf("error_B");
	}
	if (!(Arr = (double*)malloc(sizeof(double) * 4 * (n_y + n_x + 1)))) {
		printf("error_Arr");
	}



	for (i = 0; i <= n_x; i++) {
		for (j = 0; j <= n_y; j++) {
			F[j + i * (n_y + 1)] = res_ptr->f(x_a + i * step_x, y_a + j * step_y);
			//printf("F:  %f    ", F[j + i * (n_y + 1)]);
		}
		//printf("\n");
	}
	
	//compute Gx and Gy 
	BxA(F,B, n_x+1, n_y+1,step_x);
	Fill_Arr(Arr, n_x + 1);
	gauss_version_two_BOOST(Arr, Fx,B, n_x + 1, n_y + 1);
	//printf("Ax:\n");
	//print_matrix(A, n_x + 1, n_x + 1);
	
	//Transpose(Gx, n_x + 1);
	BxA_Transpose(F,B, n_y+1, n_x+1,step_y);
	Fill_Arr(Arr, n_y + 1);;
	gauss_version_two_BOOST(Arr, Fy,B, n_y + 1, n_x + 1); // we get Fy^T
	

	//Transpose(A, n_y + 1);

	// Fx = Gx * F    Fy = F * Gy^T     Fxy = Gx * F * Gy^T = Fx * Gy^T
	BxA_Transpose(Fx,B, n_y+1, n_x+1,step_y);
	Fill_Arr(Arr, n_y + 1);;
	gauss_version_two_BOOST(Arr, Fxy,B, n_y + 1, n_x + 1); //we get Fxy^T


	//Multiply_2(Gx, F, Fx, n_x + 1, n_y + 1);
	//Multiply_1(F, Gy, Fy, n_x + 1, n_y + 1);
	//Multiply_1(Fx, Gy, Fxy, n_x + 1, n_y + 1);


	//print_matrix(F, n_x + 1, n_y + 1);
	res_ptr->F = F;
	res_ptr->Fx = Fx;
	res_ptr->Fy = Fy;
	res_ptr->Fxy = Fxy;


	free(Arr);
	free(B);
}







//Arr - это по итогу коэффициенты гамма ij их будет 16*n_x*n_y
void count_Arr_1(interpolation2_ctx res_ptr) {
	int i, j;
	int	n_x = res_ptr->n_x, n_y = res_ptr->n_y;
	double x_a = res_ptr->x_a, x_b = res_ptr->x_b, y_a = res_ptr->y_a, y_b = res_ptr->y_b;
	double* Arr = (double*)malloc(sizeof(double) * 16 * n_x * n_y);
	double step_x = (x_b - x_a) / n_x, step_y = (y_b - y_a) / n_y;
	double* Ax = (double*)malloc(sizeof(double) * 16);
	double* Ay = (double*)malloc(sizeof(double) * 16);
	double* F = (double*)malloc(sizeof(double) * 16);
	double* vrem = (double*)malloc(sizeof(double) * 16);

	res_ptr->step_x = step_x;
	res_ptr->step_y = step_y;

	for (i = 0; i < 2; i++) {
		for (j = 0; j < 4; j++) {
			if (i == j) {
				Ax[4 * i + j] = 1;
				Ay[4 * i + j] = 1;
			}
			else {
				Ax[4 * i + j] = 0;
				Ay[4 * i + j] = 0;
			}

		}
	}
	Ax[4 * 2 + 0] = -3.0 / (step_x * step_x);
	Ay[4 * 2 + 0] = -3.0 / (step_y * step_y);

	Ax[4 * 2 + 1] = -2.0 / step_x;
	Ay[4 * 2 + 1] = -2.0 / step_y;

	Ax[4 * 2 + 2] = 3.0 / (step_x * step_x);
	Ay[4 * 2 + 2] = 3.0 / (step_y * step_y);

	Ax[4 * 2 + 3] = -1.0 / step_x;
	Ay[4 * 2 + 3] = -1.0 / step_y;

	Ax[4 * 3 + 0] = 2.0 / (step_x * step_x * step_x);
	Ay[4 * 3 + 0] = 2.0 / (step_y * step_y * step_y);

	Ax[4 * 3 + 1] = 1.0 / (step_x * step_x);
	Ay[4 * 3 + 1] = 1.0 / (step_y * step_y);

	Ax[4 * 3 + 2] = -2.0 / (step_x * step_x * step_x);
	Ay[4 * 3 + 2] = -2.0 / (step_y * step_y * step_y);

	Ax[4 * 3 + 3] = 1.0 / (step_x * step_x);
	Ay[4 * 3 + 3] = 1.0 / (step_y * step_y);

	if (res_ptr->method == 1) {
		for (i = 0; i < n_x; i++) {
			for (j = 0; j < n_y; j++) {
				Fill_F(res_ptr, F, i, j, step_x, step_y);

				Multiply_Ax(Ax, F, vrem);
				//Multiply(Ax, F, vrem, 4);

				Multiply_transpose_Ay(vrem, Ay, Arr + i * 16 * n_y + j * 16);
				//Multiply_transpose(vrem, Ay, Arr + i * 16 * n_y + j * 16, 4);
			}
		}
	}
	else {
		//second method
		Fill_Fs(res_ptr, step_x, step_y);
		/*printf("F:\n");
		print_matrix(res_ptr->F, n_x + 1, n_y + 1);
		printf("Fx:\n");
		print_matrix(res_ptr->Fx, n_x + 1, n_y + 1);
		printf("Fy:\n");
		print_matrix(res_ptr->Fy, n_x + 1, n_y + 1);
		printf("Fxy:\n");
		print_matrix(res_ptr->Fxy, n_x + 1, n_y + 1);*/
		for (i = 0; i < n_x; i++) {
			for (j = 0; j < n_y; j++) {
				Fill_F_2(res_ptr, F, i, j);
				
				Multiply_Ax(Ax, F, vrem);
				//Multiply(Ax, F, vrem, 4);
				
				Multiply_transpose_Ay(vrem, Ay, Arr + i * 16 * n_y + j * 16);
				//Multiply_transpose(vrem, Ay, Arr + i * 16 * n_y + j * 16, 4);
			}
		}

	}


	res_ptr->Arr = Arr;
	free(Ax);
	free(Ay);
	free(F);
	free(vrem);
}







static double power(double x, int q) {
	int i;
	double y = 1.0;
	for (i = 0; i < q; i++) {
		y = y * x;
	}
	return y;
}

static double Func(double* gamma, double x, double y) {
	//предполагаю что x=x-x_i y=y-y_i
	
	
	
	int i, j;
	double result = 0;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {

			result += gamma[i * 4 + j] * power(x, i) * power(y, j);
		}
	}
	return result;
}

double P(double x, double y, interpolation2_ctx res_ptr) {
	int i, j;
	double a, b;
	i = (int)((x - res_ptr->x_a) / res_ptr->step_x);
	j = (int)((y - res_ptr->y_a) / res_ptr->step_y);
	if (i == res_ptr->n_x) {
		i = i - 1;
	}
	if (j == res_ptr->n_y) {
		j = j - 1;
	}
	if (res_ptr->method == 1) {

		a = x - (res_ptr->x_a + i * res_ptr->step_x);
		b = y - (res_ptr->y_a + j * res_ptr->step_y);

		return Func(res_ptr->Arr + i * 16 * res_ptr->n_y + j * 16, a, b);

	}
	else {

		a = x - (res_ptr->x_a + i * res_ptr->step_x);
		b = y - (res_ptr->y_a + j * res_ptr->step_y);

		return Func(res_ptr->Arr + i * 16 * res_ptr->n_y + j * 16, a, b);

	}

}
