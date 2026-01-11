#include <math.h>
#include <stdlib.h>
#include "bat.h"
#include "bat_utils.h"

#define PI 3.14

double uniform_random(double a, double b) {
    double u = (double) rand() / (double) RAND_MAX;
    return a + (b - a) * u;
}

// double objective_function(const double point[dimension]) {
//     double sum_sq = 0.0;
//     for (int i = 0; i < dimension; i++) {
//         sum_sq += point[i] * point[i];
//     }
//     return exp(-sum_sq);
// }

double objective_function(const double x[]) {
    double sum_sq = 0.0;
    for (int d = 0; d < dimension; d++) {
        sum_sq += x[d] * x[d];
    }
    return 10.0 - sum_sq;
}

// Gaussian N(mean, stddev) using Box-Muller
double normal_random(double mean, double stddev) {
    double u1 = uniform_random(0.0, 1.0);
    double u2 = uniform_random(0.0, 1.0);
    double z0 = sqrt(-2.0 * log(u1)) * cos(2.0 * PI * u2);
    return mean + stddev * z0;
}