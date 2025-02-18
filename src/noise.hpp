#ifndef A_NOISE_H
#define A_NOISE_H

#include "FastNoiseLite.h"

class NoiseGenerator
{
    int seed_;
    FastNoiseLite noise;

  public:
    NoiseGenerator();
    NoiseGenerator(int seed);
    auto seed() -> int;
    auto set_seed(int seed) -> void;
    auto at(float x, float y) -> float;
};
#endif // A_NOISE_H