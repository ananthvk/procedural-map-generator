#include "engine.hpp"

using namespace logger;

auto Engine::load_config() -> void
{
    std::string config_path = (data_folder_path / "config.txt").generic_string();
    auto new_cfg = parser.from_file(config_path);
    if (cfg == new_cfg)
    {
        config_changed = false;
        return;
    }
    cfg = new_cfg;
    width = cfg.get("width").parse<int>();
    height = cfg.get("height").parse<int>();
    FPS = cfg.get("fps").parse<int>();
    title = cfg.get("title").as_string();
    fullscreen = cfg.get("fullscreen").try_parse<bool>(false);
    reload_interval = cfg.get("reload_interval").try_parse<int>(1);
    config_changed = true;
}

auto Engine::apply_config(bool is_update) -> void
{
    if (!config_changed)
        return;
    info("Applying new config since configuration changed...");
    factory.from_config(cfg);
    renderer.from_config(cfg, &registry);
    registry.load(data_folder_path);
    SetTargetFPS(FPS);
    SetWindowSize(width, height);
    SetWindowTitle(title.c_str());
    if (!is_currently_in_fullscreen && fullscreen)
    {
        ToggleFullscreen();
        is_currently_in_fullscreen = true;
    }
    else if (is_currently_in_fullscreen && !fullscreen)
    {
        ToggleFullscreen();
        is_currently_in_fullscreen = false;
    }
    chunk = factory.execute(registry);
    if (is_update)
        renderer.update_texture(chunk_texture, chunk);
    else
        chunk_texture = renderer.generate_texture(chunk);
}

Engine::Engine(const std::filesystem::path &data_folder_path)
    : is_currently_in_fullscreen(false), data_folder_path(data_folder_path), config_changed(true)
{
    info("Creating engine...");
    load_config();
}

auto Engine::run() -> void
{
    info("Opening window...");
    InitWindow(width, height, title.c_str());
    info("Applying config...");
    apply_config(false);

    float accumulator = 0;
    bool should_reload = false;

    while (!WindowShouldClose())
    {
        should_reload = false;
        float dt = GetFrameTime();
        accumulator += dt;

        if (accumulator > reload_interval)
        {
            accumulator = 0;
            should_reload = true;
        }
        if (IsKeyPressed(KEY_R))
            should_reload = true;

        if (should_reload)
        {
            load_config();
            apply_config(true);
        }

        draw();
    }
}

auto Engine::draw() -> void
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawTexture(chunk_texture.texture, 0, 0, RAYWHITE);
    DrawFPS(20, 20);
    DrawCircle(400, 400, 20, YELLOW);
    EndDrawing();
}

Engine::~Engine()
{
    info("Closing application...");
    chunk_texture.unload();
    CloseWindow();
}