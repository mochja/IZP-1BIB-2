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
#include <stdbool.h>
#include <string.h>

typedef struct {
	bool show_help;
	bool calculate_tan;
	unsigned int tan_range_from, tan_range_to;
	bool calculate_distances;
	bool set_height;
	double alpha, beta, height;
} params_t;

double taylor_tan(double x, unsigned int n);
double cfrac_tan(double x, unsigned int n);

double absd(double x);

int parse_args(int argc, char **argv, params_t *parameters);

int parse_int(char *str, bool *success)
{
	char *pEnd;
	errno = 0;

	long int num = strtol(str, &pEnd, 10);
	if ((pEnd != str && *pEnd != '\0') || (pEnd == str) || errno == ERANGE) {
		*success = false;
	}

	return (int)num;
}

double parse_double(char *str, bool *success)
{
	char *pEnd;
	errno = 0;

	double num = strtod(str, &pEnd, 10);
	if ((pEnd != str && *pEnd != '\0') || (pEnd == str) || errno == ERANGE) {
		*success = false;
	}

	return (double)num;
}

int main(int argc, char **argv)
{
	// set default parameters
	params_t params;
	params.show_help = false;
	params.calculate_distances = false;
	params.calculate_tan = false;
	params.set_height = false;
	params.height = 1.5;

	int result = parse_args(argc, argv, &params); // -1 err, 0 skip, 1 ok
	
	if (result == -1) {
		// handle args parsing err
	}
	else if (result == 0 || params.show_help) { // parsing skipped - show help
		// show_help();
	}

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

	for (unsigned int i = n - 1; i > 0; i--)
	{
		k -= 2;
		res = 1 / (k / x - res);
	}

	return res;
}

// Parameters parsing
int parse_args(int argc, char **argv, params_t *parameters)
{
	for (int i = 1; i < argc; i++)
	{
		const char *current_param = argv[i];

		bool didit = true;
		if (parameters->calculate_tan && (argc - i) == 3) {
			parameters->alpha = parse_double(argv[i], &didit);
			parameters->tan_range_from = parse_int(argv[i + 1], &didit);
			parameters->tan_range_to = parse_int(argv[i + 2], &didit);
			return didit ? 1 : -1;
		}
		else if (parameters->set_height && (argc - i) > 1) {
			parameters->set_height = false;
			parameters->height = parse_double(argv[i], &didit);
			if (!didit) return -1;
			continue;
		}
		else if (parameters->calculate_distances && (argc - i) >= 1 && (argc - i) <= 2) {
			parameters->alpha = parse_double(argv[i], &didit);
			if (i < argc - 1) {
				parameters->beta = parse_double(argv[i + 1], &didit);
			}
			return didit ? 1 : -1;
		}

		if (strcmp("--help", current_param) == 0) {
			parameters->show_help = true;
			return 1;
		}
		else if (strcmp("--tan", current_param) == 0) {
			parameters->calculate_tan = true;
		}
		else if (strcmp("-c", current_param) == 0) {
			parameters->set_height = true;
		}
		else if (strcmp("-m", current_param) == 0) {
			parameters->calculate_distances = true;
		}
	}
	return 0;
}