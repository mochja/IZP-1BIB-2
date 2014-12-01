/*!
 * \file proj2.c
 *
 * \author Ján Mochňak, <xmochn00@stud.fit.vutbr.cz>
 * \date 11/19/2014
 *
 * For license information read LICENSE.txt
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#define MAX_ITERATIONS 14
#define MAX_ANGLE 1.4
#define MAX_HEIGHT 100
#define DEFAULT_ITERATIONS 10
#define DEFAULT_HEIGHT 1.5

#define ERR_MSG_BUFFER_SIZE 512

typedef struct {
	bool show_help;
	bool calculate_tan;
	unsigned int tan_range_from, tan_range_to;
	bool calculate_distances;
	bool set_height;
	double alpha, beta, height;
} params_t;

typedef enum { PARAMETERS_NOT_PARSED_CORRECTLY, NOTHING_TO_DO } merror_t;

int parse_args(int argc, char **argv, params_t *parameters);

int parse_int(char *str, bool *success);
double parse_double(char *str, bool *success);

double taylor_tan(double x, unsigned int n);
double cfrac_tan(double x, unsigned int n);

double absd(double x);

int show_error_and_halt(merror_t err_code);

double calculate_distance(double height, double alpha);
double calculate_height(double height, double beta, double distance);
void show_tan_diff_table(double alpha, unsigned int from, unsigned int to);

bool is_angle_valid(double x);
bool is_height_valid(double x);
bool is_range_valid(unsigned int from, unsigned int to);

void show_help();

int main(int argc, char **argv)
{
	// set default parameters
	params_t params;
	params.height = DEFAULT_HEIGHT;

	merror_t err;

	int result = parse_args(argc, argv, &params); // -1 err, 0 skip, 1 ok

	if (result < 1)
	{
		// handle args parsing err
		err = PARAMETERS_NOT_PARSED_CORRECTLY;
		return show_error_and_halt(err);
	}
	else if (params.show_help)
	{ // parsing skipped - show help
		show_help(argv[0]);
		return EXIT_SUCCESS;
	}

	if (params.calculate_tan)
	{
		show_tan_diff_table(params.alpha, params.tan_range_from, params.tan_range_to);
	}
	else if (params.calculate_distances)
	{
		double distance = calculate_distance(params.height, params.alpha);
		printf("%.10e\n", distance);

		if ( !isnan(params.beta) ) {
			double total_height = calculate_height(params.height, params.beta, distance);
			printf("%.10e\n", total_height);
		}
	}
	else {
		err = NOTHING_TO_DO;
		return show_error_and_halt(err);
	}

	return EXIT_SUCCESS;
}

double calculate_distance(double height, double alpha)
{
	return height / cfrac_tan(alpha, DEFAULT_ITERATIONS);
}

double calculate_height(double height, double beta, double distance)
{
	return height + cfrac_tan(beta, DEFAULT_ITERATIONS) * distance;
}

void show_tan_diff_table(double alpha, unsigned int from, unsigned int to)
{
	double math_tan_result = tan(alpha);

	for (unsigned int i = from; i <= to; i++)
	{
		double taylor_tan_result = taylor_tan(alpha, i);
		double cfrac_tan_result = cfrac_tan(alpha, i);
		printf("%d %e %e %e %e %e\n", i, math_tan_result, taylor_tan_result, absd(math_tan_result - taylor_tan_result), cfrac_tan_result, absd(math_tan_result - cfrac_tan_result));
	}
}

double taylor_tan(double x, unsigned int n)
{
	if (n >= MAX_ITERATIONS) return .0;

	static const uint64_t numerator[] = { 1, 1, 2, 17, 62, 1382, 21844, 929569, 6404582, 443861162, 18888466084, 113927491862, 58870668456604 };
	static const uint64_t denominator[] = { 1, 3, 15, 315, 2835, 155925, 6081075, 638512875, 10854718875, 1856156927625, 194896477400625, 49308808782358125, 3698160658676859375 };
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
	double result = 1 / (k / x);

	for (unsigned int i = n - 1; i > 0; i--)
	{
		k -= 2;
		result = 1 / (k / x - result);
	}

	return result;
}

int parse_args(int argc, char **argv, params_t *parameters)
{
	parameters->show_help = false;
	parameters->calculate_distances = false;
	parameters->calculate_tan = false;
	parameters->set_height = false;

	for (int i = 1; i < argc; i++)
	{
		const char *current_param = argv[i];

		bool didit = true;
		if (parameters->calculate_tan && (argc - i) == 3) {
			parameters->alpha = parse_double(argv[i], &didit);
			parameters->tan_range_from = parse_int(argv[i + 1], &didit);
			parameters->tan_range_to = parse_int(argv[i + 2], &didit);
			return (didit && is_range_valid(parameters->tan_range_from, parameters->tan_range_to) && is_angle_valid(parameters->alpha)) ? 1 : -1;
		}
		else if (parameters->set_height && (argc - i) > 1) {
			parameters->set_height = false;
			parameters->height = parse_double(argv[i], &didit);
			if (!didit || !is_height_valid(parameters->height)) return -1;
			continue;
		}
		else if (parameters->calculate_distances && (argc - i) >= 1 && (argc - i) <= 2) {
			parameters->alpha = parse_double(argv[i], &didit);
			if (i < argc - 1) {
				parameters->beta = parse_double(argv[i + 1], &didit);
				didit = didit ? is_angle_valid(parameters->beta) : false;
			} else {
				parameters->beta = NAN;
			}
			return (didit && is_angle_valid(parameters->alpha)) ? 1 : -1;
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

int show_error_and_halt(merror_t err_code)
{
	char errormsg[ERR_MSG_BUFFER_SIZE];
	switch (err_code) {
	case PARAMETERS_NOT_PARSED_CORRECTLY:
		strcpy(errormsg, "Parameters not parsed correctly.");
		break;
	case NOTHING_TO_DO:
		strcpy(errormsg, "There is nothing to do. See --help for program usage.");
		break;
	default:
		strcpy(errormsg, "Unexpected error. (Custom message, was NOT defined.)");
	}
	fprintf(stderr, "%s\n", errormsg);
	return EXIT_FAILURE;
}


// helper functions and validators

double absd(double x)
{
	return (x >= 0) ? x : x * -1.;
}

int parse_int(char *str, bool *success)
{
	char *pEnd;
	errno = 0;

	long int num = strtol(str, &pEnd, 10);
	if ((pEnd != str && *pEnd != '\0') || pEnd == str || errno == ERANGE) {
		*success = false;
	}

	return (int)num;
}

double parse_double(char *str, bool *success)
{
	char *pEnd;
	errno = 0;

	double num = strtod(str, &pEnd);
	if ((pEnd != str && *pEnd != '\0') || pEnd == str || errno == ERANGE) {
		*success = false;
	}

	return (double)num;
}

bool is_angle_valid(double x)
{
	return x > 0 && x <= MAX_ANGLE;
}

bool is_range_valid(unsigned int from, unsigned int to)
{
	return from <= to && ( from > 0 && from <= MAX_ITERATIONS ) && ( to > 0 && to <= MAX_ITERATIONS );
}

bool is_height_valid(double x)
{
	return x > 0 && x <= MAX_HEIGHT;
}

void show_help(char *exe_name)
{
	printf("IZP project 2; Created by <%s>\n\n"
		"Usage of %s\n"
		"\t--help\t\tPrints this help.\n"
		"\t--tan A N M\tCalculate and print tan(a) in range from N to M iterations.\n"
		"\t[-c C] -m A [B]\tCalculate distances from given parameters. C - height, A - alpha, B - beta\n", "xmochn00@stud.fit.vutbr.cz", exe_name);
}