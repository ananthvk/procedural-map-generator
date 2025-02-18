#ifndef A_CHUNK_RENDERER_H
#define A_CHUNK_RENDERER_H
#include "chunk.hpp"
#include <raylib.h>
#include <stdlib.h>

struct ChunkTexture2D
{
    int width;
    int height;
    Color *pixels;
    Texture2D texture;

    ChunkTexture2D();

    auto unload() -> void;
};

class ChunkRenderer2D
{
    enum class RenderMode
    {
        ELEVATION_HEIGHTMAP,
        MOISTURE_HEIGHTMAP,
        BIOME_MAP
    };
    RenderMode current_render_mode;

  public:
    auto generate_texture(const Chunk &chunk) const -> ChunkTexture2D;

    auto update_texture(ChunkTexture2D &texture, const Chunk &chunk) -> void;

    auto from_config(const confparse::Config &cfg) -> void;

    auto get_color(const Chunk &chunk, int idx) const -> Color;
};

#endif // A_CHUNK_RENDERER_H