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
    float map_scale, redistribution, fudge;

  public:
    TerrainGenerationLayer(float map_scale, float redistribution, float fudge)
        : map_scale(map_scale), redistribution(redistribution), fudge(fudge)
    {
    }

    auto execute(Chunk &chunk) const -> void
    {
        static NoiseGenerator generator1;
        static NoiseGenerator generator2;
        static NoiseGenerator generator3;
        // Modify the noise generator seeds slightly

        generator1.set_seed(chunk.master_seed + 2025);
        generator2.set_seed(chunk.master_seed + 1337);
        generator3.set_seed(chunk.master_seed + 4040);

        int idx = 0;

        for (int y = 0; y < chunk.height; ++y)
        {
            for (int x = 0; x < chunk.width; ++x)
            {
                float nx = static_cast<float>(x) / chunk.width - 0.5;
                float ny = static_cast<float>(y) / chunk.height - 0.5;

                float elevation =
                    1.0f * generator1.at(1.0f * map_scale * nx, 1.0f * map_scale * ny);
                elevation += 0.5f * generator2.at(2.0f * map_scale * nx, 2.0f * map_scale * ny);
                elevation += 0.25f * generator3.at(4.0f * map_scale * nx, 4.0f * map_scale * ny);
                elevation = elevation / (1.0f + 0.5f + 0.25f);
                elevation = std::powf(elevation * fudge, redistribution);
                chunk.elevation[idx++] = elevation;
            }
        }
    }
};

auto ChunkFactory::add_layer(std::unique_ptr<Layer> layer) -> void
{
    layers.push_back(std::move(layer));
}

auto ChunkFactory::from_config(const confparse::Config &cfg) -> void
{
    layers.clear();

    int width = cfg.get("width").parse<int>();
    int height = cfg.get("width").parse<int>();
    int master_seed = cfg.get("seed").parse<int>();
    int map_scale = cfg.get("scale").parse<float>();
    int redistribution_exponent = cfg.get("redistribution").try_parse<float>(1.0f);
    int fudge = cfg.get("fudge").parse<float>();

    layers.push_back(std::make_unique<InitializationLayer>(width, height, master_seed));
    layers.push_back(std::make_unique<TerrainGenerationLayer>(map_scale, redistribution_exponent, fudge));
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
