
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char *argv[]) {
    double base, exponent;

    if (argc == 3) {
        base = atof(argv[1]);
        exponent = atof(argv[2]);
    } else {
        printf("Enter the base: ");
        fflush(stdout);

        if (scanf("%lf", &base) != 1) {
            fprintf(stderr, "Invalid number.\n");
            return 1;
        }

        printf("Enter the exponent: ");
        fflush(stdout);

        if (scanf("%lf", &exponent) != 1) {
            fprintf(stderr, "Invalid number.\n");
            return 1;
        }
    }

    printf("%g ^ %g = %g\n", base, exponent, base * exponent);
    return 0;
}
