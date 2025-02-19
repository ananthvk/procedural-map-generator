#include "noise.hpp"
#include "logger.h"
using namespace logger;
#include <time.h>

NoiseGenerator::NoiseGenerator()
{
    seed_ = static_cast<int>(time(NULL));
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
    /*
    noise.SetFrequency(0.01f);
    noise.SetFractalLacunarity(2.0f);
    noise.SetFractalGain(0.5f);
    noise.SetFractalType(FastNoiseLite::FractalType_FBm);
    */
    noise.SetSeed(seed_);
}

NoiseGenerator::NoiseGenerator(int seed)
{
    seed_ = seed;
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
    noise.SetSeed(seed_);
}

auto NoiseGenerator::seed() const -> int { return seed_; }

auto NoiseGenerator::set_seed(int seed) -> void
{
    seed_ = seed;
    noise.SetSeed(seed_);
}

auto NoiseGenerator::at(float x, float y) const -> float
{
    return noise.GetNoise(x, y) / 2.0f + 0.5f;
}

NoiseMap::NoiseMap(const std::vector<float> &frequencies, const std::vector<float> &amplitudes,
                   NoiseGenerator base_generator, float fudge, float redistribution)
    : frequencies(frequencies), amplitudes(amplitudes), base_generator(base_generator),
      fudge(fudge), redistribution(redistribution)
{
    if (frequencies.size() != amplitudes.size())
        throw std::logic_error("Frequencies vector size does not match amplitude size");

    for (int i = 0; i < frequencies.size(); ++i)
    {
        // To make sure that each noise layer added is different, add a value to the seed when
        // creating the noise generator
        NoiseGenerator copy_generator = base_generator;
        copy_generator.set_seed(base_generator.seed() + i * 7 + i / 2 + 1331);
        generators.push_back(copy_generator);
    }
}

auto NoiseMap::create_noise_map(float offset_x, float offset_y, int width, int height, float scale,
                                std::vector<float> &noise_map) const -> void
{
    int idx = 0;

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            float nx = scale * (offset_x + static_cast<float>(x) / width - 0.5f);
            float ny = scale * (offset_y + static_cast<float>(y) / height - 0.5f);
            float value = 0;
            float amplitude_sum = 0;
            for (int i = 0; i < frequencies.size(); ++i)
            {
                value += amplitudes[i] * generators[i].at(frequencies[i] * nx, frequencies[i] * ny);
                amplitude_sum += amplitudes[i];
            }

            value /= amplitude_sum;
            value = std::powf(value * fudge, redistribution);
            noise_map[idx++] = value;
        }
    }
}