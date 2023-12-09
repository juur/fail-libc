#define _XOPEN_SOURCE 700

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <errno.h>
#include <strings.h>

double mysin(double x)
{
	if (isnan(x))
		return NAN;

	if (isinf(x)) {
		errno = EDOM;
		return NAN;
	}

	x = fmod(x,  (2 * M_PI));

	while (x < 0)
		x += 2 * M_PI;

	while (x > 2 * M_PI)
		x -= 2 * M_PI;

	double epsilon = 0.1e-16;
	double sinus = 0.0;
	double sign = 1.0;
	double term = x;
	double n = 1;

	while (term > epsilon) {
		sinus += sign * term;
		sign = -sign;
		term *= x * x / (n+1) / (n+2);
		n += 2;
	}

	return sinus;
}

static inline double fac(int a)
{
	double ret = 1;
	for (int i = 1; i <= a; i++)
		ret *= i;
	return ret;
}

double mycos(double x)
{
	x = fmod(x,  (2 * M_PI));

	while (x < 0)
		x += 2 * M_PI;

	while (x > 2 * M_PI)
		x -= 2 * M_PI;

	double epsilon = 0.1e-16;
	double cosus = 1.0;
	double sign = 1.0;
	double term = 0;
	double n = 2;

	do {
		cosus += sign * term;
		sign = -sign;
		term = pow(x, n) / fac(n);
		n += 2;
	} while (term > epsilon);

	return cosus;
}

int main(int ac, char *av[])
{
	if (ac != 3)
		exit(EXIT_FAILURE);

	double x = 0.0;
	char *end;

	errno = 0;
	x = strtod(av[2], &end);

	if (x == 0 && errno)
		err(EXIT_FAILURE, "strtof");

	struct {
		const char *label;
		double (*func)(double);
		double (*my_func)(double);
	} funcs[] = {
		{"sin", sin, mysin},
		{"cos", cos, mycos},

		{NULL, NULL, NULL}
	};

	for (int i = 0; funcs[i].label; i++) {
		if (!strcasecmp(funcs[i].label, av[1])) {
			printf("   %8s(%e) = %e\n", funcs[i].label, x, funcs[i].func(x));
			printf("my %8s(%e) = %e\n", funcs[i].label, x, funcs[i].my_func(x));
			exit(EXIT_SUCCESS);
		}
	}

	errno = EINVAL;
	err(EXIT_FAILURE, "%s", av[1]);
}
