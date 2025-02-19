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
    float map_scale, redistribution, fudge, frequency1, frequency2, frequency3, frequency4,
        amplitude1, amplitude2, amplitude3, amplitude4, global_map_scale;

  public:
    TerrainGenerationLayer(const confparse::Config &cfg)
    {
        global_map_scale = cfg.get("global_map_scale").parse<float>();
        map_scale = cfg.get("terrain.scale").parse<float>() * global_map_scale;
        redistribution = cfg.get("terrain.redistribution").try_parse<float>(1.0f);
        fudge = cfg.get("terrain.fudge").parse<float>();

        frequency1 = cfg.get("terrain.frequency1").parse<float>();
        frequency2 = cfg.get("terrain.frequency2").parse<float>();
        frequency3 = cfg.get("terrain.frequency3").parse<float>();
        frequency4 = cfg.get("terrain.frequency4").parse<float>();

        amplitude1 = cfg.get("terrain.amplitude1").parse<float>();
        amplitude2 = cfg.get("terrain.amplitude2").parse<float>();
        amplitude3 = cfg.get("terrain.amplitude3").parse<float>();
        amplitude4 = cfg.get("terrain.amplitude4").parse<float>();
    }

    auto execute(Chunk &chunk, Registry &registry) const -> void
    {
        static NoiseGenerator generator1;
        static NoiseGenerator generator2;
        static NoiseGenerator generator3;
        static NoiseGenerator generator4;
        // Modify the noise generator seeds slightly

        generator1.set_seed(chunk.master_seed + 7158);
        generator2.set_seed(chunk.master_seed + 9821);
        generator3.set_seed(chunk.master_seed + 1356);
        generator4.set_seed(chunk.master_seed + 3495);

        int idx = 0;

        for (int y = 0; y < chunk.height; ++y)
        {
            for (int x = 0; x < chunk.width; ++x)
            {
                float nx = chunk.x + static_cast<float>(x) / chunk.width - 0.5;
                float ny = chunk.y + static_cast<float>(y) / chunk.height - 0.5;

                float elevation = amplitude1 * generator1.at((frequency1 * map_scale * nx),
                                                             (frequency1 * map_scale * ny));

                elevation += amplitude2 * generator2.at((frequency2 * map_scale * nx),
                                                        (frequency2 * map_scale * ny));

                elevation += amplitude3 * generator3.at((frequency3 * map_scale * nx),
                                                        (frequency3 * map_scale * ny));

                elevation += amplitude4 * generator4.at((frequency4 * map_scale * nx),
                                                        (frequency4 * map_scale * ny));

                elevation = elevation / (amplitude1 + amplitude2 + amplitude3 + amplitude4);
                elevation = std::powf(elevation * fudge, redistribution);
                chunk.elevation[idx++] = elevation;
            }
        }
    }
};

class MoistureGenerationLayer : public InPlaceLayer
{
    float map_scale, redistribution, fudge, frequency1, frequency2, frequency3, frequency4,
        amplitude1, amplitude2, amplitude3, amplitude4;
    float global_map_scale;

  public:
    MoistureGenerationLayer(const confparse::Config &cfg)
    {
        global_map_scale = cfg.get("global_map_scale").parse<float>();
        map_scale = cfg.get("moisture.scale").parse<float>() * global_map_scale;
        redistribution = cfg.get("moisture.redistribution").try_parse<float>(1.0f);
        fudge = cfg.get("moisture.fudge").parse<float>();

        frequency1 = cfg.get("moisture.frequency1").parse<float>();
        frequency2 = cfg.get("moisture.frequency2").parse<float>();
        frequency3 = cfg.get("moisture.frequency3").parse<float>();
        frequency4 = cfg.get("moisture.frequency4").parse<float>();

        amplitude1 = cfg.get("moisture.amplitude1").parse<float>();
        amplitude2 = cfg.get("moisture.amplitude2").parse<float>();
        amplitude3 = cfg.get("moisture.amplitude3").parse<float>();
        amplitude4 = cfg.get("moisture.amplitude4").parse<float>();
    }

    auto execute(Chunk &chunk, Registry &registry) const -> void
    {
        static NoiseGenerator generator1;
        static NoiseGenerator generator2;
        static NoiseGenerator generator3;
        static NoiseGenerator generator4;
        // Modify the noise generator seeds slightly

        generator1.set_seed(chunk.master_seed + 2025);
        generator2.set_seed(chunk.master_seed + 1337);
        generator3.set_seed(chunk.master_seed + 4040);
        generator4.set_seed(chunk.master_seed + 3891);

        int idx = 0;

        for (int y = 0; y < chunk.height; ++y)
        {
            for (int x = 0; x < chunk.width; ++x)
            {
                float nx = chunk.x + static_cast<float>(x) / chunk.width - 0.5;
                float ny = chunk.y + static_cast<float>(y) / chunk.height - 0.5;

                float moisture = amplitude1 * generator1.at((frequency1 * map_scale * nx),
                                                            (frequency1 * map_scale * ny));

                moisture += amplitude2 * generator2.at((frequency2 * map_scale * nx),
                                                       (frequency2 * map_scale * ny));

                moisture += amplitude3 * generator3.at((frequency3 * map_scale * nx),
                                                       (frequency3 * map_scale * ny));

                moisture += amplitude4 * generator4.at((frequency4 * map_scale * nx),
                                                       (frequency4 * map_scale * ny));

                moisture = moisture / (amplitude1 + amplitude2 + amplitude3 + amplitude4);
                moisture = std::powf(moisture * fudge, redistribution);
                chunk.moisture[idx++] = moisture;
            }
        }
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