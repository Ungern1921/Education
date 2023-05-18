#include <stdio.h>
#include <stdlib.h>
#include "interpolation.h"

int main(int argc, char** argv)
{
	interpolation_ctx	ctx;
	int method, n, k;
	double a, b;
	
	method = atoi(argv[1]); //method of interpolation
	n = atoi(argv[2]); // split points
	k = atoi(argv[3]); // number of formula
	a = atof(argv[4]); 
	b = atof(argv[5]); 
	// [a; b] - interpolation segment

	ctx = interpolation_create(method,
				   n	, k, a, b);
	//printf("hello1");
    
	//printf("val: %lf\n\n ", interpolation_calculate(ctx, 2.3));
    
    printf("norma: %e\n\n ", norma(ctx, 1000, a, b));
	//printf("hello");
	interpolation_destroy(ctx);

	return 0;
}
