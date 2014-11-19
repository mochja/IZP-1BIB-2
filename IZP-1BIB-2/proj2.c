/*!
 * \file proj2.c
 *
 * \author Ján Mochňak <janmochnak@icloud.com>
 * \date 11/19/2014
 *
 * For license information read LICENSE.txt
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

double taylor_tan(double x, unsigned int n);
double cfrac_tan(double x, unsigned int n);

double absd(double x);

int main(void)
{
	double x = 1.024;
	double a = 0.3;
	double b = 0.9;
	double c = 1.5;
	
	double math_tan_result = tan(x);

	for (unsigned int i = 6; i < 11; i++)
	{
		double taylor_tan_result = taylor_tan(x, i);
		double cfrac_tan_result = cfrac_tan(x, i);
		printf("%d %e %e %e %e %e\n", i, math_tan_result, taylor_tan_result, absd(math_tan_result - taylor_tan_result), cfrac_tan_result, absd(math_tan_result - cfrac_tan_result));
	}

	// c/d = tan(a)
	double d = c / cfrac_tan(a, 10);
	printf("%.10e\n", d);

	// vyska = v1 + c; tan(b) = v1/d
	double v = c + cfrac_tan(b, 10) * d;
	printf("%.10e\n", v);

	a = 0.15;
	b = 1.3;
	c = 1.7;

	// c/d = tan(a)
	d = c / cfrac_tan(a, 10);
	printf("%.10e\n", d);

	// vyska = v1 + c; tan(b) = v1/d
	v = c + cfrac_tan(b, 10) * d;
	printf("%.10e\n", v);

	return 0;
}

double absd(double x) { return (x >= 0) ? x : x * -1.; }

double taylor_tan(double x, unsigned int n)
{
	if (n > 13) return .0;

	static const double numerator[] = { 1, 1, 2, 17, 62, 1382, 21844, 929569, 6404582, 443861162, 18888466084, 113927491862, 58870668456604 };
	static const double denominator[] = { 1, 3, 15, 315, 2835, 155925, 6081075, 638512875, 10854718875, 1856156927625, 194896477400625, 49308808782358125, 3698160658676859375 };
	const double pow2x = x * x;

	double lastTopX = x;
	double res = 0;
	for (unsigned int i = 0; i < n; i++)
	{
		res += (numerator[i] * lastTopX) / denominator[i];
		lastTopX *= pow2x;
	}

	return res;
}

double cfrac_tan(double x, unsigned int n)
{
	double k = n * 2 - 1;
	double res = 1 / (k / x);

	for (unsigned int i = n-1; i > 0; i--)
	{
		k -= 2;
		res = 1 / (k / x - res);
	}

	return res;
}