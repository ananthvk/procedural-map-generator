#ifndef A_CHUNK_H
#define A_CHUNK_H
#include <memory>
#include <vector>

struct Chunk
{
    int width;
    int master_seed;
    int height;
    std::vector<float> elevation;
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
    ChunkFactory();

    auto add_layer(std::unique_ptr<Layer> layer) -> void;

    auto execute() const -> Chunk;
};
#endif // A_CHUNK_H