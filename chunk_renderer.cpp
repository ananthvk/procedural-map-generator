#include "chunk_renderer.hpp"

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
            unsigned char value = static_cast<unsigned char>(chunk.elevation[idx++] * 255);
            Color col = {value, value, value, 255};
            texture.pixels[y * texture.width + x] = col;
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