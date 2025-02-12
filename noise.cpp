#include "noise.hpp"
#define STB_PERLIN_IMPLEMENTATION
#include "stb_perlin.h"
#include <time.h>

NoiseGenerator::NoiseGenerator() { seed_ = static_cast<int>(time(NULL)); }

NoiseGenerator::NoiseGenerator(int seed) { seed_ = seed; }

auto NoiseGenerator::seed() -> int { return seed_; }

auto NoiseGenerator::set_seed(int seed) -> void { seed_ = seed; }

auto NoiseGenerator::at(float x, float y) -> float
{
    return stb_perlin_noise3_seed(x, y, 0, 0, 0, 0, seed_);
}

auto NoiseGenerator::at(float x, float y, float z) -> float
{
    return stb_perlin_noise3_seed(x, y, z, 0, 0, 0, seed_);
}