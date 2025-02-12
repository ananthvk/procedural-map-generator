#include "chunk.hpp"
#include "chunk_renderer.hpp"
#include "confparse.hpp"
#include "logger.h"
#include <raylib.h>
#include <raymath.h>

using namespace logger;

const char *config_file_path = "config.txt";

auto main(int argc, char *argv[]) -> int
{
    confparse::ConfigParser parser;
    ChunkFactory factory;

    int width, height, FPS;
    std::string title;

    try
    {
        auto cfg = parser.from_file(config_file_path);
        factory.from_config(cfg);
        width = cfg.get("width").parse<int>();
        height = cfg.get("height").parse<int>();
        FPS = cfg.get("fps").parse<int>();
        title = cfg.get("title").as_string();
    }
    catch (std::exception &e)
    {
        fatal("Error in config: \"{}\" {}", config_file_path, e.what());
        return 1;
    }

    auto chunk = factory.execute();

    info("Opening window...");
    InitWindow(width, height, title.c_str());
    SetTargetFPS(FPS);

    ChunkRenderer2D renderer;
    auto texture = renderer.generate_texture(chunk);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(texture.texture, 0, 0, RAYWHITE);
        DrawFPS(20, 20);
        DrawCircle(400, 400, 20, YELLOW);
        EndDrawing();
    }
    info("Closing application...");
    texture.unload();
    CloseWindow();
    return 0;
}