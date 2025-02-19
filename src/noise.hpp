#ifndef A_NOISE_H
#define A_NOISE_H

#include "FastNoiseLite.h"
#include <vector>

class NoiseGenerator
{
    int seed_;
    FastNoiseLite noise;

  public:
    NoiseGenerator();
    NoiseGenerator(int seed);
    auto seed() const -> int;
    auto set_seed(int seed) -> void;
    auto at(float x, float y) const -> float;
};

// Creates a 2D noise map in the given vector, Note: noise_map must have size atleast equal to width
// * height
class NoiseMap
{
    std::vector<float> frequencies;
    std::vector<float> amplitudes;
    NoiseGenerator base_generator;
    std::vector<NoiseGenerator> generators;
    float fudge, redistribution;

  public:
    NoiseMap() {}

    NoiseMap(const std::vector<float> &frequencies, const std::vector<float> &amplitudes,
             NoiseGenerator base_generator, float fudge, float redistribution);

    auto create_noise_map(float offset_x, float offset_y, int width, int height, float scale,
                          std::vector<float> &noise_map) const -> void;
};

#endif // A_NOISE_H