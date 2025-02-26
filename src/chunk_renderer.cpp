#include "chunk_renderer.hpp"
#include "logger.h"
using namespace logger;

ChunkTexture2D::ChunkTexture2D() : width(0), height(0), pixels(nullptr), texture{} {}

auto ChunkTexture2D::unload() -> void
{
    if (pixels)
    {
        free(pixels);
        UnloadTexture(texture);
        pixels = nullptr;
    }
}

auto ChunkRenderer2D::generate_texture(const Chunk &chunk) const -> ChunkTexture2D
{
    // Create a new texture to represent this chunk
    ChunkTexture2D texture;
    texture.width = chunk.width;
    texture.height = chunk.height;
    texture.pixels = static_cast<Color *>(malloc(texture.width * texture.height * sizeof(Color)));

    int idx = 0;

    // Look at the elevation details and fill the color accordingly
    for (int y = 0; y < texture.height; ++y)
    {
        for (int x = 0; x < texture.width; ++x)
        {
            texture.pixels[y * texture.width + x] = get_color(chunk, idx);
            idx++;
        }
    }

    // Create an image, load a texture from that image and return the texture
    Image img = {};
    img.data = texture.pixels;
    img.mipmaps = 1;
    img.width = texture.width;
    img.height = texture.height;
    img.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    texture.texture = LoadTextureFromImage(img);
    return texture;
}

auto ChunkRenderer2D::update_texture(ChunkTexture2D &texture, const Chunk &chunk) -> void
{
    if (chunk.width != texture.width || chunk.height != texture.height)
    {
        texture.unload();
        info("Chunk width/height changed, rebuilding chunk");
        texture = generate_texture(chunk);
        return;
    }

    // TODO: DRY, refactor code
    int idx = 0;

    // Look at the elevation details and fill the color accordingly
    for (int y = 0; y < texture.height; ++y)
    {
        for (int x = 0; x < texture.width; ++x)
        {
            texture.pixels[y * texture.width + x] = get_color(chunk, idx);
            idx++;
        }
    }
    UpdateTexture(texture.texture, texture.pixels);
}

auto ChunkRenderer2D::from_config(const confparse::Config &cfg, Registry *registry) -> void
{
    auto render_type = cfg.get("render_type").as_string();
    this->registry = registry;
    if (render_type == "elevation_heightmap")
    {
        current_render_mode = RenderMode::ELEVATION_HEIGHTMAP;
    }
    else if (render_type == "moisture_heightmap")
    {
        current_render_mode = RenderMode::MOISTURE_HEIGHTMAP;
    }
    else if (render_type == "biome_map")
    {
        current_render_mode = RenderMode::BIOME_MAP;
    }
    else
    {
        throw std::runtime_error("Unknown render type: " + render_type);
    }
}

auto ChunkRenderer2D::get_color(const Chunk &chunk, int idx) const -> Color
{
    if (current_render_mode == RenderMode::BIOME_MAP)
    {
        int biome = chunk.biome[idx];
        // TODO: Return a default color
        if (biome == -1)
            return {0, 0, 0, 255};
        auto color = registry->biome_registry.get(biome).render_color;
        return {color.r, color.g, color.b, static_cast<unsigned char>(color.a * 255)};
    }
    else if (current_render_mode == RenderMode::ELEVATION_HEIGHTMAP)
    {
        unsigned char value = static_cast<unsigned char>(chunk.elevation[idx] * 255);
        return {value, value, value, 255};
    }
    else if (current_render_mode == RenderMode::MOISTURE_HEIGHTMAP)
    {
        unsigned char value = static_cast<unsigned char>(chunk.moisture[idx] * 255);
        return {value, value, value, 255};
    }
    return {0, 0, 0, 255};
}