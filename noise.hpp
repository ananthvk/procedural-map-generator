#ifndef A_NOISE_H
#define A_NOISE_H
#include "stb_perlin.h"

class NoiseGenerator
{
    int seed_;

  public:
    NoiseGenerator();
    NoiseGenerator(int seed);
    auto seed() -> int;
    auto set_seed(int seed) -> void;
    auto at(float x, float y) -> float;
    auto at(float x, float y, float z) -> float;
};
#endif // A_NOISE_H