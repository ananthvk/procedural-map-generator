#include "chunk.hpp"
#include "noise.hpp"

class InitializationLayer : public InPlaceLayer
{
    int width, height, master_seed;

  public:
    InitializationLayer(int width, int height, int master_seed)
        : width(width), height(height), master_seed(master_seed)
    {
    }

    auto execute(Chunk &chunk) const -> void
    {
        chunk.width = width;
        chunk.height = height;
        chunk.master_seed = master_seed;
        chunk.elevation = std::vector<float>(width * height, 0.5f);
    }
};

class TerrainGenerationLayer : public InPlaceLayer
{
  public:
    auto execute(Chunk &chunk) const -> void
    {
        static NoiseGenerator generator;
        // Modify the noise generator seed slightly
        generator.set_seed(chunk.master_seed + 1);

        int idx = 0;

        for (int y = 0; y < chunk.height; ++y)
        {
            for (int x = 0; x < chunk.width; ++x)
            {
                float nx = x; // static_cast<float>(x) / map.width - 0.5;
                float ny = y; // static_cast<float>(y) / map.height - 0.5;
                chunk.elevation[idx++] = generator.at(nx, ny);
            }
        }
    }
};

auto ChunkFactory::add_layer(std::unique_ptr<Layer> layer) -> void
{
    layers.push_back(std::move(layer));
}

ChunkFactory::ChunkFactory()
{
    layers.push_back(std::make_unique<InitializationLayer>(800, 800, 512));
    layers.push_back(std::make_unique<TerrainGenerationLayer>());
}

auto ChunkFactory::execute() const -> Chunk
{
    Chunk chunk;
    for (const auto &layer : layers)
    {
        if (layer->type() == LayerType::INPLACE)
            static_cast<InPlaceLayer *>(layer.get())->execute(chunk);
        else
            chunk = static_cast<OutPlaceLayer *>(layer.get())->execute(chunk);
    }
    return chunk;
}
