#ifndef A_ENGINE_H
#define A_ENGINE_H
#include "chunk.hpp"
#include "chunk_renderer.hpp"
#include "confparse.hpp"
#include "engine.hpp"
#include "logger.h"
#include <raylib.h>
#include <raymath.h>

class Engine
{
    confparse::ConfigParser parser;
    confparse::Config cfg;

    ChunkFactory factory;
    ChunkRenderer2D renderer;
    ChunkTexture2D chunk_texture;
    Chunk chunk;

    int width, height, FPS, reload_interval;
    bool fullscreen;
    std::string title;

    bool is_currently_in_fullscreen;
    std::filesystem::path data_folder_path;
    bool config_changed;

    Registry registry;

    auto load_config() -> void;

    auto apply_config(bool is_update) -> void;


  public:
    Engine(const std::filesystem::path &data_folder_path);

    auto run() -> void;

    auto draw() -> void;

    ~Engine();
};

#endif // A_ENGINE_H