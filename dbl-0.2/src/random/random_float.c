#include <random/random.h>

#include <stdlib.h>

float random_float(float min, float max)
{
    return min + (float) rand() / RAND_MAX * (max - min);
}
