#include "chunk.hpp"
#include "noise.hpp"

#define TERRAIN_SEED_MAGIC_NUMBER 8021
#define MOISTURE_SEED_MAGIC_NUMBER 4712

class InitializationLayer : public InPlaceLayer
{
    int width, height, master_seed;

  public:
    InitializationLayer(int width, int height, int master_seed)
        : width(width), height(height), master_seed(master_seed)
    {
    }

    auto execute(Chunk &chunk, Registry &registry) const -> void
    {
        chunk.width = width;
        chunk.height = height;
        chunk.master_seed = master_seed;
        if (chunk.elevation.size() == static_cast<size_t>(width * height))
        {
            for (auto &e : chunk.elevation)
                e = 0.5f;
        }
        else
            chunk.elevation = std::vector<float>(width * height, 0.5f);

        if (chunk.moisture.size() == static_cast<size_t>(width * height))
        {
            for (auto &m : chunk.moisture)
                m = 0.5f;
        }
        else
            chunk.moisture = std::vector<float>(width * height, 0.5f);

        if (chunk.biome.size() == static_cast<size_t>(width * height))
        {
            for (auto &b : chunk.biome)
                b = -1;
        }
        else
            chunk.biome = std::vector<int>(width * height, -1);
    }
};

class TerrainGenerationLayer : public InPlaceLayer
{
    NoiseMap noisemap;
    float map_scale;

  public:
    TerrainGenerationLayer(const confparse::Config &cfg)
    {
        auto global_map_scale = cfg.get("global_map_scale").parse<float>();
        map_scale = cfg.get("terrain.scale").parse<float>() * global_map_scale;
        auto redistribution = cfg.get("terrain.redistribution").try_parse<float>(1.0f);
        auto fudge = cfg.get("terrain.fudge").parse<float>();
        auto octaves = cfg.get("terrain.octaves").parse<int>();
        auto seed = cfg.get("seed").parse<int>() + TERRAIN_SEED_MAGIC_NUMBER;

        std::vector<float> frequencies;
        std::vector<float> amplitudes;

        for (int i = 1; i <= octaves; ++i)
        {
            auto frequency = cfg.get("terrain.frequency" + std::to_string(i)).parse<float>();
            auto amplitude = cfg.get("terrain.amplitude" + std::to_string(i)).parse<float>();
            frequencies.push_back(frequency);
            amplitudes.push_back(amplitude);
        }
        NoiseGenerator base_generator{seed};
        noisemap = NoiseMap(frequencies, amplitudes, base_generator, fudge, redistribution);
    }

    auto execute(Chunk &chunk, Registry &registry) const -> void
    {
        noisemap.create_noise_map(chunk.x, chunk.y, chunk.width, chunk.height, map_scale,
                                  chunk.elevation);
    }
};

class MoistureGenerationLayer : public InPlaceLayer
{
    NoiseMap noisemap;
    float map_scale;

  public:
    MoistureGenerationLayer(const confparse::Config &cfg)
    {
        auto global_map_scale = cfg.get("global_map_scale").parse<float>();
        map_scale = cfg.get("moisture.scale").parse<float>() * global_map_scale;
        auto redistribution = cfg.get("moisture.redistribution").try_parse<float>(1.0f);
        auto fudge = cfg.get("moisture.fudge").parse<float>();
        auto octaves = cfg.get("moisture.octaves").parse<int>();
        auto seed = cfg.get("seed").parse<int>() + MOISTURE_SEED_MAGIC_NUMBER;

        std::vector<float> frequencies;
        std::vector<float> amplitudes;

        for (int i = 1; i <= octaves; ++i)
        {
            auto frequency = cfg.get("moisture.frequency" + std::to_string(i)).parse<float>();
            auto amplitude = cfg.get("moisture.amplitude" + std::to_string(i)).parse<float>();
            frequencies.push_back(frequency);
            amplitudes.push_back(amplitude);
        }

        NoiseGenerator base_generator{seed};
        noisemap = NoiseMap(frequencies, amplitudes, base_generator, fudge, redistribution);
    }

    auto execute(Chunk &chunk, Registry &registry) const -> void
    {
        noisemap.create_noise_map(chunk.x, chunk.y, chunk.width, chunk.height, map_scale,
                                  chunk.moisture);
    }
};

class BiomeCreationLayer : public InPlaceLayer
{

  public:
    BiomeCreationLayer(const confparse::Config &cfg) {}

    auto execute(Chunk &chunk, Registry &registry) const -> void
    {
        int idx = 0;
        for (int y = 0; y < chunk.height; ++y)
        {
            for (int x = 0; x < chunk.width; ++x)
            {
                chunk.biome[idx] = registry.biome_registry.get_biome_within_range(
                    chunk.moisture[idx], chunk.elevation[idx]);
                idx++;
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

    int width = cfg.get("chunk_side_length").parse<int>();
    int height = cfg.get("chunk_side_length").parse<int>();
    int master_seed = cfg.get("seed").parse<int>();

    layers.push_back(std::make_unique<InitializationLayer>(width, height, master_seed));
    layers.push_back(std::make_unique<TerrainGenerationLayer>(cfg));
    layers.push_back(std::make_unique<MoistureGenerationLayer>(cfg));
    layers.push_back(std::make_unique<BiomeCreationLayer>(cfg));
}

auto ChunkFactory::execute(Registry &registry, int chunk_x, int chunk_y) const -> Chunk
{
    Chunk chunk;
    chunk.x = chunk_x;
    chunk.y = chunk_y;
    logger::info("Creating chunk [{}, {}]", chunk_x, chunk_y);
    for (const auto &layer : layers)
    {
        if (layer->type() == LayerType::INPLACE)
            static_cast<InPlaceLayer *>(layer.get())->execute(chunk, registry);
        else
            chunk = static_cast<OutPlaceLayer *>(layer.get())->execute(chunk, registry);
    }
    return chunk;
}

auto ChunkFactory::execute_update(Registry &registry, int chunk_x, int chunk_y, Chunk &chunk) const
    -> void
{
    chunk.x = chunk_x;
    chunk.y = chunk_y;
    logger::info("Updating chunk [{}, {}]", chunk_x, chunk_y);
    for (const auto &layer : layers)
    {
        if (layer->type() == LayerType::INPLACE)
            static_cast<InPlaceLayer *>(layer.get())->execute(chunk, registry);
        else
            chunk = static_cast<OutPlaceLayer *>(layer.get())->execute(chunk, registry);
    }
}