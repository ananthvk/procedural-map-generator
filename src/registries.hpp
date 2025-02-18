#ifndef A_REGISTRIES_H
#define A_REGISTRIES_H

#include "csscolorparser.hpp"
#include "logger.h"
#include <filesystem>
#include <string>
#include <vector>

using color = CSSColorParser::Color;

struct Biome
{
    int register_id;
    color render_color;
    std::string string_id;
    std::string name;
    float elevation_start;
    float elevation_end;
    float moisture_start;
    float moisture_end;
};

class BiomeRegistry
{
    std::vector<Biome> biomes;

  public:
    auto register_biome(Biome biome) -> int;

    auto get(int register_id) const -> const Biome &; 

    auto get_id(const std::string &string_id) const -> int;

    auto load(const std::string &file_path) -> void;
    
    auto get_biome_within_range(float moisture, float elevation) const -> int;
};

class Registry
{
    BiomeRegistry biome_registry_;

  public:
    auto load(const std::filesystem::path &data_folder_path) -> void;

    const BiomeRegistry &biome_registry = biome_registry_;
};

#endif // A_REGISTRIES_H