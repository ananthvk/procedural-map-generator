#include "registries.hpp"
#include "confparse.hpp"
#include <sstream>

auto BiomeRegistry::register_biome(Biome biome) -> int
{
    biome.register_id = static_cast<int>(biomes.size());
    biomes.push_back(biome);
    logger::info("[Biome] Registered \"{}\" with id {}", biome.string_id, biome.register_id);
    return biome.register_id;
}

auto BiomeRegistry::get(int id) const -> const Biome & { return biomes[id]; }

auto BiomeRegistry::get_id(const std::string &string_id) const -> int
{
    for (const auto &biome : biomes)
    {
        if (biome.string_id == string_id)
            return biome.register_id;
    }
    return -1;
}

auto BiomeRegistry::load(const std::string &file_path) -> void
{
    biomes.clear();
    logger::info("Loading biome data from {}", file_path);
    auto parser = confparse::ConfigParser();
    parser.options.single_line_comments = ";";

    auto cfg = parser.from_file(file_path);

    auto biome_ids_str = cfg.get("biomes").as_string();
    std::string biome_id;
    std::stringstream ss(biome_ids_str);
    logger::info("Listed biomes: {}", biome_ids_str);


    while (std::getline(ss, biome_id, ' '))
    {
        try
        {
            auto name = cfg.get(biome_id + ".name").as_string();
            auto color = cfg.get(biome_id + ".color").as_string();
            auto elevation_start = cfg.get(biome_id + ".elevation.start").parse<float>();
            auto elevation_end = cfg.get(biome_id + ".elevation.end").parse<float>();
            auto moisture_start = cfg.get(biome_id + ".moisture.start").parse<float>();
            auto moisture_end = cfg.get(biome_id + ".moisture.end").parse<float>();

            Biome biome;
            biome.name = name;
            biome.string_id = biome_id;
            auto parsed_color = CSSColorParser::parse(color);
            if (parsed_color)
            {
                biome.render_color = parsed_color.value();
            }
            else
            {
                logger::error("Color \"{}\" is invalid", color);
                throw std::exception();
            }
            biome.elevation_start = elevation_start;
            biome.elevation_end = elevation_end;
            biome.moisture_start = moisture_start;
            biome.moisture_end = moisture_end;
            register_biome(biome);
        }
        catch (std::exception &e)
        {
            logger::error("Biome information corrupt for biome with id = \"{}\"", biome_id);
            logger::error("Message: {}", e.what());
            throw e;
        }
    }
}

auto BiomeRegistry::get_biome_within_range(float moisture, float elevation) const -> int
{
    for (const auto &biome : biomes)
    {
        if (moisture >= biome.moisture_start && moisture <= biome.moisture_end &&
            elevation >= biome.elevation_start && elevation <= biome.elevation_end)
        {
            return biome.register_id;
        }
    }
    // TODO: Add default biome support
    return -1;
}

auto Registry::load(const std::filesystem::path &data_folder_path) -> void
{

    biome_registry_.load((data_folder_path / std::filesystem::path("biomes.txt")).generic_string());
    ;
}
