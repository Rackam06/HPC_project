#ifndef BAT_H
#define BAT_H

#define dimension 2

typedef struct {
    double x_i[dimension];
    double v_i[dimension];
    double f_i;
    double A_i;
    double r_i;
    double f_value;
} Bat;

#endif
