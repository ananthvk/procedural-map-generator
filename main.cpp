#include "logger.h"
#include <raylib.h>
#include <raymath.h>

using namespace logger;

const int width = 800;
const int height = 800;
const char *title = "Procedural Map Generator";
const int FPS = 60;
const Vector2 camera_center = {width / 2.0f, height / 2.0f};
const float x_camera_pan_speed = 200;
const float y_camera_pan_speed = 200;

struct MapState
{
    // World space coordinate on which the user is centered to
    Vector2 target;
};

auto main(int argc, char *argv[]) -> int
{
    info("Opening window...");
    InitWindow(width, height, title);
    SetTargetFPS(FPS);

    MapState state = {};
    state.target = {0, 0};

    Camera2D camera = {};
    camera.rotation = 0.0f;
    camera.offset = camera_center;
    camera.target = state.target;
    camera.zoom = 1.0f;

    Texture2D noise = {};

    Image perlinNoise =
        GenImagePerlinNoise(width, height, state.target.x, state.target.y, camera.zoom);
    noise = LoadTextureFromImage(perlinNoise);
    UnloadImage(perlinNoise);

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        float wheel = GetMouseWheelMove();
        bool should_update = false;

        if (IsKeyDown(KEY_D))
        {
            state.target.x += x_camera_pan_speed * dt;
            should_update = true;
        }
        if (IsKeyDown(KEY_A))
        {
            state.target.x -= x_camera_pan_speed * dt;
            should_update = true;
        }
        if (IsKeyDown(KEY_W))
        {
            state.target.y -= y_camera_pan_speed * dt;
            should_update = true;
        }
        if (IsKeyDown(KEY_S))
        {
            state.target.y += y_camera_pan_speed * dt;
            should_update = true;
        }

        if (wheel != 0)
        {
            // Get the world point that is under the mouse
            Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

            // Set the offset to where the mouse is
            camera.offset = GetMousePosition();

            // Set the target to match, so that the camera maps the world space point
            // under the cursor to the screen space point under the cursor at any zoom
            state.target = mouseWorldPos;

            // Zoom increment
            float scaleFactor = 1.0f + (0.25f * fabsf(wheel));
            if (wheel < 0)
                scaleFactor = 1.0f / scaleFactor;
            camera.zoom = Clamp(camera.zoom * scaleFactor, 0.125f, 64.0f);
            should_update = true;
        }

        camera.target = state.target;
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
        if (should_update)
        {
            perlinNoise = GenImagePerlinNoise(width, height, state.target.x, state.target.y,
                                              1.0f / camera.zoom);
            UpdateTexture(noise, perlinNoise.data);
            UnloadImage(perlinNoise);
        }

        // Draw the map to the screen
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawTexture(noise, 0, 0, RAYWHITE);
        DrawFPS(20, 20);
        BeginMode2D(camera);
        DrawCircle(0, 0, 20, YELLOW);
        EndMode2D();

        DrawTextEx(GetFontDefault(), TextFormat("(%f, %f)", mouseWorldPos.x, -mouseWorldPos.y),
                   {20.0f, 40.0f}, 20, 2, BLACK);
        EndDrawing();
    }
    UnloadTexture(noise);
    CloseWindow();
    info("Closing application...");
    return 0;
}