#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "bat.h"
#include "bat_utils.h"

// ------- FONCTION SNAPSHOT ------------------------------------
void save_snapshot(const char *filename, Bat bats[]) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("fopen snapshot");
        return;
    }

    for (int i = 0; i < N_BATS; i++) {
        for (int d = 0; d < dimension; d++) {
            fprintf(fp, (d == 0) ? "%f" : ",%f", bats[i].x_i[d]);
        }
        fprintf(fp, "\n");
    }

    fclose(fp);
}

// -----------------------------------------------------------------

int main(void) {
    srand((unsigned int) time(NULL));

    Bat bats[N_BATS];
    Bat best_bat;

    initialize_bats(bats, &best_bat);

    for (int t = 0; t < MAX_ITERS; t++) {

        for (int i = 0; i < N_BATS; i++) {
            update_bat(bats, &best_bat, i, t);
        }

        /* snapshots aux itérations choisies */
        if (t == 0) {
            save_snapshot("snapshot_t000.csv", bats);
        } else if (t == 2500) {
            save_snapshot("snapshot_t250.csv", bats);
        } else if (t == 5000) {
            save_snapshot("snapshot_t500.csv", bats);
        } else if (t == 7500) {
            save_snapshot("snapshot_t750.csv", bats);
        }


        // afficher toutes les 100 itérations
        if (t % 100 == 0) {
            printf("[Iteration %d] Best f_value = %f  Position = (", t, best_bat.f_value);
            for (int d = 0; d < dimension; d++) {
                printf("%s%f", (d == 0 ? "" : ", "), best_bat.x_i[d]);
            }
            printf(")\n");
        }
    }


    printf("Final best f_value = %f\n", best_bat.f_value);
    printf("Final position = (");
    for (int d = 0; d < dimension; d++) {
        printf("%s%f", (d == 0 ? "" : ", "), best_bat.x_i[d]);
    }
    printf(")\n");

    return 0;
}
