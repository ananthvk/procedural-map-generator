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

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        if (IsKeyDown(KEY_D))
            state.target.x += x_camera_pan_speed * dt;
        if (IsKeyDown(KEY_A))
            state.target.x -= x_camera_pan_speed * dt;
        if (IsKeyDown(KEY_W))
            state.target.y -= y_camera_pan_speed * dt;
        if (IsKeyDown(KEY_S))
            state.target.y += y_camera_pan_speed * dt;

        camera.target = state.target;
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawFPS(20, 20);
        BeginMode2D(camera);
        DrawCircle(0, 0, 10, BLACK);
        EndMode2D();

        DrawTextEx(GetFontDefault(), TextFormat("(%f, %f)", mouseWorldPos.x, -mouseWorldPos.y),
                   {20.0f, 40.0f}, 20, 2, BLACK);
        EndDrawing();
    }
    CloseWindow();
    info("Closing application...");
    return 0;
}