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
            texture.pixels[y * texture.width + x] = get_color(chunk.biome[idx]);
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
            texture.pixels[y * texture.width + x] = get_color(chunk.biome[idx]);
            idx++;
        }
    }
    UpdateTexture(texture.texture, texture.pixels);
}

auto ChunkRenderer2D::from_config(const confparse::Config &cfg) -> void {}

auto ChunkRenderer2D::get_color(Biome biome) const -> Color
{
    if (biome == Biome::DEEP_OCEAN)
        return {35, 52, 105, 255};
    else if (biome == Biome::SHALLOW_OCEAN)
        return {78, 98, 159, 255};
    else if (biome == Biome::BEACH)
        return {208, 181, 138, 255};
    else if (biome == Biome::TEMPERATE_GRASSLAND)
        return {74, 90, 64, 255};
    else if (biome == Biome::SHRUBLAND)
        return {57, 76, 56, 255};
    else if (biome == Biome::MOUNTAIN)
        return {46, 46, 35, 255};
    else
        return {0, 0, 0, 255};
}