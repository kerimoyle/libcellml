#include <math.h>

void initializeConstants(double *states, double *variables)
{
    variables[0] = 1.0;
}

void computeComputedConstants(double *variables)
{
    variables[1] = variables[0];
}

void computeRates(double voi, double *states, double *rates, double *variables)
{
}

void computeVariables(double voi, double *states, double *rates, double *variables)
{
}

void computeStateRateBasedVariables(double voi, double *states, double *rates, double *variables)
{
}
