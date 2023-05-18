#include <assert.h>
#include <stdio.h>
#include <time.h>

#include "interpolation.h"

#define ASSERT_CM(val, msg) assert((#msg, val))

void run_tests(void);

int main()
{
	run_tests();

	return 0;
}

#define TEST_NEGATIVE_INIT(m, n, k, a, b) { \
		puts("    Wrong arguments test - m=" #m ", n=" #n ", k=" #k \
				     ", ab = [" #a "," #b "]"); \
		interpolation_ctx ctx = interpolation_create(m, n, k, a, b); \
		ASSERT_CM(!ctx, \
			  interpolation_create(m, n, k, a, b) must return 'NULL'); \
		puts("    Ok");}

#define RUN_TEST(name) { \
			puts("TEST start    : " #name); \
			test_##name(); \
			puts("TEST finished : " #name);}

void test_negative_init(void)
{
	/* wrong method */
	TEST_NEGATIVE_INIT(0, 5, 3, 0, 5)
	TEST_NEGATIVE_INIT(3, 5, 3, 0, 5)
	/* wrong n count */
	TEST_NEGATIVE_INIT(1, 2, 3, 0, 5)
	/* wrong func number */
	TEST_NEGATIVE_INIT(1, 5, -2, 0, 5)
	TEST_NEGATIVE_INIT(1, 5, 7, 0, 5)
	/* wrong [a, b] */
	TEST_NEGATIVE_INIT(1, 5, 3, 0, 0)
	TEST_NEGATIVE_INIT(1, 5, 3, 5, 0)
}

#define TEST_PERFORMANCE(m, n, k, a, b, req) { \
		puts("    Performance - m=" #m ", n=" #n ", k=" #k \
				     ", ab = [" #a "," #b "]; reqs=" #req); \
		struct timespec	tv1, tv2; \
		int tmp; double len = (b) - (a); \
		interpolation_ctx ctx = interpolation_create(m, n, k, a, b); \
		timespec_get(&tv1, TIME_UTC); \
		for (int i = 0; i < req; i++) \
			interpolation_calculate(ctx, (a) + len * i / (req)); \
		timespec_get(&tv2, TIME_UTC); \
		tmp = (tv2.tv_sec - tv1.tv_sec) * 1000 + tv2.tv_nsec / 1000000 - tv1.tv_nsec / 1000000; \
		printf("    Time: %d.%03d\n", tmp / 1000, tmp % 1000); \
		interpolation_destroy(ctx);}

void test_performance(void)
{
	/* 10^7 approximations */
	TEST_PERFORMANCE(1, 10000, 3, -100, 200, 10000000)
	/* 10^7 known points */
	TEST_PERFORMANCE(1, 10000000, 3, -100, 200, 1)
	/* 10^7 & 10^7 */
	TEST_PERFORMANCE(1, 10000000, 3, -100, 200, 10000000)
	TEST_PERFORMANCE(2, 10000, 5, -100, 200, 10000000)
	TEST_PERFORMANCE(2, 10000000, 5, -100, 200, 1)
	TEST_PERFORMANCE(2, 10000000, 5, -100, 200, 10000000)
}

void run_tests(void)
{
	RUN_TEST(negative_init)
	RUN_TEST(performance)
}
