#include "confparse.hpp"
#include "logger.h"
#include "noise.hpp"
#include <raylib.h>
#include <raymath.h>

using namespace logger;

const char *config_file_path = "config.txt";

auto get_color(float elevation) -> Color
{
    if (elevation < 0.2)
        // Deep Ocean
        return {52, 100, 235, 255};
    if (elevation < 0.5)
        // Lighter ocean
        return {52, 156, 235, 255};
    if (elevation < 0.8)
        // Land
        return {57, 153, 18, 255};
    if (elevation < 0.9)
        // Inlands
        return {18, 110, 4, 255};
    else
        return {227, 193, 232, 255};
}

class NoiseMap
{
    int width, height;
    std::vector<std::vector<float>> elevation;
    Texture2D texture;
    Color *pixels;
    NoiseGenerator generator;
    bool texture_created;
    bool dirty;

  public:
    NoiseMap(int width, int height, int seed)
        : width(width), height(height), elevation(height, std::vector<float>(width)),
          generator(seed), texture_created(false), dirty(true)
    {
    }

    ~NoiseMap() { close(); }

    auto generate(float frequency) -> void
    {
        if (!pixels)
        {
            pixels = static_cast<Color *>(malloc(width * height * sizeof(Color)));
        }
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                float nx = static_cast<float>(x) / width - 0.5;
                float ny = static_cast<float>(y) / height - 0.5;
                elevation[y][x] = generator.at(nx * frequency, ny * frequency);
            }
        }
        dirty = true;
    }

    auto generate_texture() -> void
    {
        if (!dirty)
            return;
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                // unsigned char value = static_cast<unsigned char>(elevation[y][x] * 255);
                Color col = get_color(elevation[y][x]);
                pixels[y * width + x] = col;
            }
        }

        if (!texture_created)
        {
            // If the texture has not been created yet, create it
            Image img = {};
            img.data = pixels;
            img.mipmaps = 1;
            img.width = width;
            img.height = height;
            img.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
            texture = LoadTextureFromImage(img);
            texture_created = true;
        }
        else
        {
            // Otherwise update the texture with new map data
            UpdateTexture(texture, pixels);
        }
        dirty = false;
    }

    auto get_texture() -> Texture2D { return texture; }

    auto randomize_seed() -> void { generator.set_seed(time(NULL)); }

    auto close() -> void
    {
        free(pixels);
        if (texture_created)
        {
            UnloadTexture(texture);
            texture_created = false;
        }
        pixels = nullptr;
    }
};

auto main(int argc, char *argv[]) -> int
{
    confparse::ConfigParser parser;

    int width, height, FPS, seed;
    std::string title;


    try
    {
        auto cfg = parser.from_file(config_file_path);
        width = cfg.get("width").parse<int>();
        height = cfg.get("height").parse<int>();
        FPS = cfg.get("fps").parse<int>();
        title = cfg.get("title").as_string();
        seed = cfg.get("seed").try_parse<int>(2025);
    }
    catch (std::exception &e)
    {
        fatal("Error in config: \"{}\" {}", config_file_path, e.what());
        return 1;
    }

    info("Opening window...");
    InitWindow(width, height, title.c_str());
    SetTargetFPS(FPS);

    float frequency = 100.0f;

    NoiseMap map(width, height, seed);
    map.generate(frequency);
    map.generate_texture();
    auto texture = map.get_texture();

    while (!WindowShouldClose())
    {
        // float dt = GetFrameTime();
        if (IsKeyPressed(KEY_F))
        {
            frequency -= 50.0f;
            map.generate(frequency);
        }
        if (IsKeyPressed(KEY_G))
        {
            frequency += 50.0f;
            map.generate(frequency);
        }
        if (IsKeyPressed(KEY_H))
        {
            map.randomize_seed();
            map.generate(frequency);
        }

        // Draw the map to the screen
        map.generate_texture();

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(texture, 0, 0, RAYWHITE);
        DrawFPS(20, 20);
        DrawCircle(400, 400, 20, YELLOW);
        EndDrawing();
    }
    info("Closing application...");
    map.close();
    CloseWindow();
    return 0;
}