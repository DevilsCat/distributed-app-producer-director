// Calc.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

double expectation(double x[]) {
    double sum = 0;
    for (int i = 0; i < sizeof(x); ++i) {
        sum += x[i];
    }
    return sum / sizeof(x);
}

double pearson_correlation(double x[], double y[]) {
    double exp_x = expectation(x);
    double exp_y = expectation(y);

    double sum_a = 0;
    for (int i = 0; i < sizeof(x); ++i) {
        sum_a += (x[i] - exp_x) * (y[i] - exp_y);
    }

    double sum_b = 0;


}

int main(int argc, char* argv[])
{
	return 0;
}

