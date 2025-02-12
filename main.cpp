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
    confparse::Config cfg;
    ChunkFactory factory;
    ChunkRenderer2D renderer;

    int width, height, FPS, reload_interval;
    std::string title;

    try
    {
        cfg = parser.from_file(config_file_path);
        factory.from_config(cfg);
        renderer.from_config(cfg);
        width = cfg.get("width").parse<int>();
        height = cfg.get("height").parse<int>();
        FPS = cfg.get("fps").parse<int>();
        title = cfg.get("title").as_string();
        reload_interval = cfg.get("reload_interval").try_parse<int>(1);
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

    auto texture = renderer.generate_texture(chunk);

    float accumulator = 0;
    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        accumulator += dt;
        if (accumulator > reload_interval)
        {
            try
            {
                auto new_cfg = parser.from_file(config_file_path);
                // Configuration has changed
                if (!(new_cfg == cfg))
                {
                    info("Reloading config file for periodic reload\"{}\"", config_file_path);
                    factory.from_config(new_cfg);
                    renderer.from_config(new_cfg);
                    chunk = factory.execute();
                    renderer.update_texture(texture, chunk);
                    reload_interval = new_cfg.get("reload_interval").try_parse<int>(1);
                    cfg = new_cfg;
                }
            }
            catch (std::exception &e)
            {
                fatal("Did not reload config - reverting back, Error in config: \"{}\" {}",
                      config_file_path, e.what());
                factory.from_config(cfg);
                renderer.from_config(cfg);
                chunk = factory.execute();
                renderer.update_texture(texture, chunk);
            }
            accumulator = 0.0f;
        }

        if (IsKeyPressed(KEY_R))
        {
            info("Reloading config file \"{}\"", config_file_path);
            try
            {
                auto new_cfg = parser.from_file(config_file_path);
                factory.from_config(new_cfg);
                renderer.from_config(new_cfg);
                chunk = factory.execute();
                renderer.update_texture(texture, chunk);
                cfg = new_cfg;
            }
            catch (std::exception &e)
            {
                fatal("Did not reload config - reverting back, Error in config: \"{}\" {}",
                      config_file_path, e.what());
                factory.from_config(cfg);
                renderer.from_config(cfg);
                chunk = factory.execute();
                renderer.update_texture(texture, chunk);
            }
        }
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