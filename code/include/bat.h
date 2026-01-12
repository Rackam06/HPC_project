#ifndef BAT_H
#define BAT_H

#define dimension 2
#define N_BATS     30
#define MAX_ITERS  10000

#define F_MIN      0.0
#define F_MAX      1.0

#define A0         1.0    // initial loudness
#define R0         1.0    // initial pulse rate
#define V0         0.0

#define ALPHA      0.97
#define GAMMA      0.1

#define Ub         5
#define Lb         -5

typedef struct {
    double x_i[dimension];
    double v_i[dimension];
    double f_i;
    double A_i;
    double r_i;
    double f_value;
} Bat;

/* Core Bat Algorithm functions (implemented in src/sequential.c).
 * We declare them here so every version (sequential / OpenMP / MPI) can call them.
 */
void initialize_bats(Bat bats[], Bat *best_bat);
void update_bat(Bat bats[], Bat *best_bat, int i, int t);

#endif
