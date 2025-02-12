#include "noise.hpp"
#include "logger.h"
using namespace logger;
#include <time.h>

NoiseGenerator::NoiseGenerator()
{
    seed_ = static_cast<int>(time(NULL));
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
    noise.SetSeed(seed_);
}

NoiseGenerator::NoiseGenerator(int seed)
{
    seed_ = seed;
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
    noise.SetSeed(seed_);
}

auto NoiseGenerator::seed() -> int { return seed_; }

auto NoiseGenerator::set_seed(int seed) -> void
{
    seed_ = seed;
    noise.SetSeed(seed_);
}

auto NoiseGenerator::at(float x, float y) -> float
{
    return noise.GetNoise(x, y) / 2.0f + 0.5f;
}