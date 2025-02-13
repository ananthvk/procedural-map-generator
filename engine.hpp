#ifndef A_ENGINE_H
#define A_ENGINE_H
#include "chunk.hpp"
#include "chunk_renderer.hpp"
#include "confparse.hpp"
#include "logger.h"
#include <raylib.h>
#include <raymath.h>

class Engine
{
    std::string config_file_path;
    confparse::ConfigParser parser;
    confparse::Config cfg;

    ChunkFactory factory;
    ChunkRenderer2D renderer;
    ChunkTexture2D chunk_texture;
    Chunk chunk;

    int width, height, FPS, reload_interval;
    std::string title;
    
    bool config_changed;

    auto load_config(const std::string &config_path) -> void;

    auto apply_config(bool is_update) -> void;


  public:
    Engine(const std::string &config_file_path);

    auto run() -> void;

    auto draw() -> void;

    ~Engine();
};

#endif // A_ENGINE_H