#include "utils.h"
#include <cstdlib>

namespace utils {

double rangeRandom(double min, double max) {
    return (double) rand() / RAND_MAX * (max - min) + min;
}

}