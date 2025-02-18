#ifndef A_CHUNK_H
#define A_CHUNK_H
#include "confparse.hpp"
#include <memory>
#include <vector>

// Whittaker classification
// https://en.wikipedia.org/wiki/Biome
enum class Biome
{
    // Terrestrial biomes
    ICE_DESERT,
    TUNDRA,
    BOREAL_FOREST,
    SHRUBLAND,
    TEMPERATE_GRASSLAND,
    TROPICAL_DESERT,
    SAVANNA,
    TEMPERATE_FOREST,
    TEMPERATE_RAINFOREST,
    TROPICAL_RAINFOREST,
    MOUNTAIN,

    // Aquatic biomes
    BEACH,
    LAKE,
    RIVER,
    SHALLOW_OCEAN,
    DEEP_OCEAN,
};

struct Chunk
{
    int width;
    int master_seed;
    int height;
    std::vector<float> elevation;
    std::vector<float> moisture;
    std::vector<Biome> biome;
};

enum class LayerType
{
    INPLACE,
    OUTPLACE
};

class Layer
{
  public:
    virtual auto type() const -> LayerType = 0;

    virtual ~Layer() {}
};

class InPlaceLayer : public Layer
{
  public:
    virtual auto execute(Chunk &chunk) const -> void = 0;

    auto type() const -> LayerType { return LayerType::INPLACE; }

    virtual ~InPlaceLayer() {}
};

class OutPlaceLayer : public Layer
{
  public:
    virtual auto execute(const Chunk &chunk) const -> Chunk = 0;

    auto type() const -> LayerType { return LayerType::OUTPLACE; }

    virtual ~OutPlaceLayer() {}
};

class ChunkFactory
{
    std::vector<std::unique_ptr<Layer>> layers;

  public:
    auto from_config(const confparse::Config &cfg) -> void;

    auto add_layer(std::unique_ptr<Layer> layer) -> void;

    auto execute() const -> Chunk;
};
#endif // A_CHUNK_H