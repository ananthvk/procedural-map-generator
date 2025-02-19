#ifndef A_CHUNK_H
#define A_CHUNK_H
#include "confparse.hpp"
#include "registries.hpp"
#include <memory>
#include <vector>

struct Chunk
{
    int width;
    int master_seed;
    int height;
    // Chunk coordinate or offset, identifies a chunk uniquely
    // The top left chunk is (0,0) then (0,1) and so on
    int x;
    int y;
    std::vector<float> elevation;
    std::vector<float> moisture;
    std::vector<int> biome;
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
    virtual auto execute(Chunk &chunk, Registry &registry) const -> void = 0;

    auto type() const -> LayerType { return LayerType::INPLACE; }

    virtual ~InPlaceLayer() {}
};

class OutPlaceLayer : public Layer
{
  public:
    virtual auto execute(const Chunk &chunk, Registry &registry) const -> Chunk = 0;

    auto type() const -> LayerType { return LayerType::OUTPLACE; }

    virtual ~OutPlaceLayer() {}
};

class ChunkFactory
{
    std::vector<std::unique_ptr<Layer>> layers;

  public:
    auto from_config(const confparse::Config &cfg) -> void;

    auto add_layer(std::unique_ptr<Layer> layer) -> void;

    auto execute(Registry &registry, int chunk_x, int chunk_y) const -> Chunk;

    auto execute_update(Registry &registry, int chunk_x, int chunk_y, Chunk &chunk) const -> void;
};
#endif // A_CHUNK_H