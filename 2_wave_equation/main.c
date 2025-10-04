#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

// Structs remain the same
typedef struct Coordinate
{
    int x;
    int y;
} coord_t;

typedef struct Wave_t
{
    int nx;
    int ny;
    float c;  // Wave speed
    float h;  // Spatial step (grid spacing)
    float dt; // Time step
} wave_t;

float **init_2d_matrix(int n_cells)
{
    float **m = (float **)malloc(n_cells * sizeof(float *));
    if (m == NULL)
    {
        return NULL;
    }

    for (int i = 0; i < n_cells; i++)
    {
        m[i] = (float *)calloc(n_cells, sizeof(float)); // Use calloc to initialize to 0.0f
        if (m[i] == NULL)
        {
            for (int j = 0; j < i; j++)
            {
                free(m[j]);
            }
            free(m);
            return NULL;
        }
    }
    return m;
}

void free_2d_matrix(float **m, int n_cells)
{
    if (m == NULL)
        return;
    for (int i = 0; i < n_cells; i++)
    {
        free(m[i]);
    }
    free(m);
}

void showFPS(int show_fps, int fontSize, int screenSize)
{
    if (show_fps == 1)
    {
        int fps = GetFPS();
        const char *fpsText = TextFormat("FPS: %d", fps);
        int fpsTextWidth = MeasureText(fpsText, fontSize);
        DrawText(fpsText, screenSize - fpsTextWidth - 10, 10, fontSize, GREEN);
    }
}

int normalize(float value, float min_val, float max_val)
{
    if (max_val == min_val)
        return 128;

    float normalized = 255.0f * (value - min_val) / (max_val - min_val);

    if (normalized < 0)
        return 0;
    if (normalized > 255)
        return 255;

    return (int)normalized;
}

void drawCells(float **m, int cell_num, int cell_size)
{
    // A reasonable min/max amplitude for visualization. You can adjust this.
    float min_amp = -100.0f;
    float max_amp = 100.0f;

    for (int i = 0; i < cell_num; i++)
    {
        for (int j = 0; j < cell_num; j++)
        {
            unsigned char norm_color = (unsigned char)normalize(m[i][j], min_amp, max_amp);
            Color color = {norm_color, norm_color, norm_color, norm_color};
            DrawRectangle(i * cell_size, j * cell_size, cell_size, cell_size, color);
        }
    }
}

void calculate_next(wave_t wave, float **prev, float **curr, float **next, int n_cells)
{
    // The Courant number squared. For stability, C should be <= 1/sqrt(2)
    float C = (wave.c * wave.dt) / wave.h;
    float C_sq = C * C;

    for (int i = 1; i < n_cells - 1; i++)
    {
        for (int j = 1; j < n_cells - 1; j++)
        {
            float laplacian = curr[i + 1][j] + curr[i - 1][j] + curr[i][j + 1] + curr[i][j - 1] - 4 * curr[i][j];
            next[i][j] = 2 * curr[i][j] - prev[i][j] + C_sq * laplacian;
        }
    }
}

int main(void)
{
    const int screen_size = 800;
    const int cell_num = 400;
    const int cell_size = screen_size / cell_num;

    // FIX 1: Matrices are now float**
    float **prev = init_2d_matrix(cell_num);
    float **curr = init_2d_matrix(cell_num);
    float **next = init_2d_matrix(cell_num);

    // Check if allocation was successful
    if (prev == NULL || curr == NULL || next == NULL)
    {
        printf("ERROR: Failed to allocate matrix memory.\n");
        return 1;
    }

    // --- Simulation Parameters ---
    // The stability of the simulation depends on the relationship between c, dt, and h.
    // The Courant–Friedrichs–Lewy (CFL) condition requires (c * dt / h) <= 1/sqrt(2) ≈ 0.707
    wave_t wave = {cell_num, cell_num, 1.0f, 1.0f, 0.5f}; // c, h, dt

    // --- Initial Condition: A Gaussian "drop" in the middle ---
    int cx = cell_num / 2;
    int cy = cell_num / 2;
    for (int i = 0; i < cell_num; i++)
    {
        for (int j = 0; j < cell_num; j++)
        {
            float r_sq = (float)((i - cx) * (i - cx) + (j - cy) * (j - cy));
            // A sharper pulse for a more visible effect
            curr[i][j] = 250.0f * exp(-r_sq / 20.0f);
        }
    }

    // For the first step, assume the previous state was the same as the current state
    for (int i = 0; i < cell_num; i++)
    {
        for (int j = 0; j < cell_num; j++)
        {
            prev[i][j] = curr[i][j];
        }
    }

    InitWindow(screen_size, screen_size, "2D Wave Simulation");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        // --- Update Step ---

        // Calculate the next state based on the previous two
        calculate_next(wave, prev, curr, next, cell_num);

        // FIX 2: Efficiently cycle the buffers by swapping pointers instead of copying memory
        float **temp = prev;
        prev = curr;
        curr = next;
        next = temp; // `next` now points to old `prev` memory, which is free to be overwritten.

        // --- Drawing Step ---
        BeginDrawing();
        ClearBackground(GRAY); // Middle gray for zero amplitude

        drawCells(curr, cell_num, cell_size);

        showFPS(1, 20, screen_size);
        DrawText("2D Wave Simulation", 10, 10, 20, LIME);
        EndDrawing();
    }

    // FIX 4: Free all allocated memory before closing
    free_2d_matrix(prev, cell_num);
    free_2d_matrix(curr, cell_num);
    free_2d_matrix(next, cell_num);

    CloseWindow();
    return 0;
}